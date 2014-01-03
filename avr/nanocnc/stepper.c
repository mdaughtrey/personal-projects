#include <stepper.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <typedefs.h>
#include <serutil.h>

// Axis 1
// pin 16 PB6
// pin 17 PB7
// pin 18 PG3
// pin 19 PG4

// Axis 2
// pin 36 PC1
// pin 37 PC2
// pin 38 PC3
// pin 39 PC4

const u08 StrAxisX [] PROGMEM = "AxisX:";
const u08 StrAxisY [] PROGMEM = "AxisY:";
const u08 StrCoilAOff  [] PROGMEM = "CoilAOff\r\n";
const u08 StrCoilAFwd  [] PROGMEM = "CoilAFwd\r\n";
const u08 StrCoilABack [] PROGMEM = "CoilABack\r\n";
const u08 StrCoilBOff  [] PROGMEM = "CoilBOff\r\n";
const u08 StrCoilBFwd  [] PROGMEM = "CoilBFwd\r\n";
const u08 StrCoilBBack [] PROGMEM = "CoilBBack\r\n";
const u08 StrStepFwdDone [] PROGMEM = "StepFwdDone\r\n";
const u08 StrStepBackDone [] PROGMEM = "StepBackDone\r\n";
const u08 StrCalling [] PROGMEM = "Calling\r\n";
const u08 StrCalled [] PROGMEM = "Called\r\n";
const u08 StrOverheatTimeout [] PROGMEM = "Overheat timeout\r\n";
const u08 StrInt [] PROGMEM = "Int\r\n";
const u08 StrNow [] PROGMEM = "Now: ";
const u08 StrGoto [] PROGMEM = "Goto: ";


//
// Defines
//
//#define STEPSTRING(xx) STRING(xx)
#define STEPSTRING(xx)


//
// Horizontal Bed
//
#define HORZA_PLUS_ON PORTB |= _BV(6)
#define HORZA_PLUS_OFF PORTB &= ~_BV(6)
#define HORZA_MINUS_ON PORTB |= _BV(7)
#define HORZA_MINUS_OFF PORTB &= ~_BV(7)

#define HORZB_PLUS_ON PORTG |= _BV(3)
#define HORZB_PLUS_OFF PORTG &= ~_BV(3)
#define HORZB_MINUS_ON PORTG |= _BV(4)
#define HORZB_MINUS_OFF PORTG &= ~_BV(4)

// pin 36 PC1
// pin 37 PC2
// pin 38 PC3
// pin 39 PC4
//
// Vertical Bed
//
#define VERTA_PLUS_ON PORTC |= _BV(1)
#define VERTA_PLUS_OFF PORTC &= ~_BV(1)
#define VERTA_MINUS_ON PORTC |= _BV(2)
#define VERTA_MINUS_OFF PORTC &= ~_BV(2)

#define VERTB_PLUS_ON PORTC |= _BV(3)
#define VERTB_PLUS_OFF PORTC &= ~_BV(3)
#define VERTB_MINUS_ON PORTC |= _BV(4)
#define VERTB_MINUS_OFF PORTC &= ~_BV(4)

#define LAST_STATE 3
#define INTDEC_INIT 127 
#define MAX_X 40
#define MAX_Y 40
#define INTERSTEP_DELAY _delay_ms(10);

//
// Function Declarations
//
void stepperInit(void);

void coilAxisXAOff(void);
void coilAxisXAForward(void);
void coilAxisXABack(void);
void coilAxisXBOff(void);
void coilAxisXBForward(void);
void coilAxisXBBack(void);

void coilAxisYAOff(void);
void coilAxisYAForward(void);
void coilAxisYABack(void);
void coilAxisYBOff(void);
void coilAxisYBForward(void);
void coilAxisYBBack(void);
u08 nextState(s08 state);
u08 prevState(s08 state);
void coilState(u08 axis);


//
// Typedefs
//
typedef void (* CoilControl)(void);

typedef enum
{
    AXIS_X,
    AXIS_Y,
    NUM_AXES
} Axis;

typedef enum
{
    DIR_OFF,
    DIR_FORWARD,
    DIR_BACK
} Direction;

typedef enum
{
    AFORWARD,
    ABACK,
    AOFF,
    BFORWARD,
    BBACK,
    BOFF,
    NUM_COMMANDS
} Command;



