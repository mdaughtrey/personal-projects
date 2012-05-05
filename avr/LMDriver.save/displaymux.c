#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <displaymux.h>
#include <typedefs.h>
#include <stddef.h>

//#include <serutil.h>
/* #define DEBUG */
/* #define PALETTE */
#define USE_CURRENTCOLUMN 1

#ifdef USE_CURRENTCOLUMN
#define BITSPERSCAN 2
#endif

#define NUM_ROWS (7)
#define NUM_COLUMNS (5)
#define NUM_DISPLAYS (2)
#define TRANSFORM_LIST_LENGTH (16)
#define PALETTE_COUNTDOWN_INIT (4)

#define TRANSFORM_FLIP 0x00
#define TRANSFORM_MIRROR 0x01
#define TRANSFORM_REVERSE 0x02
#define TRANSFORM_ROLL_VERT 0x03
#define TRANSFORM_ROLL_HORZ 0x04
#define TRANSFORM_SHIFT_VERT 0x05
#define TRANSFORM_SHIFT_HORZ 0x06
#define TRANSFORM_EOL 0x0f	/* End Of List */

#define DIR_UP 0x10
#define DIR_DOWN 0x20
#define DIR_LEFT 0x30
#define DIR_RIGHT 0x40

#ifdef DEBUG
u08 dispDebugChar;
//u08 msgTransformIs[] PROGMEM = "Transform is ";
u08 msgCR[] PROGMEM = "\r\n";
u08 msgSpace[] PROGMEM = "  ";
/* u08 msgDumpTxList[] PROGMEM = "Dump TxList:\r\n"; */
/* u08 msgArrow[] PROGMEM = "->"; */
/* u08 msgReset[] PROGMEM = "reset\r\n"; */
/* u08 msgGreen[] PROGMEM = "g"; */
/* u08 msgOrange[] PROGMEM = "o"; */
/* u08 msgDisp[] PROGMEM = "disp "; */
u08 msgBoot[] PROGMEM = "Init";

#define SDEBUG(x)  uart_send_sync(x, sizeof(x))
#define SNUMBER(c) uart_send_char((c) + '0')
#define SHEX(c) uart_send_hex_byte(c);

#else  /* DEBUG */
#define SDEBUG(x)
#define SNUMBER(x)
#define SHEX(x)
#endif

extern u08 font5x7[];


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
    uint8_t * port;
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
    u08 txList[TRANSFORM_LIST_LENGTH];
    u08 txListBookmark;
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
    u08 senseRowColumn;         /* high nibble = row, low nibble = column */
}display[NUM_DISPLAYS];

#define FLAG_DIM (_BV(3))	/* bright/dim */
#define FLAG_PROGCOLS (_BV(4))	/* display programmed columns */
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
static void dm_dumpTxList(struct _display * display);
static void dm_clearRows(struct _display * display, u08 start, u08 count);
static void dm_clearColumns(struct _display * display, u08 start, u08 count);

#ifdef DEBUG
u08 msgDataBit[] PROGMEM = " dataBit ";
u08 msgColIndex[] PROGMEM = " colInd ";
u08 msgNewFrame[] PROGMEM = "NewFrm/PC ";
//u08 msgPrevColOff[] PROGMEM = "PrevColOff\r\n";
u08 msgRowCtlPtr[] PROGMEM = "rCP ";
u08 msgOldRowCtlPtr[] PROGMEM = " oRCP ";
u08 msgOldPort[] PROGMEM = "oPort ";
u08 msgOldBit[] PROGMEM = " oBit ";
u08 msgNewPort[] PROGMEM = " nPort ";
u08 msgNewBit[] PROGMEM = " nBit ";
u08 msgData[] PROGMEM = " data ";
u08 msgOn[] PROGMEM = " on ";
u08 msgNewDataBit[] PROGMEM = " newDB ";
u08 msgCount[] PROGMEM = " cnt ";
#endif /* DEBUG */

