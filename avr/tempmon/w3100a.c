#include <w3100a.h>
#include <dmc40457.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

typedef union
{
        unsigned long asLong;
        unsigned short asShort[2];
        unsigned char asChar[4];
} MultiType;

extern unsigned short sensorData[8];

unsigned short lanSendCount;
MultiType txSendAddr;

char strIPHeader[] PROGMEM = "IP:";
char strMACHeader[] PROGMEM = "MAC:";
char strDrain[] PROGMEM = "Drain   ";
char strBlank[] PROGMEM = "        ";

inline void drwrite(unsigned char data)
{
    LAN_CSLOW;
    LAN_WRLOW;
    LAN_DATA_OUT;
    LAN_DATA = data;
    LAN_WRHIGH;
    LAN_CSHIGH;
    LAN_DATA_IN;
}

unsigned char drread(void)
{
    unsigned char data;
    LAN_CSLOW;
    LAN_RDLOW;
    LAN_RDHIGH;
    data = LAN_GETDATA;
    LAN_CSHIGH;
    return data;
}

void writeToRegister(unsigned int address, unsigned char data)
{
    LAN_ADDR_AR0;
    drwrite((address & 0xff00) >> 8);
    LAN_ADDR_AR1;
    drwrite(address & 0xff);
    LAN_ADDR_DR;
    drwrite(data);
}

unsigned long readFromRegister32(unsigned int address)
{
    MultiType result;

    LAN_ADDR_AR0;
    drwrite((address & 0xff00) >> 8);
    LAN_ADDR_AR1;
    drwrite(address & 0xff);
    LAN_ADDR_OR;
    drwrite(0x81); // autoincrement
    LAN_ADDR_DR;
    result.asChar[3] = drread();
    result.asChar[2] = drread();
    result.asChar[1] = drread();
    result.asChar[0] = drread();
    LAN_ADDR_OR;
    drwrite(0x80); // autoincrement off
    _delay_us(2);
    return result.asLong;
}

void writeToRegister32(unsigned int address,
                unsigned char data0, 
                unsigned char data1, 
                unsigned char data2, 
                unsigned char data3) 
{
    LAN_ADDR_AR0;
    drwrite((address & 0xff00) >> 8);
    LAN_ADDR_AR1;
    drwrite(address & 0xff);
    LAN_ADDR_OR;
    drwrite(0x81); // autoincrement
    LAN_ADDR_DR;
    drwrite(data0);
    drwrite(data1);
    drwrite(data2);
    drwrite(data3);
    LAN_ADDR_OR;
    drwrite(0x80); // autoincrement off
    _delay_us(2);
}

unsigned char readFromRegister(unsigned int address)
{
    LAN_ADDR_AR0;
    drwrite((address & 0xff00) >> 8);
    LAN_ADDR_AR1;
    drwrite(address & 0xff);
    LAN_ADDR_DR;
    return drread();
}
    

void dumpIPAddress(void)
{
    char buffer[4];
    unsigned char data;
    lcdStringPtr2(strIPHeader);
    itoa(readFromRegister(0x8e), buffer, 10);;
    lcdString2(buffer);
    lcdChar2('.');
    itoa(readFromRegister(0x8f), buffer, 10);
    lcdString2(buffer);
    lcdChar2('.');
    itoa(readFromRegister(0x90), buffer, 10);
    lcdString2(buffer);
    lcdChar2('.');
    itoa(readFromRegister(0x91), buffer, 10);
    lcdString2(buffer);
}

void dumpMACAddress(void)
{
    lcdStringPtr2(strMACHeader);
    byteToHex2(readFromRegister(0x88));
    lcdChar2(':');
    byteToHex2(readFromRegister(0x89));
    lcdChar2(':');
    byteToHex2(readFromRegister(0x8a));
    lcdChar2(':');
    byteToHex2(readFromRegister(0x8b));
    lcdChar2(':');
    byteToHex2(readFromRegister(0x8c));
    lcdChar2(':');
    byteToHex2(readFromRegister(0x8d));
}

