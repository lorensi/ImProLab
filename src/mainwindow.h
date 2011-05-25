/*
  Main window of the application.
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define VERSION_STR "0.4"
#define VERSION_NUM 0.4

#include <QMainWindow>
#include <QPrinter>
#include "mdichild.h"
#include "mask.h"

class QAction;
class QMenu;
class QWorkspace;
class MdiChild;
class QSignalMapper;
class QActionGroup;

/*!
* \class MdiMain
* \brief The class MdiMain is the main window of the application.
*  It holds all child windows.
*/
class MdiMain : public QMainWindow
{
    Q_OBJECT

  public:
    MdiMain();
    void    setPixelScale (double PixelScale);
    double  getPixelScale ();
    void    setUnitScale  (QString UnitScale);
    QString getUnitScale  ();

    void    calib(double dist);

  protected:
    void closeEvent(QCloseEvent *event);

  private slots:
    void open();
    void save();
    void filePrint();
    void editCopy();
    void editPaste();
    void editUndo();
    void editDuplic();
    void procMask();
    void procGrayScale();
    void procNegative();
    void procThreshold();
    void procHistogram();
    void analDistance();
    void analArea();
    void analCalibration();
    void analTag(bool show);
    void analTagColor();
    void analDilation();
    void analErosion();
    void analOpening();
    void analClosing();
    void analStruct();
    void analBound();
    void analFillHoles();
    void analAnalisys();
    void amplValue(QAction *a);

#ifdef Q_WS_WIN
    void camAcquire();
    void camGrab();
#endif

    void about();
    void updateMenus();
    void updateEditMenu();
    void updateAnaliseMenu();
    MdiImage *createMdiImage();
    MdiImage *cloneActiveMdiImage();

  private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    MdiChild *activeMdiChild();
    MdiImage *getActiveImage(bool quiet = false);

    QWorkspace *workspace;
    QSignalMapper *windowMapper;

    QMenu *fileMenu;
    QToolBar *fileToolBar;
      QAction *openAct;
      QList<QAction *> saveAsActs;
      QMenu *saveAsMenu;
      QAction *printAct;
      QAction *exitAct;

    QMenu *editMenu;
      QAction *duplicAct;
      QAction *undoAct;
      QAction *copyAct;
      QAction *pasteAct;
      QAction *grayscaleAct;
      //QAction *shellAct;

    QMenu *processMenu;
      QAction *maskAct;
      QAction *filterNegativeAct;
      QAction *thresholdAct;
      QAction *histogramAct;

    QMenu *analisysMenu;
      QAction *distanceAct;
      QAction *areaAct;
      QAction *calibrationAct;
      QAction *tagAct;
      QAction *tagColorAct;
      QAction *dilationAct;
      QAction *erosionAct;
      QAction *openingAct;
      QAction *closingAct;
      QAction *structAct;
      QAction *boundAct;
      QAction *fillholesAct;
      QAction *analisysAct;

#ifdef Q_WS_WIN
      QMenu *cameraMenu;
      QAction *acquireAct;
      QAction *grabAct;
#endif

    QMenu *windowMenu;
      QAction *closeAct;
      QAction *closeAllAct;
      QAction *tileAct;
      QAction *cascadeAct;
      QAction *nextAct;
      QAction *previousAct;
      QAction *separatorAct;

    QMenu *helpMenu;
      QAction *aboutAct;
      QAction *aboutQtAct;

    QPrinter printer;

    QToolBar *amplifToolBar;
      QActionGroup *amplActGroup;
      QAction *amp1000Act;
      QAction *amp500Act;
      QAction *amp200Act;
      QAction *amp100Act;
      QAction *amp50Act;

#ifdef Q_WS_WIN
    MdiCamera *mdiCam;
#endif

    Mask mask;
};

#endif
