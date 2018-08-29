/*
 * Linux ADK - hid.c
 *
 * Copyright (C) 2014 - Gary Bisson <bisson.gary@gmail.com>
 *
 * Based on usbAccReadWrite.c by Jeremy Rosen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <poll.h>
#include <libusb.h>

#include "linux-adk.h"
#include "hid.h"
#include "axis.h"

#define REPORT_ID (4)
//#define TOUCHSCREEN_ENABLE  (2)

#define PHONE_WIDTH (1080)
#define PHONE_HEIGHT (1920)

#define PHONE_WIDTH (1600)
#define PHONE_HEIGHT (1200)

#if(TOUCHSCREEN_ENABLE == 1) 
static unsigned char hid_report_desc[] = {

		0x05, 0x0d,                         // USAGE_PAGE (Digitizers)
		0x09, 0x02,                         // USAGE (Touch Screen)
		0xa1, 0x01,                         // COLLECTION (Application)
		0x85, REPORT_ID,                    //   REPORT_ID (4)
		0x09, 0x20,                         //   USAGE (Stlus)
		//0x09, 0x22,                         //   USAGE (Fingers)
		//0x09, 0x01,                         //   USAGE (Internal pen)
		//0x09, 0x02,                         //   USAGE (External pen)
		0xa1, 0x00,                         //   COLLECTION (Physical)
		//0xa1, 0x02,                         //   COLLECTION (Logical)
		0x09, 0x42,                         //     USAGE (Tip Switch)
		0x09, 0x32,                         //     USAGE (In Range)
		//0x09, 0x47,                         //     USAGE (Confidence)
		0x15, 0x00,                         //     LOGICAL_MINIMUM (0)
		0x25, 0x01,                         //     LOGICAL_MAXIMUM (1)
		0x75, 0x01,                         //     REPORT_SIZE (1)
		0x95, 0x02,                         //     REPORT_COUNT (2)
		0x81, 0x02,                         //     INPUT (Data,Var,Abs)
		0x75, 0x01,                         //     REPORT_SIZE (1)
		0x95, 0x06,                         //     REPORT_COUNT (6)
		0x81, 0x01,                         //     INPUT (Cnst,Ary,Abs)

		0x05, 0x01,                         //     USAGE_PAGE (Generic Desktop)
		//
		//0x09, 0x01,                         //   USAGE_PAGE(Pointer)
		//0xa1, 0x00,                         //   COLLECTION (Physical)
		//

		//0x35, 0x00,                         //     PHYSICAL_MINIMUM (0)         
		//0x46, 0x00, 0x00,                   //     PHYSICAL_MAXIMUM (0)
		
		//0x26, 0x10, 0x27,                   //     LOGICAL_MAXIMUM (10000)
		//0x46, 0x38, 0x04,                   //     PHYSICAL_MAXIMUM (1080)
		0x26, 0x38, 0x04,                   //     LOGICAL_MAXIMUM (1080)
		0x75, 0x10,                         //     REPORT_SIZE (16) 
		0x95, 0x01,                         //     REPORT_COUNT (1)            
		0xa4,                               //     PUSH
		//0x55, 0x0E,                         //     UNIT_EXPONENT (-2)           
		//0x65, 0x11,                         //     UNIT (cm,SI Linear)
		//0x55, 0x00,        //       Unit Exponent (0)
		//0x65, 0x00,        //       Unit (None)
		//0x45, 0x00,                         //     PHYSICAL_MAXIMUM (0)
		0x09, 0x30,                         //     USAGE (X)                    
		0x81, 0x02,                         //     INPUT (Data,Var,Abs)         
		//0x81, 0x06,                         //     INPUT (Data,Var,Ref)         
		0x26, 0x80, 0x07,                   //     LOGICAL_MAXIMUM (1920)
		0x09, 0x31,                         //     USAGE (Y)                    
		0x81, 0x02,                         //     INPUT (Data,Var,Abs)
		0xb4,                               //     POP

		//0x81, 0x06,                         //     INPUT (Data,Var,Ref)
		0xc0,                               //   END_COLLECTION
		0xc0,                               // END_COLLECTION
};


struct Report {
	unsigned char  report_id ;  // Must be set to 4
	struct {
		unsigned char   bTipSwitch : 1;
		unsigned char   bInRange   : 1;
		unsigned char   reserved   : 6;
	} status;
	unsigned short  wXData;
	unsigned short  wYData;
};
#elif (TOUCHSCREEN_ENABLE == 2)

#define LOGICAL_MINIMUM_1 (0x15)
#define LOGICAL_MAXIMUM_1 (0x25)
#define LOGICAL_MINIMUM_2 (0x16)
#define LOGICAL_MAXIMUM_2 (0x26)


#define PHYSICAL_MINIMUM_1 (0x35)
#define PHYSICAL_MAXIMUM_1 (0x45)
#define PHYSICAL_MINIMUM_2 (0x36)
#define PHYSICAL_MAXIMUM_2 (0x46)

static unsigned char hid_report_desc[] = {
		0x05, 0x0d,                         // USAGE_PAGE (Digitizers)
		0x09, 0x02,                         // USAGE (Touch Screen)
		0xa1, 0x01,                         // COLLECTION (Application)
		0x85, REPORT_ID,                    //   REPORT_ID (4)
		//0x09, 0x20,                         //   USAGE (Stlus)
		//0x09, 0x22,                         //   USAGE (Fingers)
		//0x09, 0x01,                         //   USAGE (Internal pen)
		0x09, 0x02,                         //   USAGE (External pen)
		0xa1, 0x00,                         //   COLLECTION (Physical)
		//0xa1, 0x02,                         //   COLLECTION (Logical)
		0x09, 0x42,                         //     USAGE (Tip Switch)
		0x09, 0x32,                         //     USAGE (In Range)
		//0x09, 0x47,                         //     USAGE (Confidence)
		0x15, 0x00,                         //     LOGICAL_MINIMUM (0)
		0x25, 0x01,                         //     LOGICAL_MAXIMUM (1)
		0x75, 0x01,                         //     REPORT_SIZE (1)
		0x95, 0x02,                         //     REPORT_COUNT (2)
		0x81, 0x02,                         //     INPUT (Data,Var,Abs)
		0x75, 0x01,                         //     REPORT_SIZE (1)
		0x95, 0x06,                         //     REPORT_COUNT (6)
		0x81, 0x01,                         //     INPUT (Cnst,Ary,Abs)

                      // END_COLLECTION
	0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)


	0xa4, //push
	/*0x09, 0x01,                         //   USAGE_PAGE(Pointer)*/
	/*0xa1, 0x00,                         //   COLLECTION (Physical)*/
    0x09, 0x30,                    //     USAGE (X)
	LOGICAL_MINIMUM_2, 0x00,0xF8,               //     LOGICAL_MINIMUM (-2000),
    LOGICAL_MAXIMUM_2, 0x00,0x08,               //     LOGICAL_MAXIMUM (2000)
	PHYSICAL_MINIMUM_2, 0x00,0xF8,               //     printfCAL_MINIMUM (-2000),
	PHYSICAL_MAXIMUM_2, 0x00,0x08,               //     LOGICAL_MAXIMUM (2000)
    //0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x09, 0x31,                    //     USAGE (Y)
    LOGICAL_MINIMUM_2, 0x00,0xFC,               //     LOGICAL_MINIMUM (-1000),
    LOGICAL_MAXIMUM_2, 0x00,0x04,               //     LOGICAL_MAXIMUM (1000)
	PHYSICAL_MINIMUM_2, 0x00,0xF8,               //     LOGICAL_MINIMUM (-2000),
	PHYSICAL_MAXIMUM_2, 0x00,0x08,               //     LOGICAL_MAXIMUM (2000)
    //0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0xb4, //pop

    0xc0,                          //   END_COLLECTION
	0xc0
};

