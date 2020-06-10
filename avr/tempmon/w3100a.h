#ifndef INCLUDED_W3100A
#define INCLUDED_W3100A

#include <avr/io.h> 
#include <util/delay.h>
#include <avr/pgmspace.h>

#define DDR(x) (*(&x - 1))      /* address of data direction register of port x */

#define LAN_CS_PORT PORTG
#define LAN_CS_PIN 2
#define LAN_RD_PORT PORTC
#define LAN_RD_PIN 5
#define LAN_WR_PORT PORTC
#define LAN_WR_PIN 4
#define LAN_A0_PORT PORTC
#define LAN_A0_PIN 6
#define LAN_A1_PORT PORTC
#define LAN_A1_PIN  7
#define LAN_INT_PORT PORTE
#define LAN_INT_PIN  6
#define LAN_RESET_PORT PORTC
#define LAN_RESET_PIN 3

#define LAN_CSLOW LAN_CS_PORT &= ~_BV(LAN_CS_PIN)
#define LAN_CSHIGH LAN_CS_PORT |= _BV(LAN_CS_PIN)
#define LAN_RDLOW LAN_RD_PORT &= ~_BV(LAN_RD_PIN)
#define LAN_RDHIGH LAN_RD_PORT |= _BV(LAN_RD_PIN)
#define LAN_WRLOW LAN_WR_PORT &= ~_BV(LAN_WR_PIN)
#define LAN_WRHIGH LAN_WR_PORT |= _BV(LAN_WR_PIN)
#define LAN_A0LOW LAN_A0_PORT &= ~_BV(LAN_A0_PIN)
#define LAN_A0HIGH LAN_A0_PORT |= _BV(LAN_A0_PIN)
#define LAN_A1LOW LAN_A1_PORT &= ~_BV(LAN_A1_PIN)
#define LAN_A1HIGH LAN_A1_PORT |= _BV(LAN_A1_PIN)
#define LAN_RESETLOW LAN_RESET_PORT &= ~_BV(LAN_RESET_PIN)
#define LAN_RESETHIGH LAN_RESET_PORT |= _BV(LAN_RESET_PIN)

#define LAN_DATA PORTA
#define LAN_GETDATA PINA
#define LAN_DATA_OUT DDRA = 0xff;
#define LAN_DATA_IN DDRA = 0x00;
#define LAN_ADDR_OR { LAN_A1LOW; LAN_A0LOW; } 
#define LAN_ADDR_AR0 { LAN_A1LOW; LAN_A0HIGH; }
#define LAN_ADDR_AR1 { LAN_A1HIGH; LAN_A0LOW; }
#define LAN_ADDR_DR { LAN_A1HIGH; LAN_A0HIGH; } 

#define LAN_REG_C0_CR 0x00
#define LAN_REG_C0_ISR 0x04
#define LAN_REG_C0_SSR 0xa0
#define LAN_REG_C0_SOPR 0xa1
#define LAN_REG_C0_DPR0 0xac
#define LAN_REG_C0_DPR1 0xad
#define LAN_REG_C0_SRCPORT0 0xae
#define LAN_REG_C0_SRCPORT1 0xaf

#define LAN_REG_C0_RXW_PR 0x10
#define LAN_REG_C0_RXR_PR 0x14
#define LAN_REG_C0_TXW_PR 0x40
#define LAN_REG_C0_TXR_PR 0x44
#define LAN_REG_C0_TXA_PR 0x48

#define LAN_REG_SHADOW_C0_RXW 0x1e0
#define LAN_REG_SHADOW_C0_RXR 0x1e1
#define LAN_REG_SHADOW_C0_TXA 0x1e2
#define LAN_REG_SHADOW_C0_TXW 0x1f0
#define LAN_REG_SHADOW_C0_TXR 0x1f1

void lanInit(void);
void lanLoop(void);

#endif // INCLUDED_W3100A
