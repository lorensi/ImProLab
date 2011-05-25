/*
  Main window implementations.
  Copyright (C) 2005  Lucas Lorensi dos Santos

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#include <QtGui>
#include <QVector>
#include <QPoint>

#include "mainwindow.h"
#include "mask.h"

MdiMain::MdiMain()
{
  //workspace
  workspace = new QWorkspace;

  setCentralWidget(workspace);

  connect(workspace, SIGNAL(windowActivated(QWidget *)), this, SLOT(updateMenus()));

  windowMapper = new QSignalMapper(this);
  connect(windowMapper, SIGNAL(mapped(QWidget *)), workspace, SLOT(setActiveWindow(QWidget *)));

#ifdef Q_WS_WIN
  mdiCam = NULL;
#endif

  createActions();
  createMenus();
  createToolBars();
  updateMenus();
  readSettings();
  createStatusBar();

  setWindowTitle(tr("ImProLab - Laboratório de Processamento de Imagens"));
}

void MdiMain::closeEvent(QCloseEvent *event)
{
  workspace->closeAllWindows();
  if (activeMdiChild())
  {
    event->ignore();
  }
  else
  {
    writeSettings();
    event->accept();
  }
}

void MdiMain::calib(double dist)
{
  statusBar()->showMessage(tr("dist: %1").arg(dist));
}

void MdiMain::setPixelScale(double PixelScale)
{
  if( QAction *currAct = amplActGroup->checkedAction() )
  {
    QSettings settings("PUCRS", "ImProLab");
    QString sAmp = currAct->text() + tr("_scale");
    settings.setValue(sAmp, PixelScale);

    QString sMsg = tr("Nova definição. Aumento de %1: %2 [%3]")
                    .arg(currAct->text()).arg(getPixelScale()).arg(getUnitScale());
    statusBar()->showMessage( sMsg );
  }
}

double MdiMain::getPixelScale()
{
  if( QAction *currAct = amplActGroup->checkedAction() )
  {
    QSettings settings("PUCRS", "ImProLab");
    QString sAmp = currAct->text() + tr("_scale");
    return settings.value(sAmp, 1.0).toDouble();
  }
  return 0.0;
}

void MdiMain::setUnitScale(QString UnitScale)
{
  if( QAction *currAct = amplActGroup->checkedAction() )
  {
    QSettings settings("PUCRS", "ImProLab");
    QString sAmp = currAct->text() + tr("_unit");
    settings.setValue(sAmp, UnitScale);

    QString sMsg = tr("Nova definição. Aumento de %1: %2 [%3]")
                    .arg(currAct->text()).arg(getPixelScale()).arg(getUnitScale());
    statusBar()->showMessage( sMsg );
  }
}

QString MdiMain::getUnitScale()
{
  if( QAction *currAct = amplActGroup->checkedAction() )
  {
    QSettings settings("PUCRS", "ImProLab");
    QString sAmp = currAct->text() + tr("_unit");
    return settings.value(sAmp, "pixels" ).toString();
  }
  return tr("");
}

//-------------
// --- file ---
//-------------
void MdiMain::open()
{
  QString fileName = QFileDialog::getOpenFileName(this,tr("Arquivo de imagem"),QDir::currentPath());
  if (!fileName.isEmpty())
  {
    MdiImage *child = createMdiImage();
    if (child->loadFromFile(fileName))
    {
      statusBar()->showMessage(tr("Arquivo carregado."), 2000);
      child->show();
    }
    else
    {
      child->close();
    }
  }
}

void MdiMain::save()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    QAction *action = qobject_cast<QAction *>(sender());
    QByteArray fileFormat = action->data().toByteArray();

    QString initialPath = QDir::currentPath() + "/imagem." + fileFormat;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Salvar como..."),
                                   initialPath,
                                   tr("%1 Files (*.%2);;All Files (*)")
                                   .arg(QString(fileFormat.toUpper()))
                                   .arg(QString(fileFormat)));

    if (!fileName.isEmpty())
    {
      child->save( fileName, fileFormat );
    }
  }
}

void MdiMain::filePrint()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    QPrintDialog dialog(&printer, this);
    if( dialog.exec() )
    {
      QPainter painter(&printer);
      QRect rect = painter.viewport();
      QSize size = child->getImage().size();
      size.scale(rect.size(), Qt::KeepAspectRatio);
      painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
      painter.setWindow(child->getImage().rect());
      painter.drawImage(0, 0, child->getImage() );
    }
  }
}

//-------------
// --- edit ---
//-------------
void MdiMain::editDuplic()
{
  MdiImage *newchild = cloneActiveMdiImage();
  newchild->show();
}

void MdiMain::editUndo()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    child->undo();
  }
}

void MdiMain::editCopy()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setImage ( child->getImage() );
    //clipboard->setPixmap ( QPixmap::grabWidget( child, 0, 0, child->getImage().width() , child->getImage().height() ) );
  }
}

void MdiMain::editPaste()
{
  QClipboard *clipboard = QApplication::clipboard();
  QImage image = clipboard->image();

  if( !image.isNull() )
  {
    MdiImage *child = createMdiImage();
    child->setImage( image );
    child->setWindowTitle("Proveniente da área de trabalho");
    child->show();
  }
}

/*
void MdiMain::editShell(bool show)
{
  if( show )
    shell->show();
  else
    shell->close();

}
*/

