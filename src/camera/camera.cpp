/*
  Implementation of the class used to capture image from the Camera.
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
#include "camera.h"
#include <windows.h>
#include <wingdi.h>

#define DEPTH 32

//#define PIXEL_TYPE PBITS_RGB24

#if DEPTH == 32
#define PIXEL_TYPE PBITS_RGB32
#elif DEPTH == 8
#define PIXEL_TYPE PBITS_Y8
#else
#define PIXEL_TYPE PBITS_Y8
#endif

PXC pxc;
FRAMELIB frame;

//static char szVideoType[80];
static int videotype;
static int modelnumber;
static int grab_type;

static int  ImageMaxX, ImageMaxY;

static int  FrameCount, ErrorCount;

static long fgh=0L;
static FRAME __PX_FAR *frh=NULL;
static int initflags=0;

//void CreateGrayPalette(void);

//static HPALETTE hpalette;
static HANDLE hBuf;
static BYTE __PX_HUGE *gpBits;

/*
struct
{
	BITMAPINFOHEADER head;
	RGBQUAD colors[256];
} maphead;*/


static int  iBoardRev;

int AllocBuffer(void)
{
	hBuf = GlobalAlloc(GMEM_FIXED, (long)ImageMaxX*ImageMaxY*(((PIXEL_TYPE&0xFF)+7L)>>3));
	if(hBuf == NULL)
	{
    QMessageBox::information(NULL, "ImProLab", "Cannot allocate buffer" );
		return FALSE;
	}
	gpBits = (BYTE __PX_HUGE *)GlobalLock(hBuf);
	return TRUE;
}
/*
void Camera::CreateGrayPalette(void)
{
	int i;
	struct
	{
		WORD Version;
		WORD NumberOfEntries;
		PALETTEENTRY aEntries[256];
	} Palette;

	if (hpalette)
		DeleteObject(hpalette);

	Palette.Version = 0x300;
	Palette.NumberOfEntries = 256;
	for (i=0; i<256; ++i)
	{
		Palette.aEntries[i].peRed = i;
		Palette.aEntries[i].peGreen = i;
		Palette.aEntries[i].peBlue = i;
		Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;
		maphead.colors[i].rgbBlue = i;
		maphead.colors[i].rgbGreen= i;
		maphead.colors[i].rgbRed = i;
		maphead.colors[i].rgbReserved = 0;
	}
	hpalette = CreatePalette((LOGPALETTE *)&Palette);
}

void Camera::SetBitMapHead(void)
{
	// Set up bitmap header
	maphead.head.biSize=sizeof(BITMAPINFOHEADER);
	maphead.head.biWidth=ImageMaxX;
	maphead.head.biHeight=ImageMaxY;
	maphead.head.biPlanes=1;
	maphead.head.biBitCount=PIXEL_TYPE&0xFF;
	maphead.head.biCompression=BI_RGB;
	maphead.head.biSizeImage=0;
	maphead.head.biXPelsPerMeter=0;
	maphead.head.biYPelsPerMeter=0;
	maphead.head.biClrUsed=0;
	maphead.head.biClrImportant=0;
}
*/
/*!
 * This function copies the frame buffer to a memory buffer upside down.
 * StretchDIBits expects the image to be reversed line for line. If you want
 * the image to be right side up in the buffer you need to mofify the data
 * in the call to StretchDIBits and in the maphead structure.
 */
void GetImage(FRAME __PX_FAR *frh)
{
	int i,len;
	BYTE __PX_HUGE *p;

	len=ImageMaxX*(((PIXEL_TYPE&0xFF)+7)>>3);
	p = gpBits;

	// Flip the image upside down line by line
	for(i=ImageMaxY-1; i>=0; --i)
	{
		frame.GetRow(frh, p, (short)i);
		p += len;
	}
}

