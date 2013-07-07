/*
 *  EmbedVM - Embedded Virtual Machine for uC Applications
 *
 *  Copyright (C) 2011  Clifford Wolf <clifford@clifford.at>
 *  
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <stdio.h>
#include "embedvm2.h"
#ifndef EMBEDDED
#include <decoder.h>
#endif

#include <serutil.h>
#include <vminterface.h>

//#define EVM_DEBUG 1

extern struct embedvm_s vm;

static inline int16_t signext(uint16_t val, uint16_t mask)
{
	val = val & mask;
	if ((val & ~(mask >> 1)) != 0)
		val |= ~mask;
	return val;
}

extern void embedvm_exec(void)
{
	uint8_t opcode = rom_read(vm.ip, false);
    //printf("%02x %s\n", opcode, decoder(opcode));
    //fflush(stdout);
	uint16_t addr = 0;
	int16_t a = 0, b = 0;
	int8_t sfa = 0;
#ifdef EVM_DEBUG
    uart_send_buffered('I');
    uart_send_buffered('P');
    uart_send_buffered(':');
    uart_send_hex_byte(vm.ip >> 8);
    uart_send_hex_byte(vm.ip & 0xff);
    uart_send_buffered(' ');
    uart_send_hex_byte(opcode);
    uart_send_buffered('\r');
    uart_send_buffered('\n');
#endif

	switch (opcode)
	{
	case 0x00 ... 0x3f:
		sfa = signext(opcode, 0x3f);
		embedvm_push(embedvm_local_read(sfa));
		vm.ip++;
		break;
	case 0x40 ... 0x7f:
		sfa = signext(opcode, 0x3f);
		embedvm_local_write(sfa, embedvm_pop());
		vm.ip++;
		break;
	case 0x80+0 ... 0x80+11:
	case 0xa8+0 ... 0xa8+5:
		b = embedvm_pop();
	case 0x80+12 ... 0x80+14:
		a = embedvm_pop();
		switch (opcode)
		{
			case 0x80 +  0: embedvm_push(a + b);  break;
			case 0x80 +  1: embedvm_push(a - b);  break;
			case 0x80 +  2: embedvm_push(a * b);  break;
			case 0x80 +  3: embedvm_push(a / b);  break;
			case 0x80 +  4: embedvm_push(a % b);  break;
			case 0x80 +  5: embedvm_push(a << b); break;
			case 0x80 +  6: embedvm_push(a >> b); break;
			case 0x80 +  7: embedvm_push(a & b);  break;
			case 0x80 +  8: embedvm_push(a | b);  break;
			case 0x80 +  9: embedvm_push(a ^ b);  break;
			case 0x80 + 10: embedvm_push(a && b); break;
			case 0x80 + 11: embedvm_push(a || b); break;
			case 0x80 + 12: embedvm_push(~a);     break;
			case 0x80 + 13: embedvm_push(-a);     break;
			case 0x80 + 14: embedvm_push(!a);     break;
			case 0xa8 +  0: embedvm_push(a <  b); break;
			case 0xa8 +  1: embedvm_push(a <= b); break;
			case 0xa8 +  2: embedvm_push(a == b); break;
			case 0xa8 +  3: embedvm_push(a != b); break;
			case 0xa8 +  4: embedvm_push(a >= b); break;
			case 0xa8 +  5: embedvm_push(a >  b); break;
			
		}
		vm.ip++;
		break;
	case 0x90 ... 0x97:
		a = signext(opcode, 0x07);
		if ((a & 0x04) != 0)
			a |= ~0x07;
		embedvm_push(a);
		vm.ip++;
		break;
	case 0x98:
		//a = mem_read(vm.ip+1, false) & 0x00ff;
		a = rom_read(vm.ip+1, false) & 0x00ff;
		embedvm_push(a);
		vm.ip += 2;
		break;
	case 0x99:
		//a = mem_read(vm.ip+1, false) & 0x00ff;
		a = rom_read(vm.ip+1, false) & 0x00ff;
		embedvm_push(signext(a, 0x00ff));
		vm.ip += 2;
		break;
	case 0x9a:
		//a = mem_read(vm.ip+1, true);
		a = rom_read(vm.ip+1, true);
		embedvm_push(a);
		vm.ip += 3;
		break;
	case 0x9b:
		a = embedvm_pop();
		if (0) {
	case 0x9c:
			a = 0;
		}
		vm.sp = vm.sfp;
		vm.ip = embedvm_pop();
		vm.sfp = embedvm_pop();
		if ((vm.sfp & 1) != 0)
			vm.sfp &= ~1;
		else
			embedvm_push(a);
		break;
	case 0x9d:
		embedvm_pop();
		vm.ip++;
		break;
	case 0x9e:
		addr = embedvm_pop();
		//if (mem_read(vm.ip+1, false) == 0x9d) {
		if (rom_read(vm.ip+1, false) == 0x9d) {
			embedvm_push(vm.sfp | 1);
			embedvm_push(vm.ip + 2);
		} else {
			embedvm_push(vm.sfp);
			embedvm_push(vm.ip + 1);
		}
		vm.sfp = vm.sp;
		vm.ip = addr;
		break;
	case 0x9f:
		vm.ip = embedvm_pop();
		break;
	case 0xa0 ... 0xa0+7:
		if ((opcode & 1) == 0) {
			//addr = vm.ip + signext(mem_read(vm.ip+1, false), 0x00ff);
			addr = vm.ip + signext(rom_read(vm.ip+1, false), 0x00ff);
			vm.ip += 2;
		} else {
			//addr = vm.ip + mem_read(vm.ip+1, true);
			addr = vm.ip + rom_read(vm.ip+1, true);
			vm.ip += 3;
		}
		switch (opcode)
		{
		case 0xa0:
		case 0xa1:
			vm.ip = addr;
			break;
		case 0xa2:
		case 0xa3:
			//if (mem_read(vm.ip, false) == 0x9d) {
			if (rom_read(vm.ip, false) == 0x9d) {
				embedvm_push(vm.sfp | 1);
				embedvm_push(vm.ip + 1);
			} else {
				embedvm_push(vm.sfp);
				embedvm_push(vm.ip);
			}
			vm.sfp = vm.sp;
			vm.ip = addr;
			break;
		case 0xa4:
		case 0xa5:
			if (embedvm_pop())
				vm.ip = addr;
			break;
		case 0xa6:
		case 0xa7:
			if (!embedvm_pop())
				vm.ip = addr;
			break;
		}
		break;
	case 0xae:
		embedvm_push(vm.sp);
		vm.ip++;
		break;
	case 0xaf:
		embedvm_push(vm.sfp);
		vm.ip++;
		break;
	case 0xb0 ... 0xb0+15:
		{
			uint8_t argc = embedvm_pop();
			int16_t argv[argc];
			for (sfa=0; sfa<argc; sfa++)
				argv[sfa] = embedvm_pop();
			a = call_user(opcode - 0xb0, argc, argv);
			embedvm_push(a);
		}
		vm.ip++;
		break;
	case 0xc0 ... 0xef:
		if ((opcode & 0x07) == 5) {
			/* this is a "bury" instruction */
			uint8_t depth = (opcode >> 3) & 0x07;
			int16_t stack[depth+1];
			for (sfa = 0; sfa <= depth; sfa++)
				stack[sfa] = embedvm_pop();
			embedvm_push(stack[0]);
			for (sfa = depth; sfa > 0; sfa--)
				embedvm_push(stack[sfa]);
			embedvm_push(stack[0]);
			vm.ip++;
			break;
		}
		if ((opcode & 0x07) == 6) {
			/* this is a "dig" instruction */
			uint8_t depth = (opcode >> 3) & 0x07;
			int16_t stack[depth+2];
			for (sfa = 0; sfa < depth+2; sfa++)
				stack[sfa] = embedvm_pop();
			for (sfa = depth+1; sfa > 0; sfa--)
				embedvm_push(stack[sfa-1]);
			embedvm_push(stack[depth+1]);
			vm.ip++;
			break;
		}
		sfa = ((opcode >> 3) & 0x07) == 4 || ((opcode >> 3) & 0x07) == 5 ? 1 : 0;
		switch (opcode & 0x07)
		{
		case 0:
			//addr = mem_read(vm.ip+1, false) & 0x00ff;
			addr = rom_read(vm.ip+1, false) & 0x00ff;
			vm.ip += 2;
			break;
		case 1:
			//addr = mem_read(vm.ip+1, true);
			addr = rom_read(vm.ip+1, true);
			vm.ip += 3;
			break;
		case 2:
			addr = embedvm_pop();
			vm.ip++;
			break;
		case 3:
			//addr = (embedvm_pop() << sfa) + (mem_read(vm.ip+1, false) & 0x00ff);
			addr = (embedvm_pop() << sfa) + (rom_read(vm.ip+1, false) & 0x00ff);
			vm.ip += 2;
			break;
		case 4:
			//addr = (embedvm_pop() << sfa) + mem_read(vm.ip+1, true);
			addr = (embedvm_pop() << sfa) + rom_read(vm.ip+1, true);
			vm.ip += 3;
			break;
		}
		switch ((opcode >> 3) & 0x07)
		{
		case 0:
			embedvm_push(mem_read(addr, false) & 0x00ff);
			break;
		case 1:
			mem_write(addr, embedvm_pop(), false);
			break;
		case 2:
			embedvm_push(signext(mem_read(addr, false), 0x00ff));
			break;
		case 3:
			mem_write(addr, embedvm_pop(), false);
			break;
		case 4:
			embedvm_push(mem_read(addr, true));
			break;
		case 5:
			mem_write(addr, embedvm_pop(), true);
			break;
		}
		break;
	case 0xf0 ... 0xf7:
		for (sfa = 0; sfa <= (opcode & 0x07); sfa++)
			embedvm_push(0);
		vm.ip++;
		break;
	case 0xf8 ... 0xff:
		a = embedvm_pop();
		vm.sp += 2 + 2*(opcode & 0x07);
		embedvm_push(a);
		vm.ip++;
		break;
	}
}

void embedvm_interrupt(uint16_t addr)
{
	embedvm_push(vm.sfp | 1);
	embedvm_push(vm.ip);
	vm.sfp = vm.sp;
	vm.ip = addr;
}

int16_t embedvm_pop(void)
{
	int16_t value = mem_read(vm.sp, true);
	vm.sp += 2;
	return value;
}

void embedvm_push(int16_t value)
{
	vm.sp -= 2;
	mem_write(vm.sp, value, true);
}

int16_t embedvm_local_read(int8_t sfa)
{
	uint16_t addr = vm.sfp - 2*sfa + (sfa < 0 ? +2 : -2);
	return mem_read(addr, true);
}

void embedvm_local_write(int8_t sfa, int16_t value)
{
	uint16_t addr = vm.sfp - 2*sfa + (sfa < 0 ? +2 : -2);
	mem_write(addr, value, true);
}