void dm_timerHandler(void)
{
    u08 whichIndex = 0;

    for (whichIndex = 0; whichIndex < NUM_DISPLAYS; whichIndex++)
    {
        struct _display * disp = &display[whichIndex];
        u08 flags = disp->flags;
        u08 litBitIndex = disp->currentColumnBit;
        u08 colIndex = disp->colIndex;
        u08 senseCol = disp->senseRowColumn & 0x0f;

        /* SENSE */
        if (7 == litBitIndex && colIndex == senseCol)
        {
            if (++colIndex == NUM_COLUMNS)
            {
                colIndex = 0;
            }
        }

        const u08 * colCtlPtr = &columnCtl[whichIndex][colIndex];
        u08 * rowCtlPtr = disp->rowCtlPtr;
        u08 * oldRowCtlPtr = disp->oldRowCtlPtr;
        u08 senseRow = disp->senseRowColumn >> 4;

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
                    disp->rowCtlPtr = &rowCtlOrange[whichIndex][0]; 
                }
                else
                {
                    disp->rowCtlPtr = &rowCtlGreen[whichIndex][0]; /* palette 1 == green */
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
        /* If display is dimmed, turn column off immediately */
        if (litBitIndex == 7 && flags & FLAG_DIM)
        {
            (*colOnPort) |= _BV(colOnBit);
        }
    }
}

void dm_init(void)
{
    memset(display, 0, sizeof(display));
    display[0].txList[0] = TRANSFORM_EOL;
    display[1].txList[0] = TRANSFORM_EOL;

    MCUCR |= PUD;

#ifdef KB_TBC12
    /* Enable pullup resistors for row drivers */
/*   PORTE = 0x0c; */

    /* Set direction */
    DDRA = 0xff; /*76543210 1111 1111  */
    DDRB |= 0xf0;  /*7654 1111 0000 */
    DDRC = 0xff; /*76543210 1111 1111  */
    DDRD = 0x07; /* 210 0000 0111*/
    DDRE = 0x0c; /* 32 0000 1100*/
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
    display[0].rowCtlPtr = &rowCtlGreen[0][0];
    display[1].rowCtlPtr = &rowCtlGreen[1][0];
    display[0].oldRowCtlPtr = &rowCtlGreen[0][0];
    display[1].oldRowCtlPtr = &rowCtlGreen[1][0];
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
    dm_setsense(0, 0, 0);       /* disable sense */
    dm_setsense(0, 5, 0);       /* disable sense */
/*   TCCR0A = _BV(CS01);		/* Timer 0 prescaler =CLK/256 */ 
/*   TIMSK0 |= _BV(TOIE0); */

    SDEBUG(msgBoot);
    SDEBUG(msgCR);

}


void dm_setChar(u08 which, u08 set)
{
    display[which].charIndex = set;
    dm_applyTransforms(which);
}

