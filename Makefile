all: usb.c
	gcc usb.c -lusb-1.0 -o usb
clean:
	rm usb