//-------------------
// --- processing ---
//-------------------
void MdiMain::procMask()
{
  QString fileName = QFileDialog::getOpenFileName(
                      this,
                      "Escolha um arquivo que contenha uma máscara",
                      QDir::currentPath(),
                      "Máscaras (*.mas *.mak)\nTodos (*.*)");

  if (!fileName.isEmpty())
  {
    MdiImage *child = getActiveImage();
    if( child )
    {
      QImage img = child->getImage();

      Mask mask;
      mask.load(fileName.toAscii().data());

      ImageEdit ie(img);
      QImage image = ie.convolution(mask);

      child->setImage(image);
      child->show();
    }
  }
}

void MdiMain::procGrayScale()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    QVector<QRgb> colors;
    for(int i=0; i < 256; ++i)
      colors.push_back( qRgb(i,i,i) );

    QImage a;
    a.setNumColors( colors.size() );
    a = child->getImage().copy();
    a = a.convertToFormat( QImage::Format_Indexed8, colors );
    a.setColorTable( colors );
    child->setImage( a );
  }
}

void MdiMain::procNegative()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    ImageEdit ie( child->getImage() );

    child->setImage( ie.negative() );
    child->show();
  }
}

void MdiMain::procThreshold()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    MdiThreshold *newchild = new MdiThreshold(this);
    newchild->setMdiImage( child );
    workspace->addWindow(newchild);
    newchild->show();
  }
}

void MdiMain::procHistogram()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    MdiHistogram *hist = new MdiHistogram(this);
    workspace->addWindow(hist);
    hist->setImage( child->getImage() );
    hist->show();
  }
}

//-----------------
// --- analisys ---
//-----------------

void MdiMain::analDistance()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    child->distance();
  }
}

void MdiMain::analArea()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    child->area();
  }
}

void MdiMain::analCalibration()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    child->calibration();
  }
}

void MdiMain::analTag(bool show)
{
  MdiImage *child = getActiveImage(true);
  if( child )
  {
    if( child->showTag() != show )
    {
      if( show )
      {
        double tagValue = child->getTagValue();

        bool ok;
        tagValue = QInputDialog::getDouble(this, tr("Etiqueta"),
                   tr("Tamanho da etiqueta..."),
                   tagValue, -1000000, 1000000, 2, &ok);

        if(ok)
          child->setTagValue( tagValue );

        show = ok;
      }

      child->showTag( show );
    }
  }
}


void MdiMain::analTagColor()
{
  MdiImage *child = getActiveImage(true);
  if( child )
  {
    child->setTagColor();
  }
}


void MdiMain::analDilation()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    ImageEdit ie(child->getImage());
    QImage image = ie.dilation(mask);

    child->setImage(image);
    child->show();
  }
}

void MdiMain::analErosion()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    ImageEdit ie(child->getImage());
    QImage image = ie.erosion(mask);
    child->setImage(image);
    child->show();
  }
}

