#include <embedvm2.h>
#include <avr/pgmspace.h>

#define RAM_SIZE 256 
#define ROM_SIZE 512

#define EEPROM_OFFSET_PROGRAM_CONTROL ROM_SIZE - 2
#define EEPROM_OFFSET_BINOFFSET_MSB ROM_SIZE - 4
#define EEPROM_OFFSET_BINOFFSET_LSB ROM_SIZE - 3

void vminterface_init(void);
int16_t mem_read(uint16_t addr, bool is16bit);
void mem_write(uint16_t addr, int16_t value, bool is16bit);;
int16_t call_user(uint8_t funcid, uint8_t argc, int16_t *argv);;

extern unsigned char eeprom[ROM_SIZE] PROGMEM;
