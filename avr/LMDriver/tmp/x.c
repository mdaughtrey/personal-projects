#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <inttypes.h>


char * parseIt(char * text)
{
    char * target;

    if (!strstr(text, "\\x"))
    {
        target = strdup(text);
        return target;
    }

    char * hex = "0123456789abcdef";
    target = malloc(strlen(text) + 1);
    char * start = text;
    char * end = text;

    while (end = strstr(start, "\\x"))
    {
        unsigned char text[3] = {0};
        unsigned char value[2] = {0};

        strncat(target, start, end - start);
        text[0] = end[2];
        text[1] = end[3];
        value[0] =  strtoimax(text, NULL, 10);
        strcat(target, value);
//        strncat(target, start, end - start);
        start = end + 4;
    }
    strcat(target, start);
    return target;
}

int main()
{
    char * result =  parseIt("t\\x04abcd\\x04efgh");

    printf("%s\n", result);

    /*
    char * matched = text;
    int matchCount = 0;
    while (matched = strstr(matched, "\\x"))
    {
        matched += 2;
        matchCount++;
    }
    printf("matched %u\n", matchCount);
    int toAlloc = strlen(text) + (matched * 
    */
    return 0;
}

