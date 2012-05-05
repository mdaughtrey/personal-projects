#include <avr/io.h>
#include <typedefs.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <cmdproc.h>
#include <displaymux.h>
#include <stdint.h>
#include <avr/wdt.h>

u08 command = 0;
u08 parameter[4];
u08 paramIndex = 0;
u08 paramsRemaining;

extern u08 hexTable[16];

void cmd_Init(void)
{
  command = 0;
  paramsRemaining = 0;
  paramIndex = 0;
}

u08 asciiToHex(u08 ascii1, u08 ascii2)
{
  u08 result = 0;
  u08 index;

  for (index = 0; index < 16; index++)
    {
      if (ascii1 == pgm_read_byte_near(hexTable + index))
	{
	  result = index;
	  break;
	}
    }
  result <<= 4;
  for (index = 0; index < 16; index++)
    {
      if (ascii2 == pgm_read_byte_near(hexTable + index))
	{
	  result |= index;
	  break;
	}
    }
  return result;

}
void cmd_dataHandler(u08 input)
{
  uart_send_char (input);
  
  if (command)
    {
      parameter[paramIndex++] = input;
      paramsRemaining--;
      if (paramsRemaining == 0)
	{
	  u08 which = (parameter[0] == '0' ? 0 : 1);
	  switch (command)
	    {
	      /* Set display 0 character - a<char> */
	    case 'a': dm_setChar(0, parameter[0]); break;
	      
	      /* Set display 1 character - A<char> */
	    case 'A': dm_setChar(1, parameter[0]); break;
	      
	      /* Blank character - b<01> */
	    case 'b': dm_blank(which); break;
	      
	      /* Unblank character - b<01> */
	    case 'B': dm_unBlank(which); break;
	      
	      /* Enable Custom character - c<01>  */
	    case 'c': dm_displayProgrammed(which, 1); break;
	      
	      /* Disable Custom character - C<01> */
	    case 'C': dm_displayProgrammed(which, 0); break;
	      
	      /* Dim display - d<01> */
	    case 'd': dm_setDim(which, 1); break;
	      
	      /* Undim display - D<01> */
	    case 'D': dm_setDim(which, 0); break;
	      
	      /* Flip display - f<01> */
	    case 'f': dm_setFlip(which, 1); break;
	      
	      /* Unflip display - F<01> */
	    case 'F': dm_setFlip(which, 0); break;
	      
	      /* Invert display bits - i<01> */
	    case 'i': dm_setReverse(which, 1); break;
	      
	      /* Uninvert display bits - I<01> */
	    case 'I': dm_setReverse(which, 0); break;
	      
	      /* Set Pallete Index for display 0- l<index> */
	      /* This is ignored for single-color displays */
	    case 'l': dm_setPalette(0, 
				    asciiToHex(parameter[0], parameter[1]));
	      break;

	      /* Set Pallete Index for display 1- L<HH> */
	      /* This is ignored for single-color displays */
	    case 'L': dm_setPalette(1, 
				    asciiToHex(parameter[0], parameter[1]));
	      break;
	      
	      /* Mirror display - m<01> */
	    case 'm': dm_setMirror(which, 1); break;
	      
	      /* Unmirror display M<01> */
	    case 'M': dm_setMirror(which, 0); break;

	      /* Copy character to custom character 0 - p<hex> */
	    case 'p':
	      dm_copyToCustom(0, asciiToHex(parameter[0], parameter[1])); break;

	      /* Copy character to custom character 1 - P<hex> */
	    case 'P':
	      dm_copyToCustom(1, asciiToHex(parameter[0], parameter[1])); break;

	      /* Roll matrix n row<s> - r<01><udlr><n> */
	    case 'r': dm_roll(which,
			      parameter[1],
			      parameter[2] - '0');
	      break;
	      
	      /* Shift matrix <n> rows  - s<01><udlr><n> */
	    case 's': dm_shift(which,
			       parameter[1],
			       parameter[2] - '0');
	      break;

	      /* Turn on pixel in custom character - t<01><row><column> */
	    case 't':
	      dm_pixel(which, 1, parameter[1] - '0', parameter[2] - '0');
	      break;

	      /* Turn off pixel in custom character - T<01><row><column> */
	    case 'T':
	      dm_pixel(which, 0, parameter[1] - '0', parameter[2] - '0');
	      break;
	      
	      /* Reset all transforms - !<01> */
	    case '!': dm_reset(which); break;

	      
	      /* Program custom character columns, display 0 - [01234]<HexHex> */
	    case '0': 
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	      dm_progColumn(0, command - '0', asciiToHex(parameter[0], parameter[1])); 
	      break;

	      /* Program custom character columns, display 1 - [56789]<data> */
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9': 
	      dm_progColumn(1, command -'5', asciiToHex(parameter[0], parameter[1])); 
	      break;

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
    case 'A':
    case 'b':
    case 'B':
    case 'c':
    case 'C':
    case 'd':
    case 'D':
    case 'f':
    case 'F':
    case 'i':
    case 'I':
    case 'm':
    case 'M':
    case 'R':
    case '!':
      command = input;
      paramsRemaining = 1;
      break;
      
    case 'p':
    case 'P':
    case 'l':
    case 'L':
      command = input;
      paramsRemaining = 2;
      break;

    case 'r':
    case 's':
    case 't':
    case 'T':
      command = input;
      paramsRemaining = 3;
      break;
      
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      command = input;
      paramsRemaining = 2;
      break;

    case '*': 
      _wdt_write(1);
      break;

    case '?':
      dm_dumpdisp(0);
      dm_dumpdisp(1);
      break;
      
    default:
      command = 0;
      break;
    }
}