static void dm_applyTransforms(u08 which)
{
    u08 count;
    u08 transform;
    struct _display * disp = &display[which];
    u08 * dataPtr = font5x7 + (disp->charIndex * NUM_COLUMNS);

/*   cbi(TIMSK0, TOIE0); */
/*   TIMSK0 &= ~_BV(TOIE0); */

    if (disp->flags & FLAG_PROGCOLS)
    {
        for (count = 0; count < NUM_COLUMNS; count++)
        {
            disp->bitsOut[count] = disp->progColumn[count];
        }
    }
    else
    {
        for (count = 0; count < NUM_COLUMNS; count++)
        {
            disp->bitsOut[count] = pgm_read_byte_near(dataPtr++);
        }
    }
  
    disp->colIndex = 0;
    /* test */
    disp->currentColumnBit = 8;

    dm_dumpTxList(display);

    transform = dm_findFirstTransform(disp);
    while ((transform & 0x0f) != TRANSFORM_EOL)
    {
        int tmp = (int)disp;
        u08 data;
        u08 count2;
        //      SDEBUG(msgTransformIs);
        //      SNUMBER(transform & 0x0f);
        //      SDEBUG(msgCR);
        switch (transform & 0x0f)
        {

        case TRANSFORM_FLIP:
        {
            for (count2 = 0; count2 < NUM_COLUMNS; count2++)
            {
                data = disp->bitsOut[count2];
                data = ((data & 0xAA) >>  1) | ((data & 0x55) <<  1);
                data = ((data & 0xCC) >>  2) | ((data & 0x33) <<  2);
                data = (data >> 4) | (data << 4);
                data = data >> 1;
                disp->bitsOut[count2] = data;
            }
        }
        break;

        case TRANSFORM_MIRROR:
        {
            u08 temp;
            temp = disp->bitsOut[0];
            disp->bitsOut[0] = disp->bitsOut[4];
            disp->bitsOut[4] = disp->bitsOut[1];
            disp->bitsOut[1] = disp->bitsOut[3];
            disp->bitsOut[3] = disp->bitsOut[4];
            disp->bitsOut[4] = temp;
        }
        break;
	  
        case TRANSFORM_REVERSE: 
        { 
            for (count2 = 0; count2 < NUM_COLUMNS; count2++) 
            { 
                disp->bitsOut[count2] = ~disp->bitsOut[count2] & 0x7f;
            } 
        } 
        break; 

        case TRANSFORM_ROLL_VERT:
        case TRANSFORM_ROLL_HORZ:
        {
            u08 data;
            data = dm_getTransformData(disp, 0);
            switch (data & 0xf0)
            {
            case DIR_UP: dm_rollUp(disp, data & 0x0f); break;
            case DIR_DOWN: dm_rollUp(disp, NUM_ROWS - (data & 0x0f)); break;
            case DIR_LEFT: dm_rollLeft(disp, data & 0x0f); break;
            case DIR_RIGHT: dm_rollLeft(disp, NUM_COLUMNS - (data & 0x0f)); break;
            }
        }
        break;

        case TRANSFORM_SHIFT_VERT:
        {
            u08 data;
            data = dm_getTransformData(disp, 0);
            switch (data & 0xf0)
            {
            case DIR_UP:
                data &= 0x0f;
                dm_rollUp(disp, data);
                dm_clearRows(disp, NUM_ROWS - data, data);
                break;
		
            case DIR_DOWN:
                data &= 0x0f;
                dm_rollUp(disp, NUM_ROWS - data);
                dm_clearRows(disp, 0, data);
                break;
            }
            break;
        }

        case TRANSFORM_SHIFT_HORZ:
        {
            data = dm_getTransformData(disp, 0);
            switch (data & 0xf0)
            {
            case DIR_LEFT:
                data &= 0x0f;
                dm_rollLeft(disp, data);
                dm_clearColumns(disp, NUM_COLUMNS - data, data);
                break;
		
            case DIR_RIGHT:
                data &= 0x0f;
                dm_rollLeft(disp, NUM_COLUMNS - data);
                dm_clearColumns(disp, 0, data);
                break;
            }
        }
        break;

        }
        SDEBUG(msgCR);
        transform = dm_findNextTransform(disp);
    }
/*   sei(); */
/*   TIMSK0 |= _BV(TOIE0); */
}

void dm_setReverse(u08 which, u08 set)
{
    switch (set)
    {
    case 0: dm_deleteTransform(which, TRANSFORM_REVERSE); break;
    case 1: dm_setTransform(which, TRANSFORM_REVERSE); break;
    }
    dm_applyTransforms(which);
}

void dm_setFlip(u08 which, u08 set)
{
    switch (set)
    {
    case 0: dm_deleteTransform(which, TRANSFORM_FLIP); break;
    case 1: dm_setTransform(which, TRANSFORM_FLIP); break;
    }
    dm_applyTransforms(which);
}

void dm_setMirror(u08 which, u08 set)
{
    switch (set)
    {
    case 0: dm_deleteTransform(which, TRANSFORM_MIRROR); break;
    case 1: dm_setTransform(which, TRANSFORM_MIRROR); break;
    }
    dm_applyTransforms(which);
}

void dm_setDim(u08 which, u08 set)
{
    switch (set)
    {
    case 0: display[which].flags &= ~FLAG_DIM; break;
    case 1: display[which].flags |= FLAG_DIM; break;
    }
}


