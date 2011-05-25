/*
  Children windows declarations.
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
#include <math.h>
#include "mdichild.h"
#include "mainwindow.h"

/******************
* MdiChild
******************/

/*!
* Constructs a new MdiImage and defines the widget layout.
* \param Parent   Parent widget
*/
MdiChild::MdiChild( QWidget *Parent )
{
  if( Parent )
    parent = Parent;

  setAttribute(Qt::WA_DeleteOnClose);
	//setAttribute(Qt::WA_PaintOnScreen);  //disable double-buffering

  setWindowIcon( QIcon(":/images/microscope.png") );

  setMouseTracking(false);
}

MdiImage::MdiImage(QWidget *Parent):MdiChild(Parent)
{
  QVBoxLayout *mainLayout = new QVBoxLayout;

  //label
  label = new QLabel(this);
  label->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
  label->setText( tr(" \n ") );

  mainLayout->setMargin(0);
  mainLayout->addWidget(label,1,Qt::AlignBottom);

  setLayout(mainLayout);

  bdist = false;
  barea = false;
  btag  = false;
  bcalib = false;
  tagShow = false;
  tagValue = 150.0;
  tagX = tagY = 0u;
}

/*!
* Loads an image from a file.
*
* \param fileName   Image's file name
* \return bool    True if the image was sucessful loaded
*/
bool MdiImage::loadFromFile(QString fileName)
{
  QImage image(fileName);

  if( !image.isNull() )
  {
    /*      QMessageBox::information(NULL, "ImProLab", tr("Formato do arquivo:"));
      QStringList list = image.textKeys();
      QStringList::Iterator it = list.begin();
      while( it != list.end() ) {
        QMessageBox::information(NULL, "ImProLab", tr("Texto: %1").arg(image.textKeys) );
        ++it;
	}*/

    setWindowTitle( fileName + tr("  [%1x%2 pixels]")
                    .arg(image.size().width())
                    .arg(image.size().height()) );
    resize( image.size().width(), image.size().height() + label->height() );
    images.push_back( image.copy() );
    return true;
  }
  return false;
}

/*!
* Get the last image front the stack.
*
* \return QImage    Last pushed image
*/
QImage MdiImage::getImage()
{ 
  return images.last(); 
}

/*!
* Save the last image in a file with/without scale tag.
*
* \param FileName   Image's file name
* \param FileFormat Image's format
* \return void
*/
void MdiImage::save( QString FileName, QByteArray FileFormat )
{
  QPixmap buff( images.last().size() );
  
  {
    
    QPainter painter; 
    painter.begin( &buff );

    QImage image = images.last();

    painter.drawImage(0, 0, image);
    
    if( tagShow )
      drawTag( painter );
    
    painter.end();
  }
  
  buff.save( FileName, FileFormat );
}

/*!
* Push a new image into the stack.
*
* \param Image    New image
* \param push     False to only overwrite the last image in the stack
* \return void
*/
void MdiImage::setImage(QImage Image, bool push )
{
  if( push )
    images.push_back( Image.copy() );
  else
    images.last() = Image;

  resize( Image.size().width(), Image.size().height() +
    ( label ? label->height() : 0 ) );
  update();
}


/*!
* Initiates the distance messurement process.
*
* \return void
*/
void MdiImage::distance()
{
  bdist = true;
  points.clear();
  setMouseTracking(true);
  update();
  label->setText( tr("Pressione o botão esquerdo do mouse sobre a imagem para traçar as retas."
                     "\nCom o botão direito finalize a medida.") );
}


/*!
* Initiates the area messurement process.
*
* \return
*/
void MdiImage::area()
{
  barea = true;
  points.clear();
  setMouseTracking(true);
  update();
  label->setText( tr("Pressione o botão esquerdo do mouse sobre a imagem para traçar as retas."
                     "\nCom o botão direito finalize a medida.") );
}


/*!
* Initiates the calibration process.
*
* \return
*/
void MdiImage::calibration()
{
  bcalib = true;
  points.clear();
  setMouseTracking(true);
  update();
  label->setText( tr("Trace uma reta sobre a imagem onde a distância seja conhecida.\n ") );
}


/*!
* Initiates the tag position process.
*
* \return
*/
void MdiImage::tag()
{
  btag = true;
  points.clear();
  setMouseTracking(true);
  update();
  label->setText( tr("Pressione o botão direito para finalizar o processo.") );
}


