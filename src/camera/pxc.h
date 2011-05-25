
/***************************************************************************
 | FILE NAME:       PXC.H                                                      |
 | TARGET PLATFORM: DOS 16-bit, DOS/4GW 32-bit, Windows 95 32-bit          |
 |                  Windows 3.1 16-bit, Windows NT                         |
 |                                                                         |
 |             Copyright (c) ImageNation Corporation 1998                  |
 ***************************************************************************/
#ifndef __INCLUDED_PXC200_H
#define __INCLUDED_PXC200_H

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
	#define __PX_CALL _far

#endif
#endif

typedef struct tagFRAME FRAME;

typedef struct TAGpxc {
	long pad1;
	long pad2;
/* ----------General Device Maintenance functions--------------------*/
	long (__PX_CALL *AllocateFG)(short n);
	void (__PX_CALL *FreeFG)(long fgh);
	short (__PX_CALL *ReadRevision)(long fgh);
	void (__PX_CALL *Reset)(long fgh);
	long (__PX_CALL *CheckError)(long fgh);
	short (__PX_CALL *ReadProtection)(long fgh);
	long (__PX_CALL *ReadSerial)(long fgh);

/* ----------Basic Digital I/O Functions-----------------------------*/
	short (__PX_CALL *GetIOType)(long fgh,short n);
	short (__PX_CALL *SetIOType)(long fgh,short n,short type);
	long (__PX_CALL *ReadIO)(long fgh);
	short (__PX_CALL *WriteImmediateIO)(long fgh,long mask,long state);

/* ----------Basic Frame Grabber Functions---------------------------*/
	long (__PX_CALL *Grab)(long fgh,FRAME __PX_FAR *frh,short flags);
	long (__PX_CALL *Wait)(long fgh,short flags);
	short (__PX_CALL *WaitVB)(long fgh);
	void (__PX_CALL *WaitFinished)(long fgh,long qh);
	short (__PX_CALL *IsFinished)(long fgh,long qh);
	void (__PX_CALL *KillQueue)(long fgh);
	FRAME __PX_FAR *(__PX_CALL *AllocateBuffer)(short dx,short dy,short type);
	void (__PX_CALL *FreeFrame)(FRAME __PX_FAR *frh);
	short (__PX_CALL *SetFieldCount)(long fgh,long c);
	long    (__PX_CALL *GetFieldCount)(long fgh);

/* ----------Optional FG Functions-----------------------------------*/
/* these functions must exist, but can fail to do anything useful. */
	long (__PX_CALL *GrabContinuous)(long fgh,FRAME __PX_FAR *frh,short state,
						short flags);
	short (__PX_CALL *VideoType)(long fgh);
	short (__PX_CALL *SetXResolution)(long fgh,short rez);
	short (__PX_CALL *SetYResolution)(long fgh,short rez);
	short (__PX_CALL *SetWidth)(long fgh,short w);
	short (__PX_CALL *SetHeight)(long fgh,short h);
	short (__PX_CALL *SetLeft)(long fgh,short l);
	short (__PX_CALL *SetTop)(long fgh,short t);
	short (__PX_CALL *GetXResolution)(long fgh);
	short (__PX_CALL *GetYResolution)(long fgh);
	short (__PX_CALL *GetWidth)(long fgh);
	short (__PX_CALL *GetHeight)(long fgh);
	short (__PX_CALL *GetLeft)(long fgh);
	short (__PX_CALL *GetTop)(long fgh);
	long (__PX_CALL *WaitAnyEvent)(long fgh,long ioh,long mask,long state,
				short flags);
	long (__PX_CALL *WaitAllEvents)(long fgh,long ioh,long mask,long state,
				short flags);
	long (__PX_CALL *SwitchGrab)(long fgh,FRAME __PX_FAR *f0,FRAME __PX_FAR *f1,
						FRAME __PX_FAR *f2,FRAME __PX_FAR *f3,short flags);
	long (__PX_CALL *SwitchCamera)(long fgh,short flags);
	short (__PX_CALL *GetSwitch)(long fgh);
	long (__PX_CALL *SetCamera)(long fgh,short n,short flags);
	short (__PX_CALL *GetCamera)(long fgh);
	long (__PX_CALL *SetContrast)(long fgh,float c,short flags);
	long (__PX_CALL *SetBrightness)(long fgh,float b,short flags);
	float (__PX_CALL *GetContrast)(long fgh);
	float (__PX_CALL *GetBrightness)(long fgh);

/* everything below this point may not exist in other designs */

/* ---------- HSV custom functions---------------------------*/ 
/* these functions are only useful if the frame grabber uses something
	like an HSV color model.  If it uses RGB internally, these probably
	won't be available. */

	long (__PX_CALL *SetHue)(long fgh,float h,short flags);
	long (__PX_CALL *SetSaturation)(long fgh,float s,short flags);
	float (__PX_CALL *GetHue)(long fgh);
	float (__PX_CALL *GetSaturation)(long fgh);

/* ----------- These are PXC200 custom features -------------*/
	short (__PX_CALL *SetPixelFormat)(long fgh,short type);

	float (__PX_CALL *SetVideoLevel)(long fgh,float white);
	float (__PX_CALL *GetVideoLevel)(long fgh);

	short (__PX_CALL *SetChromaControl)(long fgh,short cf);
	short (__PX_CALL *GetChromaControl)(long fgh);
	short (__PX_CALL *SetLumaControl)(long fgh,short cf);
	short (__PX_CALL *GetLumaControl)(long fgh);
	short (__PX_CALL *SetVideoDetect)(long fgh,short type);
	short (__PX_CALL *GetVideoDetect)(long fgh);


/* ----------- PXC200 high end I/O features go here ---------*/
	float (__PX_CALL *SetStrobePeriod)(long fgh,short n,float p);
	float (__PX_CALL *GetStrobePeriod)(long fgh,short n);

	short (__PX_CALL *SetHoldoffStart)(long fgh,short field,short start);
	short (__PX_CALL *SetHoldoffWidth)(long fgh,short field,short width);
	short (__PX_CALL *SetHoldoffMask)(long fgh,long mask);
	short (__PX_CALL *GetHoldoffStart)(long fgh,short field);
	short (__PX_CALL *GetHoldoffWidth)(long fgh,short field);
	long (__PX_CALL *GetHoldoffMask)(long fgh);

	float (__PX_CALL *SetDoubleStrobe)(long fgh,short n,float p);
	float (__PX_CALL *GetDoubleStrobe)(long fgh,short n);

	short (__PX_CALL *TriggerStrobe)(long fgh,short n,long mask);
	short (__PX_CALL *SyncStrobe)(long fgh,short n,short field,short line);
	short (__PX_CALL *FireStrobe)(long fgh,long mask);

	short (__PX_CALL *SetDecisionPoint)(long fgh,short field,short line);
	short (__PX_CALL *GetDecisionPoint)(long fgh,short field);

	short (__PX_CALL *SetDebounce)(long fgh,short n,short db);
	short (__PX_CALL *GetDebounce)(long fgh,short n);

	/*--------------New function for all pxc200 since sp2019(pxc100)-----*/
	short (__PX_CALL *GetModelNumber)(long fgh);


} PXC;
typedef struct TAGpxc PXC200;
#ifdef  __cplusplus
extern "C" {
#endif

short PXC200_OpenLibrary(PXC __PX_FAR *iface,short size);
void PXC200_CloseLibrary(PXC __PX_FAR *iface);
short PXC_OpenLibrary(PXC __PX_FAR *iface,short size);
void PXC_CloseLibrary(PXC __PX_FAR *iface);
short imagenation_OpenLibrary(char __PX_FAR *file,
			void __PX_FAR *iface,short count);
void imagenation_CloseLibrary(void __PX_FAR *iface);

#ifdef  __cplusplus
	}
