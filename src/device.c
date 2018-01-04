
#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include "linux-adk.h"


static int parse_endpoint(const struct libusb_endpoint_descriptor * endpoint)
{
	printf("	Endpoint\n");
	printf("		bmAttributes: 0x%x\n", endpoint->bmAttributes);
	printf("		bEndpointAddress: 0x%x\n", endpoint->bEndpointAddress);
	return 0;
}
/**
 * @brief parse_altsetting
 * @param interface
 */
static int parse_altsetting(const struct libusb_interface_descriptor *interface)
{
    int i;
#if 1
    printf("    Interface:\n");
    printf("      bInterfaceNumber:   %d\n", interface->bInterfaceNumber);
    printf("      bAlternateSetting:  %d\n", interface->bAlternateSetting);
    printf("      bNumEndpoints:      %d\n", interface->bNumEndpoints);
    printf("      bInterfaceClass:    %d\n", interface->bInterfaceClass);
    printf("      bInterfaceSubClass: %d\n", interface->bInterfaceSubClass);
    printf("      bInterfaceProtocol: %d\n", interface->bInterfaceProtocol);
    printf("      iInterface:         %d\n", interface->iInterface);
#endif

    //ADB devices
    if( interface->bInterfaceClass    == 255 &&
        interface->bInterfaceSubClass == 66  &&
        interface->bInterfaceProtocol ==  1) {
        printf("Android link attached\n");
        return 2;
    }
    //Iphone network device
    if( interface->bInterfaceClass    == 255 &&
        interface->bInterfaceSubClass == 254  &&
        interface->bInterfaceProtocol == 2) {
    }

    for (i = 0; i < interface->bNumEndpoints; i++)
		parse_endpoint(&interface->endpoint[i]);
    return 0;
}



/**
 * @brief parse_interface
 * @param interface
 */
static int parse_interface(const struct libusb_interface *interface)
{
    int ret;
    int i;

    for (i = 0; i < interface->num_altsetting; i++) {
        ret = parse_altsetting(&interface->altsetting[i]);
        if(ret == 2) return ret;
    }
    return 0;
}


/**
 * @brief parse_configuration
 * @param config
 */
int parse_configuration(struct libusb_config_descriptor *config)
{
    int i;
    int ret;
#if 1
    printf("  Configuration:\n");
    printf("    wTotalLength:         %d\n", config->wTotalLength);
    printf("    bNumInterfaces:       %d\n", config->bNumInterfaces);
    printf("    bConfigurationValue:  %d\n", config->bConfigurationValue);
    printf("    iConfiguration:       %d\n", config->iConfiguration);
    printf("    bmAttributes:         %02xh\n", config->bmAttributes);
    printf("    MaxPower:             %d\n", config->MaxPower);
#endif

    for (i = 0; i < config->bNumInterfaces; i++) {
        ret = parse_interface(&config->interface[i]);
        if(ret == 2) return ret;

    }
    return 0;
}


/**
 * @brief parse_devs 
 * @return 
 */
int parse_devs(int vid)
{
    libusb_device **devs;
    struct libusb_device_descriptor desc;
    struct libusb_config_descriptor *config;
    ssize_t cnt;
    int ret, i;

    ret = libusb_init(NULL);
    if (ret < 0)
        return ret;

    cnt = libusb_get_device_list(NULL, &devs);
    if (cnt < 0)
        return (int)cnt;

    for (i = 0; devs[i]; ++i) {

        ret = libusb_get_device_descriptor(devs[i], &desc);
        if (ret < 0) {
            printf( "failed to get device descriptori\n");
            return -1;
        }

        printf("device attached: %04x:%04x\n", desc.idVendor, desc.idProduct);
		if(vid == 0 || desc.idVendor == vid ) {
			printf("vid(0x%x) attached \n",desc.idVendor);
			ret = libusb_get_active_config_descriptor(devs[i], &config);
			if (ret != LIBUSB_SUCCESS) {
				printf("Failed libusb_get_active_config_descriptor\n");
				ret = -1;
				goto EXIT;
			}
			ret = parse_configuration(config);
		}
			
    }
EXIT:
    libusb_free_device_list(devs, 1);
    return ret;
}

/**
 * @brief open_special_vid_dev 
 * @param vid
 * @param handle
 * @return 
 */
int open_special_vid_dev(int vid, accessory_t * acc)
{
    libusb_device **devs;
	static struct libusb_device_handle *handle;
    struct libusb_device_descriptor desc;
    struct libusb_config_descriptor *config;
    ssize_t cnt;
    int ret, i;

    //ret = libusb_init(NULL);
    //if (ret < 0)
        //return ret;

    cnt = libusb_get_device_list(NULL, &devs);
    if (cnt < 0)
        return (int)cnt;

    for (i = 0; devs[i]; ++i) {

        ret = libusb_get_device_descriptor(devs[i], &desc);
        if (ret < 0) {
            printf( "failed to get device descriptori\n");
            return -1;
        }

        printf("device attached: %04x:%04x\n", desc.idVendor, desc.idProduct);
		if(desc.idVendor == vid ) {
			printf("vid(0x%x) attached \n",desc.idVendor);
			ret = libusb_get_active_config_descriptor(devs[i], &config);
			if (ret != LIBUSB_SUCCESS) {
				printf("Failed libusb_get_active_config_descriptor\n");
				ret = -1;
				goto EXIT;
			}
			ret = parse_configuration(config);

			libusb_ref_device(devs[i]);
			ret = libusb_open(devs[i],&handle);
            if (ret != LIBUSB_SUCCESS) {
                printf( "Failed libusb_open(%d)\n",ret);
                ret = -1;
                goto EXIT;
            } else {
				acc->handle = handle;
                printf( "OK libusb_open get handle\n");
				return 0;
			}
		}
			
    }
EXIT:
    libusb_free_device_list(devs, 1);
    return ret;
}


