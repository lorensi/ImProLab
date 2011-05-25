/***************************************************************************
 | FILE NAME:       FRAME.H                                                |
 | VERSION:         1.5                                                    |
 | TARGET PLATFORM: DOS/4GW 32-bit, Windows 95/98 Windows NT4.0 32-bit          |
 |                                                                         |
 |             Copyright (c) ImageNation Corporation 1998                  |
 ***************************************************************************/
#ifndef __INCLUDED_FRAME_H
#define __INCLUDED_FRAME_H


#ifndef __PX_HW
#if defined(_WIN32)
	#define __PX_HW
	#define __PX_FAR
	#define __PX_HUGE
	#define __PX_CALL __stdcall

#elif defined(__FLAT__)
	#define __PX_HW
	#define __PX_FAR
	#define __PX_HUGE
	#define __PX_CALL

#elif defined(_WINDOWS)||defined(_Windows)
	#define __PX_HW _huge
	#define __PX_FAR _far
	#define __PX_HUGE _huge
	#define __PX_CALL _far PASCAL

#else
	#define __PX_HW _huge
	#define __PX_FAR _far
	#define __PX_HUGE _huge
	#define __PX_CALL

#endif
#endif

typedef union tagbuffer {
	void __PX_HW *pointer;
	unsigned long data;
} BUFFERDATA;

typedef struct tagFRAME{
	struct tagFRAME __PX_FAR *next;
	short dx,dy;                  
	short type;
		unsigned char user_type;
		unsigned char dma_type;
		BUFFERDATA user;
		BUFFERDATA dma;
	void (__PX_CALL *close)(struct tagFRAME __PX_FAR *); 
}FRAME;


typedef struct tagFRAMELIB {
	long pad1;
	long pad2;
	FRAME __PX_FAR *(__PX_CALL *AllocateMemoryFrame)(short dx, short dy,
									unsigned short type);
	FRAME __PX_FAR *(__PX_CALL *AllocateFlatFrame)(short dx, short dy,
									unsigned short type);
	FRAME __PX_FAR *(__PX_CALL *AllocateAddress)(unsigned long physaddress,
									short dx, short dy, unsigned short type);
	FRAME __PX_FAR *(__PX_CALL *AliasFrame)(FRAME __PX_FAR *f, short x0, short y0,
									short dx, short dy, unsigned short type);
	void (__PX_CALL *FreeFrame)(FRAME __PX_FAR *f);
	short (__PX_CALL *WriteBin)(FRAME __PX_FAR *f, char __PX_FAR *filename,
									short overwrite);
	short (__PX_CALL *ReadBin)(FRAME __PX_FAR *f, char __PX_FAR *filename);
	short (__PX_CALL *GetPixel)(FRAME __PX_FAR *f, void __PX_HUGE *pixel,
									short x, short y);
	short (__PX_CALL *PutPixel)(void __PX_HUGE *pixel, FRAME __PX_FAR *f,
									short x, short y);
	short (__PX_CALL *GetRow)(FRAME __PX_FAR *f, void __PX_HUGE *buf, short row);
	short (__PX_CALL *PutRow)(void __PX_HUGE *buf, FRAME __PX_FAR *f, short row);
	short (__PX_CALL *GetColumn)(FRAME __PX_FAR *f, void __PX_HUGE *buf,
									short column);
	short (__PX_CALL *PutColumn)(void __PX_HUGE *buf, FRAME __PX_FAR *f,
									short column);
	short (__PX_CALL *GetRectangle)(FRAME __PX_FAR *f, void __PX_HUGE *buf,
									short x0, short y0, short dx, short dy);
	short (__PX_CALL *PutRectangle)(void __PX_HUGE *buf, FRAME __PX_FAR *f,
									short x0, short y0, short dx, short dy);
	short (__PX_CALL *CopyFrame)(FRAME __PX_FAR *source, short sourcex,
									short sourcey, FRAME __PX_FAR *dest, short destx,
									short desty, short dx, short dy);
	FRAME __PX_FAR *(__PX_CALL *ExtractPlane)(FRAME __PX_FAR *f, short plane);
	short (__PX_CALL *WriteBMP)(FRAME __PX_FAR *f, char __PX_FAR *filename,
									short overwrite);
	short (__PX_CALL *ReadBMP)(FRAME __PX_FAR *f, char __PX_FAR *filename);
	void __PX_HW *(__PX_CALL *FrameBuffer)(FRAME __PX_FAR *f);
	unsigned long (__PX_CALL *FrameAddress)(FRAME __PX_FAR *f);
	short (__PX_CALL *FrameWidth)(FRAME __PX_FAR *f);
	short (__PX_CALL *FrameHeight)(FRAME __PX_FAR *f);
	short (__PX_CALL *FrameType)(FRAME __PX_FAR *f);

	short (__PX_CALL *WritePNG)(FRAME __PX_FAR *f, char __PX_FAR *filename,
									short overwrite);
	short (__PX_CALL *ReadPNG)(FRAME __PX_FAR *f, char __PX_FAR *filename);
	FRAME __PX_FAR *(__PX_CALL *FrameFromPointer)(void __PX_HW *p, 
									short dx, short dy, unsigned short type);

} FRAMELIB;