/*!
* Sets the show scale flag.
*
* \param showTag    True to show the image scale
* \return
*/
void MdiImage::showTag(bool showTag)
{
  tagShow = showTag;
  
  if( tagShow )
    tag();
  
  update();
}


/*!
* Gets the show scale flag.
*
* \return bool    Returns the show scale flag
*/
bool MdiImage::showTag() 
{ 
  return tagShow; 
}


/*!
* Returns the value that is shown in the scale label.
*
* \return double    Value of the scale label
*/
double MdiImage::getTagValue() 
{ 
  return tagValue; 
}


/*!
* Set the scale label value.
*
* \param TagValue   New scale label value
* \return void
*/
void MdiImage::setTagValue( double TagValue )
{ 
  tagValue = TagValue; 
}


/*!
* Sets the scale tag's color.
*/
void MdiImage::setTagColor()
{
  QColor newColor = QColorDialog::getColor( tagColor );
  
  if (newColor.isValid())
  {
    tagColor = newColor;
    update();
  }
}

/*!
* Paint event.
*/
void MdiImage::paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  QImage image = images.last();

  painter.setRenderHint(QPainter::Antialiasing);

  //int x = image.width() > width() ? 0 : (width() - image.width())/2;
  int x = 0;
  //int y = image.height() > height() ? 0 : (height() - image.height())/2;
  int y = 0;
  painter.drawImage(x, y, image);

  if( bdist || barea || bcalib )
  {
    painter.setPen( QPen(Qt::blue, 2) );
    for( int i=0; i < points.size()-1; ++i)
      painter.drawLine ( points[i], points[i+1] );
    if( points.size() )
      painter.drawLine ( points.last(), currPos );
  }
  
  if( tagShow )
    drawTag( painter );
}


/*!
* Draw the scale tag in a QPainter.
*/
void MdiImage::drawTag( QPainter &Painter )
{
  MdiMain* main = dynamic_cast<MdiMain*>(parent);
  //if( !main ) return;
  
  QString unit = main->getUnitScale();
  double pixelScale = main->getPixelScale();
  double tagVal = tagValue/pixelScale;

  //int x = (int)( (double)getImage().width()/2.);
  int x;
  int y;
  if( btag )
  {
    x = tagX = currPos.x();
    y = tagY = currPos.y();
  }
  else
  {
    x = tagX;
    y = tagY;
  }
  
  int x0 = (int)( x - tagVal/2. );
  int x1 = (int)( x + tagVal/2. );
  
  int y0 = int( (double)y*0.99 );
  int y1 = int( (double)y*1.01 );

  Painter.setPen( QPen(tagColor,4) );
  Painter.drawLine( x0, y0, x0, y1);
  Painter.drawLine( x0, y, x1, y);
  Painter.drawLine( x1, y0, x1, y1);

  QString text = tr("%1 [%2]").arg(tagValue).arg(unit);

  QFont font( Painter.font() );
  font.setPixelSize ( 14 );
  font.setBold( true );
  Painter.setFont(font);
  //painter.drawText( (x1+x0)/2, (int)(y*1.04), tr("%1 [%2]").arg(tagValue).arg(unit) );

  int w = 200;
  int h = 15;

  int xt = (x1+x0)/2 - w/2;

  Painter.drawText( xt, y, w, h, Qt::AlignCenter, text );
}


/*!
* Pushes a copy of the current image into stack.
*
* \return void
*/
void MdiImage::push() 
{ 
  setImage( getImage() ); 
}


/*!
* Removes the last image front the stack.
*
* \return void
*/
void MdiImage::undo()
{
  if( images.size() > 1 )
  {
    images.pop_back();
    update();
  }
  else
  {
    QMessageBox::information(this, "ImProLab",
    "Não há alterações a seram desfeitas.");
  }
}


/*!
* Number of images in the stack.
*
* \return   Returns the stack size
*/
int MdiImage::imagesSize() 
{ 
  return (int) images.size(); 
}


/*!
* Mouse tracking.
*/
void MdiImage::mouseMoveEvent ( QMouseEvent * e )
{
  currPos = e->pos();
  update();
}