/** @brief _Report  */
struct Report {
	unsigned char  report_id ;  // Must be set to 4
	struct {
		unsigned char   bTipSwitch : 1;
		unsigned char   bInRange   : 1;
		unsigned char   reserved   : 6;
	} status;
	short  wXData;
	short  wYData;
	/*short Width;*/
	/*short Height;*/
};

void set_hid_logical_range(unsigned short width, unsigned short height) 
{
	unsigned char *  tmp = hid_report_desc;
	unsigned short lb_w =    0;
	unsigned short ub_w =   width;
	unsigned short lb_h =    0;
	unsigned short ub_h =   height;
	unsigned long i;
	typedef enum {
		STEP_1,
		STEP_2,
		STEP_3,
		STEP_4,
	} _STEP;
	int step =  STEP_1;
	for(i = 0 ; i < sizeof(hid_report_desc); i++) {
		/*printf("search ...tmp[%ld]= 0x%02x",i,tmp[i]);*/
		switch (step) {
			case STEP_1:
				if(tmp[i] == LOGICAL_MINIMUM_2) {
					tmp[i+1] = lb_w & 0x00FF;
					tmp[i+2] = (lb_w & 0xFF00) >> 8;
					printf("lb_w val(%d) =(0x%02x 0x%02x)",lb_w,tmp[i+1],tmp[i+2]);
					step = STEP_2;
				}
				break;
			case STEP_2:
				if(tmp[i] == LOGICAL_MAXIMUM_2) {
					tmp[i+1] = ub_w & 0x00FF;
					tmp[i+2] = (ub_w & 0xFF00) >> 8;
					printf("ub_w val(%d) =(0x%02x 0x%02x)",ub_w,tmp[i+1],tmp[i+2]);
					step = STEP_3;
				}
				break;
			case STEP_3:
				if(tmp[i] == LOGICAL_MINIMUM_2) {
					tmp[i+1] = lb_h & 0x00FF;
					tmp[i+2] = (lb_h & 0xFF00) >> 8;
					printf("lb_h val(%d) =(0x%02x 0x%02x)\n",lb_h,tmp[i+1],tmp[i+2]);
					step = STEP_4;
				}
				break;
			case STEP_4:
				//logical maximun
				if(tmp[i] == LOGICAL_MAXIMUM_2) {
					tmp[i+1] = ub_h & 0x00FF;
					tmp[i+2] = (ub_h & 0xFF00) >> 8;
					printf("ub_h val(%d) =(0x%02x 0x%02x)\n",ub_h,tmp[i+1],tmp[i+2]);
					goto end;
				}
				break;
		}
	}
end:
	/*assert(step == STEP_4);*/
	printf("hid report is:\n");
	for(i = 0 ; i < sizeof(hid_report_desc) ;i++) {
		printf("0x%02x ",tmp[i]);
		if(i % 10 == 0)
			printf("\n");
	}
	printf("\n");
}