/* some common image data types */
#define PBITS_Y8 			0x0008
#define PBITS_Y16			0x0010
#define PBITS_Yf			0x0120
#define PBITS_RGB15			0x060F
#define PBITS_RGB16			0x0210
#define PBITS_RGB24			0x0218
#define PBITS_RGB32			0x0620
#define PBITS_RGBf			0x0360
#define PBITS_YUV422		0x0810
#define PBITS_YUV444		0x0818
#define PBITS_YUV422P		0x1810
#define PBITS_YUV444P		0x1818


/* bit fields for parsing pixel types */
#define PBITS_FLOAT			0x0100
#define PBITS_BITS			0x00FF
#define PBITS_RGB				0x0200
#define PBITS_ALPHA			0x0400
#define PBITS_YUV				0x0800
#define PBITS_PLANES		0x1000

/* type fields which tell what the BUFFERDATA user means */
#define PTYPE_UFLAT			0x01 	/* pointer to array of pixels */
#define PTYPE_UPAGED		0x02	/* pointer to PAGER structure */
#define PTYPE_UPLANES   0x03  /* pointer to a list of frame structures */
/* type fields which tell what the BUFFERDATA dma means */
#define PTYPE_DFLAT			0x01	/* 32 bit physical bus address */
#define PTYPE_DLINEAR		0x02	/* linear address */
#define PTYPE_DVIRTUAL  0x03  /* pointer in user's address space */
#define PTYPE_DLOCKED		0x04	/* pointer to dma stuff */

	/* the DLOCKED value means that a dma device is using the buffer, and
	may have written arbitrary stuff into the dma field.  It will eventually
	unlock the buffer and restore the type field to something else.
		The DLINEAR value means that the buffer has a known linear address,
	but not a known physical mapping.  In flat addressing this means the user
	data matches the dma data, but in other addressing modes they may differ.*/

typedef struct tagpagedata {
	void __PX_HW *base;
	unsigned long page_length;
	unsigned long offset;
	unsigned long granularity;
	void (__PX_CALL *page)(long,FRAME __PX_FAR *);
	unsigned long data;
} PAGEDATA;

/* BMP File I/O returns */
#define SUCCESS 0
#define FILE_EXISTS 2 
#define FILE_OPEN_ERROR 3
#define BAD_WRITE 4
#define BAD_READ 5
#define WRONG_BITS 8
#define FRAME_SIZE 9
#define BAD_FILE 11
#define INVALID_FRAME 12

#ifdef  __cplusplus
extern "C" {
#endif

short FRAMELIB_OpenLibrary(FRAMELIB __PX_FAR *iface, short size);
void FRAMELIB_CloseLibrary(FRAMELIB __PX_FAR *iface);
short imagenation_OpenLibrary(char __PX_FAR *file,
			void __PX_FAR *iface,short count);
void imagenation_CloseLibrary(void __PX_FAR *iface);

#ifdef  __cplusplus
	}
#endif

typedef unsigned char 	PIX_Y8;
typedef unsigned short 	PIX_Y16;
typedef float						PIX_Yf;

typedef struct tagRGB24 {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
}												PIX_RGB24;

typedef struct tagRGB32 {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char alpha;
}												PIX_RGB32;

typedef struct tagRGB16 {
	unsigned short blue : 5;
	unsigned short green : 6;
	unsigned short red : 5;
}												PIX_RGB16;

typedef struct tagRGB15 {
	unsigned short blue : 5;
	unsigned short green : 5;
	unsigned short red : 5;
	unsigned short alpha : 1;
}												PIX_RGB15;

typedef struct tagRGBf {
	float blue;
	float green;
	float red;
}												PIX_RGBf;

#endif