#endif

#define GetInterface(h) (*(void __PX_FAR * __PX_FAR *)(h))

/* ----------- Official Portable Constants ------------------*/
/* error codes */
#define ERR_NONE        0x00000000L
#define ERR_NOT_VALID           0x00000001L
#define ERR_CORRUPT                     0x00000002L
#define ERR_IO_FAIL                     0x00000004L

/* I/O line types */
#define LATCH_RISING            0x0001
#define LATCH_FALLING           0x0002
#define LATCH_CHANGE            0x0003
#define IO_INPUT                                0x0004
#define IO_OUTPUT                               0x0005

/* queue flags */
#define IMMEDIATE       0x04
#define QUEUED          0x08


/* ----------- PXC Constants -----------------------------*/
/* error codes */
#define WARN_OVERFLOW           0x00000008L

/* queue flags */
#define EITHER          0x03
#define FIELD0          0x01
#define FIELD1          0x02
#define SINGLE_FLD      0x10

/* chroma and luma control flags */
#define COMB_FILTER             0x0001
#define SVIDEO                          0x0002
#define NOTCH_FILTER    0x0004
#define BW_DETECT                       0x0008
#define AGC                                             0x0010

#define COMB_FILTER             0x0001
#define GAMMA_CORRECT   0x0002
#define CORE_8                          0x0010
#define CORE_16                         0x0020
#define CORE_32                         0x0030
#define LOW_FILT_AUTO   0x0800
#define LOW_FILT_1              0x0900
#define LOW_FILT_2              0x0A00
#define LOW_FILT_3              0x0B00
#define PEAK_FILT_0             0x0C00
#define PEAK_FILT_1             0x0D00
#define PEAK_FILT_2             0x0E00
#define PEAK_FILT_3             0x0F00

/* video detect types */
#define AUTO_FORMAT             0x00
#define NTSC_FORMAT             0x01
#define NTSCJ_FORMAT    0x02
#define PAL_FORMAT              0x03
#define PALM_FORMAT             0x04
#define PALN_FORMAT             0x05
#define SECAM_FORMAT    0x06

/* trigger debounce flags */
#define DEBOUNCE_LONG 0x01
#define DEBOUNCE_BOTH 0x02

/* model number */
#define PXC200_LC 0x01
#define PXC200_F  0x02
#define SP2019    0x04

#define PXC200_LC_2 0x08        //for Fusion878 based LC board
#define PXC200_F_2  0x10        //for Fusion878 based F board
#define SP2019_2    0x20        //for Fusion878 based SP2019 board

#endif