Camera::Camera()
{
  pa = new PaintArea(this);

  //startBtn = new QPushButton(QIcon(":/images/open.png"), tr("&Iniciar"), this);
  //startBtn->setStatusTip(tr("Carrega os drivers da placa de captura"));
  //connect(startBtn, SIGNAL(clicked()), this, SLOT(start()));

  //stopBtn = new QPushButton(QIcon(":/images/open.png"), tr("&Parar"), this);
  //stopBtn->setStatusTip(tr("Para o processo de leitura e descarrega os drivers da memória"));
  //connect(stopBtn, SIGNAL(clicked()), this, SLOT(stop()));

  pauseBtn = new QPushButton(QIcon(":/images/open.png"), tr("&Pausar"), this);
  pauseBtn->setStatusTip(tr("Pausa o processo de leitura"));
  connect(pauseBtn, SIGNAL(clicked()), this, SLOT(pause()));

  runBtn = new QPushButton(QIcon(":/images/open.png"), tr("&Continuar"), this);
  runBtn->setStatusTip(tr("Continua o processo de leitura"));
  connect(runBtn, SIGNAL(clicked()), this, SLOT(run()));

  QHBoxLayout *btnsLayout = new QHBoxLayout;
  //btnsLayout->addWidget(startBtn);
  //btnsLayout->addWidget(stopBtn);
  btnsLayout->addWidget(pauseBtn);
  btnsLayout->addWidget(runBtn);

  sttBar = new QStatusBar;
  sttBar->setSizeGripEnabled( false );
  sttBar->setMaximumHeight( 10 );

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(btnsLayout);
  mainLayout->addWidget(pa, 1);
  mainLayout->addWidget(sttBar, 0, Qt::AlignBottom);


  setLayout(mainLayout);

  resize(400,300);
  setWindowTitle(tr("Camera"));

  start();
}

Camera::~Camera()
{
  stop();
}

void Camera::pause()
{
  runBtn->setEnabled( true );
  pauseBtn->setEnabled( false );

  pa->pause();
}

void Camera::run()
{
  runBtn->setEnabled( false );
  pauseBtn->setEnabled( true );

  pa->acquire();
}

QImage Camera::grab()
{
  //QPixmap pm;
  //pm = QPixmap::grabWidget( pa );
  //return pm.toImage();
  return pa->grab();
}

void Camera::stop()
{
  statusBar()->showMessage( tr("Descarregando driver do dispositivo...") );

  pa->setOK( false );

	if(hBuf)
	{
		GlobalUnlock(hBuf);
		GlobalFree(hBuf);
	}
//	if(hpalette)
//		DeleteObject(hpalette);
	if(frh)
		frame.FreeFrame(frh);
	if(fgh)
		pxc.FreeFG(fgh);
	if (initflags&1)
		imagenation_CloseLibrary(&pxc);
	if (initflags&2)
		imagenation_CloseLibrary(&frame);

  statusBar()->showMessage( tr("Driver descarregado com sucesso.") );
}

