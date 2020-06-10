#ifndef INCLUDED_USBDRIVER_H
#define INCLUDED_USBDRIVER_H

enum {CLK, MOSI, MISO, RESET}; //pins

#define COMP_CMD_RDEEPROM 0x85
#define COMP_CMD_WREEPROM 0x86
#define COMP_CMD_RDEEPROM16 0x87
#define COMP_CMD_WREEPROM16 0x88


// these are specifically assigned to USBtiny courtesy of Adafruit Industries,
// if you need your own VID and PIDs you can get them for cheap from
// www.mecanique.co.uk so please don't reuse these. Thanks!

const unsigned int TINYISP_VENDORID = 0x1781;
const unsigned int TINYISP_PRODUCTID = 0x0c9f;

enum
{
	// Generic requests
	USBTINY_ECHO,		// echo test
	USBTINY_READ,		// read byte (wIndex:address)
	USBTINY_WRITE,		// write byte (wIndex:address, wValue:value)
	USBTINY_CLR,		// clear bit (wIndex:address, wValue:bitno)
	USBTINY_SET,		// set bit (wIndex:address, wValue:bitno)
	// Programming requests
	USBTINY_POWERUP,	// apply power (wValue:SCK-period, wIndex:RESET)
	USBTINY_POWERDOWN,	// remove power from chip
	USBTINY_SPI,		// issue SPI command (wValue:c1c0, wIndex:c3c2)
	USBTINY_POLL_BYTES,	// set poll bytes for write (wValue:p1p2)
	USBTINY_FLASH_READ,	// read flash (wIndex:address)
	USBTINY_FLASH_WRITE,	// write flash (wIndex:address, wValue:timeout)
	USBTINY_EEPROM_READ,	// read eeprom (wIndex:address)
	USBTINY_EEPROM_WRITE,	// write eeprom (wIndex:address, wValue:timeout)
	USBTINY_DDRWRITE,        // set port direction
	USBTINY_SPI1            // a single SPI command
};

enum
{
	RESET_LOW	= 0,
	RESET_HIGH	= 1,
	SCK_MIN		= 1,	// usec (target clock >= 4 MHz)
	SCK_MAX		= 250,	// usec (target clock >= 16 KHz)
	SCK_DEFAULT	= 10,	// usec (target clock >= 0.4 MHz)
	CHUNK_SIZE	= 128,	// must be power of 2 less than 256
	USB_TIMEOUT	= 500,	// msec
};

#endif // INCLUDED_USBDRIVER_H
