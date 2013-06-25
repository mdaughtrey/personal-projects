#include <decoder.h>
#include <embedvm.h>

const char * decoder(uint8_t opcode)
{
    switch (opcode)
    {
        case 0x00 ... 0x3f:
            return "| 0 | 0 |          SFA          |           Push local variable to stack | 00 |";
        case 0x40 ... 0x7f:
            return "| 0 | 1 |          SFA          |          Pop local variable from stack | 40 |";
        case 0x80:
            return "| 1 | 0 | 0 | 0 |     0 .. 0x0  |                    Add (pop 2, push 1) | 80 |";
        case 0x81:
            return "| 1 | 0 | 0 | 0 |     1 .. 0x1  |                    Sub (pop 2, push 1) |    |";
        case 0x82:
            return "| 1 | 0 | 0 | 0 |     2 .. 0x2  |                    Mul (pop 2, push 1) |    |";
        case 0x83:
            return "| 1 | 0 | 0 | 0 |     3 .. 0x3  |                    Div (pop 2, push 1) |    |";
        case 0x84:
            return "| 1 | 0 | 0 | 0 |     4 .. 0x4  |                    Mod (pop 2, push 1) |    |";
        case 0x85:
            return "| 1 | 0 | 0 | 0 |     5 .. 0x5  |             Shift Left (pop 2, push 1) |    |";
        case 0x86:
            return "| 1 | 0 | 0 | 0 |     6 .. 0x6  |            Shift Right (pop 2, push 1) |    |";
        case 0x87:
            return "| 1 | 0 | 0 | 0 |     7 .. 0x7  |            Bitwise AND (pop 2, push 1) |    |";
        case 0x88:
            return "| 1 | 0 | 0 | 0 |     8 .. 0x8  |             Bitwise OR (pop 2, push 1) |    |";
        case 0x89:
            return "| 1 | 0 | 0 | 0 |     9 .. 0x9  |            Bitwise XOR (pop 2, push 1) |    |";
        case 0x8a:
            return "| 1 | 0 | 0 | 0 |    10 .. 0xa  |              Logic AND (pop 2, push 1) |    |";
        case 0x8b:
            return "| 1 | 0 | 0 | 0 |    11 .. 0xb  |               Logic OR (pop 2, push 1) |    |";
        case 0x8c:
            return "| 1 | 0 | 0 | 0 |    12 .. 0xc  |            Bitwise NOT (pop 1, push 1) |    |";
        case 0x8d:
            return "| 1 | 0 | 0 | 0 |    13 .. 0xd  |      Arithmetic invert (pop 1, push 1) |    |";
        case 0x8e:
            return "| 1 | 0 | 0 | 0 |    14 .. 0xe  |              Logic NOT (pop 1, push 1) |    |";
        case 0x8f:
            return "| 1 | 0 | 0 | 0 |    15 .. 0xf  |                               Reserved |    |";
        case 0x90 ... 0x97:
            return "| 1 | 0 | 0 | 1 | 0 |    VAL    |         Push immediate (VAL is signed) | 90 |";
        case 0x98:
            return "| 1 | 0 | 0 | 1 | 1 | 0 | 0 | 0 |          Push unsigned 1-byte argument | 98 |";
        case 0x99:
            return "| 1 | 0 | 0 | 1 | 1 | 0 | 0 | 1 |            Push signed 1-byte argument | 99 |";
        case 0x9a:
            return "| 1 | 0 | 0 | 1 | 1 | 0 | 1 | 0 |            Push signed 2-byte argument | 9a |";
        case 0x9b:
            return "| 1 | 0 | 0 | 1 | 1 | 0 | 1 | 1 |           Return from function (pop 1) | 9b |";
        case 0x9c:
            return "| 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 |     Return from function without value | 9c |";
        case 0x9d:
            return "| 1 | 0 | 0 | 1 | 1 | 1 | 0 | 1 |                     Drop value (pop 1) | 9d |";
        case 0x9e:
            return "| 1 | 0 | 0 | 1 | 1 | 1 | 1 | 0 |                   Call address (pop 1) | 9e |";
        case 0x9f:
            return "| 1 | 0 | 0 | 1 | 1 | 1 | 1 | 1 |                Jump to address (pop 1) | 9f |";
        case 0xa0:
            return "| 1 | 0 | 1 | 0 | 0 |     0     |             Jump (1-byte rel. address) | a0 |";
        case 0xa1:
            return "| 1 | 0 | 1 | 0 | 0 |     1     |             Jump (2-byte rel. address) |    |";
        case 0xa2:
            return "| 1 | 0 | 1 | 0 | 0 |     2     |             Call (1-byte rel. address) |    |";
        case 0xa3:
            return "| 1 | 0 | 1 | 0 | 0 |     3     |             Call (2-byte rel. address) |    |";
        case 0xa4:
            return "| 1 | 0 | 1 | 0 | 0 |     4     |   Jump IF (pop 1, 1-byte rel. address) |    |";
        case 0xa5:
            return "| 1 | 0 | 1 | 0 | 0 |     5     |   Jump IF (pop 1, 2-byte rel. address) |    |";
        case 0xa6:
            return "| 1 | 0 | 1 | 0 | 0 |     6     | Jump UNLESS (pop 1, 1-byte rel. addr.) |    |";
        case 0xa7:
            return "| 1 | 0 | 1 | 0 | 0 |     7     | Jump UNLESS (pop 1, 2-byte rel. addr.) |    |";
        case 0xa8:
            return "| 1 | 0 | 1 | 0 | 1 |     0     |            Compare < (pop 2, push 1) | a8 |";
        case 0xa9:
            return "| 1 | 0 | 1 | 0 | 1 |     1     |           Compare <= (pop 2, push 1) |    |";
        case 0xaa:
            return "| 1 | 0 | 1 | 0 | 1 |     2     |           Compare == (pop 2, push 1) |    |";
        case 0xab:
            return "| 1 | 0 | 1 | 0 | 1 |     3     |           Compare != (pop 2, push 1) |    |";
        case 0xac:
            return "| 1 | 0 | 1 | 0 | 1 |     4     |           Compare >= (pop 2, push 1) |    |";
        case 0xad:
            return "| 1 | 0 | 1 | 0 | 1 |     5     |            Compare > (pop 2, push 1) |    |";
        case 0xae:
            return "| 1 | 0 | 1 | 0 | 1 |     6     |                 Stack Pointer (push 1) | ae |";
        case 0xaf:
            return "| 1 | 0 | 1 | 0 | 1 |     7     |           Stack Frame Pointer (push 1) | af |";
        case 0xb0 ... 0xbf:
            return "| 1 | 0 | 1 | 1 |    Func-ID    |                     Call User Function | b0 |";
        case 0xc0 ... 0xc7:
            return "| 1 | 1 |     0     |     M     |       Load 8u (push 1, M is addr mode) | c0 |";
        case 0xc8 ... 0xcf:
            return "| 1 | 1 |     1     |     M     |       Store 8u (pop 1, M is addr mode) | c8 |";
        case 0xd0 ... 0xd7:
            return "| 1 | 1 |     2     |     M     |       Load 8s (push 1, M is addr mode) | d0 |";
        case 0xd8 ... 0xdf:
            return "| 1 | 1 |     3     |     M     |       Store 8s (pop 1, M is addr mode) | d8 |";
        case 0xe0 ... 0xe7:
            return "| 1 | 1 |     4     |     M     |       Load 16 (push 1, M is addr mode) | e0 |";
        case 0xe8 ... 0xef:
            return "| 1 | 1 |     5     |     M     |       Store 16 (pop 1, M is addr mode) | e8 |";

//| 1 | 1 |     K     |     5     |     Bury dup of top in depth K (max 5) | // |
//| 1 | 1 |     K     |     6     |  Dig up the element in depth K (max 5) | // |
//| 1 | 1 |     K     |     7     |                               Reserved | // |
        case 0xf0 ... 0xf7:
            return "| 1 | 1 |     6     |     N     |            Push N+1 zeros to the stack | f0 |";
        case 0xf8 ... 0xff:
            return "| 1 | 1 |     7     |     N     |            Pop N+1 values but keep top | f8 |";
        default:
            return "Unmapped opcode";
    }
}

