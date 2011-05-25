/*
  Declaration of the class used to capture image from the Camera.
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

#ifndef CAMERA_H
#define CAMERA_H

#include <QMainWindow>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QPushButton>

#include "pxc.h"
#include "iframe.h"

#define PXC_NAME  "pxc_95.dll"
#define FRAME_NAME  "frame_32.dll"
#define PXC_NT    "pxc_nt.dll"

class QAction;
class PaintArea;
class QHBoxLayout;
class QPixmap;

class Camera : public QWidget
{
  Q_OBJECT

  public:
    Camera();
    ~Camera();

  protected:
    void CreateGrayPalette(void);
    void SetBitMapHead(void);

  public slots:
    void start();
    void stop();
    void pause();
    void run();
    QImage grab();

    QStatusBar *statusBar() { return sttBar; }

  private:

    //QPushButton *startBtn;
    //QPushButton *stopBtn;
    QPushButton *pauseBtn;
    QPushButton *runBtn;

    QStatusBar *sttBar;

    PaintArea *pa;
};


class PaintArea : public QWidget
{
  Q_OBJECT

  public:
    PaintArea( Camera *Parent );
    void setOK( bool OK ) { bOK = OK; }
    void setOffsetY( int OffSetY ) { offsetY = OffSetY; }

    QImage grab() { return imgGrabbed; }

  protected:
    void paintEvent(QPaintEvent *event);
    //void resizeEvent( QResizeEvent * event );

  public slots:
    void acquire();
    void pause();
    void statistics();

  private:
    Camera *parent;
    int offsetY;
    bool bOK;
    bool bAcquire;
    QTimer *timer;

    QTimer *timerStatistics;

    //QPixmap buffer;
    QImage imgGrabbed;
};

#endif