void Camera::start()
{
#if DEPTH == -1
  pa->setOK( true );
  runBtn->setEnabled( false );
  pauseBtn->setEnabled( true );
  return;
#endif

  statusBar()->showMessage( tr("Carregando driver do dispositivo...") );
	FrameCount = 0;
	fgh = 0;
	frh = 0L;
	hBuf = NULL;

	/*-------------------------------------------------------------------------
	initialize the library
	-------------------------------------------------------------------------*/
	if (!imagenation_OpenLibrary(PXC_NAME,&pxc,sizeof(pxc)))
	{
    //QMessageBox::information(this, "ImProLab","couldn't load "PXC_NAME);
		if (!imagenation_OpenLibrary(PXC_NT,&pxc,sizeof(pxc)))
		{
      statusBar()->showMessage( tr("Impossível carregar "PXC_NT) );
			return;
		}
	}
	initflags|=1;

	if (!imagenation_OpenLibrary(FRAME_NAME,&frame,sizeof(frame)))
	{
    statusBar()->showMessage( tr("Impossível carregar "FRAME_NAME) );
		return;
	}
	initflags|=2;

	/*-------------------------------------------------------------------------
	allocate any frame grabber
	-------------------------------------------------------------------------*/
	fgh = pxc.AllocateFG(-1);
	if(!fgh)
	{
    statusBar()->showMessage( tr("AllocateFG Failed") );
		return;
	}

	Sleep(2500); /* wait for CCIR autodetect */
	videotype = pxc.VideoType(fgh);
	modelnumber = pxc.GetModelNumber(fgh);
	switch(videotype)
	{
    case 0:     /* no video */
    case 1:     /* NTSC */
      //wsprintf(szVideoType, "Video: NTSC / RS-170.  ");
      grab_type = 0;
      ImageMaxX = 640;
      ImageMaxY = 486;
      break;
    case 2:     /* CCIR */
      //wsprintf(szVideoType, "Video: CCIR / PAL.  ");
      grab_type = 0;
      ImageMaxX = 768;
      ImageMaxY = 576;
      //if(videotype == 0)
        //wsprintf(szVideoType, "Video: None.  ");
      break;
    default:
      ImageMaxX = 768;
      ImageMaxY = 576;
      statusBar()->showMessage( tr("Impossível determinar o tipo de video") );
	}

	if(GetSystemMetrics(SM_CXSCREEN) <= ImageMaxX)
	{
		ImageMaxX/=2;
		ImageMaxY/=2;
	}

	pxc.SetWidth(fgh,(short)ImageMaxX);
	pxc.SetHeight(fgh,(short)ImageMaxY);
	pxc.SetLeft(fgh,0);
	pxc.SetTop(fgh,0);
	pxc.SetXResolution(fgh,(short)ImageMaxX);
	pxc.SetYResolution(fgh,(short)ImageMaxY);
	iBoardRev = pxc.ReadRevision(fgh);

	/*-------------------------------------------------------------------------
	allocate a frame buffer
	-------------------------------------------------------------------------*/
	frh = pxc.AllocateBuffer((short)ImageMaxX, (short)ImageMaxY, PIXEL_TYPE);
	if(!frh)
	{
    statusBar()->showMessage( tr("Allocate Buffer Failed") );
		return;
	}

//	SetBitMapHead();

	if(!AllocBuffer())
	{
    statusBar()->showMessage( tr("Cannot allocate scratch buffer") );
		return;
	}

//  CreateGrayPalette();
  pa->setOK( true );

  runBtn->setEnabled( false );
  pauseBtn->setEnabled( true );

  statusBar()->showMessage( tr("Driver carregado com sucesso.") );
}

PaintArea::PaintArea( Camera *Parent )
{
  parent = Parent;

  offsetY = 0;

  bOK = false;
  bAcquire = true;

  resize( ImageMaxX, ImageMaxY );
  //setAttribute(Qt::WA_PaintOnScreen);
  //setAttribute(Qt::WA_NoBackground);
  //setAttribute(Qt::WA_NoSystemBackground);
  //setAttribute(Qt::WA_Disabled);
  //setBackgroundRole(QPalette::Foreground);

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(0);

  timerStatistics = new QTimer(this);
  connect(timerStatistics, SIGNAL(timeout()), this, SLOT(statistics()));
  timerStatistics->start(1000);
}