void set_hid_physical_range(unsigned short width, unsigned short height) 
{
	unsigned char *  tmp = hid_report_desc;
	unsigned short lb_w =    0;
	unsigned short ub_w =   width;
	unsigned short lb_h =    0;
	unsigned short ub_h =   height;
	unsigned long i;
	typedef enum {
		STEP_1,
		STEP_2,
		STEP_3,
		STEP_4,
	} _STEP;
	int step =  STEP_1;
	for(i = 0 ; i < sizeof(hid_report_desc); i++) {
		//printf("search ...tmp[%ld]= 0x%02x",i,tmp[i]);
		switch (step) {
			case STEP_1:
				if(tmp[i] == PHYSICAL_MINIMUM_2) {
					tmp[i+1] = lb_w & 0x00FF;
					tmp[i+2] = (lb_w & 0xFF00) >> 8;
					printf("lb_w val(%d) =(0x%02x 0x%02x)",lb_w,tmp[i+1],tmp[i+2]);
					step = STEP_2;
				}
				break;
			case STEP_2:
				if(tmp[i] == PHYSICAL_MAXIMUM_2) {
					tmp[i+1] = ub_w & 0x00FF;
					tmp[i+2] = (ub_w & 0xFF00) >> 8;
					printf("ub_w val(%d) =(0x%02x 0x%02x)",ub_w,tmp[i+1],tmp[i+2]);
					step = STEP_3;
				}
				break;
			case STEP_3:
				if(tmp[i] == PHYSICAL_MINIMUM_2) {
					tmp[i+1] = lb_h & 0x00FF;
					tmp[i+2] = (lb_h & 0xFF00) >> 8;
					printf("lb_h val(%d) =(0x%02x 0x%02x)\n",lb_h,tmp[i+1],tmp[i+2]);
					step = STEP_4;
				}
				break;
			case STEP_4:
				if(tmp[i] == PHYSICAL_MAXIMUM_2) {
					tmp[i+1] = ub_h & 0x00FF;
					tmp[i+2] = (ub_h & 0xFF00) >> 8;
					printf("ub_h val(%d) =(0x%02x 0x%02x)\n",ub_h,tmp[i+1],tmp[i+2]);
					goto end;
				}
				break;
		}
	}
end:
	/*assert(step == STEP_4);*/
	printf("hid report is:\n");
	for(i = 0 ; i < sizeof(hid_report_desc) ;i++) {
		printf("0x%02x ",tmp[i]);
		if(i % 10 == 0)
			printf("\n");
	}
	printf("\n");
}
#endif