s08 stepSequence[NUM_AXES];
s08 intDecrement = -1;
s16 posX = 0;
s16 posY = 0;
s16 posZ = 0;

CoilControl axisControl[NUM_AXES][NUM_COMMANDS] =
{
    {
        coilAxisXAForward,
        coilAxisXABack,
        coilAxisXAOff,
        coilAxisXBForward,
        coilAxisXBBack,
        coilAxisXBOff
    },
    { 
        coilAxisYAForward,
        coilAxisYABack,
        coilAxisYAOff,
        coilAxisYBForward,
        coilAxisYBBack,
        coilAxisYBOff
    }
};


ISR(TIMER0_OVF_vect)
{
    if (intDecrement > 0)
    {
        intDecrement--;
    }
    else if (0 == intDecrement)
    {
        TIMSK0 &= ~_BV(TOIE0);
        stepOff();
    }
}

void stepperInit(void)
{
    u08 count;
    DDRB = 0xc0;
    PORTB &= ~0xc0;

    DDRG = 0x18;
    PORTG &= ~0x18;

    DDRC = 0x1e;
    PORTC &= ~0x1e;

    stepOff();
    //
    // set up interrupt timer
    //
    TCCR0A |= _BV(CS02) | _BV(CS00); // clock/1024
    TIMSK0 |= _BV(TOIE0);
}


// --------------------------------------------------------------------------------
// X Axis routines
// --------------------------------------------------------------------------------
void coilAxisXAOff(void)
{
    STEPSTRING(StrAxisX);
    STEPSTRING(StrCoilAOff);
	HORZA_PLUS_OFF;
	HORZA_MINUS_OFF;
}

void coilAxisXAForward(void)
{
    STEPSTRING(StrAxisX);
    STEPSTRING(StrCoilAFwd);
	HORZA_PLUS_ON;
	HORZA_MINUS_OFF;
}

void coilAxisXABack(void)
{
    STEPSTRING(StrAxisX);
    STEPSTRING(StrCoilABack);
	HORZA_PLUS_OFF;
	HORZA_MINUS_ON;
}

void coilAxisXBOff(void)
{
    STEPSTRING(StrAxisX);
    STEPSTRING(StrCoilBOff);
	HORZB_PLUS_OFF;
	HORZB_MINUS_OFF;
}

void coilAxisXBForward(void)
{
    STEPSTRING(StrAxisX);
    STEPSTRING(StrCoilBFwd);
    HORZB_PLUS_ON;
    HORZB_MINUS_OFF;
}

void coilAxisXBBack(void)
{
    STEPSTRING(StrAxisX);
    STEPSTRING(StrCoilBBack);
    HORZB_PLUS_OFF;
    HORZB_MINUS_ON;
}

// --------------------------------------------------------------------------------
// Y Axis routines
// --------------------------------------------------------------------------------
void coilAxisYAOff(void)
{
    STEPSTRING(StrAxisY);
    STEPSTRING(StrCoilAOff);
	VERTA_PLUS_OFF;
	VERTA_MINUS_OFF;
}

void coilAxisYAForward(void)
{
    STEPSTRING(StrAxisY);
    STEPSTRING(StrCoilAFwd);
	VERTA_PLUS_ON;
	VERTA_MINUS_OFF;
}

void coilAxisYABack(void)
{
    STEPSTRING(StrAxisY);
    STEPSTRING(StrCoilABack);
	VERTA_PLUS_OFF;
	VERTA_MINUS_ON;
}

void coilAxisYBOff(void)
{
    STEPSTRING(StrAxisY);
    STEPSTRING(StrCoilBOff);
	VERTB_PLUS_OFF;
	VERTB_MINUS_OFF;
}

void coilAxisYBForward(void)
{
    STEPSTRING(StrAxisY);
    STEPSTRING(StrCoilBFwd);
    VERTB_PLUS_ON;
    VERTB_MINUS_OFF;
}

void coilAxisYBBack(void)
{
    STEPSTRING(StrAxisY);
    STEPSTRING(StrCoilBBack);
    VERTB_PLUS_OFF;
    VERTB_MINUS_ON;
}

u08 nextState(s08 state)
{
    state++;
    if (state > LAST_STATE)
    {
        state = 0;
    }
    return state;
}

u08 prevState(s08 state)
{
    state--;
    if (state < 0)
    {
       state = LAST_STATE;
    }
    return state;
}

