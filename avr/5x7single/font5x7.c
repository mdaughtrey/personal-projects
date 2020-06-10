#include <avr/pgmspace.h>

unsigned char font5x7[0x500] PROGMEM =
/* unsigned char font5x7[0x280] PROGMEM =  */
  {
    0, /* X        Base 00 Index 0000*/
    0, /* X        Base 00 Index 0001*/
    0, /*          Base 00 Index 0002*/
    0, /*          Base 00 Index 0003*/
    0, /*          Base 00 Index 0004*/
    /*              */
    62, /* X XXXXX  Base 01 Index 0005*/
    85, /* XX X X X Base 01 Index 0006*/
    81, /*  X X   X Base 01 Index 0007*/
    85, /*  X X X X Base 01 Index 0008*/
    62, /*   XXXXX  Base 01 Index 0009*/
    /*              */
    62, /* X XXXXX  Base 02 Index 000a*/
    107, /* XXX X XX Base 02 Index 000b*/
    111, /*  XX XXXX Base 02 Index 000c*/
    107, /*  XX X XX Base 02 Index 000d*/
    62, /*   XXXXX  Base 02 Index 000e*/
    /*              */
    28, /* X  XXX   Base 03 Index 000f*/
    62, /* X XXXXX  Base 03 Index 0010*/
    124, /*  XXXXX   Base 03 Index 0011*/
    62, /*   XXXXX  Base 03 Index 0012*/
    28, /*    XXX   Base 03 Index 0013*/
    /*              */
    8, /* X   X    Base 04 Index 0014*/
    28, /* X  XXX   Base 04 Index 0015*/
    62, /*   XXXXX  Base 04 Index 0016*/
    28, /*    XXX   Base 04 Index 0017*/
    8, /*     X    Base 04 Index 0018*/
    /*              */
    8, /* X   X    Base 05 Index 0019*/
    94, /* XX XXXX  Base 05 Index 001a*/
    119, /*  XXX XXX Base 05 Index 001b*/
    94, /*  X XXXX  Base 05 Index 001c*/
    8, /*     X    Base 05 Index 001d*/
    /*              */
    12, /* X   XX   Base 06 Index 001e*/
    94, /* XX XXXX  Base 06 Index 001f*/
    127, /*  XXXXXXX Base 06 Index 0020*/
    94, /*  X XXXX  Base 06 Index 0021*/
    12, /*     XX   Base 06 Index 0022*/
    /*              */
    0, /* X        Base 07 Index 0023*/
    0, /* X        Base 07 Index 0024*/
    0, /*          Base 07 Index 0025*/
    0, /*          Base 07 Index 0026*/
    0, /*          Base 07 Index 0027*/
    /*              */
    0, /* X        Base 08 Index 0028*/
    0, /* X        Base 08 Index 0029*/
    0, /*          Base 08 Index 002a*/
    0, /*          Base 08 Index 002b*/
    0, /*          Base 08 Index 002c*/
    /*              */
    0, /* X        Base 09 Index 002d*/
    0, /* X        Base 09 Index 002e*/
    0, /*          Base 09 Index 002f*/
    0, /*          Base 09 Index 0030*/
    0, /*          Base 09 Index 0031*/
    /*              */
    0, /* X        Base 0a Index 0032*/
    0, /* X        Base 0a Index 0033*/
    0, /*          Base 0a Index 0034*/
    0, /*          Base 0a Index 0035*/
    0, /*          Base 0a Index 0036*/
    /*              */
    48, /* X XX     Base 0b Index 0037*/
    72, /* XX  X    Base 0b Index 0038*/
    77, /*  X  XX X Base 0b Index 0039*/
    51, /*   XX  XX Base 0b Index 003a*/
    7, /*      XXX Base 0b Index 003b*/
    /*              */
    38, /* X X  XX  Base 0c Index 003c*/
    41, /* X X X  X Base 0c Index 003d*/
    121, /*  XXXX  X Base 0c Index 003e*/
    41, /*   X X  X Base 0c Index 003f*/
    38, /*   X  XX  Base 0c Index 0040*/
    /*              */
    96, /* XXX      Base 0d Index 0041*/
    127, /* XXXXXXXX Base 0d Index 0042*/
    5, /*      X X Base 0d Index 0043*/
    5, /*      X X Base 0d Index 0044*/
    7, /*      XXX Base 0d Index 0045*/
    /*              */
    96, /* XXX      Base 0e Index 0046*/
    127, /* XXXXXXXX Base 0e Index 0047*/
    5, /*      X X Base 0e Index 0048*/
    101, /*  XX  X X Base 0e Index 0049*/
    127, /*  XXXXXXX Base 0e Index 004a*/
    /*              */
    42, /* X X X X  Base 0f Index 004b*/
    28, /* X  XXX   Base 0f Index 004c*/
    119, /*  XXX XXX Base 0f Index 004d*/
    28, /*    XXX   Base 0f Index 004e*/
    42, /*   X X X  Base 0f Index 004f*/
    /*              */
    127, /* XXXXXXXX Base 10 Index 0050*/
    62, /* X XXXXX  Base 10 Index 0051*/
    28, /*    XXX   Base 10 Index 0052*/
    8, /*     X    Base 10 Index 0053*/
    0, /*          Base 10 Index 0054*/
    /*              */
    8, /* X   X    Base 11 Index 0055*/
    28, /* X  XXX   Base 11 Index 0056*/
    62, /*   XXXXX  Base 11 Index 0057*/
    127, /*  XXXXXXX Base 11 Index 0058*/
    0, /*          Base 11 Index 0059*/
    /*              */
    20, /* X  X X   Base 12 Index 005a*/
    54, /* X XX XX  Base 12 Index 005b*/
    127, /*  XXXXXXX Base 12 Index 005c*/
    54, /*   XX XX  Base 12 Index 005d*/
    20, /*    X X   Base 12 Index 005e*/
    /*              */
    0, /* X        Base 13 Index 005f*/
    95, /* XX XXXXX Base 13 Index 0060*/
    0, /*          Base 13 Index 0061*/
    95, /*  X XXXXX Base 13 Index 0062*/
    0, /*          Base 13 Index 0063*/
    /*              */
    14, /* X   XXX  Base 14 Index 0064*/
    17, /* X  X   X Base 14 Index 0065*/
    127, /*  XXXXXXX Base 14 Index 0066*/
    1, /*        X Base 14 Index 0067*/
    127, /*  XXXXXXX Base 14 Index 0068*/
    /*              */
    32, /* X X      Base 15 Index 0069*/
    74, /* XX  X X  Base 15 Index 006a*/
    85, /*  X X X X Base 15 Index 006b*/
    41, /*   X X  X Base 15 Index 006c*/
    2, /*       X  Base 15 Index 006d*/
    /*              */
    48, /* X XX     Base 16 Index 006e*/
    48, /* X XX     Base 16 Index 006f*/
    48, /*   XX     Base 16 Index 0070*/
    48, /*   XX     Base 16 Index 0071*/
    0, /*          Base 16 Index 0072*/
    /*              */
    84, /* XX X X   Base 17 Index 0073*/
    118, /* XXXX XX  Base 17 Index 0074*/
    127, /*  XXXXXXX Base 17 Index 0075*/
    118, /*  XXX XX  Base 17 Index 0076*/
    84, /*  X X X   Base 17 Index 0077*/
    /*              */
    4, /* X    X   Base 18 Index 0078*/
    6, /* X    XX  Base 18 Index 0079*/
    127, /*  XXXXXXX Base 18 Index 007a*/
    6, /*      XX  Base 18 Index 007b*/
    4, /*      X   Base 18 Index 007c*/
    /*              */
    16, /* X  X     Base 19 Index 007d*/
    48, /* X XX     Base 19 Index 007e*/
    127, /*  XXXXXXX Base 19 Index 007f*/
    48, /*   XX     Base 19 Index 0080*/
    16, /*    X     Base 19 Index 0081*/
    /*              */
    8, /* X   X    Base 1a Index 0082*/
    8, /* X   X    Base 1a Index 0083*/
    62, /*   XXXXX  Base 1a Index 0084*/
    28, /*    XXX   Base 1a Index 0085*/
    8, /*     X    Base 1a Index 0086*/
    /*              */
    8, /* X   X    Base 1b Index 0087*/
    28, /* X  XXX   Base 1b Index 0088*/
    62, /*   XXXXX  Base 1b Index 0089*/
    8, /*     X    Base 1b Index 008a*/
    8, /*     X    Base 1b Index 008b*/
    /*              */
    0, /* X        Base 1c Index 008c*/
    28, /* X  XXX   Base 1c Index 008d*/
    28, /*    XXX   Base 1c Index 008e*/
    16, /*    X     Base 1c Index 008f*/
    16, /*    X     Base 1c Index 0090*/
    /*              */
    8, /* X   X    Base 1d Index 0091*/
    28, /* X  XXX   Base 1d Index 0092*/
    8, /*     X    Base 1d Index 0093*/
    28, /*    XXX   Base 1d Index 0094*/
    8, /*     X    Base 1d Index 0095*/
    /*              */
    16, /* X  X     Base 1e Index 0096*/
    24, /* X  XX    Base 1e Index 0097*/
    28, /*    XXX   Base 1e Index 0098*/
    24, /*    XX    Base 1e Index 0099*/
    16, /*    X     Base 1e Index 009a*/
    /*              */
    4, /* X    X   Base 1f Index 009b*/
    12, /* X   XX   Base 1f Index 009c*/
    28, /*    XXX   Base 1f Index 009d*/
    12, /*     XX   Base 1f Index 009e*/
    4, /*      X   Base 1f Index 009f*/
    /*              */
    0, /* X        Base 20 Index 00a0*/
    0, /* X        Base 20 Index 00a1*/
    0, /*          Base 20 Index 00a2*/
    0, /*          Base 20 Index 00a3*/
    0, /*          Base 20 Index 00a4*/
    /*              */
    0, /* X        Base 21 Index 00a5*/
    95, /* XX XXXXX Base 21 Index 00a6*/
    0, /*          Base 21 Index 00a7*/
    0, /*          Base 21 Index 00a8*/
    0, /*          Base 21 Index 00a9*/
    /*              */
    0, /* X        Base 22 Index 00aa*/
    3, /* X     XX Base 22 Index 00ab*/
    0, /*          Base 22 Index 00ac*/
    3, /*       XX Base 22 Index 00ad*/
    0, /*          Base 22 Index 00ae*/
    /*              */
    20, /* X  X X   Base 23 Index 00af*/
    62, /* X XXXXX  Base 23 Index 00b0*/
    20, /*    X X   Base 23 Index 00b1*/
    62, /*   XXXXX  Base 23 Index 00b2*/
    20, /*    X X   Base 23 Index 00b3*/
    /*              */
    36, /* X X  X   Base 24 Index 00b4*/
    42, /* X X X X  Base 24 Index 00b5*/
    127, /*  XXXXXXX Base 24 Index 00b6*/
    42, /*   X X X  Base 24 Index 00b7*/
    18, /*    X  X  Base 24 Index 00b8*/
    /*              */
    35, /* X X   XX Base 25 Index 00b9*/
    19, /* X  X  XX Base 25 Index 00ba*/
    8, /*     X    Base 25 Index 00bb*/
    100, /*  XX  X   Base 25 Index 00bc*/
    98, /*  XX   X  Base 25 Index 00bd*/
    /*              */
    54, /* X XX XX  Base 26 Index 00be*/
    73, /* XX  X  X Base 26 Index 00bf*/
    86, /*  X X XX  Base 26 Index 00c0*/
    32, /*   X      Base 26 Index 00c1*/
    80, /*  X X     Base 26 Index 00c2*/
    /*              */
    0, /* X        Base 27 Index 00c3*/
    2, /* X     X  Base 27 Index 00c4*/
    1, /*        X Base 27 Index 00c5*/
    0, /*          Base 27 Index 00c6*/
    0, /*          Base 27 Index 00c7*/
    /*              */
    0, /* X        Base 28 Index 00c8*/
    28, /* X  XXX   Base 28 Index 00c9*/
    34, /*   X   X  Base 28 Index 00ca*/
    65, /*  X     X Base 28 Index 00cb*/
    0, /*          Base 28 Index 00cc*/
    /*              */
    0, /* X        Base 29 Index 00cd*/
    65, /* XX     X Base 29 Index 00ce*/
    34, /*   X   X  Base 29 Index 00cf*/
    28, /*    XXX   Base 29 Index 00d0*/
    0, /*          Base 29 Index 00d1*/
    /*              */
    42, /* X X X X  Base 2a Index 00d2*/
    28, /* X  XXX   Base 2a Index 00d3*/
    127, /*  XXXXXXX Base 2a Index 00d4*/
    28, /*    XXX   Base 2a Index 00d5*/
    42, /*   X X X  Base 2a Index 00d6*/
    /*              */
    8, /* X   X    Base 2b Index 00d7*/
    8, /* X   X    Base 2b Index 00d8*/
    62, /*   XXXXX  Base 2b Index 00d9*/
    8, /*     X    Base 2b Index 00da*/
    8, /*     X    Base 2b Index 00db*/
    /*              */
    64, /* XX       Base 2c Index 00dc*/
    96, /* XXX      Base 2c Index 00dd*/
    32, /*   X      Base 2c Index 00de*/
    0, /*          Base 2c Index 00df*/
    0, /*          Base 2c Index 00e0*/
    /*              */
    8, /* X   X    Base 2d Index 00e1*/
    8, /* X   X    Base 2d Index 00e2*/
    8, /*     X    Base 2d Index 00e3*/
    8, /*     X    Base 2d Index 00e4*/
    0, /*          Base 2d Index 00e5*/
    /*              */
    0, /* X        Base 2e Index 00e6*/
    96, /* XXX      Base 2e Index 00e7*/
    96, /*  XX      Base 2e Index 00e8*/
    0, /*          Base 2e Index 00e9*/
    0, /*          Base 2e Index 00ea*/
    /*              */
    64, /* XX       Base 2f Index 00eb*/
    48, /* X XX     Base 2f Index 00ec*/
    8, /*     X    Base 2f Index 00ed*/
    6, /*      XX  Base 2f Index 00ee*/
    1, /*        X Base 2f Index 00ef*/
    /*              */
    62, /* X XXXXX  Base 30 Index 00f0*/
    81, /* XX X   X Base 30 Index 00f1*/
    73, /*  X  X  X Base 30 Index 00f2*/
    69, /*  X   X X Base 30 Index 00f3*/
    62, /*   XXXXX  Base 30 Index 00f4*/
    /*              */
    0, /* X        Base 31 Index 00f5*/
    66, /* XX    X  Base 31 Index 00f6*/
    127, /*  XXXXXXX Base 31 Index 00f7*/
    64, /*  X       Base 31 Index 00f8*/
    0, /*          Base 31 Index 00f9*/
    /*              */
    66, /* XX    X  Base 32 Index 00fa*/
    97, /* XXX    X Base 32 Index 00fb*/
    81, /*  X X   X Base 32 Index 00fc*/
    73, /*  X  X  X Base 32 Index 00fd*/
    70, /*  X   XX  Base 32 Index 00fe*/
    /*              */
    34, /* X X   X  Base 33 Index 00ff*/
    65, /* XX     X Base 33 Index 0100*/
    73, /*  X  X  X Base 33 Index 0101*/
    73, /*  X  X  X Base 33 Index 0102*/
    54, /*   XX XX  Base 33 Index 0103*/
    /*              */
    24, /* X  XX    Base 34 Index 0104*/
    20, /* X  X X   Base 34 Index 0105*/
    18, /*    X  X  Base 34 Index 0106*/
    127, /*  XXXXXXX Base 34 Index 0107*/
    16, /*    X     Base 34 Index 0108*/
    /*              */
    39, /* X X  XXX Base 35 Index 0109*/
    69, /* XX   X X Base 35 Index 010a*/
    69, /*  X   X X Base 35 Index 010b*/
    69, /*  X   X X Base 35 Index 010c*/
    57, /*   XXX  X Base 35 Index 010d*/
    /*              */
    60, /* X XXXX   Base 36 Index 010e*/
    74, /* XX  X X  Base 36 Index 010f*/
    73, /*  X  X  X Base 36 Index 0110*/
    73, /*  X  X  X Base 36 Index 0111*/
    48, /*   XX     Base 36 Index 0112*/
    /*              */
    1, /* X      X Base 37 Index 0113*/
    113, /* XXXX   X Base 37 Index 0114*/
    9, /*     X  X Base 37 Index 0115*/
    5, /*      X X Base 37 Index 0116*/
    3, /*       XX Base 37 Index 0117*/
    /*              */
    54, /* X XX XX  Base 38 Index 0118*/
    73, /* XX  X  X Base 38 Index 0119*/
    73, /*  X  X  X Base 38 Index 011a*/
    73, /*  X  X  X Base 38 Index 011b*/
    54, /*   XX XX  Base 38 Index 011c*/
    /*              */
    6, /* X    XX  Base 39 Index 011d*/
    73, /* XX  X  X Base 39 Index 011e*/
    73, /*  X  X  X Base 39 Index 011f*/
    73, /*  X  X  X Base 39 Index 0120*/
    62, /*   XXXXX  Base 39 Index 0121*/
    /*              */
    0, /* X        Base 3a Index 0122*/
    54, /* X XX XX  Base 3a Index 0123*/
    54, /*   XX XX  Base 3a Index 0124*/
    0, /*          Base 3a Index 0125*/
    0, /*          Base 3a Index 0126*/
    /*              */
    0, /* X        Base 3b Index 0127*/
    64, /* XX       Base 3b Index 0128*/
    118, /*  XXX XX  Base 3b Index 0129*/
    54, /*   XX XX  Base 3b Index 012a*/
    0, /*          Base 3b Index 012b*/
    /*              */
    8, /* X   X    Base 3c Index 012c*/
    20, /* X  X X   Base 3c Index 012d*/
    34, /*   X   X  Base 3c Index 012e*/
    65, /*  X     X Base 3c Index 012f*/
    0, /*          Base 3c Index 0130*/
    /*              */
    20, /* X  X X   Base 3d Index 0131*/
    20, /* X  X X   Base 3d Index 0132*/
    20, /*    X X   Base 3d Index 0133*/
    20, /*    X X   Base 3d Index 0134*/
    0, /*          Base 3d Index 0135*/
    /*              */
    65, /* XX     X Base 3e Index 0136*/
    34, /* X X   X  Base 3e Index 0137*/
    20, /*    X X   Base 3e Index 0138*/
    8, /*     X    Base 3e Index 0139*/
    0, /*          Base 3e Index 013a*/
    /*              */
    2, /* X     X  Base 3f Index 013b*/
    1, /* X      X Base 3f Index 013c*/
    81, /*  X X   X Base 3f Index 013d*/
    9, /*     X  X Base 3f Index 013e*/
    6, /*      XX  Base 3f Index 013f*/
    /*              */
    62, /* X XXXXX  Base 40 Index 0140*/
    65, /* XX     X Base 40 Index 0141*/
    93, /*  X XXX X Base 40 Index 0142*/
    85, /*  X X X X Base 40 Index 0143*/
    94, /*  X XXXX  Base 40 Index 0144*/
    /*              */
    124, /* XXXXXX   Base 41 Index 0145*/
    18, /* X  X  X  Base 41 Index 0146*/
    17, /*    X   X Base 41 Index 0147*/
    18, /*    X  X  Base 41 Index 0148*/
    124, /*  XXXXX   Base 41 Index 0149*/
    /*              */
    127, /* XXXXXXXX Base 42 Index 014a*/
    73, /* XX  X  X Base 42 Index 014b*/
    73, /*  X  X  X Base 42 Index 014c*/
    73, /*  X  X  X Base 42 Index 014d*/
    54, /*   XX XX  Base 42 Index 014e*/
    /*              */
    62, /* X XXXXX  Base 43 Index 014f*/
    65, /* XX     X Base 43 Index 0150*/
    65, /*  X     X Base 43 Index 0151*/
    65, /*  X     X Base 43 Index 0152*/
    34, /*   X   X  Base 43 Index 0153*/
    /*              */
    127, /* XXXXXXXX Base 44 Index 0154*/
    65, /* XX     X Base 44 Index 0155*/
    65, /*  X     X Base 44 Index 0156*/
    65, /*  X     X Base 44 Index 0157*/
    62, /*   XXXXX  Base 44 Index 0158*/
    /*              */
    127, /* XXXXXXXX Base 45 Index 0159*/
    73, /* XX  X  X Base 45 Index 015a*/
    73, /*  X  X  X Base 45 Index 015b*/
    73, /*  X  X  X Base 45 Index 015c*/
    65, /*  X     X Base 45 Index 015d*/
    /*              */
    127, /* XXXXXXXX Base 46 Index 015e*/
    9, /* X   X  X Base 46 Index 015f*/
    9, /*     X  X Base 46 Index 0160*/
    9, /*     X  X Base 46 Index 0161*/
    1, /*        X Base 46 Index 0162*/
    /*              */
    62, /* X XXXXX  Base 47 Index 0163*/
    65, /* XX     X Base 47 Index 0164*/
    73, /*  X  X  X Base 47 Index 0165*/
    73, /*  X  X  X Base 47 Index 0166*/
    58, /*   XXX X  Base 47 Index 0167*/
    /*              */
    127, /* XXXXXXXX Base 48 Index 0168*/
    8, /* X   X    Base 48 Index 0169*/
    8, /*     X    Base 48 Index 016a*/
    8, /*     X    Base 48 Index 016b*/
    127, /*  XXXXXXX Base 48 Index 016c*/
    /*              */
    0, /* X        Base 49 Index 016d*/
    65, /* XX     X Base 49 Index 016e*/
    127, /*  XXXXXXX Base 49 Index 016f*/
    65, /*  X     X Base 49 Index 0170*/
    0, /*          Base 49 Index 0171*/
    /*              */
    32, /* X X      Base 4a Index 0172*/
    64, /* XX       Base 4a Index 0173*/
    64, /*  X       Base 4a Index 0174*/
    63, /*   XXXXXX Base 4a Index 0175*/
    0, /*          Base 4a Index 0176*/
    /*              */
    127, /* XXXXXXXX Base 4b Index 0177*/
    8, /* X   X    Base 4b Index 0178*/
    20, /*    X X   Base 4b Index 0179*/
    34, /*   X   X  Base 4b Index 017a*/
    65, /*  X     X Base 4b Index 017b*/
    /*              */
    127, /* XXXXXXXX Base 4c Index 017c*/
    64, /* XX       Base 4c Index 017d*/
    64, /*  X       Base 4c Index 017e*/
    64, /*  X       Base 4c Index 017f*/
    64, /*  X       Base 4c Index 0180*/
    /*              */
    127, /* XXXXXXXX Base 4d Index 0181*/
    2, /* X     X  Base 4d Index 0182*/
    12, /*     XX   Base 4d Index 0183*/
    2, /*       X  Base 4d Index 0184*/
    127, /*  XXXXXXX Base 4d Index 0185*/
    /*              */
    127, /* XXXXXXXX Base 4e Index 0186*/
    2, /* X     X  Base 4e Index 0187*/
    4, /*      X   Base 4e Index 0188*/
    8, /*     X    Base 4e Index 0189*/
    127, /*  XXXXXXX Base 4e Index 018a*/
    /*              */
    62, /* X XXXXX  Base 4f Index 018b*/
    65, /* XX     X Base 4f Index 018c*/
    65, /*  X     X Base 4f Index 018d*/
    65, /*  X     X Base 4f Index 018e*/
    62, /*   XXXXX  Base 4f Index 018f*/
    /*              */
    127, /* XXXXXXXX Base 50 Index 0190*/
    9, /* X   X  X Base 50 Index 0191*/
    9, /*     X  X Base 50 Index 0192*/
    9, /*     X  X Base 50 Index 0193*/
    6, /*      XX  Base 50 Index 0194*/
    /*              */
    62, /* X XXXXX  Base 51 Index 0195*/
    65, /* XX     X Base 51 Index 0196*/
    81, /*  X X   X Base 51 Index 0197*/
    33, /*   X    X Base 51 Index 0198*/
    94, /*  X XXXX  Base 51 Index 0199*/
    /*              */
    127, /* XXXXXXXX Base 52 Index 019a*/
    9, /* X   X  X Base 52 Index 019b*/
    25, /*    XX  X Base 52 Index 019c*/
    41, /*   X X  X Base 52 Index 019d*/
    70, /*  X   XX  Base 52 Index 019e*/
    /*              */
    38, /* X X  XX  Base 53 Index 019f*/
    73, /* XX  X  X Base 53 Index 01a0*/
    73, /*  X  X  X Base 53 Index 01a1*/
    73, /*  X  X  X Base 53 Index 01a2*/
    50, /*   XX  X  Base 53 Index 01a3*/
    /*              */
    1, /* X      X Base 54 Index 01a4*/
    1, /* X      X Base 54 Index 01a5*/
    127, /*  XXXXXXX Base 54 Index 01a6*/
    1, /*        X Base 54 Index 01a7*/
    1, /*        X Base 54 Index 01a8*/
    /*              */
    63, /* X XXXXXX Base 55 Index 01a9*/
    64, /* XX       Base 55 Index 01aa*/
    64, /*  X       Base 55 Index 01ab*/
    64, /*  X       Base 55 Index 01ac*/
    63, /*   XXXXXX Base 55 Index 01ad*/
    /*              */
    31, /* X  XXXXX Base 56 Index 01ae*/
    32, /* X X      Base 56 Index 01af*/
    64, /*  X       Base 56 Index 01b0*/
    32, /*   X      Base 56 Index 01b1*/
    31, /*    XXXXX Base 56 Index 01b2*/
    /*              */
    63, /* X XXXXXX Base 57 Index 01b3*/
    64, /* XX       Base 57 Index 01b4*/
    56, /*   XXX    Base 57 Index 01b5*/
    64, /*  X       Base 57 Index 01b6*/
    63, /*   XXXXXX Base 57 Index 01b7*/
    /*              */
    99, /* XXX   XX Base 58 Index 01b8*/
    20, /* X  X X   Base 58 Index 01b9*/
    8, /*     X    Base 58 Index 01ba*/
    20, /*    X X   Base 58 Index 01bb*/
    99, /*  XX   XX Base 58 Index 01bc*/
    /*              */
    7, /* X    XXX Base 59 Index 01bd*/
    8, /* X   X    Base 59 Index 01be*/
    112, /*  XXX     Base 59 Index 01bf*/
    8, /*     X    Base 59 Index 01c0*/
    7, /*      XXX Base 59 Index 01c1*/
    /*              */
    97, /* XXX    X Base 5a Index 01c2*/
    81, /* XX X   X Base 5a Index 01c3*/
    73, /*  X  X  X Base 5a Index 01c4*/
    69, /*  X   X X Base 5a Index 01c5*/
    67, /*  X    XX Base 5a Index 01c6*/
    /*              */
    0, /* X        Base 5b Index 01c7*/
    127, /* XXXXXXXX Base 5b Index 01c8*/
    65, /*  X     X Base 5b Index 01c9*/
    0, /*          Base 5b Index 01ca*/
    0, /*          Base 5b Index 01cb*/
    /*              */
    1, /* X      X Base 5c Index 01cc*/
    6, /* X    XX  Base 5c Index 01cd*/
    8, /*     X    Base 5c Index 01ce*/
    48, /*   XX     Base 5c Index 01cf*/
    64, /*  X       Base 5c Index 01d0*/
    /*              */
    0, /* X        Base 5d Index 01d1*/
    65, /* XX     X Base 5d Index 01d2*/
    127, /*  XXXXXXX Base 5d Index 01d3*/
    0, /*          Base 5d Index 01d4*/
    0, /*          Base 5d Index 01d5*/
    /*              */
    0, /* X        Base 5e Index 01d6*/
    2, /* X     X  Base 5e Index 01d7*/
    1, /*        X Base 5e Index 01d8*/
    2, /*       X  Base 5e Index 01d9*/
    0, /*          Base 5e Index 01da*/
    /*              */
    64, /* XX       Base 5f Index 01db*/
    64, /* XX       Base 5f Index 01dc*/
    64, /*  X       Base 5f Index 01dd*/
    64, /*  X       Base 5f Index 01de*/
    64, /*  X       Base 5f Index 01df*/
    /*              */
    0, /* X        Base 60 Index 01e0*/
    1, /* X      X Base 60 Index 01e1*/
    2, /*       X  Base 60 Index 01e2*/
    0, /*          Base 60 Index 01e3*/
    0, /*          Base 60 Index 01e4*/
    /*              */
    32, /* X X      Base 61 Index 01e5*/
    84, /* XX X X   Base 61 Index 01e6*/
    84, /*  X X X   Base 61 Index 01e7*/
    84, /*  X X X   Base 61 Index 01e8*/
    120, /*  XXXX    Base 61 Index 01e9*/
    /*              */
    127, /* XXXXXXXX Base 62 Index 01ea*/
    68, /* XX   X   Base 62 Index 01eb*/
    68, /*  X   X   Base 62 Index 01ec*/
    68, /*  X   X   Base 62 Index 01ed*/
    56, /*   XXX    Base 62 Index 01ee*/
    /*              */
    56, /* X XXX    Base 63 Index 01ef*/
    68, /* XX   X   Base 63 Index 01f0*/
    68, /*  X   X   Base 63 Index 01f1*/
    68, /*  X   X   Base 63 Index 01f2*/
    68, /*  X   X   Base 63 Index 01f3*/
    /*              */
    56, /* X XXX    Base 64 Index 01f4*/
    68, /* XX   X   Base 64 Index 01f5*/
    68, /*  X   X   Base 64 Index 01f6*/
    68, /*  X   X   Base 64 Index 01f7*/
    127, /*  XXXXXXX Base 64 Index 01f8*/
    /*              */
    56, /* X XXX    Base 65 Index 01f9*/
    84, /* XX X X   Base 65 Index 01fa*/
    84, /*  X X X   Base 65 Index 01fb*/
    84, /*  X X X   Base 65 Index 01fc*/
    88, /*  X XX    Base 65 Index 01fd*/
    /*              */
    4, /* X    X   Base 66 Index 01fe*/
    126, /* XXXXXXX  Base 66 Index 01ff*/
    5, /*      X X Base 66 Index 0200*/
    1, /*        X Base 66 Index 0201*/
    0, /*          Base 66 Index 0202*/
    /*              */
    72, /* XX  X    Base 67 Index 0203*/
    84, /* XX X X   Base 67 Index 0204*/
    84, /*  X X X   Base 67 Index 0205*/
    84, /*  X X X   Base 67 Index 0206*/
    60, /*   XXXX   Base 67 Index 0207*/
    /*              */
    127, /* XXXXXXXX Base 68 Index 0208*/
    8, /* X   X    Base 68 Index 0209*/
    4, /*      X   Base 68 Index 020a*/
    4, /*      X   Base 68 Index 020b*/
    120, /*  XXXX    Base 68 Index 020c*/
    /*              */
    0, /* X        Base 69 Index 020d*/
    0, /* X        Base 69 Index 020e*/
    125, /*  XXXXX X Base 69 Index 020f*/
    0, /*          Base 69 Index 0210*/
    0, /*          Base 69 Index 0211*/
    /*              */
    32, /* X X      Base 6a Index 0212*/
    64, /* XX       Base 6a Index 0213*/
    61, /*   XXXX X Base 6a Index 0214*/
    0, /*          Base 6a Index 0215*/
    0, /*          Base 6a Index 0216*/
    /*              */
    127, /* XXXXXXXX Base 6b Index 0217*/
    16, /* X  X     Base 6b Index 0218*/
    40, /*   X X    Base 6b Index 0219*/
    68, /*  X   X   Base 6b Index 021a*/
    0, /*          Base 6b Index 021b*/
    /*              */
    0, /* X        Base 6c Index 021c*/
    0, /* X        Base 6c Index 021d*/
    127, /*  XXXXXXX Base 6c Index 021e*/
    0, /*          Base 6c Index 021f*/
    0, /*          Base 6c Index 0220*/
    /*              */
    120, /* XXXXX    Base 6d Index 0221*/
    4, /* X    X   Base 6d Index 0222*/
    24, /*    XX    Base 6d Index 0223*/
    4, /*      X   Base 6d Index 0224*/
    120, /*  XXXX    Base 6d Index 0225*/
    /*              */
    124, /* XXXXXX   Base 6e Index 0226*/
    8, /* X   X    Base 6e Index 0227*/
    4, /*      X   Base 6e Index 0228*/
    4, /*      X   Base 6e Index 0229*/
    120, /*  XXXX    Base 6e Index 022a*/
    /*              */
    56, /* X XXX    Base 6f Index 022b*/
    68, /* XX   X   Base 6f Index 022c*/
    68, /*  X   X   Base 6f Index 022d*/
    68, /*  X   X   Base 6f Index 022e*/
    56, /*   XXX    Base 6f Index 022f*/
    /*              */
    124, /* XXXXXX   Base 70 Index 0230*/
    20, /* X  X X   Base 70 Index 0231*/
    20, /*    X X   Base 70 Index 0232*/
    8, /*     X    Base 70 Index 0233*/
    0, /*          Base 70 Index 0234*/
    /*              */
    8, /* X   X    Base 71 Index 0235*/
    20, /* X  X X   Base 71 Index 0236*/
    20, /*    X X   Base 71 Index 0237*/
    124, /*  XXXXX   Base 71 Index 0238*/
    0, /*          Base 71 Index 0239*/
    /*              */
    124, /* XXXXXX   Base 72 Index 023a*/
    8, /* X   X    Base 72 Index 023b*/
    4, /*      X   Base 72 Index 023c*/
    4, /*      X   Base 72 Index 023d*/
    0, /*          Base 72 Index 023e*/
    /*              */
    72, /* XX  X    Base 73 Index 023f*/
    84, /* XX X X   Base 73 Index 0240*/
    84, /*  X X X   Base 73 Index 0241*/
    84, /*  X X X   Base 73 Index 0242*/
    36, /*   X  X   Base 73 Index 0243*/
    /*              */
    0, /* X        Base 74 Index 0244*/
    4, /* X    X   Base 74 Index 0245*/
    127, /*  XXXXXXX Base 74 Index 0246*/
    68, /*  X   X   Base 74 Index 0247*/
    0, /*          Base 74 Index 0248*/
    /*              */
    60, /* X XXXX   Base 75 Index 0249*/
    64, /* XX       Base 75 Index 024a*/
    64, /*  X       Base 75 Index 024b*/
    60, /*   XXXX   Base 75 Index 024c*/
    0, /*          Base 75 Index 024d*/
    /*              */
    12, /* X   XX   Base 76 Index 024e*/
    48, /* X XX     Base 76 Index 024f*/
    64, /*  X       Base 76 Index 0250*/
    48, /*   XX     Base 76 Index 0251*/
    12, /*     XX   Base 76 Index 0252*/
    /*              */
    60, /* X XXXX   Base 77 Index 0253*/
    64, /* XX       Base 77 Index 0254*/
    48, /*   XX     Base 77 Index 0255*/
    64, /*  X       Base 77 Index 0256*/
    60, /*   XXXX   Base 77 Index 0257*/
    /*              */
    68, /* XX   X   Base 78 Index 0258*/
    40, /* X X X    Base 78 Index 0259*/
    16, /*    X     Base 78 Index 025a*/
    40, /*   X X    Base 78 Index 025b*/
    68, /*  X   X   Base 78 Index 025c*/
    /*              */
    76, /* XX  XX   Base 79 Index 025d*/
    80, /* XX X     Base 79 Index 025e*/
    80, /*  X X     Base 79 Index 025f*/
    60, /*   XXXX   Base 79 Index 0260*/
    0, /*          Base 79 Index 0261*/
    /*              */
    68, /* XX   X   Base 7a Index 0262*/
    100, /* XXX  X   Base 7a Index 0263*/
    84, /*  X X X   Base 7a Index 0264*/
    76, /*  X  XX   Base 7a Index 0265*/
    68, /*  X   X   Base 7a Index 0266*/
    /*              */
    0, /* X        Base 7b Index 0267*/
    8, /* X   X    Base 7b Index 0268*/
    54, /*   XX XX  Base 7b Index 0269*/
    65, /*  X     X Base 7b Index 026a*/
    0, /*          Base 7b Index 026b*/
    /*              */
    0, /* X        Base 7c Index 026c*/
    0, /* X        Base 7c Index 026d*/
    119, /*  XXX XXX Base 7c Index 026e*/
    0, /*          Base 7c Index 026f*/
    0, /*          Base 7c Index 0270*/
    /*              */
    0, /* X        Base 7d Index 0271*/
    65, /* XX     X Base 7d Index 0272*/
    54, /*   XX XX  Base 7d Index 0273*/
    8, /*     X    Base 7d Index 0274*/
    0, /*          Base 7d Index 0275*/
    /*              */
    2, /* X     X  Base 7e Index 0276*/
    1, /* X      X Base 7e Index 0277*/
    2, /*       X  Base 7e Index 0278*/
    4, /*      X   Base 7e Index 0279*/
    2, /*       X  Base 7e Index 027a*/
    /*              */
    0, /* X        Base 7f Index 027b*/
    62, /* X XXXXX  Base 7f Index 027c*/
    34, /*   X   X  Base 7f Index 027d*/
    62, /*   XXXXX  Base 7f Index 027e*/
    0, /*          Base 7f Index 027f*/
    /*              */
    20, /* X  X X   Base 80 Index 0280*/
    62, /* X XXXXX  Base 80 Index 0281*/
    85, /*  X X X X Base 80 Index 0282*/
    65, /*  X     X Base 80 Index 0283*/
    34, /*   X   X  Base 80 Index 0284*/
    /*              */
    0, /* X        Base 81 Index 0285*/
    62, /* X XXXXX  Base 81 Index 0286*/
    34, /*   X   X  Base 81 Index 0287*/
    62, /*   XXXXX  Base 81 Index 0288*/
    0, /*          Base 81 Index 0289*/
    /*              */
    0, /* X        Base 82 Index 028a*/
    88, /* XX XX    Base 82 Index 028b*/
    56, /*   XXX    Base 82 Index 028c*/
    0, /*          Base 82 Index 028d*/
    0, /*          Base 82 Index 028e*/
    /*              */
    72, /* XX  X    Base 83 Index 028f*/
    62, /* X XXXXX  Base 83 Index 0290*/
    9, /*     X  X Base 83 Index 0291*/
    9, /*     X  X Base 83 Index 0292*/
    2, /*       X  Base 83 Index 0293*/
    /*              */
    88, /* XX XX    Base 84 Index 0294*/
    56, /* X XXX    Base 84 Index 0295*/
    0, /*          Base 84 Index 0296*/
    88, /*  X XX    Base 84 Index 0297*/
    56, /*   XXX    Base 84 Index 0298*/
    /*              */
    16, /* X  X     Base 85 Index 0299*/
    0, /* X        Base 85 Index 029a*/
    16, /*    X     Base 85 Index 029b*/
    0, /*          Base 85 Index 029c*/
    16, /*    X     Base 85 Index 029d*/
    /*              */
    4, /* X    X   Base 86 Index 029e*/
    4, /* X    X   Base 86 Index 029f*/
    127, /*  XXXXXXX Base 86 Index 02a0*/
    4, /*      X   Base 86 Index 02a1*/
    4, /*      X   Base 86 Index 02a2*/
    /*              */
    20, /* X  X X   Base 87 Index 02a3*/
    20, /* X  X X   Base 87 Index 02a4*/
    127, /*  XXXXXXX Base 87 Index 02a5*/
    20, /*    X X   Base 87 Index 02a6*/
    20, /*    X X   Base 87 Index 02a7*/
    /*              */
    0, /* X        Base 88 Index 02a8*/
    2, /* X     X  Base 88 Index 02a9*/
    1, /*        X Base 88 Index 02aa*/
    2, /*       X  Base 88 Index 02ab*/
    0, /*          Base 88 Index 02ac*/
    /*              */
    19, /* X  X  XX Base 89 Index 02ad*/
    107, /* XXX X XX Base 89 Index 02ae*/
    100, /*  XX  X   Base 89 Index 02af*/
    26, /*    XX X  Base 89 Index 02b0*/
    25, /*    XX  X Base 89 Index 02b1*/
    /*              */
    72, /* XX  X    Base 8a Index 02b2*/
    85, /* XX X X X Base 8a Index 02b3*/
    86, /*  X X XX  Base 8a Index 02b4*/
    85, /*  X X X X Base 8a Index 02b5*/
    36, /*   X  X   Base 8a Index 02b6*/
    /*              */
    0, /* X        Base 8b Index 02b7*/
    8, /* X   X    Base 8b Index 02b8*/
    20, /*    X X   Base 8b Index 02b9*/
    34, /*   X   X  Base 8b Index 02ba*/
    0, /*          Base 8b Index 02bb*/
    /*              */
    62, /* X XXXXX  Base 8c Index 02bc*/
    65, /* XX     X Base 8c Index 02bd*/
    127, /*  XXXXXXX Base 8c Index 02be*/
    73, /*  X  X  X Base 8c Index 02bf*/
    73, /*  X  X  X Base 8c Index 02c0*/
    /*              */
    0, /* X        Base 8d Index 02c1*/
    62, /* X XXXXX  Base 8d Index 02c2*/
    34, /*   X   X  Base 8d Index 02c3*/
    62, /*   XXXXX  Base 8d Index 02c4*/
    0, /*          Base 8d Index 02c5*/
    /*              */
    68, /* XX   X   Base 8e Index 02c6*/
    101, /* XXX  X X Base 8e Index 02c7*/
    86, /*  X X XX  Base 8e Index 02c8*/
    77, /*  X  XX X Base 8e Index 02c9*/
    68, /*  X   X   Base 8e Index 02ca*/
    /*              */
    0, /* X        Base 8f Index 02cb*/
    62, /* X XXXXX  Base 8f Index 02cc*/
    34, /*   X   X  Base 8f Index 02cd*/
    62, /*   XXXXX  Base 8f Index 02ce*/
    0, /*          Base 8f Index 02cf*/
    /*              */
    0, /* X        Base 90 Index 02d0*/
    62, /* X XXXXX  Base 90 Index 02d1*/
    34, /*   X   X  Base 90 Index 02d2*/
    62, /*   XXXXX  Base 90 Index 02d3*/
    0, /*          Base 90 Index 02d4*/
    /*              */
    0, /* X        Base 91 Index 02d5*/
    0, /* X        Base 91 Index 02d6*/
    14, /*     XXX  Base 91 Index 02d7*/
    13, /*     XX X Base 91 Index 02d8*/
    0, /*          Base 91 Index 02d9*/
    /*              */
    0, /* X        Base 92 Index 02da*/
    11, /* X   X XX Base 92 Index 02db*/
    7, /*      XXX Base 92 Index 02dc*/
    0, /*          Base 92 Index 02dd*/
    0, /*          Base 92 Index 02de*/
    /*              */
    14, /* X   XXX  Base 93 Index 02df*/
    13, /* X   XX X Base 93 Index 02e0*/
    0, /*          Base 93 Index 02e1*/
    14, /*     XXX  Base 93 Index 02e2*/
    13, /*     XX X Base 93 Index 02e3*/
    /*              */
    11, /* X   X XX Base 94 Index 02e4*/
    7, /* X    XXX Base 94 Index 02e5*/
    0, /*          Base 94 Index 02e6*/
    11, /*     X XX Base 94 Index 02e7*/
    7, /*      XXX Base 94 Index 02e8*/
    /*              */
    28, /* X  XXX   Base 95 Index 02e9*/
    62, /* X XXXXX  Base 95 Index 02ea*/
    62, /*   XXXXX  Base 95 Index 02eb*/
    62, /*   XXXXX  Base 95 Index 02ec*/
    28, /*    XXX   Base 95 Index 02ed*/
    /*              */
    0, /* X        Base 96 Index 02ee*/
    8, /* X   X    Base 96 Index 02ef*/
    8, /*     X    Base 96 Index 02f0*/
    8, /*     X    Base 96 Index 02f1*/
    0, /*          Base 96 Index 02f2*/
    /*              */
    8, /* X   X    Base 97 Index 02f3*/
    8, /* X   X    Base 97 Index 02f4*/
    8, /*     X    Base 97 Index 02f5*/
    8, /*     X    Base 97 Index 02f6*/
    8, /*     X    Base 97 Index 02f7*/
    /*              */
    2, /* X     X  Base 98 Index 02f8*/
    1, /* X      X Base 98 Index 02f9*/
    2, /*       X  Base 98 Index 02fa*/
    1, /*        X Base 98 Index 02fb*/
    0, /*          Base 98 Index 02fc*/
    /*              */
    1, /* X      X Base 99 Index 02fd*/
    7, /* X    XXX Base 99 Index 02fe*/
    1, /*        X Base 99 Index 02ff*/
    7, /*      XXX Base 99 Index 0300*/
    7, /*      XXX Base 99 Index 0301*/
    /*              */
    72, /* XX  X    Base 9a Index 0302*/
    85, /* XX X X X Base 9a Index 0303*/
    86, /*  X X XX  Base 9a Index 0304*/
    85, /*  X X X X Base 9a Index 0305*/
    36, /*   X  X   Base 9a Index 0306*/
    /*              */
    0, /* X        Base 9b Index 0307*/
    34, /* X X   X  Base 9b Index 0308*/
    20, /*    X X   Base 9b Index 0309*/
    8, /*     X    Base 9b Index 030a*/
    0, /*          Base 9b Index 030b*/
    /*              */
    56, /* X XXX    Base 9c Index 030c*/
    68, /* XX   X   Base 9c Index 030d*/
    124, /*  XXXXX   Base 9c Index 030e*/
    84, /*  X X X   Base 9c Index 030f*/
    88, /*  X XX    Base 9c Index 0310*/
    /*              */
    0, /* X        Base 9d Index 0311*/
    62, /* X XXXXX  Base 9d Index 0312*/
    34, /*   X   X  Base 9d Index 0313*/
    62, /*   XXXXX  Base 9d Index 0314*/
    0, /*          Base 9d Index 0315*/
    /*              */
    68, /* XX   X   Base 9e Index 0316*/
    101, /* XXX  X X Base 9e Index 0317*/
    86, /*  X X XX  Base 9e Index 0318*/
    77, /*  X  XX X Base 9e Index 0319*/
    68, /*  X   X   Base 9e Index 031a*/
    /*              */
    12, /* X   XX   Base 9f Index 031b*/
    17, /* X  X   X Base 9f Index 031c*/
    112, /*  XXX     Base 9f Index 031d*/
    17, /*    X   X Base 9f Index 031e*/
    12, /*     XX   Base 9f Index 031f*/
    /*              */
    0, /* X        Base a0 Index 0320*/
    0, /* X        Base a0 Index 0321*/
    0, /*          Base a0 Index 0322*/
    0, /*          Base a0 Index 0323*/
    0, /*          Base a0 Index 0324*/
    /*              */
    0, /* X        Base a1 Index 0325*/
    125, /* XXXXXX X Base a1 Index 0326*/
    125, /*  XXXXX X Base a1 Index 0327*/
    0, /*          Base a1 Index 0328*/
    0, /*          Base a1 Index 0329*/
    /*              */
    28, /* X  XXX   Base a2 Index 032a*/
    34, /* X X   X  Base a2 Index 032b*/
    127, /*  XXXXXXX Base a2 Index 032c*/
    34, /*   X   X  Base a2 Index 032d*/
    20, /*    X X   Base a2 Index 032e*/
    /*              */
    74, /* XX  X X  Base a3 Index 032f*/
    109, /* XXX XX X Base a3 Index 0330*/
    89, /*  X XX  X Base a3 Index 0331*/
    73, /*  X  X  X Base a3 Index 0332*/
    34, /*   X   X  Base a3 Index 0333*/
    /*              */
    34, /* X X   X  Base a4 Index 0334*/
    28, /* X  XXX   Base a4 Index 0335*/
    20, /*    X X   Base a4 Index 0336*/
    28, /*    XXX   Base a4 Index 0337*/
    34, /*   X   X  Base a4 Index 0338*/
    /*              */
    43, /* X X X XX Base a5 Index 0339*/
    44, /* X X XX   Base a5 Index 033a*/
    124, /*  XXXXX   Base a5 Index 033b*/
    44, /*   X XX   Base a5 Index 033c*/
    43, /*   X X XX Base a5 Index 033d*/
    /*              */
    0, /* X        Base a6 Index 033e*/
    119, /* XXXX XXX Base a6 Index 033f*/
    119, /*  XXX XXX Base a6 Index 0340*/
    0, /*          Base a6 Index 0341*/
    0, /*          Base a6 Index 0342*/
    /*              */
    32, /* X X      Base a7 Index 0343*/
    74, /* XX  X X  Base a7 Index 0344*/
    85, /*  X X X X Base a7 Index 0345*/
    41, /*   X X  X Base a7 Index 0346*/
    2, /*       X  Base a7 Index 0347*/
    /*              */
    3, /* X     XX Base a8 Index 0348*/
    3, /* X     XX Base a8 Index 0349*/
    0, /*          Base a8 Index 034a*/
    3, /*       XX Base a8 Index 034b*/
    3, /*       XX Base a8 Index 034c*/
    /*              */
    62, /* X XXXXX  Base a9 Index 034d*/
    93, /* XX XXX X Base a9 Index 034e*/
    99, /*  XX   XX Base a9 Index 034f*/
    85, /*  X X X X Base a9 Index 0350*/
    62, /*   XXXXX  Base a9 Index 0351*/
    /*              */
    8, /* X   X    Base aa Index 0352*/
    21, /* X  X X X Base aa Index 0353*/
    21, /*    X X X Base aa Index 0354*/
    30, /*    XXXX  Base aa Index 0355*/
    0, /*          Base aa Index 0356*/
    /*              */
    8, /* X   X    Base ab Index 0357*/
    20, /* X  X X   Base ab Index 0358*/
    42, /*   X X X  Base ab Index 0359*/
    20, /*    X X   Base ab Index 035a*/
    34, /*   X   X  Base ab Index 035b*/
    /*              */
    8, /* X   X    Base ac Index 035c*/
    8, /* X   X    Base ac Index 035d*/
    8, /*     X    Base ac Index 035e*/
    24, /*    XX    Base ac Index 035f*/
    0, /*          Base ac Index 0360*/
    /*              */
    0, /* X        Base ad Index 0361*/
    8, /* X   X    Base ad Index 0362*/
    8, /*     X    Base ad Index 0363*/
    8, /*     X    Base ad Index 0364*/
    0, /*          Base ad Index 0365*/
    /*              */
    62, /* X XXXXX  Base ae Index 0366*/
    127, /* XXXXXXXX Base ae Index 0367*/
    83, /*  X X  XX Base ae Index 0368*/
    109, /*  XX XX X Base ae Index 0369*/
    62, /*   XXXXX  Base ae Index 036a*/
    /*              */
    2, /* X     X  Base af Index 036b*/
    2, /* X     X  Base af Index 036c*/
    2, /*       X  Base af Index 036d*/
    2, /*       X  Base af Index 036e*/
    2, /*       X  Base af Index 036f*/
    /*              */
    0, /* X        Base b0 Index 0370*/
    2, /* X     X  Base b0 Index 0371*/
    5, /*      X X Base b0 Index 0372*/
    2, /*       X  Base b0 Index 0373*/
    0, /*          Base b0 Index 0374*/
    /*              */
    0, /* X        Base b1 Index 0375*/
    36, /* X X  X   Base b1 Index 0376*/
    46, /*   X XXX  Base b1 Index 0377*/
    36, /*   X  X   Base b1 Index 0378*/
    0, /*          Base b1 Index 0379*/
    /*              */
    18, /* X  X  X  Base b2 Index 037a*/
    25, /* X  XX  X Base b2 Index 037b*/
    21, /*    X X X Base b2 Index 037c*/
    18, /*    X  X  Base b2 Index 037d*/
    0, /*          Base b2 Index 037e*/
    /*              */
    10, /* X   X X  Base b3 Index 037f*/
    17, /* X  X   X Base b3 Index 0380*/
    21, /*    X X X Base b3 Index 0381*/
    10, /*     X X  Base b3 Index 0382*/
    0, /*          Base b3 Index 0383*/
    /*              */
    0, /* X        Base b4 Index 0384*/
    4, /* X    X   Base b4 Index 0385*/
    2, /*       X  Base b4 Index 0386*/
    1, /*        X Base b4 Index 0387*/
    0, /*          Base b4 Index 0388*/
    /*              */
    126, /* XXXXXXX  Base b5 Index 0389*/
    8, /* X   X    Base b5 Index 038a*/
    16, /*    X     Base b5 Index 038b*/
    8, /*     X    Base b5 Index 038c*/
    30, /*    XXXX  Base b5 Index 038d*/
    /*              */
    14, /* X   XXX  Base b6 Index 038e*/
    17, /* X  X   X Base b6 Index 038f*/
    127, /*  XXXXXXX Base b6 Index 0390*/
    1, /*        X Base b6 Index 0391*/
    127, /*  XXXXXXX Base b6 Index 0392*/
    /*              */
    0, /* X        Base b7 Index 0393*/
    24, /* X  XX    Base b7 Index 0394*/
    24, /*    XX    Base b7 Index 0395*/
    0, /*          Base b7 Index 0396*/
    0, /*          Base b7 Index 0397*/
    /*              */
    0, /* X        Base b8 Index 0398*/
    64, /* XX       Base b8 Index 0399*/
    80, /*  X X     Base b8 Index 039a*/
    32, /*   X      Base b8 Index 039b*/
    0, /*          Base b8 Index 039c*/
    /*              */
    0, /* X        Base b9 Index 039d*/
    18, /* X  X  X  Base b9 Index 039e*/
    31, /*    XXXXX Base b9 Index 039f*/
    16, /*    X     Base b9 Index 03a0*/
    0, /*          Base b9 Index 03a1*/
    /*              */
    14, /* X   XXX  Base ba Index 03a2*/
    17, /* X  X   X Base ba Index 03a3*/
    17, /*    X   X Base ba Index 03a4*/
    14, /*     XXX  Base ba Index 03a5*/
    0, /*          Base ba Index 03a6*/
    /*              */
    34, /* X X   X  Base bb Index 03a7*/
    20, /* X  X X   Base bb Index 03a8*/
    42, /*   X X X  Base bb Index 03a9*/
    20, /*    X X   Base bb Index 03aa*/
    8, /*     X    Base bb Index 03ab*/
    /*              */
    23, /* X  X XXX Base bc Index 03ac*/
    8, /* X   X    Base bc Index 03ad*/
    52, /*   XX X   Base bc Index 03ae*/
    34, /*   X   X  Base bc Index 03af*/
    121, /*  XXXX  X Base bc Index 03b0*/
    /*              */
    23, /* X  X XXX Base bd Index 03b1*/
    8, /* X   X    Base bd Index 03b2*/
    76, /*  X  XX   Base bd Index 03b3*/
    106, /*  XX X X  Base bd Index 03b4*/
    81, /*  X X   X Base bd Index 03b5*/
    /*              */
    49, /* X XX   X Base be Index 03b6*/
    21, /* X  X X X Base be Index 03b7*/
    58, /*   XXX X  Base be Index 03b8*/
    36, /*   X  X   Base be Index 03b9*/
    122, /*  XXXX X  Base be Index 03ba*/
    /*              */
    48, /* X XX     Base bf Index 03bb*/
    72, /* XX  X    Base bf Index 03bc*/
    77, /*  X  XX X Base bf Index 03bd*/
    64, /*  X       Base bf Index 03be*/
    48, /*   XX     Base bf Index 03bf*/
    /*              */
    112, /* XXXX     Base c0 Index 03c0*/
    41, /* X X X  X Base c0 Index 03c1*/
    38, /*   X  XX  Base c0 Index 03c2*/
    40, /*   X X    Base c0 Index 03c3*/
    112, /*  XXX     Base c0 Index 03c4*/
    /*              */
    112, /* XXXX     Base c1 Index 03c5*/
    40, /* X X X    Base c1 Index 03c6*/
    38, /*   X  XX  Base c1 Index 03c7*/
    41, /*   X X  X Base c1 Index 03c8*/
    112, /*  XXX     Base c1 Index 03c9*/
    /*              */
    112, /* XXXX     Base c2 Index 03ca*/
    42, /* X X X X  Base c2 Index 03cb*/
    37, /*   X  X X Base c2 Index 03cc*/
    42, /*   X X X  Base c2 Index 03cd*/
    112, /*  XXX     Base c2 Index 03ce*/
    /*              */
    114, /* XXXX  X  Base c3 Index 03cf*/
    41, /* X X X  X Base c3 Index 03d0*/
    38, /*   X  XX  Base c3 Index 03d1*/
    41, /*   X X  X Base c3 Index 03d2*/
    112, /*  XXX     Base c3 Index 03d3*/
    /*              */
    112, /* XXXX     Base c4 Index 03d4*/
    41, /* X X X  X Base c4 Index 03d5*/
    36, /*   X  X   Base c4 Index 03d6*/
    41, /*   X X  X Base c4 Index 03d7*/
    112, /*  XXX     Base c4 Index 03d8*/
    /*              */
    112, /* XXXX     Base c5 Index 03d9*/
    40, /* X X X    Base c5 Index 03da*/
    37, /*   X  X X Base c5 Index 03db*/
    40, /*   X X    Base c5 Index 03dc*/
    112, /*  XXX     Base c5 Index 03dd*/
    /*              */
    126, /* XXXXXXX  Base c6 Index 03de*/
    9, /* X   X  X Base c6 Index 03df*/
    127, /*  XXXXXXX Base c6 Index 03e0*/
    73, /*  X  X  X Base c6 Index 03e1*/
    73, /*  X  X  X Base c6 Index 03e2*/
    /*              */
    14, /* X   XXX  Base c7 Index 03e3*/
    81, /* XX X   X Base c7 Index 03e4*/
    81, /*  X X   X Base c7 Index 03e5*/
    49, /*   XX   X Base c7 Index 03e6*/
    10, /*     X X  Base c7 Index 03e7*/
    /*              */
    124, /* XXXXXX   Base c8 Index 03e8*/
    85, /* XX X X X Base c8 Index 03e9*/
    86, /*  X X XX  Base c8 Index 03ea*/
    68, /*  X   X   Base c8 Index 03eb*/
    68, /*  X   X   Base c8 Index 03ec*/
    /*              */
    124, /* XXXXXX   Base c9 Index 03ed*/
    86, /* XX X XX  Base c9 Index 03ee*/
    85, /*  X X X X Base c9 Index 03ef*/
    68, /*  X   X   Base c9 Index 03f0*/
    68, /*  X   X   Base c9 Index 03f1*/
    /*              */
    124, /* XXXXXX   Base ca Index 03f2*/
    86, /* XX X XX  Base ca Index 03f3*/
    85, /*  X X X X Base ca Index 03f4*/
    70, /*  X   XX  Base ca Index 03f5*/
    0, /*          Base ca Index 03f6*/
    /*              */
    125, /* XXXXXX X Base cb Index 03f7*/
    84, /* XX X X   Base cb Index 03f8*/
    85, /*  X X X X Base cb Index 03f9*/
    68, /*  X   X   Base cb Index 03fa*/
    0, /*          Base cb Index 03fb*/
    /*              */
    0, /* X        Base cc Index 03fc*/
    69, /* XX   X X Base cc Index 03fd*/
    126, /*  XXXXXX  Base cc Index 03fe*/
    68, /*  X   X   Base cc Index 03ff*/
    0, /*          Base cc Index 0400*/
    /*              */
    0, /* X        Base cd Index 0401*/
    68, /* XX   X   Base cd Index 0402*/
    126, /*  XXXXXX  Base cd Index 0403*/
    69, /*  X   X X Base cd Index 0404*/
    0, /*          Base cd Index 0405*/
    /*              */
    0, /* X        Base ce Index 0406*/
    70, /* XX   XX  Base ce Index 0407*/
    125, /*  XXXXX X Base ce Index 0408*/
    70, /*  X   XX  Base ce Index 0409*/
    0, /*          Base ce Index 040a*/
    /*              */
    0, /* X        Base cf Index 040b*/
    69, /* XX   X X Base cf Index 040c*/
    124, /*  XXXXX   Base cf Index 040d*/
    69, /*  X   X X Base cf Index 040e*/
    0, /*          Base cf Index 040f*/
    /*              */
    73, /* XX  X  X Base d0 Index 0410*/
    127, /* XXXXXXXX Base d0 Index 0411*/
    73, /*  X  X  X Base d0 Index 0412*/
    65, /*  X     X Base d0 Index 0413*/
    62, /*   XXXXX  Base d0 Index 0414*/
    /*              */
    124, /* XXXXXX   Base d1 Index 0415*/
    10, /* X   X X  Base d1 Index 0416*/
    17, /*    X   X Base d1 Index 0417*/
    34, /*   X   X  Base d1 Index 0418*/
    125, /*  XXXXX X Base d1 Index 0419*/
    /*              */
    56, /* X XXX    Base d2 Index 041a*/
    69, /* XX   X X Base d2 Index 041b*/
    70, /*  X   XX  Base d2 Index 041c*/
    68, /*  X   X   Base d2 Index 041d*/
    56, /*   XXX    Base d2 Index 041e*/
    /*              */
    56, /* X XXX    Base d3 Index 041f*/
    68, /* XX   X   Base d3 Index 0420*/
    70, /*  X   XX  Base d3 Index 0421*/
    69, /*  X   X X Base d3 Index 0422*/
    56, /*   XXX    Base d3 Index 0423*/
    /*              */
    56, /* X XXX    Base d4 Index 0424*/
    70, /* XX   XX  Base d4 Index 0425*/
    69, /*  X   X X Base d4 Index 0426*/
    70, /*  X   XX  Base d4 Index 0427*/
    56, /*   XXX    Base d4 Index 0428*/
    /*              */
    58, /* X XXX X  Base d5 Index 0429*/
    69, /* XX   X X Base d5 Index 042a*/
    70, /*  X   XX  Base d5 Index 042b*/
    69, /*  X   X X Base d5 Index 042c*/
    56, /*   XXX    Base d5 Index 042d*/
    /*              */
    56, /* X XXX    Base d6 Index 042e*/
    69, /* XX   X X Base d6 Index 042f*/
    68, /*  X   X   Base d6 Index 0430*/
    69, /*  X   X X Base d6 Index 0431*/
    56, /*   XXX    Base d6 Index 0432*/
    /*              */
    68, /* XX   X   Base d7 Index 0433*/
    40, /* X X X    Base d7 Index 0434*/
    16, /*    X     Base d7 Index 0435*/
    40, /*   X X    Base d7 Index 0436*/
    68, /*  X   X   Base d7 Index 0437*/
    /*              */
    62, /* X XXXXX  Base d8 Index 0438*/
    81, /* XX X   X Base d8 Index 0439*/
    73, /*  X  X  X Base d8 Index 043a*/
    69, /*  X   X X Base d8 Index 043b*/
    62, /*   XXXXX  Base d8 Index 043c*/
    /*              */
    60, /* X XXXX   Base d9 Index 043d*/
    65, /* XX     X Base d9 Index 043e*/
    66, /*  X    X  Base d9 Index 043f*/
    64, /*  X       Base d9 Index 0440*/
    60, /*   XXXX   Base d9 Index 0441*/
    /*              */
    60, /* X XXXX   Base da Index 0442*/
    64, /* XX       Base da Index 0443*/
    66, /*  X    X  Base da Index 0444*/
    65, /*  X     X Base da Index 0445*/
    60, /*   XXXX   Base da Index 0446*/
    /*              */
    60, /* X XXXX   Base db Index 0447*/
    66, /* XX    X  Base db Index 0448*/
    65, /*  X     X Base db Index 0449*/
    66, /*  X    X  Base db Index 044a*/
    60, /*   XXXX   Base db Index 044b*/
    /*              */
    60, /* X XXXX   Base dc Index 044c*/
    65, /* XX     X Base dc Index 044d*/
    64, /*  X       Base dc Index 044e*/
    65, /*  X     X Base dc Index 044f*/
    60, /*   XXXX   Base dc Index 0450*/
    /*              */
    12, /* X   XX   Base dd Index 0451*/
    16, /* X  X     Base dd Index 0452*/
    114, /*  XXX  X  Base dd Index 0453*/
    17, /*    X   X Base dd Index 0454*/
    12, /*     XX   Base dd Index 0455*/
    /*              */
    127, /* XXXXXXXX Base de Index 0456*/
    20, /* X  X X   Base de Index 0457*/
    20, /*    X X   Base de Index 0458*/
    8, /*     X    Base de Index 0459*/
    0, /*          Base de Index 045a*/
    /*              */
    126, /* XXXXXXX  Base df Index 045b*/
    41, /* X X X  X Base df Index 045c*/
    73, /*  X  X  X Base df Index 045d*/
    78, /*  X  XXX  Base df Index 045e*/
    48, /*   XX     Base df Index 045f*/
    /*              */
    32, /* X X      Base e0 Index 0460*/
    85, /* XX X X X Base e0 Index 0461*/
    86, /*  X X XX  Base e0 Index 0462*/
    84, /*  X X X   Base e0 Index 0463*/
    120, /*  XXXX    Base e0 Index 0464*/
    /*              */
    32, /* X X      Base e1 Index 0465*/
    84, /* XX X X   Base e1 Index 0466*/
    86, /*  X X XX  Base e1 Index 0467*/
    85, /*  X X X X Base e1 Index 0468*/
    120, /*  XXXX    Base e1 Index 0469*/
    /*              */
    32, /* X X      Base e2 Index 046a*/
    86, /* XX X XX  Base e2 Index 046b*/
    85, /*  X X X X Base e2 Index 046c*/
    86, /*  X X XX  Base e2 Index 046d*/
    120, /*  XXXX    Base e2 Index 046e*/
    /*              */
    34, /* X X   X  Base e3 Index 046f*/
    85, /* XX X X X Base e3 Index 0470*/
    86, /*  X X XX  Base e3 Index 0471*/
    85, /*  X X X X Base e3 Index 0472*/
    120, /*  XXXX    Base e3 Index 0473*/
    /*              */
    32, /* X X      Base e4 Index 0474*/
    85, /* XX X X X Base e4 Index 0475*/
    84, /*  X X X   Base e4 Index 0476*/
    85, /*  X X X X Base e4 Index 0477*/
    120, /*  XXXX    Base e4 Index 0478*/
    /*              */
    32, /* X X      Base e5 Index 0479*/
    84, /* XX X X   Base e5 Index 047a*/
    85, /*  X X X X Base e5 Index 047b*/
    84, /*  X X X   Base e5 Index 047c*/
    120, /*  XXXX    Base e5 Index 047d*/
    /*              */
    116, /* XXXX X   Base e6 Index 047e*/
    84, /* XX X X   Base e6 Index 047f*/
    124, /*  XXXXX   Base e6 Index 0480*/
    84, /*  X X X   Base e6 Index 0481*/
    92, /*  X XXX   Base e6 Index 0482*/
    /*              */
    0, /* X        Base e7 Index 0483*/
    92, /* XX XXX   Base e7 Index 0484*/
    98, /*  XX   X  Base e7 Index 0485*/
    34, /*   X   X  Base e7 Index 0486*/
    20, /*    X X   Base e7 Index 0487*/
    /*              */
    56, /* X XXX    Base e8 Index 0488*/
    85, /* XX X X X Base e8 Index 0489*/
    86, /*  X X XX  Base e8 Index 048a*/
    84, /*  X X X   Base e8 Index 048b*/
    72, /*  X  X    Base e8 Index 048c*/
    /*              */
    56, /* X XXX    Base e9 Index 048d*/
    84, /* XX X X   Base e9 Index 048e*/
    86, /*  X X XX  Base e9 Index 048f*/
    85, /*  X X X X Base e9 Index 0490*/
    72, /*  X  X    Base e9 Index 0491*/
    /*              */
    56, /* X XXX    Base ea Index 0492*/
    86, /* XX X XX  Base ea Index 0493*/
    85, /*  X X X X Base ea Index 0494*/
    86, /*  X X XX  Base ea Index 0495*/
    72, /*  X  X    Base ea Index 0496*/
    /*              */
    56, /* X XXX    Base eb Index 0497*/
    85, /* XX X X X Base eb Index 0498*/
    84, /*  X X X   Base eb Index 0499*/
    85, /*  X X X X Base eb Index 049a*/
    72, /*  X  X    Base eb Index 049b*/
    /*              */
    0, /* X        Base ec Index 049c*/
    1, /* X      X Base ec Index 049d*/
    122, /*  XXXX X  Base ec Index 049e*/
    0, /*          Base ec Index 049f*/
    0, /*          Base ec Index 04a0*/
    /*              */
    0, /* X        Base ed Index 04a1*/
    0, /* X        Base ed Index 04a2*/
    122, /*  XXXX X  Base ed Index 04a3*/
    1, /*        X Base ed Index 04a4*/
    0, /*          Base ed Index 04a5*/
    /*              */
    0, /* X        Base ee Index 04a6*/
    2, /* X     X  Base ee Index 04a7*/
    121, /*  XXXX  X Base ee Index 04a8*/
    2, /*       X  Base ee Index 04a9*/
    0, /*          Base ee Index 04aa*/
    /*              */
    0, /* X        Base ef Index 04ab*/
    2, /* X     X  Base ef Index 04ac*/
    120, /*  XXXX    Base ef Index 04ad*/
    2, /*       X  Base ef Index 04ae*/
    0, /*          Base ef Index 04af*/
    /*              */
    5, /* X    X X Base f0 Index 04b0*/
    50, /* X XX  X  Base f0 Index 04b1*/
    77, /*  X  XX X Base f0 Index 04b2*/
    72, /*  X  X    Base f0 Index 04b3*/
    48, /*   XX     Base f0 Index 04b4*/
    /*              */
    122, /* XXXXX X  Base f1 Index 04b5*/
    17, /* X  X   X Base f1 Index 04b6*/
    10, /*     X X  Base f1 Index 04b7*/
    9, /*     X  X Base f1 Index 04b8*/
    112, /*  XXX     Base f1 Index 04b9*/
    /*              */
    48, /* X XX     Base f2 Index 04ba*/
    73, /* XX  X  X Base f2 Index 04bb*/
    74, /*  X  X X  Base f2 Index 04bc*/
    72, /*  X  X    Base f2 Index 04bd*/
    48, /*   XX     Base f2 Index 04be*/
    /*              */
    48, /* X XX     Base f3 Index 04bf*/
    72, /* XX  X    Base f3 Index 04c0*/
    74, /*  X  X X  Base f3 Index 04c1*/
    73, /*  X  X  X Base f3 Index 04c2*/
    48, /*   XX     Base f3 Index 04c3*/
    /*              */
    48, /* X XX     Base f4 Index 04c4*/
    74, /* XX  X X  Base f4 Index 04c5*/
    73, /*  X  X  X Base f4 Index 04c6*/
    74, /*  X  X X  Base f4 Index 04c7*/
    48, /*   XX     Base f4 Index 04c8*/
    /*              */
    50, /* X XX  X  Base f5 Index 04c9*/
    73, /* XX  X  X Base f5 Index 04ca*/
    74, /*  X  X X  Base f5 Index 04cb*/
    73, /*  X  X  X Base f5 Index 04cc*/
    48, /*   XX     Base f5 Index 04cd*/
    /*              */
    48, /* X XX     Base f6 Index 04ce*/
    74, /* XX  X X  Base f6 Index 04cf*/
    72, /*  X  X    Base f6 Index 04d0*/
    74, /*  X  X X  Base f6 Index 04d1*/
    48, /*   XX     Base f6 Index 04d2*/
    /*              */
    0, /* X        Base f7 Index 04d3*/
    8, /* X   X    Base f7 Index 04d4*/
    42, /*   X X X  Base f7 Index 04d5*/
    8, /*     X    Base f7 Index 04d6*/
    0, /*          Base f7 Index 04d7*/
    /*              */
    56, /* X XXX    Base f8 Index 04d8*/
    100, /* XXX  X   Base f8 Index 04d9*/
    84, /*  X X X   Base f8 Index 04da*/
    76, /*  X  XX   Base f8 Index 04db*/
    56, /*   XXX    Base f8 Index 04dc*/
    /*              */
    56, /* X XXX    Base f9 Index 04dd*/
    65, /* XX     X Base f9 Index 04de*/
    66, /*  X    X  Base f9 Index 04df*/
    32, /*   X      Base f9 Index 04e0*/
    120, /*  XXXX    Base f9 Index 04e1*/
    /*              */
    56, /* X XXX    Base fa Index 04e2*/
    64, /* XX       Base fa Index 04e3*/
    66, /*  X    X  Base fa Index 04e4*/
    33, /*   X    X Base fa Index 04e5*/
    120, /*  XXXX    Base fa Index 04e6*/
    /*              */
    56, /* X XXX    Base fb Index 04e7*/
    66, /* XX    X  Base fb Index 04e8*/
    65, /*  X     X Base fb Index 04e9*/
    34, /*   X   X  Base fb Index 04ea*/
    120, /*  XXXX    Base fb Index 04eb*/
    /*              */
    56, /* X XXX    Base fc Index 04ec*/
    66, /* XX    X  Base fc Index 04ed*/
    64, /*  X       Base fc Index 04ee*/
    34, /*   X   X  Base fc Index 04ef*/
    120, /*  XXXX    Base fc Index 04f0*/
    /*              */
    12, /* X   XX   Base fd Index 04f1*/
    16, /* X  X     Base fd Index 04f2*/
    114, /*  XXX  X  Base fd Index 04f3*/
    17, /*    X   X Base fd Index 04f4*/
    12, /*     XX   Base fd Index 04f5*/
    /*              */
    127, /* XXXXXXXX Base fe Index 04f6*/
    18, /* X  X  X  Base fe Index 04f7*/
    18, /*    X  X  Base fe Index 04f8*/
    18, /*    X  X  Base fe Index 04f9*/
    12, /*     XX   Base fe Index 04fa*/
    /*              */
    12, /* X   XX   Base ff Index 04fb*/
    17, /* X  X   X Base ff Index 04fc*/
    112, /*  XXX     Base ff Index 04fd*/
    17, /*    X   X Base ff Index 04fe*/
    12 /*     XX   Base ff Index 04ff*/
  };
