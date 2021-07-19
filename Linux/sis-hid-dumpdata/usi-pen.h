#ifndef _USI_PEN_H
#define _USI_PEN_H

#include<stdint.h>


#define _USI_24BITCOLOR	// Turn Off for 8BitColor

#define REPORTID			0	// Report ID count
#define SIS_PEN_REPORT_ID	0x5F

#ifdef _USI_24BITCOLOR
#define CIRD1				48	// Color Count
#define CIRD2				49	// Color Count
#define CIRD3				50	// Color Count
#endif

typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;


typedef struct _USI_PEN_DATA_HID_REPORT{
	UINT8		ReportId;
	UINT16		X;
	UINT16		Y;
	UINT8		TransducerIndex;
	UINT16		TipPressure;
	UINT16		BarelPressure;
	
/*
	union
	{
		struct {
			UINT8 TipSwitch						: 1; 
			UINT8 BarrelSwitch					: 1;
			UINT8 SecondaryBarrelSwitch			: 1;
			UINT8 Invert						: 1;
			UINT8 Eraser						: 1;
			UINT8 InRange						: 1;
			UINT8 Rsvd							: 2;
		} ButtonFields;
		//UINT8 AsUINT8;
	}ButtonData;
*/
	UINT8 ButtonData;

	UINT16	XTilt;
	UINT16	YTilt;
	UINT16	Twist;
	UINT16	AccelerationAxisX;
	UINT16	AccelerationAxisY;
	UINT16	AccelerationAxisZ;
	UINT16	AngularVelocityAxisX;
	UINT16	AngularVelocityAxisY;
	UINT16	AngularVelocityAxisZ;
	UINT16	HeadingAxisX;
	UINT16	HeadingAxisY;
	UINT16	HeadingAxisZ;
	UINT8	BatteryStrength;
	UINT64	TransducerSerialNumber64bit;
	UINT32	TransducerSerialNumber32bit;
#ifdef _USI_24BITCOLOR
	union
	{
		struct {
			UINT8 id_1;
			UINT8 id_2;
			UINT8 id_3;
		} PreferredColorFields;
	}PreferredColor;

#else
	UINT8	PreferredColor;
#endif //_USI_24BITCOLOR
	UINT8	PreferredLineWidth;
	
	union
	{
		struct {
			UINT8 Ink					: 1;
			UINT8 Pencil				: 1;
			UINT8 Highlighter			: 1;
			UINT8 ChiselMarker			: 1;
			UINT8 Brush					: 1;
			UINT8 NoPreferredLineStyle	: 1;
			UINT8 Rsvd					: 2;
		} PreferredLineStyleFields;
		UINT8 PreferredLineStyleData;
	};

	UINT16	VendorDefinedField_1;
	UINT16	ScanTime;

} USI_PEN_DATA_HID_REPORT, *PUSI_PEN_DATA_HID_REPORT;


#endif
