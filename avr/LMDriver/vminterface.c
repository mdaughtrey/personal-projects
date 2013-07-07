#include <vminterface.h>
//#include <avr/pgmspace.h>
#include <embedvm2.h>
#include <stddef.h>
#include <avr/eeprom.h>

#ifdef EMBEDDED
#include <cmdproc.h>
#endif
#include <serutil.h>
//#define VMI_DEBUG 1

u16 binOffset;
u16 mainOffset;
unsigned char memory[RAM_SIZE];
//unsigned char memory[RAM_SIZE] = {0};
//unsigned char eeprom[ROM_SIZE] PROGMEM;
extern unsigned char programControl;

struct embedvm_s vm = {
    0xffff, RAM_SIZE, RAM_SIZE, NULL,
	mem_read, mem_write, call_user // , &rom_read
};

#define UNUSED __attribute__((unused))

void vminterface_init(void)
{
    int ii;
    vm.ip = 0xffff;
    vm.sp = RAM_SIZE;
    vm.sfp = RAM_SIZE;

    mainOffset = eeprom_read_byte((uint8_t*)EEPROM_OFFSET_MAINOFFSET_MSB) << 8;
    mainOffset |= eeprom_read_byte((uint8_t*)EEPROM_OFFSET_MAINOFFSET_LSB);

    binOffset = eeprom_read_byte((uint8_t*)EEPROM_OFFSET_BINOFFSET_MSB) << 8;
    binOffset |= eeprom_read_byte((uint8_t*)EEPROM_OFFSET_BINOFFSET_LSB);
    for (ii = 0; ii < binOffset & ii < RAM_SIZE; ii++)
    {
        memory[ii] = eeprom_read_byte ((uint8_t*)ii); 
    }
}

int16_t mem_read(uint16_t addr, bool is16bit)
{
#ifdef VMI_DEBUG
    uart_send_buffered('R');
    uart_send_buffered(':');
    uart_send_hex_byte(addr >> 8);
    uart_send_hex_byte(addr & 0xff);
    uart_send_buffered('\r');
    uart_send_buffered('\n');
#endif // VMI_DEBUG
	if (is16bit)
		return (memory[addr] << 8) | memory[addr+1];
	return memory[addr];
}

void mem_write(uint16_t addr, int16_t value, bool is16bit)
{
#ifdef VMI_DEBUG
    uart_send_buffered('W');
    uart_send_buffered(':');
    uart_send_hex_byte(addr >> 8);
    uart_send_hex_byte(addr & 0xff);
    uart_send_buffered(' ');
    uart_send_hex_byte(value >> 8);
    uart_send_hex_byte(value & 0xff);
    uart_send_buffered('\r');
    uart_send_buffered('\n');
#endif // VMI_DEBUG

	if (is16bit) {
		memory[addr] = value >> 8;
		memory[addr+1] = value;
	} else
		memory[addr] = value;
}

int16_t call_user(uint8_t funcid, uint8_t argc, int16_t *argv)
{
    char tmp = *(char *)argv;

    //uart_send_hex_byte(funcid);

	if (funcid == 0)
    {
        programControl = 0;
	}

    if (funcid == 1)
    {
#ifdef VMI_DEBUG
        uart_send_buffered('E');
        uart_send_hex_byte(tmp);
//        uart_send_buffered(' ');
        uart_send_buffered('\r');
        uart_send_buffered('\n');
#endif // VMI_DEBUG
        cmd_dataHandler(tmp);
    }
    return 0;
//
//	printf("Called user function %d with %d args:", funcid, argc);
//
//	for (i = 0; i < argc; i++) {
//		printf(" %d", argv[i]);
//		ret += argv[i];
//	}
//
//	printf("\n");
//	fflush(stdout);
//
//	return ret ^ funcid;
    return 0;
}

int16_t rom_read(uint16_t addr, bool is16bit)
{
	if (is16bit)
    {
		return (eeprom_read_byte((uint8_t*)addr) << 8) | eeprom_read_byte((uint8_t*)addr+1);
    }
	return eeprom_read_byte((uint8_t*)addr);
}

