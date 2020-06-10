#include <lcd.h>
#include <serutil.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

char msgShortTest[] PROGMEM = "Short Circuit Check\r\n";
char msgShortFound[] PROGMEM = " Short Found\r\n";
char msgDone[] PROGMEM = "Test Done\r\n";
char msgCRLF[] PROGMEM = "\r\n";
char msgSequence[] PROGMEM = "Sequence\r\n";

#define DDR_LCD_PORT DDRA  

static void shortCheck(void)
{
    unsigned char count;
    uart_send_sync (msgShortTest, sizeof(msgShortTest));
    for (count = 0; count < 8; count++)
    {
        uart_send_char('0' + count);
        uart_send_char(' ');
        DDR_LCD_PORT = _BV(count);
        LCD_PORT = _BV(count);
        uart_send_hex_byte(PINA);
        uart_send_char(' ');
        _delay_ms(100);
        if (PINA != _BV(count))
        {
            uart_send_sync(msgShortFound, sizeof(msgShortFound));
        }
    }
    DDR_LCD_PORT = 0;
    uart_send_sync(msgDone, sizeof(msgDone));
}

static void sequence(void)
{
    unsigned char count;
//    DDRA = 0xff;

    uart_send_sync (msgSequence, sizeof(msgSequence));

    for (count = 0; count < 8; count++)
    {
        uart_send_char('0' + count);
        uart_send_sync (msgCRLF, sizeof(msgCRLF));
        _delay_ms(1000);
        DDRA = _BV(count);
        PORTA = _BV(count);
    }
    PORTA = 0;
    DDRA = 0x00;
    uart_send_sync (msgDone, sizeof(msgDone));
}

void testMode(void)
{
        while (1)
        {
                sequence();
                _delay_ms(1000);
        }
}
