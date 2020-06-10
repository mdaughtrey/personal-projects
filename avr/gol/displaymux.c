#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <displaymux.h>
#include <typedefs.h>
#include <stddef.h>
#include <string.h>

//#include <serutil.h>
/* #define PALETTE */
#define USE_CURRENTCOLUMN 1

#ifdef USE_CURRENTCOLUMN
#define BITSPERSCAN 4
#endif

#define NUM_ROWS (7)
#define NUM_COLUMNS (5)
#define NUM_DISPLAYS (2)




#ifdef LTP1157
u08 columnCtl[NUM_DISPLAYS][NUM_COLUMNS][2] PROGMEM =
{
    /* Display 0 */
    0x02, 5,			/* PORTA */
    0x08, 4,			/* PORTC */
    0x08, 3,			/* PORTC */
    0x02, 2,			/* PORTA */
    0x08, 1,			/* PORTC */
    /* Display 1 */
    0x11, 6,			/* PORTF */
    0x05, 7,			/* PORTB */
    0x05, 6,			/* PORTB */
    0x11, 3,			/* PORTF */
    0x05, 4			/* PORTB */
};

u08 rowCtl[NUM_DISPLAYS][NUM_ROWS][2] PROGMEM =
{
    /* Display 0 */
    0x02, 1,			/* PORTA */
    0x02, 6,			/* PORTA */
    0x02, 0,			/* PORTA */
    0x08, 2,			/* PORTC */
    0x08, 6,			/* PORTC */
    0x08, 0,			/* PORTC */
    0x08, 5,			/* PORTC */

    /* Display 1 */
    0x11, 2,			/* PORTF */
    0x11, 7,			/* PORTF */
    0x11, 1,			/* PORTF */
    0x05, 5,			/* PORTB */
    0x14, 4,			/* PORTG */
    0x0e, 7,			/* PORTE */
    0x14, 3			/* PORTG */
};
#endif  /* !LTP1157 */
#ifdef LTP1257
u08 columnCtl[NUM_DISPLAYS][NUM_COLUMNS][2] PROGMEM =
{
    /* Display 0 */
    0x08, 4,			/* PORTC */
    0x08, 3,			/* PORTC */
    0x08, 2,			/* PORTC */
    0x08, 1,			/* PORTC */
    0x08, 0,			/* PORTC */
    /* Display 1 */
    0x0b, 0,			/* PORTD */
    0x14, 4,			/* PORTG */
    0x14, 3,			/* PORTG */
    0x05, 7,			/* PORTB */
    0x05, 6			/* PORTB */
};

u08 rowCtlGreen[NUM_DISPLAYS][NUM_ROWS][2] PROGMEM =
{
    /* Display 0 */
    0x14, 2,			/* PORTG */
    0x02, 6,			/* PORTA */
    0x02, 4,			/* PORTA */
    0x02, 2,			/* PORTA */
    0x02, 0,			/* PORTA */
    0x14, 1,			/* PORTG */
    0x08, 6,			/* PORTC */

    /* Display 1 */
    0x11, 6,			/* PORTF */
    0x11, 4,			/* PORTF */
    0x11, 2,			/* PORTF */
    0x11, 0,			/* PORTF */
    0x0E, 3,			/* PORTE */
    0x05, 5,			/* PORTB */
    0x0B, 2			/* PORTD */
};

u08 rowCtlOrange[NUM_DISPLAYS][NUM_ROWS][2] PROGMEM =
{
    /* Display 0 */
    0x08, 7,			/* PORTC */
    0x02, 7,			/* PORTA */
    0x02, 5,			/* PORTA */
    0x02, 3,			/* PORTA */
    0x02, 1,			/* PORTA */
    0x14, 0,			/* PORTG */
    0x08, 5,			/* PORTC */

    /* Display 1 */
    0x11, 7,			/* PORTF */
    0x11, 5,			/* PORTF */
    0x11, 3,			/* PORTF */
    0x11, 1,			/* PORTF */
    0x0E, 2,			/* PORTE */
    0x05, 4,			/* PORTB */
    0x0B, 1			/* PORTD */
};
#endif /* LTP1157 */

/* KingBright TBC12-22EGWA */

#ifdef KB_TBC12
typedef struct
{
    volatile uint8_t * port;
    uint8_t pin;
}PortPin;

