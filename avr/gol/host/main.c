#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 12 
#define COLS 9 

unsigned int colony[ROWS] = {0};
unsigned int newColony[ROWS] = {0};

void initColony(void)
{
	int ii;
//	for (ii = 0; ii < ROWS; ii++)
//	{
//		colony[ii] = 0;
//	}
//	colony[2] = 0b00001000;
//	colony[3] = 0b00000100;
//	colony[4] = 0b00011100;

	for (ii = 0; ii < ROWS; ii++)
	{
		colony[ii] = rand() & 65535;
	}
}
void printColony(void)
{
	int ii;
	for (ii = 1; ii < ROWS - 1; ii++)
	{
		int jj;
		for (jj = 1; jj < COLS - 1; jj++)
		{
			printf("%c ", (colony[ii] & (1 << jj)) ? '*' : '.');
		}
		printf("\n");
	}
}

unsigned char bc[] = {
	0, // 0b00000000
	1, // 0b00000001
	1, // 0b00000010
	2, // 0b00000011
	1, // 0b00000100
	2, // 0b00000101
	2, // 0b00000110
	3, // 0b00000111
};

void evolve()
{
	unsigned char ii;
	unsigned char jj;

	for (ii = 1; ii < ROWS - 1; ii++)
	{
		newColony[ii] = 0;
		for (jj = 1; jj < COLS - 1; jj++)
		{
			unsigned char above = (colony[ii - 1] >> (jj -1)) & 0x07;
			unsigned char beside = (colony[ii] >> (jj - 1)) & 0x05;
			unsigned char below = (colony[ii + 1] >> (jj -1)) & 0x07;
			unsigned char bitcount = bc[above] + bc[beside] + bc[below];
			if (!(colony[ii] & (1 << jj)))
			{
				if (bitcount == 3)
				{
					newColony[ii] |= (1 << jj);
				}
			}
			else if (bitcount == 2 || bitcount == 3)
			{
				newColony[ii] |= (1 << jj);
			}
		}
	}

	unsigned int temp;
	temp = newColony[1];
	newColony[0] = newColony[ROWS - 2];
	newColony[ROWS - 1] = temp;
	for (ii = 0; ii < ROWS; ii++)
	{
//		newColony[ii] &= 0b0111110;
		unsigned int wrapWord = 0;
		if (newColony[ii] & 2)
		{
			wrapWord |= (1 << 8);
		}
		if (newColony[ii] & (1 << 7))
		{
			wrapWord |= 1;
		}
		newColony[ii] |= wrapWord;
		colony[ii] = newColony[ii];
	}
}

int main(void)
{
	srand(time(NULL));
	initColony();
	int count = 0;
	while (1)
	{
		printf("Generation %d\n", count++);
		printColony();
		printf("\n");
		evolve();
		unsigned int ii;
		//for (ii = 0; ii < 10000000; ii++);
		sleep(1);
	}
}
