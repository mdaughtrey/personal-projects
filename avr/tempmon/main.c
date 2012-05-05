#include <avr/io.h> 
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <dmc40457.h>

#define SENSOR_SCK_PORT         PORTF
#define SENSOR_SCK_PIN          6
#define SENSOR_IO_PORT   PORTF
#define SENSOR_IO_PIN    7

#define SENSOR0_CS_PORT PORTE
#define SENSOR0_CS_PIN 7
#define SENSOR1_CS_PORT PORTB
#define SENSOR1_CS_PIN 4
#define SENSOR2_CS_PORT PORTB
#define SENSOR2_CS_PIN 5
#define SENSOR3_CS_PORT PORTB
#define SENSOR3_CS_PIN 6
#define SENSOR4_CS_PORT PORTB
#define SENSOR4_CS_PIN 7
#define SENSOR5_CS_PORT PORTG
#define SENSOR5_CS_PIN 3
#define SENSOR6_CS_PORT PORTG
#define SENSOR6_CS_PIN 4
#define SENSOR7_CS_PORT PORTC
#define SENSOR7_CS_PIN 0

#define SENSOR_SCKHIGH SENSOR_SCK_PORT |= _BV(SENSOR_SCK_PIN);
#define SENSOR_SCKLOW SENSOR_SCK_PORT &= ~_BV(SENSOR_SCK_PIN);

#define SENSOR0_CSHIGH SENSOR0_CS_PORT |= _BV(SENSOR0_CS_PIN);
#define SENSOR0_CSLOW SENSOR0_CS_PORT &= ~_BV(SENSOR0_CS_PIN);
#define SENSOR1_CSHIGH SENSOR1_CS_PORT |= _BV(SENSOR1_CS_PIN);
#define SENSOR1_CSLOW SENSOR1_CS_PORT &= ~_BV(SENSOR1_CS_PIN);
#define SENSOR2_CSHIGH SENSOR2_CS_PORT |= _BV(SENSOR2_CS_PIN);
#define SENSOR2_CSLOW SENSOR2_CS_PORT &= ~_BV(SENSOR2_CS_PIN);
#define SENSOR3_CSHIGH SENSOR3_CS_PORT |= _BV(SENSOR3_CS_PIN);
#define SENSOR3_CSLOW SENSOR3_CS_PORT &= ~_BV(SENSOR3_CS_PIN);
#define SENSOR4_CSHIGH SENSOR4_CS_PORT |= _BV(SENSOR4_CS_PIN);
#define SENSOR4_CSLOW SENSOR4_CS_PORT &= ~_BV(SENSOR4_CS_PIN);
#define SENSOR5_CSHIGH SENSOR5_CS_PORT |= _BV(SENSOR5_CS_PIN);
#define SENSOR5_CSLOW SENSOR5_CS_PORT &= ~_BV(SENSOR5_CS_PIN);
#define SENSOR6_CSHIGH SENSOR6_CS_PORT |= _BV(SENSOR6_CS_PIN);
#define SENSOR6_CSLOW SENSOR6_CS_PORT &= ~_BV(SENSOR6_CS_PIN);
#define SENSOR7_CSHIGH SENSOR7_CS_PORT |= _BV(SENSOR7_CS_PIN);
#define SENSOR7_CSLOW SENSOR7_CS_PORT &= ~_BV(SENSOR7_CS_PIN);

unsigned short sensorData[8];

void sensorInit(void)
{
    DDRE |= _BV(SENSOR0_CS_PIN);
    PORTE |= _BV(SENSOR0_CS_PIN); 
    DDRB = 0xf0; // 1111 0000 Sensors 1,2,3,4
    PORTB |= 0xf0;
    DDRG |= _BV(SENSOR5_CS_PIN) | _BV(SENSOR6_CS_PIN);
    PORTG |= _BV(SENSOR5_CS_PIN) | _BV(SENSOR6_CS_PIN);
    DDRC |= _BV(SENSOR7_CS_PIN);
    PORTC |= _BV(SENSOR7_CS_PIN);
}

