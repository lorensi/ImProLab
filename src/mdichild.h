/*
  Children windows implementations.
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

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QImage>
#include <QStatusBar>
#include <QSlider>
#include <QVector>
#include <QBoxLayout>
#include <QLabel>
#include <QMultiMap>
#include <QMap>
#include <QTextEdit>
#include <QLineEdit>
#include <QGridLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QTableWidget>
#include <QSplitter>

#include "mask.h"

#ifdef Q_WS_WIN
#include "camera/camera.h"
#endif

/* forward declarations */
class MdiImage;
class MdiHistogram;
class MdiAnalisys;

/*!
  \class MdiChild
  \brief The MdiChild class is the base class for any child window of
  the instance QWorkspace, that holds all child windows of main window.
*/
class MdiChild : public QWidget
{
  Q_OBJECT

  public:
    
    MdiChild(QWidget *Parent = 0);

  private slots:

  protected:
    
    QWidget *parent;
};

/*!
  \class MdiImage
  \brief The MdiImage class basically holds a stack of imagens,
   displaying the last one pushed.
*/
class MdiImage : public MdiChild
{
  public:

    MdiImage(QWidget *Parent = 0);
  
    bool loadFromFile(QString fileName);

    QImage getImage();
    void   setImage(QImage Image, bool push = true );

    void   undo();
    void   push();
    int    imagesSize();

    void   distance();
    void   area();
    void   calibration();
    void   tag();

    void   showTag(bool showTag);
    bool   showTag();
    double getTagValue();
    void   setTagValue( double TagValue );
    void   drawTag( QPainter &Painter );
    void   setTagColor();
  
    void   save( QString FileName, QByteArray FileFormat );

  protected:
     
    void paintEvent(QPaintEvent *event);
    void mousePressEvent ( QMouseEvent * e );
    void mouseMoveEvent ( QMouseEvent * e );

    QVector<QPoint> points;
    QPoint currPos;

    QVector<QImage> images;
    QImage buff;

    bool bdist;
    bool barea;
    bool btag;
    bool bcalib;
  
    bool tagShow;
    double tagValue;
    unsigned int tagX;
    unsigned int tagY;
    QColor tagColor;

  private:
    
    QLabel *label;
    double calcDist(bool Scaled=true);
    double calcArea();
};

/*!
* \class MdiThreshold
* \brief This class is used to binarization of an image.
*/
class MdiThreshold : public MdiChild
{
  Q_OBJECT

  public:
    
    MdiThreshold(QWidget *Parent = 0);
    void setMdiImage(MdiImage *MdiImage);

  public slots:
    
    void setValue(int value);

  protected:
    
    void threshold(int value);
    void afterCreate();

  protected slots:
    
    void closeEvent( QCloseEvent * );

  private:
    
    QSlider *slider;
    QImage image;
    QImage buffer;
    MdiHistogram *hist;
    MdiImage *mdiImage;
    QPushButton *btnOk;
};

/*!
* \class MdiHistogram
* \brief Used to display the image's histogram.
*/
class MdiHistogram : public MdiChild
{
  Q_OBJECT

  public:
    
    MdiHistogram(QWidget *Parent = 0);

    void setValues( QVector<int> Values );
    void setImage( QImage Image );

  protected:
    
    void paintEvent(QPaintEvent *event);

  private:
    
    QVector<int> values;
    int max;
};

#ifdef Q_WS_WIN
/*!
* \class MdiCamera
*/
class MdiCamera : public Camera
{
  Q_OBJECT

  public:
    
    MdiCamera(QWidget *Parent = 0);
};
#endif

/*!
  \struct ImageLabelingData
  \brief The class ImgLabelingData is used by the method ImageEdit::labeling()
  to return a high descriction information about the objects founded in the image,
  like number of objects and mean of the sizes.
*/
struct ImgData
{
  uint color;
  int id;
  int x, y;
  int size;
};

/*!
* \class AnalisysPaintArea
* \brief Used to display a image and the label of each founded object
*        by the labeling method of ImageEdit.
* \sa ImageEdit
*/
class AnalisysPaintArea : public QWidget
{
  Q_OBJECT

  public:
    
    AnalisysPaintArea( QVector<ImgData> Data, QImage ImageData );

  protected:
    
    void paintEvent(QPaintEvent *event);

  private:
    
    QVector<ImgData> data;
    QImage imgData;
};

/*!
  \class MdiAnalisys
  \brief Used to display the image's data.
*/
class MdiAnalisys : public MdiChild
{
  Q_OBJECT

  public:
    
    MdiAnalisys(QWidget *Parent, QVector<ImgData> Data, QImage ImageData);

  private:
    
    QTableWidget *tabWidget;
    AnalisysPaintArea *paintArea;
};


/*!
  \class ImageEdit
  \brief The class ImageEdit involves all oparations that can be applied to an image.
*/
class ImageEdit : public QObject
{
  Q_OBJECT

  public:
    
    ImageEdit(QImage Img) { image = Img.copy(); }

    QImage getImage() { return image; }
    void   setImage(QImage Img) { image = Img.copy(); }

    ///! dot operations
    QImage negative();
    QVector<int> histogram();
    QImage subtration(QImage ImgSub, unsigned RowOffset = 0, unsigned ColOffset = 0);

    ///!area operations
    QImage convolution(Mask &Mask, unsigned Row = 0, unsigned Col = 0, int Width = -1, int Height = -1);
    ///!for binary images only...
    QImage dilation(Mask &Mask, unsigned Row = 0, unsigned Col = 0, int Width = -1, int Height = -1);
    QImage erosion(Mask &Mask, unsigned Row = 0, unsigned Col = 0, int Width = -1, int Height = -1);
    QImage fillholes(unsigned Row = 0, unsigned Col = 0, int Width = -1, int Height = -1);


    QVector<ImgData> labeling();

  private:
    
    QImage image;
};

#endif