/*!
* Mouse press event.
*/
void MdiImage::mousePressEvent ( QMouseEvent * e )
{
  if( e->button() == Qt::LeftButton )
  {
    points.push_back( e->pos() );
    update();
  }
  else if( e->button() == Qt::RightButton )
  {
    setMouseTracking(false);
    update();

    MdiMain* main = dynamic_cast<MdiMain*>(parent);
    if( !main ) return;

    QString unit = main->getUnitScale();
    double pixelScale = main->getPixelScale();

    if( barea )
    {
      barea = false;
      label->setText( tr("Pontos: %1   Area: %2 [%3^2]\n ").
                    arg(points.size()).arg(calcArea()).arg(unit) );
    }

    if( bdist )
    {
      bdist = false;
      label->setText( tr("Pontos: %1   Distância: %2 [%3]\n ").
                    arg(points.size()).arg(calcDist()).arg(unit) );
    }

    if( bcalib )
    {
      bcalib = false;

      double dist = calcDist(false);

      bool ok;
      double d = QInputDialog::getDouble(this, tr("Calibração"),
                 tr("%1 pixels equivale a...").arg(dist),
                 dist, -1000000, 1000000, 2, &ok);

      if (ok)
      {
        //define a escala de pixels para amplificação atual
        pixelScale = d/dist;
        main->setPixelScale( pixelScale );

        unit = QInputDialog::getText(this, tr("Calibração: unidade"),
                tr("Nova unidade..."), QLineEdit::Normal, unit, &ok);

        if (ok && !unit.isEmpty())
        {
          main->setUnitScale(unit);
        }

        QMessageBox::information(this, "ImProLab",
        tr("Novo fator de scala: %1 [%2/pixels]").arg(pixelScale).arg(unit) );
      }

      label->setText( tr(" \n ") );
    }
    
    if( btag )
    {
      btag = false;
      label->setText( tr("Posição: X = %1   Y = %2").arg( tagX ).arg( tagY ) );
    }
  }
}

double MdiImage::calcDist(bool Scaled)
{
  MdiMain* main = dynamic_cast<MdiMain*>(parent);
  if( !main ) return 0;

  double pixelScale = main->getPixelScale();

  double res = 0.0;
  for( int i=0; i < points.size()-1; ++i)
  {
    int x0 = points[i].x();
    int y0 = points[i].y();
    int x1 = points[i+1].x();
    int y1 = points[i+1].y();

    res += sqrt( pow( x0-x1, 2 ) + pow( y0-y1, 2 ) );
  }
  return (Scaled ? res*pixelScale : res );
}

double MdiImage::calcArea()
{
  MdiMain* main = dynamic_cast<MdiMain*>(parent);
  if( !main ) return 0;

  double pixelScale = main->getPixelScale();

  int n = points.size();
  if (n < 3) return 0.0;

  double dArea = 0.0;
  for (int i=0; i<n; ++i)
  {
      int j = i + 1;
      if (j == n) j = 0;
      dArea += points[i].x() * points[j].y() - points[i].y() * points[j].x();
  }

  dArea /= 2;
  if (dArea < 0) dArea *= -1;     // area may be negative
  return dArea*(pixelScale*pixelScale);
}

/******************
* MdiThreshold
******************/

MdiThreshold::MdiThreshold(QWidget *Parent):MdiChild(Parent)
{
  setWindowTitle(tr("Thresholding"));

  //histogram
  hist = new MdiHistogram;

  //slider
  slider = new QSlider;
  slider->setOrientation( Qt::Horizontal );
  connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));

  //ok button
  btnOk = new QPushButton(tr("&OK"));
  btnOk->setDefault( true );
  btnOk->setAutoDefault( true );
  connect(btnOk, SIGNAL(clicked()), this, SLOT(close()));

  //layout
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->setMargin(0);
  mainLayout->addWidget(hist);
  mainLayout->addWidget(slider);
  mainLayout->addWidget(btnOk);

  setLayout(mainLayout);

  resize(400,200);
}


/*!
* Sets the MdiImage that will be modify when the slider chenges.
*/
void MdiThreshold::setMdiImage(MdiImage *MdiImage)
{
  mdiImage = MdiImage;
  mdiImage->push();

  image = mdiImage->getImage();
  buffer = mdiImage->getImage().copy(QRect());

  hist->setImage( mdiImage->getImage() );

  slider->setMinimum(-1);
  slider->setMaximum( image.numColors() );
  slider->setSliderPosition( image.numColors()/2 );
}

void MdiThreshold::setValue(int value)
{
  setWindowTitle(tr("Slider value: %1").arg(value));
  threshold(value);
}