#if 0

+-------------------------------+----------------------------------------+----+
| MSB                       LSB |                            Instruction | Off|
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 0 | 0 |          SFA          |           Push local variable to stack | 00 |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 0 | 1 |          SFA          |          Pop local variable from stack | 40 |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 1 | 0 | 0 | 0 |     0 .. 0x0  |                    Add (pop 2, push 1) | 80 |
| 1 | 0 | 0 | 0 |     1 .. 0x1  |                    Sub (pop 2, push 1) |    |
| 1 | 0 | 0 | 0 |     2 .. 0x2  |                    Mul (pop 2, push 1) |    |
| 1 | 0 | 0 | 0 |     3 .. 0x3  |                    Div (pop 2, push 1) |    |
| 1 | 0 | 0 | 0 |     4 .. 0x4  |                    Mod (pop 2, push 1) |    |
| 1 | 0 | 0 | 0 |     5 .. 0x5  |             Shift Left (pop 2, push 1) |    |
| 1 | 0 | 0 | 0 |     6 .. 0x6  |            Shift Right (pop 2, push 1) |    |
| 1 | 0 | 0 | 0 |     7 .. 0x7  |            Bitwise AND (pop 2, push 1) |    |
| 1 | 0 | 0 | 0 |     8 .. 0x8  |             Bitwise OR (pop 2, push 1) |    |
| 1 | 0 | 0 | 0 |     9 .. 0x9  |            Bitwise XOR (pop 2, push 1) |    |
| 1 | 0 | 0 | 0 |    10 .. 0xa  |              Logic AND (pop 2, push 1) |    |
| 1 | 0 | 0 | 0 |    11 .. 0xb  |               Logic OR (pop 2, push 1) |    |
| 1 | 0 | 0 | 0 |    12 .. 0xc  |            Bitwise NOT (pop 1, push 1) |    |
| 1 | 0 | 0 | 0 |    13 .. 0xd  |      Arithmetic invert (pop 1, push 1) |    |
| 1 | 0 | 0 | 0 |    14 .. 0xe  |              Logic NOT (pop 1, push 1) |    |
| 1 | 0 | 0 | 0 |    15 .. 0xf  |                               Reserved |    |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 1 | 0 | 0 | 1 | 0 |    VAL    |         Push immediate (VAL is signed) | 90 |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 1 | 0 | 0 | 1 | 1 | 0 | 0 | 0 |          Push unsigned 1-byte argument | 98 |
| 1 | 0 | 0 | 1 | 1 | 0 | 0 | 1 |            Push signed 1-byte argument | 99 |
| 1 | 0 | 0 | 1 | 1 | 0 | 1 | 0 |            Push signed 2-byte argument | 9a |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 1 | 0 | 0 | 1 | 1 | 0 | 1 | 1 |           Return from function (pop 1) | 9b |
| 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 |     Return from function without value | 9c |
| 1 | 0 | 0 | 1 | 1 | 1 | 0 | 1 |                     Drop value (pop 1) | 9d |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 1 | 0 | 0 | 1 | 1 | 1 | 1 | 0 |                   Call address (pop 1) | 9e |
| 1 | 0 | 0 | 1 | 1 | 1 | 1 | 1 |                Jump to address (pop 1) | 9f |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 1 | 0 | 1 | 0 | 0 |     0     |             Jump (1-byte rel. address) | a0 |
| 1 | 0 | 1 | 0 | 0 |     1     |             Jump (2-byte rel. address) |    |
| 1 | 0 | 1 | 0 | 0 |     2     |             Call (1-byte rel. address) |    |
| 1 | 0 | 1 | 0 | 0 |     3     |             Call (2-byte rel. address) |    |
| 1 | 0 | 1 | 0 | 0 |     4     |   Jump IF (pop 1, 1-byte rel. address) |    |
| 1 | 0 | 1 | 0 | 0 |     5     |   Jump IF (pop 1, 2-byte rel. address) |    |
| 1 | 0 | 1 | 0 | 0 |     6     | Jump UNLESS (pop 1, 1-byte rel. addr.) |    |
| 1 | 0 | 1 | 0 | 0 |     7     | Jump UNLESS (pop 1, 2-byte rel. addr.) |    |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 1 | 0 | 1 | 0 | 1 |     0     |            Compare "<" (pop 2, push 1) | a8 |
| 1 | 0 | 1 | 0 | 1 |     1     |           Compare "<=" (pop 2, push 1) |    |
| 1 | 0 | 1 | 0 | 1 |     2     |           Compare "==" (pop 2, push 1) |    |
| 1 | 0 | 1 | 0 | 1 |     3     |           Compare "!=" (pop 2, push 1) |    |
| 1 | 0 | 1 | 0 | 1 |     4     |           Compare ">=" (pop 2, push 1) |    |
| 1 | 0 | 1 | 0 | 1 |     5     |            Compare ">" (pop 2, push 1) |    |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 1 | 0 | 1 | 0 | 1 |     6     |                 Stack Pointer (push 1) | ae |
| 1 | 0 | 1 | 0 | 1 |     7     |           Stack Frame Pointer (push 1) | af |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 1 | 0 | 1 | 1 |    Func-ID    |                     Call User Function | b0 |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 1 | 1 |     0     |     M     |       Load 8u (push 1, M is addr mode) | c0 |
| 1 | 1 |     1     |     M     |       Store 8u (pop 1, M is addr mode) | c8 |
| 1 | 1 |     2     |     M     |       Load 8s (push 1, M is addr mode) | d0 |
| 1 | 1 |     3     |     M     |       Store 8s (pop 1, M is addr mode) | d8 |
| 1 | 1 |     4     |     M     |       Load 16 (push 1, M is addr mode) | e0 |
| 1 | 1 |     5     |     M     |       Store 16 (pop 1, M is addr mode) | e8 |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 1 | 1 |     K     |     5     |     Bury dup of top in depth K (max 5) | // |
| 1 | 1 |     K     |     6     |  Dig up the element in depth K (max 5) | // |
| 1 | 1 |     K     |     7     |                               Reserved | // |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 1 | 1 |     6     |     N     |            Push N+1 zeros to the stack | f0 |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
| 1 | 1 |     7     |     N     |            Pop N+1 values but keep top | f8 |
+---+---+---+---+---+---+---+---+----------------------------------------+----+
#endif
