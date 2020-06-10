#ifndef INCLUDED_DIAGLEDS
#define INCLUDED_DIAGLEDS

#define DIAGLED_INIT {DDRA = 0xff; PORTA = 0xff; }
#define DIAGLED_ON(led) {PORTA &= ~(1<<led);}
#define DIAGLED_OFF(led) {PORTA |= 1<<led;}

#endif /* INCLUDED_DIAGLEDS */
