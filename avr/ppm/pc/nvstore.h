#ifndef INCLUDE_NVSTORE_H
#define INCLUDE_NVSTORE_H

#include <typedefs.h>

void nvInit(void);
/*
char * nvLabel(u08 id);
u08 nvLabelLength(u08 id);

u08 nvNumEntries(void);
u08 nvLastId(void );
*/

u08 nvNumEntries(void);
void nvSelect(u08 index);
void nvLoadLabel(char * str, u08 length);

u08 nvAllocate(void);
void nvSetLabel(char * inputString);
void nvSetUrl(char * inputString);
void nvSetId(char * inputString);
void nvSetPassword(char * inputString);
void nvCommit();
void nvRelease();

#endif //  INCLUDE_NVSTORE_H