PortPin columnCtl[NUM_DISPLAYS][NUM_COLUMNS] PROGMEM =
{
    {
        { &PORTC, 4 },			/* PORTC */
        { &PORTC, 3 },			/* PORTC */
        { &PORTC, 2 },			/* PORTC */
        { &PORTC, 1 },			/* PORTC */
        { &PORTC, 0 },			/* PORTC */
    },
    {
        /* Display 1 */
        { &PORTD, 0 },			/* PORTD */
        { &PORTG, 4 },			/* PORTG */
        { &PORTG, 3 },			/* PORTG */
        { &PORTB, 7 },			/* PORTB */
        { &PORTB, 6 }			/* PORTB */
    }
};

PortPin rowCtlGreen[NUM_DISPLAYS][NUM_ROWS] PROGMEM =
{
    {
        /* Display 0 */
        { &PORTG, 2 },			/* PORTG */
        { &PORTA, 6 },			/* PORTA */
        { &PORTA, 4 },			/* PORTA */
        { &PORTA, 2 },			/* &PORTA */
        { &PORTA, 0 },			/* &PORTA */
        { &PORTG, 1 },			/* PORTG */
        { &PORTC, 6 },			/* PORTC */
    },
    {
        /* Display 1 */

        { &PORTF, 6 },		/* PORTF */
        { &PORTF, 4 },			/* PORTF */
        { &PORTF, 2 },			/* PORTF */
        { &PORTF, 0 },			/* PORTF */
        { &PORTE, 3 },			/* PORTE */
        { &PORTB, 5 },			/* PORTB */
        { &PORTD, 2 }			/* PORTD */
    }
};

PortPin rowCtlOrange[NUM_DISPLAYS][NUM_ROWS] PROGMEM =
{
    {
        /* Display 0 */
        { &PORTC, 7 },			/* PORTC */
        { &PORTA, 7 },			/* PORTA */
        { &PORTA, 5 },			/* PORTA */
        { &PORTA, 3 },			/* PORTA */
        { &PORTA, 1 },			/* PORTA */
        { &PORTG, 0 },			/* PORTG */
        { &PORTC, 5 },			/* PORTC */
    },
    {
        /* Display 1 */
        { &PORTF, 7 },			/* PORTF */
        { &PORTF, 5 },			/* PORTF */
        { &PORTF, 3 },			/* PORTF */
        { &PORTF, 1 },			/* PORTF */
        { &PORTE, 2 },			/* PORTE */
        { &PORTB, 4 },			/* PORTB */
        { &PORTD, 1 }			/* PORTD */
    }
};
#endif

struct _display
{
    u08 charIndex;
    u08 colIndex;
    u08 flags;
    u08 progColumn[NUM_COLUMNS];
    u08 bitsOut[NUM_COLUMNS];
#ifdef USE_CURRENTCOLUMN
    u08 currentColumn;
    u08 currentColumnBit;
#endif
    //  u08 portDataSelect;		/* palette currently being displayed */
    u08 paletteSelected;		/* Change to this pallete */
#ifdef PALETTE
    u08 paletteCountdown;		/* countdown to next palette change */
#endif
    u08 * rowCtlPtr;
    u08 * oldRowCtlPtr;
    uint8_t * litPort;
    u08 litBit;
}display[NUM_DISPLAYS];

#define FLAG_BLANK (_BV(5))	/* blank character */

static void dm_deleteTransform(u08 which, u08 command);
static void dm_setTransform(u08 which, u08 command);
static u08 dm_findNextTransform(struct _display * display);
static u08 dm_getTransformData(struct _display * display, u08 index);
static void dm_setTransformData(struct _display * display, u08 index, u08 data);
static u08 dm_findFirstTransform(struct _display * display);
static u08 dm_cmdLength(u08 command);
static void dm_applyTransforms(u08 which);
static void dm_rollUp(struct _display * display, u08 count);
static void dm_rollLeft(struct _display * display, u08 count);
static void dm_clearRows(struct _display * display, u08 start, u08 count);
static void dm_clearColumns(struct _display * display, u08 start, u08 count);