static void *receive_loop(void *arg)
{
	while (!stop_acc) {
		int ret = 0;
		int i;
		int nfds = 1;
		struct timeval tv, zero_tv;
		const struct libusb_pollfd **poll_list;
		fd_set rfds, wfds, efds;
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		zero_tv.tv_sec = 0;
		zero_tv.tv_usec = 0;

		ret = libusb_get_next_timeout(NULL, &tv);
		if (ret < 0) {
			if (ret)
				printf("USB error : %s\n",
				       libusb_error_name(ret));
			break;
		}

		if (ret == 1 && tv.tv_sec == 0 && tv.tv_usec == 0) {
			ret = libusb_handle_events_timeout_completed(NULL,
								     &zero_tv,
								     NULL);
			if (ret) {
				if (ret)
					printf("USB error : %s\n",
					       libusb_error_name(ret));
				break;
			}
			continue;
		}

		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
		FD_SET(0, &rfds);
		poll_list = libusb_get_pollfds(NULL);
		if (poll_list) {
			for (i = 0; poll_list[i] != NULL; i++) {
				const struct libusb_pollfd *cur_poll;

				cur_poll = poll_list[i];
				nfds = cur_poll->fd >= nfds
					? cur_poll->fd + 1 : nfds;
				if (cur_poll->events & POLLIN)
					FD_SET(cur_poll->fd, &rfds);
				if (cur_poll->events & POLLOUT)
					FD_SET(cur_poll->fd, &wfds);
			}
			free(poll_list);
		} else
			break;

		if (tv.tv_sec == 0 && tv.tv_usec == 0)
			ret = select(nfds, &rfds, &wfds, &efds, NULL);
		else
			ret = select(nfds, &rfds, &wfds, &efds, &tv);

		if (libusb_handle_events_timeout_completed
		    (NULL, &zero_tv, NULL))
			break;
	}

	return NULL;
}

static int open_device(struct libusb_device_handle **handle,
		      libusb_device * device, int interface)
{
	int kernel_claimed = 0, ret;

	libusb_ref_device(device);
	ret = libusb_open(device, handle);
	if (ret) {
		printf("Unable to open usb device [%#08x]\n", ret);
		*handle = NULL;
		return -1;
	}

	printf("libusb_kernel_driver_active\n");
	ret = libusb_kernel_driver_active(*handle, interface);
	if (ret == 1) {
		if (libusb_detach_kernel_driver(*handle, interface)) {
			printf("Unable to grab usb device\n");
			libusb_close(*handle);
			*handle = NULL;
			return -1;
		}
		kernel_claimed = 1;
	}

	printf("libusb_claim_interface\n");
	ret = libusb_claim_interface(*handle, interface);
	if (ret) {
		printf("Failed to claim interface %d.\n", interface);
		if (kernel_claimed) {
			libusb_attach_kernel_driver(*handle, interface);
			kernel_claimed = 0;
		}
		libusb_close(*handle);
		*handle = NULL;
		return -1;
	}

	return 0;
}

