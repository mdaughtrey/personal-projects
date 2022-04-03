# https://www.toomanyatoms.com/computer/usb_keyboard_codes.html

hidmap = {
'PAUSE': 0x48,  # Pause / Break
'BS': 0x2a,  # Backspace / Delete
'TAB': 0x2b,  # Tab
'CLR': 0x53,  # Clear
'ENTER': 0x28,  # Enter
'LSHIFT': 0xe1,  # Shift (Left)
'RSHIFT': 0xe5,  # Shift (Left)
'LCTRL': 0xe0,  # Ctrl (left)
'RCTRL': 0xe4,  # Ctrl (left)
'LALT': 0xe2,  # Alt (left)
'RALT': 0xe6,  # Alt (left)
'BREAK': 0x48,  # Pause / Break
'CLOCK': 0x39,  # Caps Lock
'ESC': 0x29,  # Escape
'SPC': 0x2c,  # Spacebar
'PGUP': 0x4b,  # Page Up
'PGDN': 0x4e,  # Page Down
'END': 0x4d,  # End
'HOME': 0x4a,  # Home
'LEFT': 0x50,  # Left Arrow
'UP': 0x52,  # Up Arrow
'RIGHT': 0x4f,  # Right Arrow
'DOWN': 0x51,  # Down Arrow
#'#SEL': 0x77,  # Select
'EXEC': 0x74,  # Execute
'PRTSCRN': 0x46,  # Print Screen
'INS': 0x49,  # Insert
'DEL': 0x4c,  # Delete
'HELP': 0x75,  # Help
'0': 0x27,  # 0
'1': 0x1e,  # 1
'2': 0x1f,  # 2
'3': 0x20,  # 3
'4': 0x21,  # 4
'5': 0x22,  # 5
'6': 0x23,  # 6
'7': 0x24,  # 7
'8': 0x25,  # 8
'9': 0x26,  # 9
')': ['LSHIFT','0'], # 48: 0x27,  # 0
'!': ['LSHIFT','1'], # 48: 0x27,  # 0
'@': ['LSHIFT','2'], # 50: 0x1f,  # 2
'#': ['LSHIFT','3'], # 3
'$': ['LSHIFT','4'], # 4
'%': ['LSHIFT','5'], # 5
'^': ['LSHIFT','6'], # 6
'&': ['LSHIFT','7'], # 7
'*': ['LSHIFT','8'], # 8
'(': ['LSHIFT','9'], # 9
';': 0x33,  # Semicolon
':': ['LSHIFT',';'],
'<': ['LSHIFT',','],
'=': 0x2e,  # Equal sign
'A': ['LSHIFT','a'], #: 65: 0x04,  # a
'B': ['LSHIFT','b'], #: 66: 0x05,  # b
'C': ['LSHIFT','c'], #: 67: 0x06,  # c
'D': ['LSHIFT','d'], #: 68: 0x07,  # d
'E': ['LSHIFT','e'], #: 69: 0x08,  # e
'F': ['LSHIFT','f'], #: 70: 0x09,  # f
'G': ['LSHIFT','g'], #: 71: 0x0a,  # g
'H': ['LSHIFT','h'], #: 72: 0x0b,  # h
'I': ['LSHIFT','i'], #: 73: 0x0c,  # i
'J': ['LSHIFT','j'], #: 74: 0x0d,  # j
'K': ['LSHIFT','k'], #: 75: 0x0e,  # k
'L': ['LSHIFT','l'], #: 76: 0x0f,  # l
'M': ['LSHIFT','m'], #: 77: 0x10,  # m
'N': ['LSHIFT','n'], #: 78: 0x11,  # n
'O': ['LSHIFT','o'], #: 79: 0x12,  # o
'P': ['LSHIFT','p'], #: 80: 0x13,  # p
'Q': ['LSHIFT','q'], #: 81: 0x14,  # q
'R': ['LSHIFT','r'], #: 82: 0x15,  # r
'S': ['LSHIFT','s'], #: 83: 0x16,  # s
'T': ['LSHIFT','t'], #: 84: 0x17,  # t
'U': ['LSHIFT','u'], #: 85: 0x18,  # u
'V': ['LSHIFT','v'], #: 86: 0x19,  # v
'W': ['LSHIFT','w'], #: 87: 0x1a,  # w
'X': ['LSHIFT','x'], #: 88: 0x1b,  # x
'Y': ['LSHIFT','y'], #: 89: 0x1c,  # y
'Z': ['LSHIFT','z'], #: 90: 0x1d,  # z
'a': 0x04,  # a
'b': 0x05,  # b
'c': 0x06,  # c
'd': 0x07,  # d
'e': 0x08,  # e
'f': 0x09,  # f
'g': 0x0a,  # g
'h': 0x0b,  # h
'i': 0x0c,  # i
'j': 0x0d,  # j
'k': 0x0e,  # k
'l': 0x0f,  # l
'm': 0x10,  # m
'n': 0x11,  # n
'o': 0x12,  # o
'p': 0x13,  # p
'q': 0x14,  # q
'r': 0x15,  # r
's': 0x16,  # s
't': 0x17,  # t
'u': 0x18,  # u
'v': 0x19,  # v
'w': 0x1a,  # w
'x': 0x1b,  # x
'y': 0x1c,  # y
'z': 0x1d,  # z
'LWIN': 0xe3,  # Windows key / Meta Key (Left)
'RWIN': 0xe7,  # Windows key / Meta Key (Left)
'N0': 0x62,  # Numpad 0
'N1': 0x59,  # Numpad 1
'N2': 0x5a,  # Numpad 2
'N3': 0x5b,  # Numpad 3
'N4': 0x5c,  # Numpad 4
'N5': 0x5d,  # Numpad 5
'N6': 0x5e,  # Numpad 6
'N7': 0x5f,  # Numpad 7
'N8': 0x60,  # Numpad 8
'N9': 0x61,  # Numpad 9
'F1': 0x3a,  # F1
'F2': 0x3b,  # F2
'F3': 0x3c,  # F3
'F4': 0x3d,  # F4
'F5': 0x3e,  # F5
'F6': 0x3f,  # F6
'F7': 0x40,  # F7
'F8': 0x41,  # F8
'F9': 0x42,  # F9
'F10': 0x43,  # F10
'F11': 0x44,  # F11
'F12': 0x45,  # F12
'F13': 0x68,  # F13
'F14': 0x69,  # F14
'F15': 0x6a,  # F15
'F16': 0x6b,  # F16
'F17': 0x6c,  # F17
'F18': 0x6d,  # F18
'F19': 0x6e,  # F19
'F20': 0x6f,  # F20
'F21': 0x70,  # F21
'F22': 0x71,  # F22
'F23': 0x72,  # F23
'F24': 0x73,  # F23
'KEXEC': 0x74,
'KHELP': 0x75,
'KMENU': 0x76,
'KSEL': 0x77,
'KSTOP': 0x78,
'KAGAIN': 0x79,
'KUNDO': 0x7a,
'KCUT': 0x7b,
'KCOPY': 0x7c,
'KPASTE': 0x7d,
'KFIND': 0x7e,
'KMUTE': 0x7f,
'KVOLU': 0x80,
'KVOLD': 0x81,
'NLOCK': 0x53,  # Num Lock
'SLOCK': 0x47,  # Scroll Lock
'!': 0x1e,  # !
'#': 0x32,  # Hash
'-': 0x2d,  # Minus
'PPAUSE':  0xe8,  # Media play/pause
'REFRESH':  0xfa,  # Refresh
';': 0x33,  # Semicolon
'=': 0x2e,  # Equal sign
'+': ['LSHIFT','='],
',': 0x36,  # Comma
'-': 0x2d,  # Minus sign
'_': ['LSHIFT','-'], 
'.': 0x37,  # Period
'>': ['LSHIFT','.'],
'/': 0x38,  # Forward slash
'?': ['LSHIFT', '/'],
'`': 0x35,  # Accent grave
'[': 0x2f,  # Left bracket ([, {])
']': 0x30,  # Right bracket (], })
'{': ['LSHIFT','['],
'}': ['LSHIFT',']'],
'\\': 0x31,  # Back slash
'|': ['LSHIFT','\\'],
'\'': 0x34,  # Single quote
'"': ['LSHIFT', '\''],  # Single quote
'~': ['LSHIFT','`'] # tilde
}