void dm_timerHandler(void)
{
    u08 whichIndex = 0;

    for (whichIndex = 0; whichIndex < NUM_DISPLAYS; whichIndex++)
    {
        struct _display * disp = &display[whichIndex];
        u08 flags = disp->flags;
        u08 litBitIndex = disp->currentColumnBit;
        u08 colIndex = disp->colIndex;

        const u08 * colCtlPtr = (const u08 *)&columnCtl[whichIndex][colIndex];
        u08 * rowCtlPtr = disp->rowCtlPtr;
        u08 * oldRowCtlPtr = disp->oldRowCtlPtr;

        /* If we're at the start of a new frame... */
        if (litBitIndex == 7 &&
            colIndex == 0)
        {
            //	  oldRowCtlPtr = rowCtlPtr;
            /* Set up the color choice for this frame */
#ifdef PALETTE
            if (disp->paletteCountdown < disp->paletteSelected)
#else
                if (0 == disp->paletteSelected) /* palette zero = orange */
#endif
                {
                    disp->rowCtlPtr = (u08 *)&rowCtlOrange[whichIndex][0]; 
                }
                else
                {
                    disp->rowCtlPtr = (u08 *)&rowCtlGreen[whichIndex][0]; /* palette 1 == green */
                }
            if (rowCtlPtr != disp->rowCtlPtr) /*  If we changed palettes... */
            {
                oldRowCtlPtr = rowCtlPtr; /* get the old palette pointer */
                rowCtlPtr = disp->rowCtlPtr; /* get the new palette pointer */
                disp->oldRowCtlPtr = oldRowCtlPtr; /* store the old palette pointer */
            }
#ifdef PALETTE
            if (disp->paletteCountdown-- == 0)
            {
                disp->paletteCountdown = PALETTE_COUNTDOWN_INIT;
            }
#endif
        }
        u08 data;
        if (litBitIndex < 7)		/* if we're in a frame, restore the current data */
        {
            data = disp->currentColumn;
        }
        else
        {
            data = disp->bitsOut[colIndex]; /* else read from frame buffer */
            litBitIndex = 0;
        }
      
        u08 * colOnPort = pgm_read_byte_near(colCtlPtr + offsetof(PortPin, port));
        u08 colOnBit = pgm_read_byte_near(colCtlPtr + offsetof(PortPin, pin));
      
        /* If we're starting a new frame... */
        if (litBitIndex == 0)
        {
            (*disp->litPort) |= _BV(disp->litBit); /* turn off the previous column */
            disp->litPort = colOnPort;		 /* store the new column port */
            disp->litBit = colOnBit;		 /* store the new column bit */
        }

        /* Turn off the old row value, turn on the new row value */
        u08 litBits = 0;
        u08 count;
        //            u08 newDataBit = 0;

        /* run through the current columns respecting BITSPERSCAN */
        /* run through bits 0-6 (all LEDs in the current lit column). */
        /* Turn them all off at every run. Once count > litBitIndex, illuminate up to */
        /* BITSPERSCAN LEDs. */
        for (count = 0; count < 7; count++)
        {
/* SENSE */
            u08 * newPort = pgm_read_byte_near(rowCtlPtr + offsetof(PortPin, port));
            u08 newBit = pgm_read_byte_near(rowCtlPtr + offsetof(PortPin, pin));
            u08 * oldPort = pgm_read_byte_near(oldRowCtlPtr + offsetof(PortPin, port));
            u08 oldBit = pgm_read_byte_near(oldRowCtlPtr + offsetof(PortPin, pin));

            (*oldPort) &= ~_BV(oldBit); /* turn off the bit of the previous column */
      
            if (litBits >= BITSPERSCAN ||
                count < litBitIndex)
            {
                    (*newPort) &= ~_BV(newBit); /* turn off the bit on this column */
            }
            else
            {
                if (data & (1 << count))
                {
                    (*newPort) |= _BV(newBit);
                    litBitIndex = count;
                    litBits++;
                    data &= ~(1 << count);
                }
                else
                {
                    (*newPort) &= ~_BV(newBit);
                }
            }
            rowCtlPtr += sizeof(PortPin); /* point to the next row */
            oldRowCtlPtr += sizeof(PortPin); /* point to the next row */
        }
        //      litBitIndex = newDataBit;	/* store the index of the bit to light next */

        /* If display is not blanked, turn on the current column */
        if ((flags & FLAG_BLANK) == 0)
        {
            (*colOnPort) &= ~_BV(colOnBit);
        }
      
        disp->currentColumn = data;
      
        //      if ((data >> litBitIndex) == 0)
        if (0 == data)
        {
            litBitIndex = 7;
            ++colIndex;
	  
            if (colIndex == NUM_COLUMNS)
            {
                colIndex = 0;
            }
        }
        disp->currentColumnBit = litBitIndex;
        disp->colIndex = colIndex;
    }
}

