#include <stdio.h>
#include <string.h>


union 
{
    unsigned char asByte[2];
    unsigned short asShort;
}data;

FILE * openNew(int count, unsigned char byte)
{
    FILE * output;
    char filename[100];
    sprintf(filename, "%u", count);
    strcat(filename, ".JPG");
    output = fopen(filename, "w");
    printf("Opened %s\n", filename);
    fputc(data.asByte[1], output);
    return output;
}
    
int main(int argc, char ** argv)
{
    int count = 1;
    printf("Hello \n");
    FILE * input = fopen("/Users/mattd/dvdimage.bin", "r");
    FILE * output = NULL;

    while (!feof(input))
    {
        data.asByte[1] = data.asByte[0];
        data.asByte[0] = fgetc(input);
        printf("data is %04x\n", data.asShort);
        if (data.asShort == 0xffd9)
        {
            if (output != NULL)
            {
                printf("Closing\n");
//                fputc(data.asByte[1], output);
                fputc(data.asByte[0], output);
                fclose(output);
                output = NULL;
                continue;
            }
        }
        if (data.asShort == 0xffd8)
        {
            if (output == NULL)
            {
                output = openNew(count, data.asByte[1]);
                count++;
            }
            continue;
        }
        else
        {
            if (output != NULL)
            {
                fputc(data.asByte[1], output);
            }
        }


    }
    fclose(input);
}

