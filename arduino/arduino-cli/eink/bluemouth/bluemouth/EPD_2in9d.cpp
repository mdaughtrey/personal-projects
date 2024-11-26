/*****************************************************************************
* | File      	:   EPD_2in9d.c
* | Author      :   Waveshare team
* | Function    :   2.9inch e-paper d
* | Info        :
*----------------
* |	This version:   V2.0
* | Date        :   2019-06-12
* | Info        :
* -----------------------------------------------------------------------------
* V3.0(2019-06-12):
* 1.Change:
*    lut_vcomDC[]  => EPD_2IN9D_lut_vcomDC[]
*    lut_ww[] => EPD_2IN9D_lut_ww[]
*    lut_bw[] => EPD_2IN9D_lut_bw[]
*    lut_wb[] => EPD_2IN9D_lut_wb[]
*    lut_bb[] => EPD_2IN9D_lut_bb[]
*    lut_vcom1[] => EPD_2IN9D_lut_vcom1[]
*    lut_ww1[] => EPD_2IN9D_lut_ww1[]
*    lut_bw1[] => EPD_2IN9D_lut_bw1[]
*    lut_wb1[] => EPD_2IN9D_lut_wb1[]
*    lut_bb1[] => EPD_2IN9D_lut_bb1[]
*    EPD_Reset() => EPD_2IN9D_Reset()
*    EPD_SendCommand() => EPD_2IN9D_SendCommand()
*    EPD_SendData() => EPD_2IN9D_SendData()
*    EPD_WaitUntilIdle() => EPD_2IN9D_ReadBusy()
*    EPD_SetFullReg() => EPD_2IN9D_SetFullReg()
*    EPD_SetPartReg() => EPD_2IN9D_SetPartReg()
*    EPD_TurnOnDisplay() => EPD_2IN9D_TurnOnDisplay()
*    EPD_Init() => EPD_2IN9D_Init()
*    EPD_Clear() => EPD_2IN9D_Clear()
*    EPD_Display() => EPD_2IN9D_Display()
*    EPD_Sleep() => EPD_2IN9D_Sleep()
*
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include <Wire.h>
#include <Arduino.h>
#include "EPD_2in9d.h"
//#include "Debug.h"
#define EPD_SCK_PIN  13
#define EPD_MOSI_PIN 14
#define EPD_CS_PIN   15
#define EPD_RST_PIN  26
#define EPD_DC_PIN   27
#define EPD_BUSY_PIN 25

#define GPIO_PIN_SET   1
#define GPIO_PIN_RESET 0

/**
 * GPIO read and write
**/
#define DEV_Digital_Write(_pin, _value) digitalWrite(_pin, _value == 0? LOW:HIGH)
#define DEV_Digital_Read(_pin) digitalRead(_pin)