void PaintArea::paintEvent(QPaintEvent *event)
{
  if( bOK )
  {
    if(bAcquire)
    {
      ++FrameCount;

      /*
      //setUpdatesEnabled(false);

      //HDC hdc = this->getDC();
      HDC hdc = buffer.getDC();

      // grab the image and bump the frame count
      pxc.Grab(fgh, frh, (short)grab_type);
      ++FrameCount;

      // bump the error count if necessary
      if(pxc.CheckError(fgh)&ERR_CORRUPT)
        ++ErrorCount;

      // flip the image (GetImage) and display it
      GetImage(frh);

      if (hpalette&&(PIXEL_TYPE==PBITS_Y8))
      {
        SelectPalette(hdc, hpalette, TRUE);
        RealizePalette(hdc);
      }

      SetDIBitsToDevice( hdc,
      0, 0, ImageMaxX, ImageMaxY,
      //0, 0, this->width(), this->height(),
      0, 0, 0, ImageMaxY,
      gpBits, (LPBITMAPINFO)&maphead, DIB_RGB_COLORS);

      //this->releaseDC(hdc);
      buffer.releaseDC(hdc);
      */

#if DEPTH == 32

      // grab the image and bump the frame count
      pxc.Grab(fgh, frh, (short)grab_type);

      // bump the error count if necessary
      if(pxc.CheckError(fgh)&ERR_CORRUPT)
        ++ErrorCount;

      // flip the image (GetImage) and display it
      GetImage(frh);

      QImage buffer( (BYTE*)gpBits, ImageMaxX, ImageMaxY, QImage::Format_RGB32 );

      {
        QPainter painter(this);
        painter.drawImage( 0, 0, buffer );
        imgGrabbed = buffer.copy();
      }

#elif DEPTH == 8

      // grab the image and bump the frame count
      pxc.Grab(fgh, frh, (short)grab_type);

      // bump the error count if necessary
      if(pxc.CheckError(fgh)&ERR_CORRUPT)
        ++ErrorCount;

      // flip the image (GetImage) and display it
      GetImage(frh);

      QImage buffer( (BYTE*)gpBits, ImageMaxX, ImageMaxY, QImage::Format_Indexed8 );
      QVector<QRgb> colors;
      for(int i=0; i < 256; ++i)
        colors.push_back( qRgb(i,i,i) );

      buffer.setColorTable( colors );
      buffer.setNumColors( colors.size() );

      {
        QPainter painter(this);
        painter.drawImage( 0, 0, buffer );
        imgGrabbed = buffer.copy();
      }

#elif DEPTH == -1

      static BITMAPFILEHEADER *pbmfh;
      static BITMAPINFO *pbmi;
      static BYTE *pBits;
      static int cxDib, cyDib;

      DWORD dwFileSize, dwHighSize, dwBytesRead;
      HANDLE hFile;
      hFile = CreateFile (L"C:\\teste.bmp", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
      dwFileSize = GetFileSize(hFile, &dwHighSize);
      pbmfh = (BITMAPFILEHEADER*)malloc (dwFileSize);
      ReadFile (hFile,pbmfh,dwFileSize,&dwBytesRead,NULL);
      pbmi= (BITMAPINFO*)(pbmfh+1);
      pBits = (BYTE*)pbmfh + pbmfh->bfOffBits;
      cxDib = pbmi->bmiHeader.biWidth;
      cyDib = abs(pbmi->bmiHeader.biHeight);

      QImage buffer( (BYTE*)pBits, cxDib, cyDib, QImage::Format_Indexed8 );
      QVector<QRgb> colors;
      for(int i=0; i < 256; ++i)
        colors.push_back( qRgb(i,i,i) );

      buffer.setColorTable( colors );

      {
        QPainter painter(this);
        painter.drawImage( 0, 0, buffer );
        imgGrabbed = buffer.copy();
      }

      free (pbmfh);

#endif

    }
    else
    {
      QPainter painter(this);
      painter.drawImage( 0, 0, imgGrabbed );

      //QPainter painter(this);
      //painter.drawText( size().width()/2, size().height()/2, tr("Pause") );
    }
  }
  else
  {
    QPainter painter(this);
    painter.drawText( size().width()/2, size().height()/2, tr("Driver não carregado") );
  }
}

void PaintArea::acquire()
{
  bAcquire = true;
}

void PaintArea::pause()
{
  parent->statusBar()->showMessage( tr("Pause.") );
  bAcquire = false;
}

void PaintArea::statistics()
{
  if(bAcquire && bOK )
  {
    parent->statusBar()->showMessage( tr(" FPS: %1    Erros: %2").arg(FrameCount).arg(ErrorCount) );
    FrameCount = 0;
  }
}