void coilState(u08 axis)
{
    switch (stepSequence[axis])
    {
    case 0: 
        axisControl[axis][AFORWARD]();
        axisControl[axis][BBACK]();
        break;

    case 1:
        axisControl[axis][AFORWARD]();
        axisControl[axis][BFORWARD]();
        break;

    case 2:
        axisControl[axis][ABACK]();
        axisControl[axis][BFORWARD]();
        break;

    case 3:
        axisControl[axis][ABACK]();
        axisControl[axis][BBACK]();
        break;

    }
}

//
// Bresenham's line algorithm
//
//void draw2d(int x0, int y0, int x1, int y1)
void draw2d(s16 gotoX, s16 gotoY)
{
    s16 xNow = posX;
    s16 yNow = posY;
    int dx = abs(gotoX - xNow), sx = xNow < gotoX ? 1 : -1;
    int dy = abs(gotoY - yNow), sy = yNow < gotoY ? 1 : -1; 
    int err = (dx>dy ? dx : -dy)/2, e2;


    //int limit;
    //for(limit = 0; limit < 20; limit++)
    for(;;)
    {
        //STEPSTRING(StrNow);
        //uart_send_u16(xNow);
        //uart_send_buffered(':');
        //uart_send_u16(yNow);
        //uart_send_buffered(' ');

        //STEPSTRING(StrGoto);
        //uart_send_u16(gotoX);
        //uart_send_buffered(':');
        //uart_send_u16(gotoY);
        //uart_send_buffered(' ');

        //stepReportAxes();
        while (xNow > posX)
        {
            stepRight();
        }

        while (xNow < posX)
        {
            stepLeft();
        }

        while (yNow > posY)
        {
            stepForward();
        }
        
        while (yNow < posY)
        {
            stepBack();
        }

        if (posX == gotoX && posY == gotoY)
        {
            break;
        }


        e2 = err;
        if (e2 >-dx) { err -= dy; xNow += sx; }
        if (e2 < dy) { err += dx; yNow += sy; }
    }
}

//
// External Interface
//
void stepForward(void)
{
//    if (posY > MAX_Y)
//    {
//        return;
//    }
    posY++;
    intDecrement = INTDEC_INIT;;
    TIMSK0 |= _BV(TOIE0);
    stepSequence[AXIS_X] = prevState(stepSequence[AXIS_X]);
    coilState(AXIS_X);
    INTERSTEP_DELAY;
}

void stepBack(void)
{
//    if (posY == 0)
//    {
//        return;
//    }
    posY--;
    intDecrement = INTDEC_INIT;;
    TIMSK0 |= _BV(TOIE0);
    stepSequence[AXIS_X] = nextState(stepSequence[AXIS_X]);
    coilState(AXIS_X);
    INTERSTEP_DELAY;
}

void stepLeft(void)
{
//    if (posX == 0)
//    {
//        return;
//    }
    posX--;
    intDecrement = INTDEC_INIT;;
    TIMSK0 |= _BV(TOIE0);
    stepSequence[AXIS_Y] = nextState(stepSequence[AXIS_Y]);
    coilState(AXIS_Y);
    INTERSTEP_DELAY;
}

void stepRight(void)
{
//    if (posX > MAX_X)
//    {
//        return;
//    }
    posX++;
    intDecrement = INTDEC_INIT;;
    TIMSK0 |= _BV(TOIE0);
    stepSequence[AXIS_Y] = prevState(stepSequence[AXIS_Y]);
    coilState(AXIS_Y);
    INTERSTEP_DELAY;
}

void stepOff(void)
{
    stepSequence[AXIS_X] = 0;
    axisControl[AXIS_X][AOFF]();
    axisControl[AXIS_X][BOFF]();

    stepSequence[AXIS_Y] = 0;
    axisControl[AXIS_Y][AOFF]();
    axisControl[AXIS_Y][BOFF]();
}

void stepGoto(s16 x, s16 y, s16 z)
{
    draw2d(x, y);
}

void stepSetZero(void)
{
    posX = 0;
    posY = 0;
    posZ = 0;
    stepReportAxes();
}

void stepReportAxes(void)
{
    uart_send_u16(posX);
    uart_send_buffered(':');
    uart_send_u16(posY);
    uart_send_buffered(':');
    uart_send_u16(posZ);
    uart_crlf();
}


