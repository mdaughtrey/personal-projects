#include <nvstore.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>
//
// Header:
// u08 labelSize (0 == free)
// u08 UrlSize
// u08 idSize
// u08 passwordSize
// char [...] label
// char [...] URL
// char [...] id
// char [...] password

#define NVSIZE 2048
#define MIN(a, b) a < b ? a : b


typedef struct
{
    u08 labelSize;
    u08 urlSize;
    u08 idSize;
    u08 passwordSize;
}Header;

u08 nvMemory[NVSIZE];
u16 memIndex;


void nvOpen(void)
{
    FILE * fMemory;
    fMemory = fopen("nvmemory.mem", "r");
    if (fMemory)
    {
        fread(nvMemory, NVSIZE, 1, fMemory);
        fclose(fMemory);
    }
    memIndex = 0;
}

void nvClose(void)
{
    FILE * fMemory;
    fMemory = fopen("nvmemory.mem", "w");
    fwrite(nvMemory, NVSIZE, 1, fMemory);
    fclose(fMemory);
}


/*
char * nvLabel(u08 id)
{
    return "nv";
}

u08 nvLabelLength(u08 id)
{
    return 2;
}
*/

u08 nextHeader(void)
{
    Header * header = (Header *)nvMemory + memIndex;
    if (header->labelSize)
    {
        memIndex += sizeof(Header)
            + header->labelSize
            + header->urlSize
            + header->idSize
            + header->passwordSize;
        header = (Header *)nvMemory + memIndex;
        assert(memIndex < NVSIZE);
        return 1;
    }
    return 0;
}

void nvSelect(u08 index)
{
    memIndex = 0;
    while (index--)
    {
        nextHeader();
    }
}

u08 nvNumEntries(void)
{
    u08 count = 0;
    memIndex = 0;
    while (nextHeader())
    {
        count++;
    }
    return count;
}
/*

u08 nvLastId(void )
{
    return 0;
}
*/

u08 nvAllocate(void)
{
    nvOpen();
    Header * header = (Header *)nvMemory + memIndex;
    while (header->labelSize)
    {
        memIndex += sizeof(header)
            + header->labelSize
            + header->urlSize
            + header->idSize
            + header->passwordSize;

        if ((memIndex + sizeof(Header)) > NVSIZE)
        {
            assert(0);
            return 0;
        }
        header = (Header *)nvMemory + memIndex;
    }
    header->urlSize = 0;
    header->idSize = 0;
    header->passwordSize = 0;
    nvClose();
    return 1;
}

void nvSetLabel(char * inputString)
{
    Header * header = (Header *)nvMemory + memIndex;
    header->labelSize = strlen(inputString);
    memcpy(nvMemory + memIndex + sizeof(Header), inputString, header->labelSize);
    nvClose();
}

void nvSetUrl(char * inputString)
{
    Header * header = (Header *)nvMemory + memIndex;
    header->urlSize = strlen(inputString);
    memcpy(nvMemory
            + memIndex
            + header->labelSize
            + sizeof(Header), inputString, header->urlSize);
    nvClose();
}

void nvSetId(char * inputString)
{
    Header * header = (Header *)nvMemory + memIndex;
    header->idSize = strlen(inputString);
    memcpy(nvMemory
            + memIndex
            + sizeof(Header)
            + header->labelSize
            + header->urlSize, inputString, header->idSize);
    nvClose();
}

void nvSetPassword(char * inputString)
{
    Header * header = (Header *)nvMemory + memIndex;
    header->passwordSize = strlen(inputString);
    memcpy(nvMemory
            + memIndex
            + sizeof(Header)
            + header->labelSize
            + header->urlSize
            + header->idSize, inputString, header->passwordSize);
    nvClose();
}

void nvRelease()
{
    Header * header = (Header *)nvMemory + memIndex;
    header->labelSize = 0;
    nvClose();
}

void nvInit(void)
{
    nvOpen();
}


void nvLoadLabel(char * str, u08 length)
{
    Header * header = (Header *)nvMemory + memIndex;
    memIndex += sizeof(Header); 
    strncpy(str, (char *)(nvMemory + memIndex), MIN(length, header->labelSize));
}


