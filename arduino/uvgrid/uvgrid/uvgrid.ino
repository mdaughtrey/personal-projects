/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */
typedef unsigned char u08;
typedef unsigned short u16;

const u08 LEDArray[3][3] = 
{
  8, 7, 9,
  5, 6, 4,
  2, 3, 17
};

u08 rotArray[3][3];

#define NUMPATTERNS 15
u08 Patterns[NUMPATTERNS][3] = 
{
  B00000111, B00000000, B00000000,
  B00000000, B00000101, B00000000,
  B00000000, B00000111, B00000000,
  B00000101, B00000101, B00000101,
  B00000101, B00000010, B00000101,
  B00000111, B00000101, B00000111,
  B00000100, B00000000, B00000001,
  B00000011, B00000001, B00000000,
  B00000000, B00000010, B00000000,
  B00000111, B00000001, B00000001,
  B00000101, B00000010, B00000010,
  B00000101, B00000000, B00000010,
  B00000111, B00000010, B00000000,
  B00000111, B00000101, B00000101,
  B00000110, B00000010, B00000011
};

void lightIt(u16 ltime, u08 duty)
{
    digitalWrite(9, LOW);
    delay(ltime);
    digitalWrite(9, HIGH);
    
  while(1) {
    digitalWrite(9, LOW);
    delayMicroseconds(duty);
    digitalWrite(9, HIGH);
    delay(1);
  }
}

void allon()
{
  u08 row, col;
  for (row = 0; row < 3; row++)
  {
    for (col = 0; col < 3; col++)
    {
      digitalWrite(rotArray[row][col],LOW );
    }
  }

}

// the setup function runs once when you press reset or power the board
u08 pnum;
void setup() {
  u08 ii;
  for (ii = 2; ii < 10; ii++) { pinMode(ii, OUTPUT); }
  pinMode(17, OUTPUT);
  pnum = 0;
  randomSeed(analogRead(0));
  //allon();
}

void rotate(u08 count)
{
  u08 ii, jj;
  for (ii = 0; ii < 3; ii++) {
    for (jj = 0; jj < 3; jj++) {
      rotArray[ii][jj] = LEDArray[ii][jj]; }}

  u08 aa;
  while (count)
  {
    aa = rotArray[0][0];
    rotArray[0][0] = rotArray[2][0];
    rotArray[2][0] = rotArray[2][2];
    rotArray[2][2] = rotArray[0][2];
    rotArray[0][2] = aa; // rotArray[0][0];

    aa = rotArray[0][1];
    rotArray[0][1] = rotArray[1][0];
    rotArray[1][0] = rotArray[2][1];
    rotArray[2][1] = rotArray[1][2];
    rotArray[1][2] = aa;
    count--;
  }
}

void pattern(u08 num)
{
  rotate(random(0, 4));
  u08 row, col;
  u08 flip = random(0, 2);
  for (row = 0; row < 3; row++)
  {
    for (col = 0; col < 3; col++)
    {
      digitalWrite(rotArray[row][col], Patterns[num][row] & (1 << col) ? flip : !flip );
    }
  }
  delay(2000);
  for (row = 0; row < 3; row++)
  {
    for (col = 0; col < 3; col++)
    {
      digitalWrite(rotArray[row][col],HIGH );
    }
  }
}

// the loop function runs over and over again forever
void loop()
{
  pattern(random(0, NUMPATTERNS + 1));
  delay(20000);
  
}
