#ifndef INCLUDED_CMDPROC2_H
#define INCLUDED_CMDPROC2_H

#define CMD_ADD 'a'
#define CMD_SUB 'u'
///
/// \brief f = Flip Characters
/// \details { flip characters vertically }
/// \example fLxxxx
/// \param L - { number of characters (1-9, a=10, b=11, ...) }
/// \param x - 0 or 1 to enable/disable flip for each character
///
#define CMD_FLIP 'f'

///
/// \brief m = Mirror Characters
/// \details { Mirror characters horizontally }
/// \example mLxxxx
/// \param L - { number of characters (1-9, a=10, b=11, ...) }
/// \param x - 0 or 1 to enable/disable mirror for each character
///
#define CMD_MIRROR 'm'

///
/// \brief i = Invert Character Pixels
/// \details { reverse each pixel state }
/// \example iLxxxx
/// \param L - { number of characters (1-9, a=10, b=11, ...) }
/// \param x - 0 or 1 to turn inversion off/on for each character
///
#define CMD_INVERT 'i'

///
/// \brief R = Shift Characters Right
/// \details {shift characters right.} 
/// \example RXnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to shift character
///
#define CMD_SHIFT_RIGHT 'R'

///
/// \brief L = Shift Characters Left
/// \details { Shift characters left }
/// \example LXnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to shift for each character
///
#define CMD_SHIFT_LEFT 'L'

///
/// \brief U = Shift Characters Up
/// \details {Shift characters up.}
/// \example UXnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to shift for each character
///
#define CMD_SHIFT_UP 'U'

///
/// \brief D = Shift Characters Down
/// \details {shift characters down.}
/// \example DXnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to shift for each character
///
#define CMD_SHIFT_DOWN 'D'

///
/// \brief r = Roll Characters Right
/// \details {Roll characters right. The pixels on the right will wrap around to the left side.}
/// \example rXnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to roll each character
///
#define CMD_ROLL_RIGHT 'r'
///
/// \brief l = Roll Characters Left
/// \details { Roll characters left. The pixels on the left will wrap around to the right side.}
/// \example lXnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to roll for each character
///
#define CMD_ROLL_LEFT 'l'
///
/// \brief u = Roll Characters Up
/// \details { Roll characters up. The pixels on the top will wrap around to the bottom.}
/// \example uXnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to roll each character
///
#define CMD_ROLL_UP 'u'
///
/// \brief d = Roll Characters Down
/// \details { Roll characters down. The pixels on the bottom will wrap around to the top.}
/// \example dXnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to roll each character
///
#define CMD_ROLL_DOWN 'd'
///
/// \brief t = Set Text
/// \details { Set all text characters in one command }
/// \example tXcccc
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param c - character
///
#define CMD_SET_TEXT 't'
///
/// \brief p = Set palette
/// \details { Set the palette 
/// \example pXnnnnn
/// \param X - { character position (0-9, a=10, b=11...)}
/// \param n - palette code (0,1,2) for each character
///
#define CMD_SET_PALETTE 'p'
///
/// \brief ! = Reset Transforms
/// \details { Reset characters transforms}
/// \example !Xnnnnnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - 1 or 0 (reset/do not reset transforms)
///
#define CMD_RESET_TRANSFORMS '!'
///
/// \brief c = Enable/disable custom character
/// \details {Enable/disable custom character}
/// \example cXnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - 1 or 0 to enable or disable each character
///
#define CMD_CUSTOM_CHARACTER 'c'
///
/// \brief s = copy to custom character
/// \details {Copy to custom character buffer}
/// \example sXccc
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param c - code for each character 
///
#define CMD_COPY_CUSTOM 's'
///
/// \brief x = Turn on pixel
/// \details {Turn on a single pixel}
/// \example xIrc
/// \param I - character index
/// \param r = row (0-6)
/// \param c = column (0-4)
///
#define CMD_PIXEL_ON 'x'

///
/// \brief X = Turn off pixel
/// \details {Turn off a single pixel}
/// \example XIrc
/// \param I - character index
/// \param r = row (0-6)
/// \param c = column (0-4)
///
#define CMD_PIXEL_OFF 'X'

///
/// \brief b = bitmap
/// \details { Write to pixels a column at a time }
/// \example bXabcdeA...
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param a - byte for character 0, column 0, LSB is row 0, MSB ignored
/// \param b - byte for character 0, column 1, LSB is row 0, MSB ignored
/// \param c - byte for character 0, column 2, LSB is row 0, MSB ignored
/// \param d - byte for character 0, column 3, LSB is row 0, MSB ignored
/// \param e - byte for character 0, column 4, LSB is row 0, MSB ignored
/// \param A - byte for character 1, column 0, LSB is row 0, MSB ignored
///
#define CMD_BITMAP 'b'

///
/// \brief Y = Program Control
/// \details { Control the uploaded program }
/// \example YX
/// \param X - 0 = stop program execution
/// \param X - 1 = run program once
/// \param X - 2 = run program in a loop
///
#define CMD_RUN_PROGRAM 'Y'
#define CMD_DUMP_EEPROM 'y'

///
/// \brief Z = upload program
/// \details { Upload a program }
/// \example ZLLxxxxxxxxxxxxxxxCC
/// \param LL = 16-bit program length
/// \param x  = opcode (LL opcodes sent)
/// \param CC =  16-bit checksum
///
#define CMD_UPLOAD 'Z'

#endif // INCLUDED_CMDPROC2_H

