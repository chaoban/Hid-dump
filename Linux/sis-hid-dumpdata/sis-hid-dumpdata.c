// SPDX-License-Identifier: GPL-2.0
/*
 * Hidraw Userspace Example
 *
 * Copyright (c) 2010 Alan Ott <alan@signal11.us>
 * Copyright (c) 2010 Signal 11 Software
 *
 * The code may be used by anyone for any purpose,
 * and can serve as a starting point for developing
 * applications using hidraw.
 */

/* Linux */
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

/*
 * Ugly hack to work around failing compilation on systems that don't
 * yet populate new version of hidraw.h to userspace.
 */
#ifndef HIDIOCSFEATURE
#warning Please have your distro update the userspace kernel headers
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif

/* Unix */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>

#include "usi-pen.h"

#ifdef _USI_24BITCOLOR
#define _ONLY_DUMP_COLOR
#endif

//#define _CHAOBAN_TEST

const char *bus_str(int bus);

int main(int argc, char **argv)
{
	int fd;
	int i, res, desc_size = 0;
	char buf[256];
	char *nReportID;
	struct hidraw_report_descriptor rpt_desc;
	struct hidraw_devinfo info;
	char *device = "/dev/hidraw0";
	
	if (argc > 2)
	{
		nReportID = argv[1];
		device = argv[2];
	}else if (argc > 1)
		device = argv[1];

	/* Open the Device with non-blocking reads. In real life,
	   don't use a hard coded path; use libudev instead. */
	fd = open(device, O_RDWR|O_NONBLOCK);

	if (fd < 0) {
		printf("Unable to open device %s\n", device);
		return 1;
	}

	memset(&rpt_desc, 0x0, sizeof(rpt_desc));
	memset(&info, 0x0, sizeof(info));
	memset(buf, 0x0, sizeof(buf));

	/* Get Report Descriptor Size */
	res = ioctl(fd, HIDIOCGRDESCSIZE, &desc_size);
	if (res < 0)
		perror("HIDIOCGRDESCSIZE");
	else
		printf("Report Descriptor Size: %d\n", desc_size);

	/* Get Report Descriptor */
	rpt_desc.size = desc_size;
	res = ioctl(fd, HIDIOCGRDESC, &rpt_desc);
	if (res < 0) {
		perror("HIDIOCGRDESC");
	} else {
		printf("Report Descriptor:\n");
		for (i = 0; i < rpt_desc.size; i++)
			printf("%hhx ", rpt_desc.value[i]);
		puts("\n");
	}

	/* Get Raw Name */
	res = ioctl(fd, HIDIOCGRAWNAME(256), buf);
	if (res < 0)
		perror("HIDIOCGRAWNAME");
	else
		printf("Raw Name: %s\n", buf);

	/* Get Physical Location */
	res = ioctl(fd, HIDIOCGRAWPHYS(256), buf);
	if (res < 0)
		perror("HIDIOCGRAWPHYS");
	else
		printf("Raw Phys: %s\n", buf);

	/* Get Raw Info */
	res = ioctl(fd, HIDIOCGRAWINFO, &info);
	if (res < 0) {
		perror("HIDIOCGRAWINFO");
	} else {
		printf("Raw Info:\n");
		printf("\tbustype: %d (%s)\n",
			info.bustype, bus_str(info.bustype));
		printf("\tvendor: 0x%04hx\n", info.vendor);
		printf("\tproduct: 0x%04hx\n", info.product);
	}

	/* Set Feature */
	buf[0] = 0x9; /* Report Number */
	buf[1] = 0xff;
	buf[2] = 0xff;
	buf[3] = 0xff;
	res = ioctl(fd, HIDIOCSFEATURE(4), buf);
	if (res < 0)
		perror("HIDIOCSFEATURE");
	else
		printf("ioctl HIDIOCSFEATURE returned: %d\n", res);

	/* Get Feature */
	buf[0] = 0x9; /* Report Number */
	res = ioctl(fd, HIDIOCGFEATURE(256), buf);
	if (res < 0) {
		perror("HIDIOCGFEATURE");
	} else {
		printf("ioctl HIDIOCGFEATURE returned: %d\n", res);
		printf("Report data (not containing the report number):\n\t");
		for (i = 0; i < res; i++)
			printf("%hhx ", buf[i]);
		puts("\n");
	}

/* Parse data filed */
	PUSI_PEN_DATA_HID_REPORT pUsiPenDataReport = (PUSI_PEN_DATA_HID_REPORT)buf;

#ifdef _ONLY_DUMP_COLOR
	if (SIS_PEN_REPORT_ID == ((pUsiPenDataReport->ReportId) & 0xff))
	{
		printf("[Report ID: %hhx]\n", buf[REPORTID]);
		printf("[Preferred Color.id_1: %hhx]\n", buf[CIRD1]);
		printf("[Preferred Color.id_1: %hhx]\n", buf[CIRD2]);
		printf("[Preferred Color.id_1: %hhx]\n", buf[CIRD3]);
	}
#else
	printf("[Report ID: %d] \n[X: %d] \n[Y: %d] \n[TransducerIndex: %d] \n[TipPressure: %d] \n[BarrelPressure: %d] \n[ButtonData: %d] \n[X-Tilt: %d]\n[Y-Tilt: %d] \n[Twist: %d]\n",
			pUsiPenDataReport->ReportId, pUsiPenDataReport->X, pUsiPenDataReport->Y,
			pUsiPenDataReport->TransducerIndex, pUsiPenDataReport->TipPressure,
			pUsiPenDataReport->BarelPressure, pUsiPenDataReport->ButtonData, pUsiPenDataReport->XTilt, pUsiPenDataReport->YTilt, pUsiPenDataReport->Twist);
/*
	printf("[TipSwitch: %" PRIu16 "] \n[BarrelSwitch: %d] \n[SecondaryBarrelSwitch: %d] \n[Invert: %d] \n[Eraser: %d] \n[InRange: %d]\n",
			pUsiPenDataReport->ButtonData.ButtonFields.TipSwitch, pUsiPenDataReport->ButtonData.ButtonFields.BarrelSwitch, pUsiPenDataReport->ButtonData.ButtonFields.SecondaryBarrelSwitch,
			pUsiPenDataReport->ButtonData.ButtonFields.Invert, pUsiPenDataReport->ButtonData.ButtonFields.Eraser, pUsiPenDataReport->ButtonData.ButtonFields.InRange);
*/	
	printf("[Accel-X: %d Y: %d Z: %d] \n[Angular-Velocity-X: %d Y: %d Z: %d] \n[Heading-X: %d Y: %d Z: %d]\n",
			pUsiPenDataReport->AccelerationAxisX, pUsiPenDataReport->AccelerationAxisY, pUsiPenDataReport->AccelerationAxisZ, 
			pUsiPenDataReport->AngularVelocityAxisX, pUsiPenDataReport->AngularVelocityAxisY, pUsiPenDataReport->AngularVelocityAxisZ,
			pUsiPenDataReport->HeadingAxisX, pUsiPenDataReport->HeadingAxisY, pUsiPenDataReport->HeadingAxisZ);

#ifdef _USI_24BITCOLOR
	printf("[BatteryStrength: %d] \n[Transducer Serial 64b: %"PRIu64"] \n[Transducer Serial 32b: %"PRIu32"] \n[Preferred Color.id_1: %d] \n[Preferred Color.id_2: %d] \n[Preferred Color.id_3: %d] \n[LineWidth: %d]\n",
			pUsiPenDataReport->BatteryStrength, pUsiPenDataReport->TransducerSerialNumber64bit, pUsiPenDataReport->TransducerSerialNumber32bit,
			pUsiPenDataReport->PreferredColor.PreferredColorFields.id_1, pUsiPenDataReport->PreferredColor.PreferredColorFields.id_2, pUsiPenDataReport->PreferredColor.PreferredColorFields.id_3, pUsiPenDataReport->PreferredLineWidth);
#else
	printf("[BatteryStrength: %d] \n[Transducer Serial 64b: %llx]\n[Transducer Serial 32b: %lx] \n[Preferred Color: %d] \n[LineWidth: %d]\n",
			pUsiPenDataReport->BatteryStrength, pUsiPenDataReport->TransducerSerialNumber64bit, pUsiPenDataReport->TransducerSerialNumber32bit,
			pUsiPenDataReport->PreferredColor, pUsiPenDataReport->PreferredLineWidth);
#endif // _USI_24BITCOLOR
	printf("[PreferredLineStyle: %d] \n[VendorDefinedField: %d] \n[ScanTime: %d] \n", pUsiPenDataReport->PreferredLineStyleData, pUsiPenDataReport->VendorDefinedField_1, pUsiPenDataReport->ScanTime);
#endif // _DUMP_COLOR 

#ifdef _CHAOBAN_TEST
	/* Send a Report to the Device */
	buf[0] = 0x1; /* Report Number */
	buf[1] = 0x77;
	res = write(fd, buf, 2);
	if (res < 0) {
		printf("Error: %d\n", errno);
		perror("write");
	} else {
		printf("write() wrote %d bytes\n", res);
	}

	/* Get a report from the device */
	res = read(fd, buf, 16);
	if (res < 0) {
		perror("read");
	} else {
		printf("read() read %d bytes:\n\t", res);
		for (i = 0; i < res; i++)
			printf("%hhx ", buf[i]);
		puts("\n");
	}
#endif
	close(fd);
	return 0;
}

const char *
bus_str(int bus)
{
	switch (bus) {
	case BUS_USB:
		return "USB";
		break;
	case BUS_HIL:
		return "HIL";
		break;
	case BUS_BLUETOOTH:
		return "Bluetooth";
		break;
	case BUS_VIRTUAL:
		return "Virtual";
		break;
	default:
		return "Other";
		break;
	}
}