static void callback_hid(struct libusb_transfer *transfer)
{
	accessory_t *acc = transfer->user_data;
	int rc = 0;

	if (transfer->status == LIBUSB_TRANSFER_COMPLETED) {
		struct libusb_transfer *android_transfer;
		unsigned char *keybuf;
		int rc;

		android_transfer = libusb_alloc_transfer(0);

		int s = 0;

		printf("orig hid report data from mouse:\n");
		for(s = 0 ; s <  transfer->actual_length; s ++) {
			printf("0x%02x ",transfer->buffer[s]);
		};
		printf("\n");

#if(TOUCHSCREEN_ENABLE == 1) 
		transfer->actual_length = sizeof(struct Report);

		//set Report for each moving..
		//(0,0) -->(1000,1000) 
		//(100,100) -->按下　(900,900)-->放开
		static unsigned short counts = 0;

		struct Report report;
		report.report_id = REPORT_ID;
		if(counts != 0) {
			report.wXData = counts % 1080;
			report.wYData = counts * 1920 / 1080 % 1920 ;
		} else 
			report.wXData = report.wYData = 0;

		if(report.wXData >= 200  && report.wXData <= 900) 
			report.status.bTipSwitch = 1;
		else
			report.status.bTipSwitch = 0;
		counts ++;


		
		transfer->buffer = (unsigned char*)&report;
#elif (TOUCHSCREEN_ENABLE == 2)
		transfer->actual_length = sizeof(struct Report);

		//set Report for each moving..
		//(0,0) -->(1000,1000) 
		//(100,100) -->按下　(900,900)-->放开
		static unsigned short counts = 0;

		struct Report report;
		report.report_id = REPORT_ID;
		if(counts != 0) {
			report.wXData = counts * 4 % PHONE_HEIGHT;
			report.wYData = counts * 4 % PHONE_WIDTH ;
		} else 
			report.wXData = report.wYData = 0;

		if(report.wXData >= 200  && report.wXData <= 900) 
			report.status.bTipSwitch = 1;
		else
			report.status.bTipSwitch = 0;
		counts ++;

		/*report.Width = PHONE_HEIGHT;*/
		/*report.Height = PHONE_WIDTH;*/

		transfer->buffer = (unsigned char*)&report;
#endif
		keybuf = malloc(transfer->actual_length + LIBUSB_CONTROL_SETUP_SIZE);
		memcpy(keybuf + LIBUSB_CONTROL_SETUP_SIZE, transfer->buffer,
		       transfer->actual_length);

		printf("hid report data:\n");
		for(s = 0 ; s <  transfer->actual_length; s ++) {
			printf("0x%02x ",transfer->buffer[s]);
		}; 
		printf("\n");
		

		libusb_fill_control_setup(keybuf,
					  LIBUSB_ENDPOINT_OUT |
					  LIBUSB_REQUEST_TYPE_VENDOR,
					  AOA_SEND_HID_EVENT, 1, 0,
					  transfer->actual_length);

		libusb_fill_control_transfer(android_transfer, acc->handle,
					     keybuf, NULL, NULL, 0);

		android_transfer->flags =
		    LIBUSB_TRANSFER_FREE_BUFFER | LIBUSB_TRANSFER_FREE_TRANSFER;

		rc = libusb_submit_transfer(android_transfer);
		if (rc)
			printf("USB error : %s\n", libusb_error_name(rc));

		rc = libusb_submit_transfer(transfer);
		if (rc)
			printf("USB error : %s\n", libusb_error_name(rc));

	} else if (transfer->status == LIBUSB_TRANSFER_TIMED_OUT) {
		rc = libusb_submit_transfer(transfer);
		if (rc)
			printf("USB error : %s\n", libusb_error_name(rc));
	}
}

unsigned char search_hid(hid_device * hid)
{
	libusb_device **list;
	struct libusb_device_descriptor desc;
	ssize_t cnt;
	ssize_t i = 0;
	int j;
	libusb_device *device;

	hid->handle = NULL;

	/* List every USB device attached */
	cnt = libusb_get_device_list(NULL, &list);
	if (cnt < 0)
		goto error0;

	for (i = 0; i < cnt; i++) {
		device = list[i];
		int r;

		printf("cnt(%d) i(%d) \n",cnt,i);

		r = libusb_get_device_descriptor(device, &desc);
		if (r < 0)
			continue;

		printf("desc.bDeviceClass = %d\n",desc.bDeviceClass);
		if (desc.bDeviceClass == LIBUSB_CLASS_HID) {
			goto found;
			break;
		} else if (desc.bDeviceClass == LIBUSB_CLASS_PER_INTERFACE) {
			struct libusb_config_descriptor *current_config;
			int r;

			printf("libusb_get_active_config_descriptor\n");
			r = libusb_get_active_config_descriptor(device,
								&current_config);
			if (r < 0) {
				printf("get active desc error(%d)",r);
				continue;
			}
			printf("parse hid cfg interface\n");
			//extern int parse_configuration(struct libusb_config_descriptor *config);
			//parse_configuration(current_config);

			printf("bNumInterfaces(%d)\n",current_config->bNumInterfaces);

			for (j = 0; j < current_config->bNumInterfaces; j++) {
				int k;
				printf("num_altsetting(%d)\n",current_config->interface[j].num_altsetting);
				for (k = 0;
				     k <
				     current_config->
				     interface[j].num_altsetting; k++) {
					if (current_config->
					    interface[j].altsetting[k].
					    bInterfaceClass ==
					    LIBUSB_CLASS_HID) {
						hid->endpoint_in =
						    current_config->interface
						    [j].
						    altsetting[k].endpoint
						    [0].bEndpointAddress;
						hid->packet_size =
						    current_config->interface
						    [j].
						    altsetting[k].endpoint[0].
						    wMaxPacketSize;
						printf("set endpoint(0x%x)\n",hid->endpoint_in);
						goto found;
					}
				}
			}
			printf("libusb_free_config_descriptor\n");
			libusb_free_config_descriptor(current_config);
		}
	}

	printf("found ...\n");
found:
	if (i == cnt)
		goto error0;

	printf("open_device...\n");
	if (open_device(&hid->handle, device, j) < 0)
		goto error0;


	printf("GET_DESCRIPTOR...\n");
	hid->descriptor_size = libusb_control_transfer(hid->handle,
						       LIBUSB_ENDPOINT_IN |
						       LIBUSB_RECIPIENT_INTERFACE,
						       LIBUSB_REQUEST_GET_DESCRIPTOR,
						       LIBUSB_DT_REPORT << 8, 0,
						       hid->descriptor, 256, 0);
	if (hid->descriptor_size < 0)
		goto error1;

	libusb_free_device_list(list, 1);
	printf("=> found HID device vid 0x%x pid 0x%x\n", desc.idVendor,
	       desc.idProduct);
	int s = 0;
	printf("descriptor: \n");
	for(s = 0 ; s <  hid->descriptor_size; s ++) {
		printf("0x%02x ",hid->descriptor[s]);
	}; 
	printf("\n");

	return 0;
error1:
	libusb_close(hid->handle);
error0:
	libusb_free_device_list(list, 1);
	return -1;
}