void MdiMain::analOpening()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    ImageEdit ie(child->getImage());
    ie.erosion(mask);
    QImage image = ie.dilation(mask);
    child->setImage(image);
    child->show();
  }
}

void MdiMain::analClosing()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    ImageEdit ie(child->getImage());
    ie.dilation(mask);
    QImage image = ie.erosion(mask);
    child->setImage(image);
    child->show();
  }
}

void MdiMain::analStruct()
{

  QString fileName = QFileDialog::getOpenFileName(
                      this,
                      "Escolha um arquivo que contenha uma máscara, a estrutura do elemento",
                      QDir::currentPath(),
                      "Máscaras (*.mas *.mak)\nTodos (*.*)");

  if (!fileName.isEmpty())
  {
    mask.load(fileName.toAscii().data());
  }
}

void MdiMain::analBound()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    QImage img = child->getImage();

    ImageEdit ie(img);
    Mask mask(3,1.0);
    ie.erosion(mask);
    child->setImage( ie.subtration(img) );

    child->show();
  }
}

void MdiMain::analFillHoles()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    QImage img = child->getImage();

    ImageEdit ie(img);
    child->setImage( ie.fillholes() );

    child->show();
  }
}

void MdiMain::analAnalisys()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    QStringList items;
    items << tr("Branco") << tr("Preto");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("ImProLab"), tr("Qual é a cor de fundo?"), items, 0, false, &ok);
    if (ok && !item.isEmpty())
    {
      ImageEdit ie(child->getImage());

      //transform to white objects
      if( item == "Preto" )
        ie.negative();

      //openning
      ie.erosion(mask);
      ie.dilation(mask);

      //closing
      ie.dilation(mask);
      ie.erosion(mask);

      //fillholes
      ie.fillholes();

      //negative
      ie.negative();

      //labeling
      QVector<ImgData> data = ie.labeling();

      //negative
      ie.negative();

      //boundarray
      QImage imgBeforeBound = ie.getImage();
      Mask mask(3,1.0);
      ie.erosion(mask);
      ie.subtration(imgBeforeBound);

      //new mdi window
      MdiAnalisys *ana = new MdiAnalisys( this, data, ie.getImage() );
      workspace->addWindow(ana);
      ana->show();
      //child->setImage( pixPaint.toImage() );
      //child->show();
    }
  }
}

void MdiMain::amplValue(QAction *a)
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    child->update();
  }

  createStatusBar();
}

#ifdef Q_WS_WIN
//---------------
// --- camera ---
//---------------

void MdiMain::camAcquire()
{
  foreach( QWidget *widget, workspace->windowList() )
  {
    if( widget == mdiCam )
    {
      widget->show();
      widget->setFocus( Qt::OtherFocusReason );
      return;
    }
  }

  mdiCam = new MdiCamera(this);
  workspace->addWindow(mdiCam);
  mdiCam->show();
}

void MdiMain::camGrab()
{
  try
  {
    QImage img = mdiCam->grab();

    MdiImage *child = createMdiImage();
    child->setImage( img );
    child->show();

    statusBar()->showMessage(tr("Imagem capturada"), 2000);
  }
  catch(...)
  {
    QMessageBox::warning(this, "ImProLab", "Impossivel obter imagem!!!");
  }
}
#endif
//--------------
// --- about ---
//--------------

void MdiMain::about()
{
  QMessageBox::about(this, tr("Sobre ImProLab"),
    tr("<p><b>          ImProLab (versão " VERSION_STR ")</b></p>"
       "<p>Copyright (C) 2005-2008  Lucas Lorensi dos Santos</p>"
       "<p>Copyright (C) 2008  Filipi Damasceno Vianna</p>"
       "<tt>http://www.em.pucrs.br/~lucas/improlab</tt>"
       "<p>Este programa é um software livre; você pode redistribui-lo e/ou "
       "modifica-lo dentro dos termos da Licença Pública Geral GNU como "
       "publicada pela Fundação do Software Livre (FSF); na versão 2 da "
       "Licença, ou (na sua opnião) qualquer versão.<br>"
       "Este programa é distribuido na esperança que possa ser  util, "
       "mas SEM NENHUMA GARANTIA; sem uma garantia implicita de ADEQUAÇÃO a qualquer"
       " MERCADO ou APLICAÇÃO EM PARTICULAR. Veja a"
       " Licença Pública Geral GNU para maiores detalhes.</p>" ));
}

