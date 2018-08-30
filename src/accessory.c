/*
 * Linux ADK - accessory.c
 *
 * Copyright (C) 2013 - Gary Bisson <bisson.gary@gmail.com>
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
#include <unistd.h>
#include <string.h>

#include <libusb-1.0/libusb.h>

#include "linux-adk.h"
#include "hid.h"

static unsigned char valid_mouse_report_descriptor [] = 
{
	//通用桌面设备
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    //鼠标
    0x09, 0x02,                    // USAGE (Mouse)
    //集合
    0xa1, 0x01,                    // COLLECTION (Application)
    //指针设备
    0x09, 0x01,                    //   USAGE (Pointer)
    //集合
    0xa1, 0x00,                    //   COLLECTION (Physical)
    //按键
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    //使用最小值1
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    //使用最大值3。1表示左键，2表示右键，3表示中键
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    //逻辑最小值0
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    //逻辑最大值1
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    //数量为3
    0x95, 0x03,                    //     REPORT_COUNT (3)
    //大小为1bit
    0x75, 0x01,                    //     REPORT_SIZE (1)
    //输入，变量，数值，绝对值
    //以上3个bit分别表示鼠标的三个按键情况，最低位（bit-0）为左键
    //bit-1为右键，bit-2为中键，按下时对应的位值为1，释放时对应的值为0
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    //填充5个bit，补足一个字节
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    //用途页为通用桌面
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    //用途为X
    0x09, 0x30,                    //     USAGE (X)
    //用途为Y
    0x09, 0x31,                    //     USAGE (Y)
    //用途为滚轮
    0x09, 0x38,                    //     USAGE (Wheel)
    //逻辑最小值为-127
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    //逻辑最大值为+127
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    //大小为8个bits
    0x75, 0x08,                    //     REPORT_SIZE (8)
    //数量为3个，即分别代表x,y,滚轮
    0x95, 0x03,                    //     REPORT_COUNT (3)
    //输入，变量，值，相对值
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    //关集合
    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
};

void accessory_main(accessory_t * acc)
{
	int ret = 0;
	hid_device_t hid;

	//memcpy(hid.descriptor,
			//valid_mouse_report_descriptor,
			//sizeof(valid_mouse_report_descriptor));

	//hid.descriptor_size =  sizeof(valid_mouse_report_descriptor);
	//hid.handle = acc->handle;
	//hid.endpoint_in = 
#if 1
	/* In case of Audio/HID support */
	/*if (acc->pid >= AOA_AUDIO_PID) {*/
		/*[> Audio warning <]*/
		/*printf("Device should now be recognized as valid ALSA card...\n");*/
		/*printf("  => arecord -l\n");*/

		
	/*}*/
/* HID handling */
		if(search_hid(&hid) == 0) {
			/*register_hid_callback(acc, &hid);*/
			send_hid_descriptor(acc, &hid);
		}
#else

	/* HID handling */
	if(search_hid(&hid) == 0) {
		register_hid_callback(acc, &hid);

		set_hid_logical_range(1080,1920);
		set_hid_physical_range(0,0);
		send_hid_descriptor(acc, &hid);
	}
#endif

	/* If we have an accessory interface */
	if ((acc->pid != AOA_AUDIO_ADB_PID) && (acc->pid != AOA_AUDIO_PID)) {
		uint8_t acc_buf[512];
		int transferred, i;
		int errors = 20;
#if 0
		printf("detach..\n");
		libusb_set_auto_detach_kernel_driver(acc->handle,1);

		
		usleep(10000);
		printf("claiming ...\n");
#endif
#if 1
		/* Claiming first (accessory )interface from the opened device */
		ret =
		    libusb_claim_interface(acc->handle,
					   AOA_ACCESSORY_INTERFACE);
		if (ret != 0) {
			printf("Error %d claiming interface...\n", ret);
			return;
		}
#endif
		printf("Snooping loop\n");

		static int t = 0;
		/* Snooping loop; Display every data received from device */
		while (!stop_acc) {
			memset(acc_buf,++t % 512,sizeof(acc_buf));

			ret =
			    libusb_bulk_transfer(acc->handle,
						 AOA_ACCESSORY_EP_OUT, acc_buf,
						 sizeof(acc_buf), &transferred,
						 500);
			printf("EP_OUT ret(%d)\n",ret);

			ret =
			    libusb_bulk_transfer(acc->handle,

						 AOA_ACCESSORY_EP_IN, acc_buf,
						 sizeof(acc_buf), &transferred,
						 500);
#if 1
			if (ret < 0) {
				if (ret == LIBUSB_ERROR_TIMEOUT) {
					printf("timeout...\n");
					continue;
				}
				printf("bulk transfer error %d\n", ret);
				if (--errors == 0)
					break;
				else
					sleep(1);
			}
#endif
#if 0

			printf("Received %d bytes\n", transferred);
			for (i = 0; i < transferred;) {
				printf("%#2.2x ", acc_buf[i++]);
				if (!(i % 8))
					printf("\n");
			}
			printf("\n");
#endif
		}
	}

	if ((acc->pid >= AOA_AUDIO_PID) && (hid.handle))
		pthread_join(hid.rx_thread, NULL);
}