void dm_init(void)
{
    memset(display, 0, sizeof(display));

    MCUCR |= PUD;

#ifdef KB_TBC12
    /* Enable pullup resistors for row drivers */
/*   PORTE = 0x0c; */
    /* Enable pullup resistors general purpose input */
    PORTE = 0xc3;
   
    /* Set direction */
    DDRA = 0xff; /*76543210 1111 1111  */
    DDRB |= 0xf0;  /*7654 1111 0000 */
    DDRC = 0xff; /*76543210 1111 1111  */
    DDRD |= 0b00000111;
    DDRE |= 0x0c; /* 32 0000 1100*/
    DDRF = 0xff; /* 76543210 11111111*/
    DDRG = 0x1f; /* 43210 0001 1111*/

    PORTB |= 0xc0; /* 1100 0000 */
    PORTC = 0x1f; /* 0001 1111 */
    PORTD = 0xf9; /* 1111 1001 */
    PORTE &= 0xf3;
    PORTG = 0x18; /* 0001 1000 */
    PORTF = 0;

    display[0].colIndex = 4;
    display[1].colIndex = 4;
    display[0].rowCtlPtr = (u08 *)&rowCtlGreen[0][0];
    display[1].rowCtlPtr = (u08 *)&rowCtlGreen[1][0];
    display[0].oldRowCtlPtr = (u08 *)&rowCtlGreen[0][0];
    display[1].oldRowCtlPtr = (u08 *)&rowCtlGreen[1][0];
    display[0].currentColumnBit = 7;
    display[1].currentColumnBit = 7;

#endif
#ifdef LTP1157
    DDRA = 0x67; /*65210 0110 0111 */
    DDRB = 0xf0; /*4567  1111 0000 */
    DDRC = 0x7f; /*0123456 0111 1111 */
    DDRD = 0x00; /* */
    DDRE = 0x80; /* 7 1000 0000*/
    DDRF = 0xce; /* 76321 1100 1110 */
    DDRG = 0x18; /* 34 0001 1000 */
    display[0].litPort = PORTA;
    display[0].litBit = 5;
#endif
#ifdef LTP1157 
    DDRA  = 0xff;		/* 76543210 1111 1111 */
    PORTA = 0xff;
    DDRB  = 0xf0;		/* 7654 1111 0000 */
    PORTB = 0xf0;
    DDRC  = 0xff;		/* 76543210 1111 1111 */
    PORTC = 0xff;
    DDRD  = 0x07;		/* 210 0000 0111 */
    PORTD = 0x07;
    DDRE  = 0x0c;		/* 32 0000 1100 */
    PORTE = 0x0c;
    DDRF  = 0xff;		/* 76543210 1111 1111 */
    PORTF = 0xff;
    DDRG  = 0x1f;		/* 43210 0001 1111 */
    PORTG = 0x1f;
    display[0].litPort = PORTC;
    display[0].litBit = 4;
    display[1].litPort = PORTB;
    display[1].litBit = 6;
#endif
}


static void dm_applyTransforms(u08 which)
{
    u08 count;
    u08 transform;
    struct _display * disp = &display[which];
    for (count = 0; count < NUM_COLUMNS; count++)
    {
	    disp->bitsOut[count] = disp->progColumn[count];
    }
    disp->colIndex = 0;
    disp->currentColumnBit = 8;
}

void dm_blank(u08 which, u08 set)
{
    if (set)
    {
        display[which].flags |= FLAG_BLANK;
    }
    else
    {
        display[which].flags &= ~FLAG_BLANK;
    }
}

void dm_setPalette(u08 which, u08 index)
{
/*   cli(); */
#ifdef PALETTE
    display[which].paletteCountdown = PALETTE_COUNTDOWN_INIT;
#endif
    display[which].paletteSelected = index;
/*   sei(); */
}

static void dm_clearRows(struct _display * display, u08 start, u08 count)
{
    u08 index;

    for (index = 0; index < NUM_COLUMNS; index++)
    {
        display->bitsOut[index] &= ~(((1 << count) - 1) << start);
    }
}

static void dm_clearColumns(struct _display * display, u08 start, u08 count)
{
    do
    {
        --count;
        display->bitsOut[start + count] = 0;
    } while (count > 0);
}

void dm_progColumn(u08 which, u08 column, u08 data)
{
    display[which].progColumn[column] = data;
    dm_applyTransforms(which);
}

void dm_pixel(u08 which, u08 set, u08 row, u08 column)
{
    if (set == 1)
    {
        display[which].progColumn[column] |= (1 << row);
    }
    else
    {
        display[which].progColumn[column] &= ~(1 << row);
    }
    dm_applyTransforms(which);
}