/**
 * full screen update LUT
**/
const unsigned char EPD_2IN9D_lut_vcomDC[] = {
    0x00, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x60, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,
};
const unsigned char EPD_2IN9D_lut_ww[] = {
    0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
    0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char EPD_2IN9D_lut_bw[] = {
    0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x0F, 0x0F, 0x00, 0x00, 0x03,
    0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
    0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char EPD_2IN9D_lut_wb[] = {
    0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char EPD_2IN9D_lut_bb[] = {
    0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


/**
 * partial screen update LUT
**/
const unsigned char EPD_2IN9D_lut_vcom1[] = {
    0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    ,0x00, 0x00,
};
const unsigned char EPD_2IN9D_lut_ww1[] = {
    0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char EPD_2IN9D_lut_bw1[] = {
    0x80, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char EPD_2IN9D_lut_wb1[] = {
    0x40, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char EPD_2IN9D_lut_bb1[] = {
    0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void DEV_SPI_WriteByte(unsigned char data)
{
    //SPI.beginTransaction(spi_settings);
    digitalWrite(EPD_CS_PIN, GPIO_PIN_RESET);

    for (int i = 0; i < 8; i++)
    {
        if ((data & 0x80) == 0) digitalWrite(EPD_MOSI_PIN, GPIO_PIN_RESET); 
        else                    digitalWrite(EPD_MOSI_PIN, GPIO_PIN_SET);

        data <<= 1;
        digitalWrite(EPD_SCK_PIN, GPIO_PIN_SET);     
        digitalWrite(EPD_SCK_PIN, GPIO_PIN_RESET);
    }

    //SPI.transfer(data);
    digitalWrite(EPD_CS_PIN, GPIO_PIN_SET);
    //SPI.endTransaction();	
}

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void EPD_2IN9D_Reset(void)
{
    digitalWrite(EPD_RST_PIN, 1);
    delay(200);
    digitalWrite(EPD_RST_PIN, 0);
    delay(200);
    digitalWrite(EPD_RST_PIN, 1);
    delay(200);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void EPD_2IN9D_SendCommand(unsigned char Reg)
{
    digitalWrite(EPD_DC_PIN, 0);
    digitalWrite(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    digitalWrite(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void EPD_2IN9D_SendData(unsigned char Data)
{
    digitalWrite(EPD_DC_PIN, 1);
    digitalWrite(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    digitalWrite(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void EPD_2IN9D_ReadBusy(void)
{
    //Debug("e-Paper busy\r\n");
    unsigned char busy;
    do {
        EPD_2IN9D_SendCommand(0x71);
        busy = DEV_Digital_Read(EPD_BUSY_PIN);
        busy =!(busy & 0x01);
    } while(busy);
    delay(200);
    //Debug("e-Paper busy release\r\n");
}

/******************************************************************************
function :	LUT download
parameter:
******************************************************************************/
static void EPD_SetFullReg(void)
{
    EPD_2IN9D_SendCommand(0X50);			//VCOM AND DATA INTERVAL SETTING
    EPD_2IN9D_SendData(0xb7);		//WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

    unsigned int count;
    EPD_2IN9D_SendCommand(0x20);
    for(count=0; count<44; count++) {
        EPD_2IN9D_SendData(EPD_2IN9D_lut_vcomDC[count]);
    }

    EPD_2IN9D_SendCommand(0x21);
    for(count=0; count<42; count++) {
        EPD_2IN9D_SendData(EPD_2IN9D_lut_ww[count]);
    }

    EPD_2IN9D_SendCommand(0x22);
    for(count=0; count<42; count++) {
        EPD_2IN9D_SendData(EPD_2IN9D_lut_bw[count]);
    }

    EPD_2IN9D_SendCommand(0x23);
    for(count=0; count<42; count++) {
        EPD_2IN9D_SendData(EPD_2IN9D_lut_wb[count]);
    }

    EPD_2IN9D_SendCommand(0x24);
    for(count=0; count<42; count++) {
        EPD_2IN9D_SendData(EPD_2IN9D_lut_bb[count]);
    }
}

/******************************************************************************
function :	LUT download
parameter:
******************************************************************************/
static void EPD_2IN9D_SetPartReg(void)
{
    EPD_2IN9D_SendCommand(0x82);			//vcom_DC setting
    EPD_2IN9D_SendData(0x00);
    EPD_2IN9D_SendCommand(0X50);
    EPD_2IN9D_SendData(0xb7);
	
    unsigned int count;
    EPD_2IN9D_SendCommand(0x20);
    for(count=0; count<44; count++) {
        EPD_2IN9D_SendData(EPD_2IN9D_lut_vcom1[count]);
    }

    EPD_2IN9D_SendCommand(0x21);
    for(count=0; count<42; count++) {
        EPD_2IN9D_SendData(EPD_2IN9D_lut_ww1[count]);
    }

    EPD_2IN9D_SendCommand(0x22);
    for(count=0; count<42; count++) {
        EPD_2IN9D_SendData(EPD_2IN9D_lut_bw1[count]);
    }

    EPD_2IN9D_SendCommand(0x23);
    for(count=0; count<42; count++) {
        EPD_2IN9D_SendData(EPD_2IN9D_lut_wb1[count]);
    }

    EPD_2IN9D_SendCommand(0x24);
    for(count=0; count<42; count++) {
        EPD_2IN9D_SendData(EPD_2IN9D_lut_bb1[count]);
    }
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void EPD_2IN9D_TurnOnDisplay(void)
{
    EPD_2IN9D_SendCommand(0x12);		 //DISPLAY REFRESH
    delay(10);     //!!!The delay here is necessary, 200uS at least!!!

    EPD_2IN9D_ReadBusy();
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EPD_2IN9D_Init(void)
{
    EPD_2IN9D_Reset();

    EPD_2IN9D_SendCommand(0x01);	//POWER SETTING
    EPD_2IN9D_SendData(0x03);
    EPD_2IN9D_SendData(0x00);
    EPD_2IN9D_SendData(0x2b);
    EPD_2IN9D_SendData(0x2b);
    EPD_2IN9D_SendData(0x03);

    EPD_2IN9D_SendCommand(0x06);	//boost soft start
    EPD_2IN9D_SendData(0x17);     //A
    EPD_2IN9D_SendData(0x17);     //B
    EPD_2IN9D_SendData(0x17);     //C

    EPD_2IN9D_SendCommand(0x04);
    EPD_2IN9D_ReadBusy();

    EPD_2IN9D_SendCommand(0x00);	//panel setting
    EPD_2IN9D_SendData(0xbf);     //LUT from OTP，128x296
    EPD_2IN9D_SendData(0x0e);     //VCOM to 0V fast

    EPD_2IN9D_SendCommand(0x30);	//PLL setting
    EPD_2IN9D_SendData(0x3a);     // 3a 100HZ   29 150Hz 39 200HZ	31 171HZ

    EPD_2IN9D_SendCommand(0x61);	//resolution setting
    EPD_2IN9D_SendData(EPD_2IN9D_WIDTH);
    EPD_2IN9D_SendData((EPD_2IN9D_HEIGHT >> 8) & 0xff);
    EPD_2IN9D_SendData(EPD_2IN9D_HEIGHT & 0xff);

    EPD_2IN9D_SendCommand(0x82);	//vcom_DC setting
    EPD_2IN9D_SendData(0x28);
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_2IN9D_Clear(void)
{
    unsigned short int Width, Height;
    Width = (EPD_2IN9D_WIDTH % 8 == 0)? (EPD_2IN9D_WIDTH / 8 ): (EPD_2IN9D_WIDTH / 8 + 1);
    Height = EPD_2IN9D_HEIGHT;

    EPD_2IN9D_SendCommand(0x10);
    for (unsigned short int j = 0; j < Height; j++) {
        for (unsigned short int i = 0; i < Width; i++) {
            EPD_2IN9D_SendData(0x00);
        }
    }

    EPD_2IN9D_SendCommand(0x13);
    for (unsigned short int j = 0; j < Height; j++) {
        for (unsigned short int i = 0; i < Width; i++) {
            EPD_2IN9D_SendData(0xFF);
        }
    }

    EPD_SetFullReg();
    EPD_2IN9D_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_2IN9D_Display(const unsigned char *Image)
{
    unsigned short int Width, Height;
    Width = (EPD_2IN9D_WIDTH % 8 == 0)? (EPD_2IN9D_WIDTH / 8 ): (EPD_2IN9D_WIDTH / 8 + 1);
    Height = EPD_2IN9D_HEIGHT;

     EPD_2IN9D_SendCommand(0x10);
     for (unsigned short int j = 0; j < Height; j++) {
         for (unsigned short int i = 0; i < Width; i++) {
             EPD_2IN9D_SendData(0x00);
         }
     }
    // Dev_Delay_ms(10);

    EPD_2IN9D_SendCommand(0x13);
    for (unsigned short int j = 0; j < Height; j++) {
        for (unsigned short int i = 0; i < Width; i++) {
            EPD_2IN9D_SendData(Image[i + j * Width]);
        }
    }
    // Dev_Delay_ms(10);

    EPD_SetFullReg();
    EPD_2IN9D_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_2IN9D_DisplayPart(const unsigned char *Image)
{
    /* Set partial Windows */
    EPD_2IN9D_SetPartReg();
    EPD_2IN9D_SendCommand(0x91);		//This command makes the display enter partial mode
    EPD_2IN9D_SendCommand(0x90);		//resolution setting
    EPD_2IN9D_SendData(0);           //x-start
    EPD_2IN9D_SendData(EPD_2IN9D_WIDTH - 1);       //x-end

    EPD_2IN9D_SendData(0);
    EPD_2IN9D_SendData(0);     //y-start
    EPD_2IN9D_SendData(EPD_2IN9D_HEIGHT / 256);
    EPD_2IN9D_SendData(EPD_2IN9D_HEIGHT % 256 - 1);  //y-end
    EPD_2IN9D_SendData(0x28);

    unsigned short int Width;
    Width = (EPD_2IN9D_WIDTH % 8 == 0)? (EPD_2IN9D_WIDTH / 8 ): (EPD_2IN9D_WIDTH / 8 + 1);
    
    /* send data */
    EPD_2IN9D_SendCommand(0x13);
    for (unsigned short int j = 0; j < EPD_2IN9D_HEIGHT; j++) {
        for (unsigned short int i = 0; i < Width; i++) {
            EPD_2IN9D_SendData(Image[i + j * Width]);
        }
    }

    /* Set partial refresh */    
    EPD_2IN9D_TurnOnDisplay();
}


/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_2IN9D_Sleep(void)
{
    EPD_2IN9D_SendCommand(0X50);
    EPD_2IN9D_SendData(0xf7);
    EPD_2IN9D_SendCommand(0X02);  	//power off
    EPD_2IN9D_ReadBusy();
    EPD_2IN9D_SendCommand(0X07);  	//deep sleep
    EPD_2IN9D_SendData(0xA5);
}
