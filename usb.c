#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include "usb.h"
#include <malloc.h>
/*
 bmRequestType
| 0:4                              | 5:6                     | 7 
recipient libusb_request_recipient | libusb_request_type     | data transfer direction
*/

#define TRANS_TYPE_READ 0xc0
#define EEPROM 0xa2
#define TIMEOUT 1000
#define LOCATION (1<<4)
libusb_device **list;
libusb_device *found = NULL;
uint16_t lang;
struct libusb_device_descriptor desc;

void main(int argc, char *argv[]){
struct libusb_context *context;
int ret = libusb_init(&context);
if(ret<0){error();}
libusb_set_debug(context,3); //max level
if(argc==3){
int vid,pid;
vid=strtoul(argv[1],NULL,0);
pid=strtoul(argv[2],NULL,0);
printf("\n vendorId:%d productId%d",vid, pid);
read_device(vid,pid);
return;

}
list_devices();

libusb_exit(context);
}


void read_data(libusb_device *device){
    libusb_device_handle *dev_handle=NULL;
	libusb_open(device, &dev_handle);
	if(desc.iManufacturer){
	unsigned char data[500];
	libusb_get_string_descriptor_ascii(dev_handle, desc.iManufacturer, data, 500);
	printf("\n Manufacturer: %s", data);
	}  
	if(desc.iProduct){
	unsigned char data[500];
	libusb_get_string_descriptor_ascii(dev_handle, desc.iProduct, data, 500);
	printf("\n Product: %s", data);
	}
	if(desc.iSerialNumber){
	unsigned char data[500];
	libusb_get_string_descriptor_ascii(dev_handle, desc.iSerialNumber, data, 500);
	printf("\n SerialNumber: %s", data);
	}
 	printf("\n");
	libusb_close(dev_handle);	

}


void read_device(int vid, int pid){
struct libusb_device_handle *dev_handle;
// Vendor=0489 ProdID=d601
vid=0x0489; pid=0xd601;   	
dev_handle=libusb_open_device_with_vid_pid(NULL,vid,pid);
	
unsigned char data[500];
libusb_get_string_descriptor_ascii(dev_handle, 1, data, 500);
printf("\n Manufacturer: %s", data);
libusb_get_string_descriptor_ascii(dev_handle, pid, data, 500);
printf("\n Product: %s", data);
libusb_get_string_descriptor_ascii(dev_handle, 0, data, 500);
printf("\n SerialNumber: %s", data);
int r = libusb_claim_interface(dev_handle,0);
libusb_attach_kernel_driver(dev_handle,0);	
if (r!=0) {
printf("\nNot claimed! Code: [%d]",r);
}
unsigned char *buffer;
int length =400;
buffer = malloc(length);
libusb_control_transfer(dev_handle,TRANS_TYPE_READ,EEPROM,0,LOCATION,buffer,length,TIMEOUT);
printf("\nBuffer [%s]", buffer);
libusb_close(dev_handle);	


}

void list_devices(){
ssize_t cnt = libusb_get_device_list(NULL, &list);
int err = 0;
if (cnt < 0){  error(); }

int i;
for (i = 0; i < cnt; i++) {
    libusb_device *device = list[i];
    int ret = libusb_get_device_descriptor(device, &desc);
	if(ret !=  LIBUSB_SUCCESS ) {continue;}
	printf("\n device: %d [%p] Bus: %04x Device: %04x", i, device, libusb_get_bus_number(device), libusb_get_device_address(device));
	printf("\n idVendor: %x",  desc.idVendor);
	printf("\n idProduct: %x",  desc.idProduct);
	printf("\n usb: %x",  desc.bcdUSB );
	printf("\n type: %x",  desc.bDescriptorType );
	printf("\n manifacture: %x",  desc.iManufacturer);
	printf("\n prod: %x",  desc.iProduct );
	printf("\n SerialNumber: %x",  desc.iSerialNumber );
	read_data(device);

	
}
libusb_free_device_list (list, cnt);
}
