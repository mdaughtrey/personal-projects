#include <avr/io.h>
#include <typedefs.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <cmdproc.h>
#include <displaymux.h>

/* u08 helpString1[] PROGMEM = "\ */
/* a - ascii <char>\n\ */
/* c - primary color index <0-255>\n\ */
/* d - dim <1/0>\n\ */
/* i - invert <1/0>\n\ */
/* m - mirror <1/0>\n\ */
/* p - display programmed matrix <1/0>\n"; */

/* u08 helpString2[] PROGMEM = "\ */
/* r - reverse bits <1/0>\n\ */
/* s - ascii string until NULL\n\ */
/* 0 - programmed column 0 <pri><sec><ter>\n\ */
/* 1 - programmed column 1 <pri><sec><ter>\n\ */
/* 2 - programmed column 2 <pri><sec><ter>\n\ */
/* 3 - programmed column 3 <pri><sec><ter>\n\ */
/* 4 - programmed column 4 <pri><sec><ter>\n\ */
/* "; */
u08 prompt[] PROGMEM = "\n:>";

u08 command = 0;
u08 parameter[3];
u08 paramIndex = 0;
u08 paramsRemaining;

static void cmdDisplayProgrammedMatrix(void)
{
  dm_displayProgrammed(parameter[0]);
}

static void cmdStringUntilNull(void)
{
}

static void cmdAscii(void)
{
  dm_setChar(parameter[0]);
}

static void cmdDim(void)
{
  dm_setDim(parameter[0]);
}

static void cmdSetPrimaryColorIndex(void)
{
}

static void cmdInvert(void)
{
  dm_setInvert(parameter[0]);
}

static void cmdMirror(void)
{
  dm_setMirror(parameter[0]);
}

static void cmdReverse(void)
{
  dm_setReverse(parameter[0]);
}

static void cmdProgColumn(u08 column)
{
  dm_setColumn(column, parameter[0]);
}

void cmd_Init(void)
{
  command = 0;
  paramsRemaining = 0;
  paramIndex = 0;
}

void cmd_dataHandler(u08 input)
{

/*   if (command != 's') */
/*     { */
/*       uart_send_sync (prompt, sizeof(prompt)); */
/*     } */
/*   if (command == 's') */
/*     { */
/*       if (input == '/') */
/* 	{ */
/* 	  command = 0; */
/* 	  return; */
/* 	} */
      uart_send_char (input);
/*       return; */
/*     } */
    
    if (command)
      {
	parameter[paramIndex++] = input;
	paramsRemaining--;
	if (paramsRemaining == 0)
	  {
	    switch (command)
	      {
	      case 'a': cmdAscii(); break;
	      case 'c': cmdSetPrimaryColorIndex(); break;
	      case 'd': cmdDim(); break;
	      case 'i': cmdInvert(); break;
	      case 'm': cmdMirror(); break;
	      case 'p': cmdDisplayProgrammedMatrix();break;
	      case 'r': cmdReverse(); break;
	      case '0':
	      case '1':
	      case '2':
	      case '3':
	      case '4': cmdProgColumn(command - '0'); break;
	      }
	    command = 0;
	    paramIndex = 0;
	  }
	return;
      }
    
    paramsRemaining = 0;
    paramIndex = 0;
    switch (input)
      {
      case 'a':
      case 'c':
      case 'd':
      case 'i':
      case 'm':
      case 'p':
      case 'r':
	command = input;
	paramsRemaining = 1;
	break;
	
      case 's':
	cmdStringUntilNull();
	command = input;
	break;
	
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
	command = input;
	paramsRemaining = 3;
	break;
	
      default:
	command = 0;
	break;
      }
}