void MdiThreshold::threshold(int value)
{
  uchar *line;
  uchar *line2;

  int highest = buffer.numColors()-1;

  unsigned nblack = 0, nwhite = 0;

  for( int i=0; i < buffer.height(); ++i)
  {
    for( int j=0; j < buffer.width(); ++j)
    {
      line = buffer.scanLine(i);
      int gray = line[j] ;

      line2 = image.scanLine(i);

      if( gray > value )
      {
        line2[j] = highest;
        ++nwhite;
      }
      else
      {
        line2[j] = 0;
        ++nblack;
      }
    }
  }

  setWindowTitle( windowTitle() + tr("  Claros: %1\%   Escuros: %2\%")
                .arg(100.0*(double)nwhite/(double)(nwhite+nblack))
                .arg(100.0*(double)nblack/(double)(nwhite+nblack)) );

  mdiImage->setImage( image, false );
}

void MdiThreshold::closeEvent( QCloseEvent * )
{
  QPushButton *btn = dynamic_cast<QPushButton*>( sender() );

  if( btn )
  {
    //mdiImage->setImage( image.convertToFormat(QImage::Format_Mono), false );
  }
  else
    mdiImage->undo();
}

/******************
* MdiHistogram
******************/
MdiHistogram::MdiHistogram(QWidget *Parent):MdiChild(Parent)
{
  setWindowTitle(tr("Histograma"));
  resize(400,200);
}

/*!
* Set the values that will diplay.
*/
void MdiHistogram::setValues( QVector<int> Values )
{
  values = Values;
  max = 0;
  for( int i=0; i < values.size(); ++i)
    max = values[i] > max ? values[i] : max;
}

void MdiHistogram::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.setPen( QPen(Qt::red, 1) );

  painter.setRenderHint(QPainter::Antialiasing);

  int w = this->width();
  int h = this->height();
  int s = values.size();

  for( int i=0; i < s ; ++i)
  {
    int x = w * i/s;
    int y0 = h;
    int y1 = (int)( (double)h - (double)values[i]/(double)max*(double)h*0.9 );
    painter.drawLine( QLine( x, y1, x, y0)  );
  }
}

    /*!
    * Calculate the values using the Image parameter.
    */
void MdiHistogram::setImage( QImage Image )
{
  ImageEdit ie(Image);
  setValues( ie.histogram() );
}

/******************
* ImageEdit
******************/
QImage ImageEdit::convolution(Mask &Mask, unsigned Row, unsigned Col, int Width, int Height)
{
  QImage buffer( image.copy() );

  if( Width = -1 ) Width = image.width();
  if( Height= -1 ) Height = image.height();

  unsigned colOffset = Mask.colCount()/2;
  unsigned rowOffset = Mask.rowCount()/2;

  double sum;

  unsigned maxsampleval = image.numColors()-1;
  unsigned minsampleval = 0;

  //compensa o efeito da borda
  Col += colOffset;
  Row += rowOffset;

  Width -= Mask.colCount() - 1;
  Height -= Mask.rowCount() - 1;

  unsigned colEnd = Col + Width;
  unsigned rowEnd = Row + Height;

  for( unsigned imageRow = Row; imageRow < rowEnd; ++imageRow)
  {
    unsigned tempRow = imageRow - rowOffset;
    uchar *tempLine = buffer.scanLine(imageRow);

    for( unsigned imageCol = Col; imageCol < colEnd; ++imageCol)
    {
      unsigned tempCol = imageCol - colOffset;
      sum = 0.0;

      for( unsigned kernelRow = 0; kernelRow < Mask.rowCount(); ++kernelRow)
      {
        uchar *line = image.scanLine(tempRow+kernelRow);
        for( unsigned kernelCol = 0; kernelCol < Mask.colCount(); ++kernelCol)
        {
          unsigned gray = line[tempCol+kernelCol];
          double kernelCoef = Mask(kernelRow, kernelCol);

          sum += gray * kernelCoef;
        }
      }

      sum = (sum < minsampleval ) ? minsampleval : (int)sum;
      sum = (sum > maxsampleval ) ? maxsampleval : (int)sum;

      tempLine[imageCol] = (unsigned)sum;
    }
  }
  image = buffer;
  return image;
}

QImage ImageEdit::negative()
{
  QImage buffer( image.copy() );

  uchar *line;

  for( int i=0; i < buffer.height(); ++i)
  {
    for( int j=0; j < buffer.width(); ++j)
    {
      line = buffer.scanLine(i);
      line[j] = 255 - line[j];
    }
  }
  image = buffer;
  return image;
}