int register_hid_callback(accessory_t * acc, hid_device * hid)
{
	struct libusb_transfer *hid_transfer;
	unsigned char *keybuf;
	int rc;

	AxisSetting set;
	set.css.base_x = 0 ;
	set.css.base_y = 0 ;
	set.css.view_height = set.css.full_height = PHONE_WIDTH ;
	set.css.view_width = set.css.full_width = PHONE_HEIGHT;

	set.pss.full_width =  PHONE_HEIGHT;
	set.pss.full_height =  PHONE_WIDTH;

	axis_init(&set,0);


	keybuf = malloc(hid->packet_size);

	hid_transfer = libusb_alloc_transfer(0);
	if (hid_transfer == NULL) {
		libusb_close(hid->handle);
		return -1;
	}

	libusb_fill_interrupt_transfer(hid_transfer, hid->handle,
				       hid->endpoint_in,
				       keybuf, hid->packet_size, callback_hid,
				       acc, 0);

	rc = libusb_submit_transfer(hid_transfer);
	if (rc != 0) {
		if (rc)
			printf("USB error : %s\n", libusb_error_name(rc));
		libusb_close(hid->handle);
		return -1;
	}

	return 0;
}

int send_hid_descriptor(accessory_t * acc, hid_device * hid)
{
	int ret;

	printf("register hid\n");
	ret = libusb_control_transfer(acc->handle, LIBUSB_ENDPOINT_OUT |
				      LIBUSB_REQUEST_TYPE_VENDOR,
#ifdef TOUCHSCREEN_ENABLE
				      AOA_REGISTER_HID, 1, sizeof(hid_report_desc),
#else
				      AOA_REGISTER_HID, 1, hid->descriptor_size,
#endif
				      NULL, 0, 0);
	if (ret < 0) {
		printf("couldn't register HID device on the android device : %s\n",
		       libusb_error_name(ret));
		libusb_close(hid->handle);
		return -1;
	}

	printf("send hid report\n");
	ret = libusb_control_transfer(acc->handle, LIBUSB_ENDPOINT_OUT |
				      LIBUSB_REQUEST_TYPE_VENDOR,
				      AOA_SET_HID_REPORT_DESC, 1, 0,
#ifdef TOUCHSCREEN_ENABLE
					  hid_report_desc, sizeof(hid_report_desc), 0);
#else
					  hid->descriptor, hid->descriptor_size, 0);
#endif
	if (ret < 0) {
		printf("couldn't send HID descriptor to the android device\n");
		libusb_close(hid->handle);
		return -1;
	}

	pthread_create(&hid->rx_thread, NULL, receive_loop, NULL);

	return 0;
}