MdiImage *MdiMain::getActiveImage( bool quiet )
{
  MdiImage *child = dynamic_cast<MdiImage*>( activeMdiChild() );
  if( !child && !quiet )
  {
    QMessageBox::information(this, "ImProLab",
    "Nenhum imagem encontra-se selecionada.\n"
    "Selecione a janela a qual contém a imagem a ser utilizada para este aplicativo.");
  }
  return child;
}

MdiImage* MdiMain::cloneActiveMdiImage()
{
  MdiImage *child = getActiveImage();
  if( child )
  {
    MdiImage *newchild = createMdiImage();
    newchild->setImage( child->getImage() );
    newchild->setWindowTitle( child->windowTitle() );
    return newchild;
  }
  return NULL;
}

void MdiMain::updateMenus()
{
  MdiChild *mdichild = activeMdiChild();
  if( mdichild )
  {
    MdiHistogram *hist = dynamic_cast<MdiHistogram*>(mdichild);
    if( hist )
    {
      bool enable = (bool)hist;
      processMenu->setEnabled( enable );
      analisysMenu->setEnabled( enable );
    }
  }

#ifdef Q_WS_WIN
  /*
  grabAct->setEnabled( false );
  foreach( QWidget *widget, workspace->windowList() )
  {
    if( widget == mdiCam )
    {
      grabAct->setEnabled( true );
      break;
    }
  }*/

  //if( MdiCamera *cam = dynamic_cast<MdiCamera*>(workspace->activeWindow()) )
  //grabAct->setEnabled( mdiCam ? true : false );
  bool bHasCam = dynamic_cast<MdiCamera*>(workspace->activeWindow());
  grabAct->setEnabled( bHasCam );
  //acquireAct->setEnabled( !bHasCam );
#endif

  windowMenu->setEnabled( mdichild != 0 );

  MdiImage *mdiImage = dynamic_cast<MdiImage*>(mdichild);

  bool hasImg = (bool)mdiImage;

  processMenu->setEnabled( hasImg );
  analisysMenu->setEnabled( hasImg );
  duplicAct->setEnabled( hasImg );
  copyAct->setEnabled( hasImg );
  saveAsMenu->setEnabled( hasImg );
  printAct->setEnabled( hasImg );
}

void MdiMain::updateEditMenu()
{
  MdiImage *mdiImage = dynamic_cast<MdiImage*>(activeMdiChild());

  bool hasImg = (bool)mdiImage;

  grayscaleAct->setEnabled( hasImg );

  undoAct->setEnabled( hasImg && mdiImage->imagesSize() > 1 );

  QClipboard *clipboard = QApplication::clipboard();
  QImage image = clipboard->image();
  pasteAct->setEnabled( !image.isNull() );
}

void MdiMain::updateAnaliseMenu()
{
  MdiImage *mdiImage = dynamic_cast<MdiImage*>(activeMdiChild());
  bool hasImg = (bool)mdiImage;
  tagAct->setChecked( hasImg && mdiImage->showTag() );
  tagColorAct->setEnabled( hasImg && mdiImage->showTag() );
}

MdiImage *MdiMain::createMdiImage()
{
  MdiImage *child = new MdiImage(this);
  workspace->addWindow(child);
  return child;
}