QVector<int> ImageEdit::histogram()
{
  QVector<int> values(256,0);
  uchar *line;

  for( int i=0; i < image.height(); ++i)
  {
    for( int j=0; j < image.width(); ++j)
    {
      line = image.scanLine(i);
      int gray = line[j];
      values[gray] = values[gray] + 1;
    }
  }

  return values;
}

QImage ImageEdit::dilation(Mask &Mask, unsigned Row, unsigned Col, int Width, int Height)
{
  QImage buffer( image.copy() );

  if( Width = -1 ) Width = image.width();
  if( Height= -1 ) Height = image.height();

  unsigned colOffset = Mask.colCount()/2;
  unsigned rowOffset = Mask.rowCount()/2;

  //compensa o efeito da borda
  Col += colOffset;
  Row += rowOffset;

  Width -= Mask.colCount() - 1;
  Height -= Mask.rowCount() - 1;

  unsigned colEnd = Col + Width;
  unsigned rowEnd = Row + Height;

  for( unsigned imageRow = Row; imageRow < rowEnd; ++imageRow)
  {
    unsigned tempRow = imageRow - rowOffset;
    uchar *imgLine = image.scanLine(imageRow);

    for( unsigned imageCol = Col; imageCol < colEnd; ++imageCol)
    {
      unsigned tempCol = imageCol - colOffset;

      if( imgLine[imageCol] < 255 )
      {
        for( unsigned kernelRow = 0; kernelRow <  Mask.rowCount(); ++kernelRow)
        {
          uchar *line = image.scanLine(tempRow+kernelRow);
          uchar *outLine = buffer.scanLine(tempRow+kernelRow);

          for( unsigned kernelCol = 0; kernelCol <  Mask.colCount(); ++kernelCol)
          {
            int gray = line[tempCol+kernelCol];
            bool kernelCoef = (bool)Mask(kernelRow, kernelCol);

            if( kernelCoef )
              outLine[tempCol+kernelCol] = gray ? 0 : gray;
          }
        }
      }
    }
  }

  image = buffer;
  return image;
}


/*!
* Erode every founded object in the image, using the kernel mask pass by parameter.
*/
QImage ImageEdit::erosion(Mask &Mask, unsigned Row, unsigned Col, int Width, int Height)
{
  QImage buffer( image.copy() );

  if( Width = -1 ) Width = image.width();
  if( Height= -1 ) Height = image.height();

  unsigned colOffset = Mask.colCount()/2;
  unsigned rowOffset = Mask.rowCount()/2;

  //compensa o efeito da borda
  Col += colOffset;
  Row += rowOffset;

  Width -= Mask.colCount() - 1;
  Height -= Mask.rowCount() - 1;

  unsigned colEnd = Col + Width;
  unsigned rowEnd = Row + Height;

  for( unsigned imageRow = Row; imageRow < rowEnd; ++imageRow)
  {
    unsigned tempRow = imageRow - rowOffset;
    uchar *imgLine = image.scanLine(imageRow);
    uchar *tempLine = buffer.scanLine(imageRow);

    for( unsigned imageCol = Col; imageCol < colEnd; ++imageCol)
    {
      unsigned tempCol = imageCol - colOffset;

      bool hascon = false;
      if( imgLine[imageCol] < 255 )
      {
        for( unsigned kernelRow = 0; kernelRow <  Mask.rowCount() && !hascon; ++kernelRow)
        {
          uchar *line = image.scanLine(tempRow+kernelRow);

          for( unsigned kernelCol = 0; kernelCol <  Mask.colCount() && !hascon; ++kernelCol)
          {
            int gray = line[tempCol+kernelCol];
            bool kernelCoef = (bool)Mask(kernelRow, kernelCol);

            hascon = gray == 255 && kernelCoef;
          }
        }
      }

      if( hascon )
        tempLine[imageCol] = 255;
    }
  }

  image = buffer;
  return image;
}

QImage ImageEdit::subtration(QImage ImgSub, unsigned RowOffset, unsigned ColOffset)
{
  QImage buffer( image.copy() );

  int width = image.width() < ImgSub.width() ? image.width() : ImgSub.width();
  int height = image.height() < ImgSub.height() ? image.height() : ImgSub.height();

  for( int row = RowOffset; row < height; ++row)
  {
    uchar *imgLine = image.scanLine(row);
    uchar *subLine = ImgSub.scanLine(row);
    uchar *bufLine = buffer.scanLine(row);

    for( int col = ColOffset;  col < width; ++col)
    {
      int gray = imgLine[col] - subLine[col];

      gray = (gray < 0 ) ? 0 : gray;
      gray = (gray > 255 ) ? 255 : gray;

      bufLine[col] = gray;
    }
  }

  image = buffer;
  negative();
  return image;
}


