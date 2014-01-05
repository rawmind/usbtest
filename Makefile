all: usb.c usb.h
	gcc usb.c -lusb-1.0 -o usbtest
clean:
	rm usbtest