void MdiMain::createActions()
{
  // file
  openAct = new QAction(QIcon(":/images/open.png"), tr("&Abrir..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Abre um arquivo existente"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  /*
  saveAct = new QAction(QIcon(":/images/save.png"), tr("&Salvar..."), this);
  saveAct->setShortcut(tr("Ctrl+S"));
  saveAct->setStatusTip(tr("Salva imagem corrente"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
  */


  foreach (QByteArray format, QImageWriter::supportedImageFormats())
  {
    QString text = tr("%1...").arg(QString(format).toUpper());

    QAction *action = new QAction(text, this);
    action->setData(format);
    connect(action, SIGNAL(triggered()), this, SLOT(save()));
    saveAsActs.append(action);
  }

  printAct = new QAction(QIcon(":/images/copy.png"), tr("&Imprimir..."), this);
  printAct->setShortcut(tr("Ctrl+P"));
  printAct->setStatusTip(tr("Imprimi a imagem atual"));
  connect(printAct, SIGNAL(triggered()), this, SLOT(filePrint()));

  exitAct = new QAction(QIcon(":/images/copy.png"), tr("&Sair"), this);
  exitAct->setShortcut(tr("Ctrl+Q"));
  exitAct->setStatusTip(tr("Sai do aplicativo"));
  connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

  // edit
  duplicAct = new QAction(QIcon(":/images/copy.png"), tr("&Duplicar"), this);
  duplicAct->setShortcut(tr("Ctrl+D"));
  duplicAct->setStatusTip(tr("Duplica a imagem da janela atual em uma nova janela"));
  connect(duplicAct, SIGNAL(triggered()), this, SLOT(editDuplic()));

  undoAct = new QAction(QIcon(":/images/open.png"), tr("&Desfazer"), this);
  undoAct->setShortcut(tr("Ctrl+Z"));
  undoAct->setStatusTip(tr("Duplica a imagem da janela atual em uma nova janela"));
  connect(undoAct, SIGNAL(triggered()), this, SLOT(editUndo()));

  copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copiar"), this);
  copyAct->setShortcut(tr("Ctrl+C"));
  copyAct->setStatusTip(tr("Copia a imagem da janela atual para área de trabalho"));
  connect(copyAct, SIGNAL(triggered()), this, SLOT(editCopy()));

  pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Colar"), this);
  pasteAct->setShortcut(tr("Ctrl+V"));
  pasteAct->setStatusTip(tr("Cria uma nova janela contendo a imagem da área de trabalho"));
  connect(pasteAct, SIGNAL(triggered()), this, SLOT(editPaste()));

  //shellAct = new QAction(QIcon(":/images/open.png"), tr("&Console"), this);
  //shellAct->setShortcut(tr("F8"));
  //shellAct->setCheckable( true );
  //shellAct->setChecked( false );
  //shellAct->setStatusTip(tr("Mostra ou esconede o console do sistema."));
  //connect(shellAct, SIGNAL(toggled(bool)), this, SLOT(editShell(bool)));

  // processing
  maskAct = new QAction(QIcon(":/images/open.png"), tr("Aplicar Máscara"), this);
  maskAct->setStatusTip(tr("Aplica uma máscara na imagem atual."));
  maskAct->setShortcut(tr("Ctrl+M"));
  connect(maskAct, SIGNAL(triggered()), this, SLOT(procMask()));

  grayscaleAct = new QAction(QIcon(":/images/paste.png"), tr("&Escala de cinza..."), this);
  //grayscaleAct->setShortcut(tr("Ctrl+V"));
  grayscaleAct->setStatusTip(tr("Converte a imagem atual para escala de cinza"));
  connect(grayscaleAct, SIGNAL(triggered()), this, SLOT(procGrayScale()));

  filterNegativeAct = new QAction(QIcon(":/images/open.png"), tr("Negativo"), this);
  filterNegativeAct->setStatusTip(tr("Negativo da imagem atual."));
  filterNegativeAct->setShortcut(tr("Ctrl+N"));
  connect(filterNegativeAct, SIGNAL(triggered()), this, SLOT(procNegative()));

  thresholdAct = new QAction(QIcon(":/images/open.png"), tr("Binarização"), this);
  thresholdAct->setStatusTip(tr("Linearização da imagem atual."));
  thresholdAct->setShortcut(tr("Ctrl+B"));
  connect(thresholdAct, SIGNAL(triggered()), this, SLOT(procThreshold()));

  histogramAct = new QAction(QIcon(":/images/histogram.png"), tr("Histograma"), this);
  histogramAct->setStatusTip(tr("Histograma da imagem atual."));
  histogramAct->setShortcut(tr("Ctrl+H"));
  connect(histogramAct, SIGNAL(triggered()), this, SLOT(procHistogram()));

  // analisys
  distanceAct = new QAction(QIcon(":/images/open.png"), tr("&Distância"), this);
  distanceAct->setShortcut(tr("Ctrl+L"));
  distanceAct->setStatusTip(tr("Mede a distância entre dos pontos quaisquer de uma imagem"));
  connect(distanceAct, SIGNAL(triggered()), this, SLOT(analDistance()));

  areaAct = new QAction(QIcon(":/images/open.png"), tr("&Area"), this);
  areaAct->setShortcut(tr("Ctrl+A"));
  areaAct->setStatusTip(tr("Mede a área de um poligono dentro de uma imagem"));
  connect(areaAct, SIGNAL(triggered()), this, SLOT(analArea()));

  calibrationAct = new QAction(QIcon(":/images/open.png"), tr("&Calibração"), this);
  calibrationAct->setShortcut(tr("Ctrl+K"));
  calibrationAct->setStatusTip(tr("Calibra a escala do sistema através de uma distância conhecida"));
  connect(calibrationAct, SIGNAL(triggered()), this, SLOT(analCalibration()));

  tagAct = new QAction(QIcon(":/images/tag.png"), tr("&Etiqueta de Escala"), this);
  tagAct->setShortcut(tr("Ctrl+E"));
  tagAct->setCheckable( true );
  tagAct->setChecked( false );
  tagAct->setStatusTip(tr("Mostra na imagem uma etiqueta de escala"));
  connect(tagAct, SIGNAL(toggled(bool)), this, SLOT(analTag(bool)));
  
  tagColorAct = new QAction(QIcon(":/images/tag.png"), tr("&Cor Etiqueta..."), this);
  //tagColorAct->setShortcut(tr("Ctrl+E"));
  tagColorAct->setStatusTip(tr("Modifica a cor da etiqueta da imagem atual"));
  connect(tagColorAct, SIGNAL(triggered()), this, SLOT(analTagColor()));

  dilationAct = new QAction(QIcon(":/images/open.png"), tr("Dilatação"), this);
  dilationAct->setStatusTip(tr("Dilatação da imagem atual."));
  dilationAct->setShortcut(tr("Ctrl+Shift+D"));
  connect(dilationAct, SIGNAL(triggered()), this, SLOT(analDilation()));

  erosionAct = new QAction(QIcon(":/images/open.png"), tr("Erosão"), this);
  erosionAct->setStatusTip(tr("Erosão da imagem atual."));
  erosionAct->setShortcut(tr("Ctrl+Shift+E"));
  connect(erosionAct, SIGNAL(triggered()), this, SLOT(analErosion()));

  openingAct = new QAction(QIcon(":/images/open.png"), tr("Abertura"), this);
  openingAct->setStatusTip(tr("Abertura(opening) da imagem atual."));
  openingAct->setShortcut(tr("Ctrl+Shift+A"));
  connect(openingAct, SIGNAL(triggered()), this, SLOT(analOpening()));

  closingAct = new QAction(QIcon(":/images/open.png"), tr("Fechamento"), this);
  closingAct->setStatusTip(tr("Fechamento(closing) da imagem atual."));
  closingAct->setShortcut(tr("Ctrl+Shift+F"));
  connect(closingAct, SIGNAL(triggered()), this, SLOT(analClosing()));

  structAct = new QAction(QIcon(":/images/open.png"), tr("Estrutura..."), this);
  structAct->setStatusTip(tr("Definição da estrutura do elemento usada nas operações de morfologia(erosão, dilatação)."));
  connect(structAct, SIGNAL(triggered()), this, SLOT(analStruct()));

  boundAct = new QAction(QIcon(":/images/open.png"), tr("Borda"), this);
  boundAct->setStatusTip(tr("Extração da borda da imagem atual."));
  boundAct->setShortcut(tr("Ctrl+Shift+B"));
  connect(boundAct, SIGNAL(triggered()), this, SLOT(analBound()));

  fillholesAct = new QAction(QIcon(":/images/open.png"), tr("Preencher Buracos"), this);
  fillholesAct->setStatusTip(tr("Preenche os buracos da imagem atual. Esta precisa estar binarizada."));
  fillholesAct->setShortcut(tr("Ctrl+Shift+H"));
  connect(fillholesAct, SIGNAL(triggered()), this, SLOT(analFillHoles()));

  analisysAct = new QAction(QIcon(":/images/open.png"), tr("Analisar..."), this);
  analisysAct->setStatusTip(tr("Realiza uma análise dos objetos encontrados em uma imagem quanto a quatidade, área, entre outras. A imagem precisa estar binarizada."));
  //analisysAct->setShortcut(tr("Ctrl+Shift+L"));
  connect(analisysAct, SIGNAL(triggered()), this, SLOT(analAnalisys()));

#ifdef Q_WS_WIN
  //camera
  acquireAct = new QAction(QIcon(":/images/open.png"), tr("&Camera..."), this);
  acquireAct->setStatusTip( tr("Mostra a janela de captura de imagens"));
  connect(acquireAct, SIGNAL(triggered()), this, SLOT(camAcquire()));

  grabAct = new QAction(QIcon(":/images/open.png"), tr("&Capturar"), this);
  grabAct->setStatusTip( tr("Captura a imagem atual em uma nova janela para processamento"));
  connect(grabAct, SIGNAL(triggered()), this, SLOT(camGrab()));
#endif

  // windows
  closeAct = new QAction(tr("&Fechar"), this);
  closeAct->setShortcut(tr("Ctrl+F4"));
  closeAct->setStatusTip(tr("Fechar a janela ativa"));
  connect(closeAct, SIGNAL(triggered()), workspace, SLOT(closeActiveWindow()));

  closeAllAct = new QAction(tr("Fecha &Todas"), this);
  closeAllAct->setStatusTip(tr("Fecha todas janelas abertas"));
  connect(closeAllAct, SIGNAL(triggered()), workspace, SLOT(closeAllWindows()));

  tileAct = new QAction(tr("&Tile"), this);
  tileAct->setStatusTip(tr("Tile the windows"));
  connect(tileAct, SIGNAL(triggered()), workspace, SLOT(tile()));

  cascadeAct = new QAction(tr("&Cascade"), this);
  cascadeAct->setStatusTip(tr("Cascade the windows"));
  connect(cascadeAct, SIGNAL(triggered()), workspace, SLOT(cascade()));

  nextAct = new QAction(tr("&Próxima"), this);
  nextAct->setShortcut(tr("Ctrl+F6"));
  nextAct->setStatusTip(tr("Move the focus to the next window"));
  connect(nextAct, SIGNAL(triggered()),
          workspace, SLOT(activateNextWindow()));

  previousAct = new QAction(tr("&Anterior"), this);
  previousAct->setShortcut(tr("Ctrl+Shift+F6"));
  previousAct->setStatusTip(tr("Move the focus to the previous "
                               "window"));
  connect(previousAct, SIGNAL(triggered()),
          workspace, SLOT(activatePreviousWindow()));

  separatorAct = new QAction(this);
  separatorAct->setSeparator(true);


  // lentes/aumentos
  amplActGroup = new QActionGroup(this);
  connect(amplActGroup, SIGNAL(triggered(QAction *)), this, SLOT(amplValue(QAction *)));

  amp50Act = new QAction(QIcon(":/images/50x.png"), tr("50x"), amplActGroup);
  amp50Act->setCheckable(true);
  amp50Act->setChecked(true);

  amp100Act = new QAction(QIcon(":/images/100x.png"), tr("100x"), amplActGroup);
  amp100Act->setCheckable(true);

  amp200Act = new QAction(QIcon(":/images/200x.png"), tr("200x"), amplActGroup);
  amp200Act->setCheckable(true);

  amp500Act = new QAction(QIcon(":/images/500x.png"), tr("500x"), amplActGroup);
  amp500Act->setCheckable(true);

  amp1000Act = new QAction(QIcon(":/images/1000x.png"), tr("1000x"), amplActGroup);
  amp1000Act->setCheckable(true);


  // about
  aboutAct = new QAction(tr("&Sobre ImProLab..."), this);
  //aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAct = new QAction(tr("Sobre Qt..."), this);
  //aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MdiMain::createMenus()
{
  saveAsMenu = new QMenu(tr("&Salvar como..."), this);
  foreach (QAction *action, saveAsActs)
    saveAsMenu->addAction(action);

  fileMenu = menuBar()->addMenu(tr("Arquivo"));
  fileMenu->addAction(openAct);
  fileMenu->addMenu(saveAsMenu);
  fileMenu->addSeparator();
  fileMenu->addAction(printAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  editMenu = menuBar()->addMenu(tr("Editar"));
  connect(editMenu, SIGNAL(aboutToShow()), this, SLOT(updateEditMenu()));
  editMenu->addAction(duplicAct);
  editMenu->addAction(undoAct);
  editMenu->addAction(copyAct);
  editMenu->addAction(pasteAct);

  processMenu = menuBar()->addMenu(tr("Processamento"));
  processMenu->addAction(maskAct);
  processMenu->addAction(grayscaleAct);
  processMenu->addAction(filterNegativeAct);
  processMenu->addAction(thresholdAct);
  processMenu->addAction(histogramAct);

  analisysMenu = menuBar()->addMenu(tr("Análise"));
  connect(analisysMenu, SIGNAL(aboutToShow()), this, SLOT(updateAnaliseMenu()));
  analisysMenu->addAction(distanceAct);
  analisysMenu->addAction(areaAct);
  analisysMenu->addAction(calibrationAct);
  analisysMenu->addAction(tagAct);
  analisysMenu->addAction(tagColorAct);  
  analisysMenu->addSeparator();
  analisysMenu->addAction(dilationAct);
  analisysMenu->addAction(erosionAct);
  analisysMenu->addAction(openingAct);
  analisysMenu->addAction(closingAct);
  analisysMenu->addAction(structAct);
  analisysMenu->addSeparator();
  analisysMenu->addAction(boundAct);
  analisysMenu->addSeparator();
  analisysMenu->addAction(fillholesAct);
  analisysMenu->addSeparator();
  analisysMenu->addAction(analisysAct);

  windowMenu = menuBar()->addMenu(tr("Janelas"));
  windowMenu->addAction(closeAct);
  windowMenu->addAction(closeAllAct);
  windowMenu->addSeparator();
  windowMenu->addAction(tileAct);
  windowMenu->addAction(cascadeAct);
  windowMenu->addSeparator();
  windowMenu->addAction(nextAct);
  windowMenu->addAction(previousAct);

#ifdef Q_WS_WIN
  cameraMenu = menuBar()->addMenu(tr("Camera"));
  cameraMenu->addAction(acquireAct);
  cameraMenu->addAction(grabAct);
#endif

  helpMenu = menuBar()->addMenu(tr("Ajuda"));
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
}

void MdiMain::createToolBars()
{
  fileToolBar = addToolBar(tr("File"));
  //fileToolBar->addAction(newAct);
  fileToolBar->addAction(openAct);
  //fileToolBar->addMenu(saveAsMenu);
  //fileToolBar->addAction(saveAct);

  //editToolBar = addToolBar(tr("Edit"));
  //editToolBar->addAction(cutAct);
  //editToolBar->addAction(copyAct);
  //editToolBar->addAction(pasteAct);

  amplifToolBar = addToolBar(tr("Amplif"));
  amplifToolBar->addActions(amplActGroup->actions());
}

void MdiMain::createStatusBar()
{
  if( QAction *currAct = amplActGroup->checkedAction() )
  {
    QSettings settings("PUCRS", "ImProLab");
    QString sAmp = currAct->text();

    statusBar()->showMessage(tr("Aumento de %1. Escala dos pixels: %2 [%3]")
                            .arg(sAmp).arg(getPixelScale()).arg(getUnitScale()));
  }
}

void MdiMain::readSettings()
{
  QSettings settings("PUCRS", "ImProLab");

  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(400, 400)).toSize();

  move(pos);
  resize(size);

  //pixelScale = settings.value("pixelScale", 1.0).toDouble();
  //unitScale = settings.value("unitScale", "pixels" ).toString();

  //mask - default 8-C
  mask.bitArray( settings.value("mask", QBitArray(9,true) ).toBitArray() );
}

void MdiMain::writeSettings()
{
  QSettings settings("PUCRS", "ImProLab");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
  //settings.setValue("pixelScale", pixelScale);
  //settings.setValue("unitScale", unitScale);
  settings.setValue("mask", mask.bitArray() );
}

MdiChild *MdiMain::activeMdiChild()
{
  return dynamic_cast<MdiChild *>(workspace->activeWindow());
}