/*!
* Fill all open holes, except the ones who has some conection with the bundarray of
* the image.
*
*  - First:
*      Convert the image to gray and white insted of black and white.
*  - Second:
*      Make the complement of the result: gray -> white; white -> gray.
*  - Third:
*      Seed image: sets all bounds(bottom, top, left, right) to 0 (black).
*  - Fourth:
*      Propagate the seed image to all diretions: top to bottom,...
*/
QImage ImageEdit::fillholes(unsigned Row, unsigned Col, int Width, int Height)
{
  QImage buffer( image.copy() );

  if( Width = -1 ) Width = image.width();
  if( Height= -1 ) Height = image.height();

  //to gray...
  for( int row = Row; row < Height; ++row)
  {
    uchar *bufLine = buffer.scanLine(row);

    for( int col = Col;  col < Width; ++col)
    {
      bufLine[col] = bufLine[col] == 0 ? 128 : 255;
    }
  }

  //complement....
  uchar *line;
  for( int i=0; i < buffer.height(); ++i)
  {
    for( int j=0; j < buffer.width(); ++j)
    {
      line = buffer.scanLine(i);
      if( line[j] == 128 )
        line[j] = 255;
      else if( line[j] == 255 )
        line[j] = 128;
    }
  }

  //seed image...
  for( int i=0; i < buffer.height(); ++i)
  {
    for( int j=0; j < buffer.width(); j+=buffer.width()-1 )
    {
      line = buffer.scanLine(i);
      line[j] = 0;

      if( i == 0 || i == buffer.height()-1 )
        for( int jj=0; jj < buffer.width(); ++jj )
          line[jj] = 0;
    }
  }

  //propagation of the seed...
  Mask mask(3,1); //N4
  mask(0,0) = 0;
  mask(0,2) = 0;
  mask(2,0) = 0;
  mask(2,2) = 0;


  unsigned colOffset = mask.colCount()/2;
  unsigned rowOffset = mask.rowCount()/2;

  Col += colOffset;
  Row += rowOffset;

  Width -= mask.colCount() - 1;
  Height -= mask.rowCount() - 1;

  unsigned colEnd = Col + Width;
  unsigned rowEnd = Row + Height;

  //top to bottom
  for( unsigned row = Row; row < rowEnd; ++row)
  {
    unsigned tempRow = row - rowOffset;
    uchar *buffLine = buffer.scanLine(row);

    for( unsigned col = Col; col < colEnd; ++col )
    {
      unsigned tempCol = col - colOffset;

      bool hascon = false;
      if( buffLine[col] == 128 )
      {
        for( unsigned kernelRow = 0; kernelRow <  mask.rowCount() && !hascon; ++kernelRow)
        {
          uchar *line = buffer.scanLine(tempRow+kernelRow);

          for( unsigned kernelCol = 0; kernelCol <  mask.colCount() && !hascon; ++kernelCol)
          {
            int gray = line[tempCol+kernelCol];
            bool kernelCoef = (bool)mask(kernelRow, kernelCol);

            hascon = (gray == 0) && kernelCoef;
          }
        }
      }

      if( hascon )
        buffLine[col] = 0;
    }
  }

  //bottom to top
  for( unsigned row = rowEnd-1; row >= rowOffset; --row)
  {
    unsigned tempRow = row - rowOffset;
    uchar *buffLine = buffer.scanLine(row);

    for( unsigned col = Col; col < colEnd; ++col )
    {
      unsigned tempCol = col - colOffset;

      bool hascon = false;
      if( buffLine[col] == 128 )
      {
        for( unsigned kernelRow = 0; kernelRow <  mask.rowCount() && !hascon; ++kernelRow)
        {
          uchar *line = buffer.scanLine(tempRow+kernelRow);

          for( unsigned kernelCol = 0; kernelCol <  mask.colCount() && !hascon; ++kernelCol)
          {
            int gray = line[tempCol+kernelCol];
            bool kernelCoef = (bool)mask(kernelRow, kernelCol);

            hascon = (gray == 0) && kernelCoef;
          }
        }
      }

      if( hascon )
        buffLine[col] = 0;
    }
  }


  //missing left to right and rigth to left!!!!


  for( unsigned row = Row; row < rowEnd; ++row)
  {
    uchar *bufLine = buffer.scanLine(row);

    for( unsigned col = Col;  col < colEnd; ++col)
    {
      if( bufLine[col] == 0 )
        bufLine[col] = 255;
      else if( bufLine[col] == 255 )
        bufLine[col] = 0;
      else
        bufLine[col] = 0;
    }
  }

  //fix border
  for( int i=0; i < buffer.height(); ++i)
  {
    for( int j=0; j < buffer.width(); j+=buffer.width()-1 )
    {
      buffer.scanLine(i)[j] = image.scanLine(i)[j];

      if( i == 0 || i == buffer.height()-1 )
        for( int jj=0; jj < buffer.width(); ++jj )
          buffer.scanLine(i)[jj] = image.scanLine(i)[jj];
    }
  }

  image = buffer;
  return image;
}

