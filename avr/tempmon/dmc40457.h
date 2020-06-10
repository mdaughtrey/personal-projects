#ifndef INCLUDE_DMC40457
#define INCLUDE_DMC40457
#include <avr/io.h>

#define LCD_PORT         PORTA        /**< port for the LCD lines   */
#define LCD_DATA0_PORT   PORTE     /**< port for 4bit data bit 0 */
#define LCD_DATA1_PORT   PORTE     /**< port for 4bit data bit 1 */
#define LCD_DATA2_PORT   PORTF     /**< port for 4bit data bit 2 */
#define LCD_DATA3_PORT   PORTF     /**< port for 4bit data bit 3 */
#define LCD_DATA0_PIN    3            /**< pin for 4bit data bit 0  */
#define LCD_DATA1_PIN    2            /**< pin for 4bit data bit 1  */
#define LCD_DATA2_PIN    0            /**< pin for 4bit data bit 2  */
#define LCD_DATA3_PIN    1            /**< pin for 4bit data bit 3  */
#define LCD_RS_PORT      PORTF     /**< port for RS line         */
#define LCD_RS_PIN       3            /**< pin  for RS line         */
#define LCD_RW_PORT      PORTF     /**< port for RW line         */
#define LCD_RW_PIN       2            /**< pin  for RW line         */
#define LCD_E_PORT       PORTF     /**< port for Enable line     */
#define LCD_E_PIN        4            /**< pin  for Enable line     */
#define LCD_E2_PORT      PORTF
#define LCD_E2_PIN       5

#define D0HIGH LCD_DATA0_PORT |= _BV(LCD_DATA0_PIN)
#define D0LOW LCD_DATA0_PORT &= ~_BV(LCD_DATA0_PIN)
#define D1HIGH LCD_DATA1_PORT |= _BV(LCD_DATA1_PIN)
#define D1LOW LCD_DATA1_PORT &= ~_BV(LCD_DATA1_PIN)
#define D2HIGH LCD_DATA2_PORT |= _BV(LCD_DATA2_PIN)
#define D2LOW LCD_DATA2_PORT &= ~_BV(LCD_DATA2_PIN)
#define D3HIGH LCD_DATA3_PORT |= _BV(LCD_DATA3_PIN)
#define D3LOW LCD_DATA3_PORT &= ~_BV(LCD_DATA3_PIN)

#define E1HIGH LCD_E_PORT |= _BV(LCD_E_PIN)
#define E1LOW LCD_E_PORT &= ~_BV(LCD_E_PIN)
#define E2HIGH LCD_E2_PORT |= _BV(LCD_E2_PIN)
#define E2LOW LCD_E2_PORT &= ~_BV(LCD_E2_PIN)
#define RSHIGH LCD_RS_PORT |= _BV(LCD_RS_PIN)
#define RSLOW LCD_RS_PORT &= ~_BV(LCD_RS_PIN)
#define RWHIGH LCD_RW_PORT |= _BV(LCD_RW_PIN)
#define RWLOW LCD_RW_PORT &= ~_BV(LCD_RW_PIN)

void e(void);
void e2(void);
void e4(unsigned char data, unsigned char display);
void lcdInit(void);
inline void lcdChar(unsigned char data);
void toHex(unsigned short data);
void lcdLineCol(unsigned char line, unsigned char col, unsigned char disp);

#endif // DMC40457