unsigned short readSensorRaw(unsigned char which)
{
    unsigned short data = 0;
    unsigned char count;
    SENSOR_SCKHIGH;
    switch (which)
    {
            case 0: SENSOR0_CSLOW; break;
            case 1: SENSOR1_CSLOW; break;
            case 2: SENSOR2_CSLOW; break;
            case 3: SENSOR3_CSLOW; break;
            case 4: SENSOR4_CSLOW; break;
            case 5: SENSOR5_CSLOW; break;
            case 6: SENSOR6_CSLOW; break;
            case 7: SENSOR7_CSLOW; break;
    }
    for (count = 0; count < 16; count++)
    {
        SENSOR_SCKLOW;
        _delay_us(100);
        data |= ((PINF & _BV(SENSOR_IO_PIN)) >> SENSOR_IO_PIN);
        SENSOR_SCKHIGH;
        data <<= 1;
    }
    switch (which)
    {
            case 0: SENSOR0_CSHIGH; break;
            case 1: SENSOR1_CSHIGH; break;
            case 2: SENSOR2_CSHIGH; break;
            case 3: SENSOR3_CSHIGH; break;
            case 4: SENSOR4_CSHIGH; break;
            case 5: SENSOR5_CSHIGH; break;
            case 6: SENSOR6_CSHIGH; break;
            case 7: SENSOR7_CSHIGH; break;
    }
    return data;
}

unsigned short readSensor(unsigned char which)
{
    unsigned short data = 0;
    char count;
    SENSOR_SCKLOW;
    switch (which)
    {
            case 0: SENSOR0_CSLOW; break;
            case 1: SENSOR1_CSLOW; break;
            case 2: SENSOR2_CSLOW; break;
            case 3: SENSOR3_CSLOW; break;
            case 4: SENSOR4_CSLOW; break;
            case 5: SENSOR5_CSLOW; break;
            case 6: SENSOR6_CSLOW; break;
            case 7: SENSOR7_CSLOW; break;
    }
    for (count = 0; count < 13; count++)
    {
        data <<= 1;
        SENSOR_SCKHIGH;
        _delay_ms(1);
        data |= ((PINF & _BV(SENSOR_IO_PIN)) >> SENSOR_IO_PIN);
        SENSOR_SCKLOW;
    }
    switch (which)
    {
            case 0: SENSOR0_CSHIGH; break;
            case 1: SENSOR1_CSHIGH; break;
            case 2: SENSOR2_CSHIGH; break;
            case 3: SENSOR3_CSHIGH; break;
            case 4: SENSOR4_CSHIGH; break;
            case 5: SENSOR5_CSHIGH; break;
            case 6: SENSOR6_CSHIGH; break;
            case 7: SENSOR7_CSHIGH; break;
    }
    data &= 0x0fff;
    return data;
}

void toBin(unsigned short data)
{
    char ii;
    for (ii = 15; ii >= 0; ii--)
    {
        if (data & (1 << ii))
            lcdChar('1');
        else
            lcdChar('0');
    }
}

unsigned char displayTemp(unsigned short data)
{
        char buffer[6];
        if (data & 0x01) data |= 0x02; // 1/16th of a degree
        if (data & 0x02) data |= 0x04; // 1/8th
        if (data & 0x04) data |= 0x08; // 1/4
        if (data & 0x08) data |= 0x10; // 1/2
        if (data & 0x10) data |= 0x20; // 1
        data >>= 4;
        data *= 18;
        data += 320;
        data /= 10;
        itoa(data, buffer, 10);
        lcdString(buffer);
        lcdChar(0xdf);
        lcdChar('F');
        lcdChar(' ');
        lcdChar(' ');
        return data & 0xff;
}

inline void lcdLoop(void)
{
        lcdLineCol(0,0,0);
        displayTemp(sensorData[0] = readSensor(0));
        lcdLineCol(0,10,0);
        displayTemp(sensorData[1] = readSensor(1));
        lcdLineCol(0,20,0);
        displayTemp(sensorData[2] = readSensor(2));
        lcdLineCol(0,30,0);
        displayTemp(sensorData[3] = readSensor(3));
        lcdLineCol(1,0,0);
        displayTemp(sensorData[4] = readSensor(4));
        lcdLineCol(1,10,0);
        displayTemp(sensorData[5] = readSensor(5));
        lcdLineCol(1,20,0);
        displayTemp(sensorData[6] = readSensor(6));
        lcdLineCol(1,30,0);
        displayTemp(sensorData[7] = readSensor(7));
}

int main(void)
{
    lcdInit();
    lanInit();
    sensorInit();
    while (1)
    {   
            lcdLoop();
            lanLoop();
    }
    return 0;
}