/*!
* Label all objects in the image, identifying and classifying then.
*/
QVector<ImgData> ImageEdit::labeling()
{
  QImage imgLabel( image.copy() );
  //imgLabel = imgLabel.convertToFormat( QImage::Format_RGB32 );

	int intLabel = 255;

  uint label1 = 0;
  uint label2 = 0;

  int iter = 0;

  for( bool again=true, swap=false; again; ++iter)
  {
    again = false;

    for( int imageRow = 0; imageRow < imgLabel.height() && !again; ++imageRow)
    {
      uchar *labLine = imgLabel.scanLine(imageRow);

      for( int imageCol = 0; imageCol < imgLabel.width() && !again; ++imageCol)
      {
        if( labLine[imageCol] == 255 )
        {
          if( imageRow > 0 )
          {
            uchar *labLineBefore = imgLabel.scanLine(imageRow-1);

            if( imageCol > 0 )
            {
              if( labLineBefore[imageCol] > 0 && labLine[imageCol-1] > 0 )
              {
                if( labLineBefore[imageCol] == labLine[imageCol-1] )
                  labLine[imageCol] = labLineBefore[imageCol];
                else
                {
                  if( labLineBefore[imageCol] < labLine[imageCol-1] )
                  {
                    labLine[imageCol] = labLineBefore[imageCol];
                    //equivalency table
                    label1 = labLineBefore[imageCol];
                    label2 = labLine[imageCol-1];
                    again = true;
                  }
                  else
                  {
                    labLine[imageCol] = labLine[imageCol-1];
                    //equivalency table
                    label1 = labLine[imageCol-1];
                    label2 = labLineBefore[imageCol];
                    again = true;
                  }
                }
              }
              else if( labLineBefore[imageCol] > 0 )
                labLine[imageCol] = labLineBefore[imageCol];
              else if( labLine[imageCol-1] > 0 )
                labLine[imageCol] = labLine[imageCol-1];
              else
                labLine[imageCol] = --intLabel;
            }
            else
            {
              if( labLineBefore[imageCol] > 0 )
                labLine[imageCol] = labLineBefore[imageCol];
              else
                labLine[imageCol] = --intLabel;
            }
          }
          else
          {
            if( imageCol > 0 )
            {
              if( labLine[imageCol-1] > 0 )
                labLine[imageCol] = labLine[imageCol-1];
            }
            else
            {
              labLine[imageCol] = --intLabel;
            }
          }
        }
        else if( swap && labLine[imageCol] == label2 )
        {
          labLine[imageCol] = label1;
        }
      }
    }
    swap = again;
  }

  QMessageBox::information(NULL, "ImProLab", tr("Número de iterações: %1").arg(iter) );

  //analisys
  typedef QPair< int, int > point;
  typedef QVector<point> points;
  QMap< uint, points > mapObj;
  QMap< uint, points >::iterator itObj;

  for( int imageRow = 0; imageRow < imgLabel.height(); ++imageRow)
  {
    uchar *labLine = imgLabel.scanLine(imageRow);

    for( int imageCol = 0; imageCol < imgLabel.width(); ++imageCol)
    {
      if( labLine[imageCol] > 0 )
      {
        itObj = mapObj.find( labLine[imageCol] );
        if( itObj == mapObj.end() )
          mapObj.insert( labLine[imageCol], points(1, point(imageRow, imageCol) ) );
        else
          itObj.value().append( point(imageRow, imageCol) );
      }
    }
  }

  QVector< ImgData > data;

  int id = 0;
  itObj = mapObj.begin();
  while( itObj != mapObj.end() )
  {
    ++id;
    double acc_x = 0.0, acc_y = 0.0;

    for( int i=0; i < itObj.value().size(); ++i)
    {
      acc_x += (double)itObj.value()[i].second;
      acc_y += (double)itObj.value()[i].first;
    }

    double x_mean = ( acc_x / (double)itObj.value().size() );
    double y_mean = ( acc_y / (double)itObj.value().size() );

    ImgData imgdata;

    imgdata.id = id;
    imgdata.x = (int)x_mean;
    imgdata.y = (int)y_mean;
    imgdata.size = itObj.value().size();
    imgdata.color = itObj.key();

    data.append( imgdata );
    ++itObj;
  }

  image = imgLabel;
  return data;
}