void lanInit(void)
{
    DDR(LAN_CS_PORT) |= _BV(LAN_CS_PIN);
    DDR(LAN_RD_PORT) |= _BV(LAN_RD_PIN);
    DDR(LAN_WR_PORT) |= _BV(LAN_WR_PIN);
    DDR(LAN_A0_PORT) |= _BV(LAN_A0_PIN);
    DDR(LAN_A1_PORT) |= _BV(LAN_A1_PIN);
    DDR(LAN_RESET_PORT) |= _BV(LAN_RESET_PIN);
    LAN_RESETLOW;
    _delay_ms(1);
    LAN_CSHIGH;
    LAN_RDHIGH;
    LAN_WRHIGH;
    LAN_RESETHIGH;
    //
    // set up indirect interface
    //
    LAN_ADDR_OR;
    drwrite(0x80); // 1000 0000 IND_EN + MSB/LSB
    //
    // Software reset
    //
    writeToRegister(LAN_REG_C0_CR, 0x80);
    //
    // Set tx and rx buffer sizes
    // 
    writeToRegister(0x95, 0x03); // 8Kb rx buffer
    writeToRegister(0x96, 0x03); // 8Kb tx buffer
    //
    // Set up gateway address
    //
    writeToRegister32(0x80, 192, 168, 1, 1);
    //
    // Set up Subnet mask
    //
    writeToRegister32(0x84, 255, 255, 255, 0);
    //
    // MAC address
    //
    writeToRegister32(0x88, 0x00, 0x15, 0xf2, 0xbd);
    writeToRegister(0x8c, 0xdc);
    writeToRegister(0x8d, 0x93);
    //
    // IP Address
    // 
    writeToRegister32(0x8e, 192, 168, 1, 96);
    //
    // Sysinit
    // 
    writeToRegister(LAN_REG_C0_CR, 0x01);
    while (!(readFromRegister(LAN_REG_C0_ISR) & 0x01));
    dumpIPAddress();
    lcdChar2(' ');
    dumpMACAddress();
    lanSendCount = 0;
    txSendAddr.asLong = 0;
}

void drainQueue(void)
{
    MultiType txAckAddr;

    lcdLineCol(1, 30, 1);
    lcdStringPtr2(strDrain);
    do 
    {
            readFromRegister(LAN_REG_SHADOW_C0_TXA);
            _delay_us(2);
            txAckAddr.asLong = readFromRegister32(LAN_REG_C0_TXA_PR);
    } while (txSendAddr.asLong != txAckAddr.asLong);
    lcdLineCol(1, 30, 1);
    lcdStringPtr2(strBlank);
}

void sendData(void)
{
    MultiType txAckAddr;
    unsigned char ii;

    readFromRegister(LAN_REG_SHADOW_C0_TXA);
    _delay_us(2);
    txAckAddr.asLong = readFromRegister32(LAN_REG_C0_TXA_PR);
    readFromRegister(LAN_REG_SHADOW_C0_TXW);
    _delay_us(2);
   txSendAddr.asLong = readFromRegister32(LAN_REG_C0_TXW_PR) & 0x1fff;

    for (ii = 0; ii < 8; ii++)
    {
            char buffer[8];
            memset(buffer, 0, 8);
            itoa(sensorData[ii], buffer, 10);
            unsigned char jj;
            for (jj = 0; jj < 8 && buffer[jj]; jj++)
            {
                writeToRegister(0x4000 + txSendAddr.asLong++, buffer[jj]);
            }
            writeToRegister(0x4000 + txSendAddr.asLong++, ',');
    }
    writeToRegister(0x4000 + txSendAddr.asLong++, '\r');
    writeToRegister(0x4000 + txSendAddr.asLong++, '\n');
    lcdLineCol(1, 10, 1);
    toHex2(lanSendCount++);
    lcdChar2(':');
    toHex2(txSendAddr.asShort[0]);
    lcdChar2(':');
    toHex2(txAckAddr.asShort[0]);
    //
    // wait for pending sends
    while (readFromRegister(LAN_REG_C0_CR) & 0x20);
    writeToRegister32(LAN_REG_C0_TXW_PR, txSendAddr.asChar[3], txSendAddr.asChar[2],
        txSendAddr.asChar[1], txSendAddr.asChar[0]);
    writeToRegister(LAN_REG_C0_CR, 0x20); // send
    
}


void lanLoop(void)
{
    unsigned char socketState = readFromRegister(LAN_REG_C0_SSR);
    lcdLineCol(1, 0, 1);
    toHex2(socketState);
    switch (socketState)
    {
            case 0x00: // closed 
                    // set up tx read, write and ack memory pointers
                    writeToRegister(LAN_REG_C0_SOPR, 0x01); // SOCK_STREAM(TCP)
                    //
                    // This is the port number you want to make available for connection
                    // 
                    writeToRegister(LAN_REG_C0_SRCPORT0, 0x00);
                    writeToRegister(LAN_REG_C0_SRCPORT1, 23); // telnet
                    writeToRegister(LAN_REG_C0_CR, 0x02); // sockinit
                    unsigned char isr;
                    while (0 == (isr = readFromRegister(LAN_REG_C0_ISR)));
                    if (!(0x02 & isr))
                    {
                        break;
                    }
                    writeToRegister(LAN_REG_C0_ISR, 0);
                    writeToRegister(LAN_REG_C0_CR, 0x08); // enable listen
                    break;
        
            case 0x06: // established
                    sendData();
                    while (readFromRegister(LAN_REG_C0_CR) & 0x20);
                    writeToRegister(LAN_REG_C0_CR, 0x10); // close
                    break;

    }
}