void dm_displayProgrammed(u08 which, u08 set)
{
    switch (set)
    {
    case 0: display[which].flags &= ~FLAG_PROGCOLS; break;
    case 1: display[which].flags |= FLAG_PROGCOLS; break;
    }
    dm_applyTransforms(which);
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

void dm_shift(u08 which, u08 direction, u08 count)
{
    struct _display * disp = &display[which];
    switch (count)
    {
    case 0:
        switch (direction)
        {
        case 'u':
        case 'd':
            dm_deleteTransform(which, TRANSFORM_SHIFT_VERT);
            break;

        case 'l':
        case 'r':
            dm_deleteTransform(which, TRANSFORM_SHIFT_HORZ);
            break;
        }
        break;

/*     case 1: */
    default:
        switch (direction)
        {
        case 'u':
            dm_setTransform(which, TRANSFORM_SHIFT_VERT);
            dm_setTransformData(disp, 0, DIR_UP | count);
            break;

        case 'd':
            dm_setTransform(which, TRANSFORM_SHIFT_VERT);
            dm_setTransformData(disp, 0, DIR_DOWN | count);
            break;

        case 'l':
            dm_setTransform(which, TRANSFORM_SHIFT_HORZ);
            dm_setTransformData(disp, 0, DIR_LEFT | count);
            break;

        case 'r':
            dm_setTransform(which, TRANSFORM_SHIFT_HORZ);
            dm_setTransformData(disp, 0, DIR_RIGHT | count);
            break;
        }
    }
    dm_applyTransforms(which);
}

void dm_roll(u08 which, u08 direction, u08 count)
{
    struct _display * disp = &display[which];
    switch (count)
    {
    case 0:
        switch (direction)
        {
        case 'u':
        case 'd':
            dm_deleteTransform(which, TRANSFORM_ROLL_VERT);
            break;

        case 'l':
        case 'r':
            dm_deleteTransform(which, TRANSFORM_ROLL_HORZ);
            break;
        }
        break;

    default:
        switch (direction)
        {
        case 'u':
            dm_setTransform(which, TRANSFORM_ROLL_VERT);
            dm_setTransformData(disp, 0, DIR_UP | count);
            break;

        case 'd':
            dm_setTransform(which, TRANSFORM_ROLL_VERT);
            dm_setTransformData(disp, 0, DIR_DOWN | count);
            break;

        case 'l':
            dm_setTransform(which, TRANSFORM_ROLL_HORZ);
            dm_setTransformData(disp, 0, DIR_LEFT | count);
            break;

        case 'r':
            dm_setTransform(which, TRANSFORM_ROLL_HORZ);
            dm_setTransformData(disp, 0, DIR_RIGHT | count);
            break;
        }
    }
    dm_applyTransforms(which);
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

static void dm_deleteTransform(u08 which, u08 command)
{
    struct _display * disp = &display[which];
    u08 test = (dm_findFirstTransform(disp) & 0x0f);

    while (test != TRANSFORM_EOL)
    {
        if (test == command)
        {
            u08 length = dm_cmdLength(command);
            dm_dumpTxList(disp);

            memmove(&disp->txList[disp->txListBookmark],
                    &disp->txList[disp->txListBookmark + length],
                    TRANSFORM_LIST_LENGTH - disp->txListBookmark - length);

            dm_dumpTxList(disp);
            break;
        }
        test = (dm_findNextTransform(disp) & 0x0f);
    }

}

static void dm_setTransform(u08 which, u08 command)
{
    struct _display * disp = &display[which];


    u08 test = dm_findFirstTransform(disp);
  
    while ((test & 0x0f) != TRANSFORM_EOL)
    {
        if (test == command)
        {
            break;
        }
        test = dm_findNextTransform(disp);
    }

    disp->txList[disp->txListBookmark++] = command;
    if ((test & 0x0f) == TRANSFORM_EOL)
    {
        disp->txList[disp->txListBookmark] = TRANSFORM_EOL;
    }
}

static u08 dm_findFirstTransform(struct _display * display)
{
    display->txListBookmark = 0;
    return display->txList[0];
}

static u08 dm_cmdLength(u08 command)
{
    switch (command & 0x0f)
    {
    case TRANSFORM_FLIP:
    case TRANSFORM_MIRROR:
    case TRANSFORM_REVERSE:
        return 1;

    case TRANSFORM_ROLL_VERT:
    case TRANSFORM_ROLL_HORZ:
    case TRANSFORM_SHIFT_VERT:
    case TRANSFORM_SHIFT_HORZ:
        return 2;
    }
    return 0;
}

static u08 dm_findNextTransform(struct _display * display)
{
    display->txListBookmark += dm_cmdLength(display->txList[display->txListBookmark] & 0x0f);
    return display->txList[display->txListBookmark];
}

static u08 dm_getTransformData(struct _display * display, u08 index)
{
    return display->txList[display->txListBookmark + index + 1];
}

static void dm_setTransformData(struct _display * display, u08 index, u08 data)
{
    u08 * dataPtr = &display->txList[display->txListBookmark + index];
    if (*dataPtr == TRANSFORM_EOL)
    {
        *dataPtr++ = data;
        *dataPtr = TRANSFORM_EOL;
    }
    else
    {
        *dataPtr = data;
    }
}

static void dm_rollUp(struct _display * display, u08 count)
{
    u08 temp;
    u08 index;

    for (index = 0; index < NUM_COLUMNS; index++)
    {
        u08 data = display->bitsOut[index];
        temp = data & ((1 << count) - 1);
        data >>= count;
        data |= temp << (NUM_ROWS - count);
        display->bitsOut[index] = data;
    }
}

static void dm_rollLeft(struct _display * display, u08 count)
{
    u08 temp;
    u08 index;

    while (count)
    {
        temp = display->bitsOut[0];
        for (index = 0; index < NUM_COLUMNS - 1; index++)
        {
            display->bitsOut[index] = display->bitsOut[index + 1];
        }
        display->bitsOut[NUM_COLUMNS - 1] = temp;
        count--;
    }
}

static void dm_dumpTxList(struct _display * display)
{
/*   u08 count = 0; */
/*   while (display->txList[count] != TRANSFORM_EOL) */
/*     { */
/*       SNUMBER(count); */
/*       SDEBUG(msgArrow); */
/*       SNUMBER(display->txList[count]); */
/*       SDEBUG(msgCR); */
/*       count++; */
/*     } */
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

void dm_reset(u08 which)
{
    display[which].txListBookmark = 0;
    display[which].txList[0] = TRANSFORM_EOL;
    dm_applyTransforms(which);
}

void dm_progColumn(u08 which, u08 column, u08 data)
{
    display[which].progColumn[column] = data;
    if (display[which].flags & FLAG_PROGCOLS)
    {
        dm_applyTransforms(which);
    }
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
    if (display[which].flags & FLAG_PROGCOLS)
    {
        dm_applyTransforms(which);
    }
}


void dm_copyToCustom(u08 which, u08 charIndex)
{
    u08 count;
    struct _display * disp = &display[which];
    u08 * dataPtr = font5x7 + (charIndex * NUM_COLUMNS);

    for (count = 0; count < NUM_COLUMNS; count++)
    {
        disp->progColumn[count] = pgm_read_byte_near(dataPtr++);
    }
    dm_applyTransforms(which);
}

void dm_setsense(u08 set, u08 row, u08 column)
{
    if (row <= 4)
    {
        display[0].senseRowColumn = set ? (row << 4) & column : 0xff;
    }
    else if (row >= 5 && row <= 9)
    {
        display[1].senseRowColumn = set ? (row << 4) & column : 0xff;
    }
}

void dm_dumpdisp(u08 which)
{
/*   u08 count; */
/*   struct _display * disp = &display[which]; */

/*   SDEBUG(msgDisp); */
/*   SNUMBER(which); */
/*   SDEBUG(msgCR); */

/*   uart_send_hex_byte(disp->charIndex); */
/*   SDEBUG(msgSpace); */
/*   uart_send_hex_byte(disp->colIndex); */
/*   SDEBUG(msgSpace); */
/*   uart_send_hex_byte(disp->flags); */
/*   SDEBUG(msgCR); */

/*   for (count = 0; count < TRANSFORM_LIST_LENGTH; count++) */
/*     { */
/*       uart_send_hex_byte(disp->txList[count]); */
/*       SDEBUG(msgSpace); */
/*     } */
/*   SDEBUG(msgCR); */
/*   uart_send_hex_byte(disp->txListBookmark); */
/*   SDEBUG(msgCR); */

/*   for (count = 0; count < NUM_COLUMNS; count++) */
/*     { */
/*       uart_send_hex_byte(disp->progColumn[count]); */
/*       SDEBUG(msgSpace); */
/*     } */
/*   SDEBUG(msgCR); */

/*   for (count = 0; count < NUM_COLUMNS; count++) */
/*     { */
/*       uart_send_hex_byte(disp->bitsOut[count]); */
/*       SDEBUG(msgSpace); */
/*     } */
/*   SDEBUG(msgCR); */

/* #ifdef USE_CURRENTCOLUMN   */
/*   uart_send_hex_byte(disp->currentColumn); */
/*   SDEBUG(msgSpace); */
/* #endif */
/*   //  uart_send_hex_byte(disp->portDataSelect); */
/*   // SDEBUG(msgSpace); */
/*   uart_send_hex_byte(disp->paletteSelected); */
/*   SDEBUG(msgSpace); */
/*   SDEBUG(msgCR); */
        }