AnalisysPaintArea::AnalisysPaintArea( QVector<ImgData> Data, QImage ImageData )
{
  data = Data;
  imgData = ImageData;
}

void AnalisysPaintArea::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);

  painter.drawImage( 0, 0, imgData );

  QFont font( painter.font() );
  font.setPixelSize ( 10 );
  font.setBold( true );
  painter.setFont(font);

  int w = 100;
  int h = 20;

  for( int i=0; i < data.size(); ++i )
  {
    painter.setPen( QPen(Qt::black, 4) );
    QString text = tr("%1").arg( data[i].id );
    painter.drawText( data[i].x - w/2, data[i].y-h/2, w, h, Qt::AlignCenter, text );
  }
}


MdiAnalisys::MdiAnalisys(QWidget *Parent, QVector<ImgData> Data, QImage ImageData):MdiChild(Parent)
{
  setWindowTitle(tr("Análise"));

  //frame
  paintArea = new AnalisysPaintArea(Data,ImageData);

  //table
  tabWidget = new QTableWidget;
  tabWidget->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed );

  //layout
  /*QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(paintArea, 0, 0);
  mainLayout->addWidget(tabWidget, 1, 0, -1, -1, Qt::AlignBottom);*/

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(paintArea);
  mainLayout->addWidget(tabWidget);
  setLayout(mainLayout);

  resize( ImageData.width(), ImageData.height()+100 );

  tabWidget->setRowCount( Data.size()+1 );
  tabWidget->setColumnCount( 4 );

  MdiMain* main = dynamic_cast<MdiMain*>(parent);
  if( !main ) return;

  QString unit = main->getUnitScale();
  double pixelScale = main->getPixelScale();

  QStringList labels;
  labels << tr("Identificação") << tr("Area [%1^2]").arg(unit) <<
            tr("x [pixel]") << tr("y [pixel]");
  tabWidget->setHorizontalHeaderLabels( labels );

  double acc = 0.0;

  for( int i=0; i < Data.size(); ++i )
  {
    QTableWidgetItem *newItem = new QTableWidgetItem( tr("%1").arg(Data[i].id) );
    tabWidget->setItem(i, 0, newItem);

    newItem = new QTableWidgetItem( tr("%1").arg(Data[i].size*pixelScale*pixelScale) );
    tabWidget->setItem(i, 1, newItem);
    acc += Data[i].size;

    newItem = new QTableWidgetItem(tr("%1").arg(Data[i].x) );
    tabWidget->setItem(i, 2, newItem);

    newItem = new QTableWidgetItem(tr("%1").arg(Data[i].y) );
    tabWidget->setItem(i, 3, newItem);
  }

  QTableWidgetItem *newItem = new QTableWidgetItem(tr("Média"));
  tabWidget->setItem(Data.size(), 0, newItem);

  double media = acc*pixelScale*pixelScale / (double)Data.size();
  newItem = new QTableWidgetItem(tr("%1").arg( media ) );
  tabWidget->setItem(Data.size(), 1, newItem);

  newItem = new QTableWidgetItem(tr("Desvio padrão"));
  tabWidget->setItem(Data.size(), 2, newItem);

  double sigma = 0;
  for(int i=0; i< Data.size(); i++)
  {
    sigma += pow((Data[i].size - media), 2);
  }
  sigma = sqrt(sigma/(double)Data.size());
  newItem = new QTableWidgetItem(tr("%1").arg( sigma ) );
  tabWidget->setItem(Data.size(), 3, newItem);


}

#ifdef Q_WS_WIN
MdiCamera::MdiCamera(QWidget *Parent):Camera()
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowIcon( QIcon(":/images/microscope.png") );
  setMouseTracking(false);
}
#endif
