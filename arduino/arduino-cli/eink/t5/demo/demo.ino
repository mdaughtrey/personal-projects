#include"demo.h"
//IO settings
// Busy IO4
// Reset IO16
// MOSI IO23
// CLK IO18
// DC IO17
// CS IO5

int BUSY_Pin = 4; 
int RES_Pin = 16; 
int DC_Pin = 17; 
int CS_Pin = 5; 
int SCK_Pin = 18; 
int SDI_Pin = 23; 

// int BUSY_Pin = 8; 
// int RES_Pin = 9; 
// int DC_Pin = 10; 
// int CS_Pin = 11; 
// int SCK_Pin = 12; 
//int SDI_Pin = 13; 

#define EPD_W21_MOSI_0  digitalWrite(SDI_Pin,LOW)
#define EPD_W21_MOSI_1  digitalWrite(SDI_Pin,HIGH) 

#define EPD_W21_CLK_0 digitalWrite(SCK_Pin,LOW)
#define EPD_W21_CLK_1 digitalWrite(SCK_Pin,HIGH)

#define EPD_W21_CS_0 digitalWrite(CS_Pin,LOW)
#define EPD_W21_CS_1 digitalWrite(CS_Pin,HIGH)

#define EPD_W21_DC_0  digitalWrite(DC_Pin,LOW)
#define EPD_W21_DC_1  digitalWrite(DC_Pin,HIGH)
#define EPD_W21_RST_0 digitalWrite(RES_Pin,LOW)
#define EPD_W21_RST_1 digitalWrite(RES_Pin,HIGH)
#define isEPD_W21_BUSY digitalRead(BUSY_Pin)
//250*122///////////////////////////////////////

#define MONOMSB_MODE 1
#define MONOLSB_MODE 2 
#define RED_MODE     3

#define MAX_LINE_BYTES 16// =128/8
#define MAX_COLUMN_BYTES  250

#define ALLSCREEN_GRAGHBYTES  4000

////////FUNCTION//////
void driver_delay_us(unsigned int xus);
void driver_delay_xms(unsigned long xms);
void DELAY_S(unsigned int delaytime);     
void SPI_Delay(unsigned char xrate);
void SPI_Write(unsigned char value);
void Epaper_Write_Command(unsigned char command);
void Epaper_Write_Data(unsigned char command);
//EPD
void Epaper_READBUSY(void);
void SPI_Write(unsigned char TxData);
void Epaper_Write_Command(unsigned char cmd);
void Epaper_Write_Data(unsigned char data);

void EPD_HW_Init(void); //Electronic paper initialization
void EPD_Part_Init(void); //Local refresh initialization

void EPD_Part_Update(void); 
void EPD_Update(void);

void EPD_WhiteScreen_Black(void);
void EPD_WhiteScreen_White(void);
void EPD_DeepSleep(void);
//Display 
void EPD_WhiteScreen_ALL(void);
void EPD_SetRAMValue_BaseMap(const unsigned char * datas);
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);

void setup() {
   pinMode(BUSY_Pin, INPUT); 
   pinMode(RES_Pin, OUTPUT);  
   pinMode(DC_Pin, OUTPUT);    
   pinMode(CS_Pin, OUTPUT);    
   pinMode(SCK_Pin, OUTPUT);    
   pinMode(SDI_Pin, OUTPUT);    
}
////////Partial refresh schematic////////////////

/////Y/// (0,0)        /---/(x,y)
          //                 /---/
          //                /---/  
          //x
          //
          //
//Tips//
/*When the electronic paper is refreshed in full screen, the picture flicker is a normal phenomenon, and the main function is to clear the display afterimage in the previous picture.
  When the local refresh is performed, the screen does not flash.*/
/*When you need to transplant the driver, you only need to change the corresponding IO. The BUSY pin is the input mode and the others are the output mode. */
 
void loop() {
    unsigned char fen_L,fen_H,miao_L,miao_H; 

    EPD_HW_Init(); //Electronic paper initialization
    EPD_WhiteScreen_ALL(); //Full screen refresh picture 1
    delay(2000);
    
//////////////////////Partial refresh digital presentation//////////////////////////////////////  
    EPD_HW_Init(); //Electronic paper initialization
    EPD_SetRAMValue_BaseMap(gImage_basemap);  //Partial refresh background color
    EPD_Part_Init();//Local refresh initialization
    delay(100);
    EPD_Dis_Part(0,32,gImage_num1,32,32); //x,y,DATA,resolution 32*32
    EPD_Dis_Part(0,32,gImage_num2,32,32); //x,y,DATA,resolution 32*32
    EPD_Dis_Part(0,32,gImage_num3,32,32); //x,y,DATA,resolution 32*32
    EPD_Dis_Part(0,32,gImage_num4,32,32); //x,y,DATA,resolution 32*32
    EPD_Dis_Part(0,32,gImage_num5,32,32); //x,y,DATA,resolution 32*32
    EPD_Dis_Part(0,32,gImage_num6,32,32); //x,y,DATA,resolution 32*32
    EPD_Dis_Part(0,32,gImage_num7,32,32); //x,y,DATA,resolution 32*32
    EPD_Dis_Part(0,32,gImage_num8,32,32); //x,y,DATA,resolution 32*32
    EPD_Dis_Part(0,32,gImage_num9,32,32); //x,y,DATA,resolution 32*32
   delay(2000);   
  ////////////////////////////////////////////////////////////////////////  
      //Clean
    EPD_HW_Init();//Electronic paper initialization
    EPD_WhiteScreen_White();  //Show all white
    EPD_DeepSleep();  //Enter deep sleep,Sleep instruction is necessary, please do not delete!!!
    delay(3000);
//    while(1); 
}


///////////////////EXTERNAL FUNCTION////////////////////////////////////////////////////////////////////////
/////////////////////delay//////////////////////////////////////
void driver_delay_us(unsigned int xus)  //1us
{
  for(;xus>1;xus--);
}
void driver_delay_xms(unsigned long xms) //1ms
{  
    unsigned long i = 0 , j=0;

    for(j=0;j<xms;j++)
  {
        for(i=0; i<256; i++);
    }
}
void DELAY_S(unsigned int delaytime)     
{
  int i,j,k;
  for(i=0;i<delaytime;i++)
  {
    for(j=0;j<4000;j++)           
    {
      for(k=0;k<222;k++);
                
    }
  }
}
//////////////////////SPI///////////////////////////////////
void SPI_Delay(unsigned char xrate)
{
  unsigned char i;
  while(xrate)
  {
    for(i=0;i<2;i++);
    xrate--;
  }
}


void SPI_Write(unsigned char value)                                    
{                                                           
    unsigned char i;  
   SPI_Delay(1);
    for(i=0; i<8; i++)   
    {
        EPD_W21_CLK_0;
       SPI_Delay(1);
       if(value & 0x80)
          EPD_W21_MOSI_1;
        else
          EPD_W21_MOSI_0;   
        value = (value << 1); 
       SPI_Delay(1);
       driver_delay_us(1);
        EPD_W21_CLK_1; 
        SPI_Delay(1);
    }
}

void Epaper_Write_Command(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_0;                   
  EPD_W21_DC_0;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}
void Epaper_Write_Data(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}

/////////////////EPD settings Functions/////////////////////

/////////////////////////////////////LUT//////////////////////////////////////////////
const unsigned char LUT_DATA[] PROGMEM= {   
0x80,0x60,0x40,0x00,0x00,0x00,0x00,             //LUT0: BB:     VS 0 ~7
0x10,0x60,0x20,0x00,0x00,0x00,0x00,             //LUT1: BW:     VS 0 ~7
0x80,0x60,0x40,0x00,0x00,0x00,0x00,             //LUT2: WB:     VS 0 ~7
0x10,0x60,0x20,0x00,0x00,0x00,0x00,             //LUT3: WW:     VS 0 ~7
0x00,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT4: VCOM:   VS 0 ~7

0x03,0x03,0x00,0x00,0x02,                       // TP0 A~D RP0
0x09,0x09,0x00,0x00,0x02,                       // TP1 A~D RP1
0x03,0x03,0x00,0x00,0x02,                       // TP2 A~D RP2
0x00,0x00,0x00,0x00,0x00,                       // TP3 A~D RP3
0x00,0x00,0x00,0x00,0x00,                       // TP4 A~D RP4
0x00,0x00,0x00,0x00,0x00,                       // TP5 A~D RP5
0x00,0x00,0x00,0x00,0x00,                       // TP6 A~D RP6

0x15,0x41,0xA8,0x32,0x30,0x0A,
};
const unsigned char LUT_DATA_part[] PROGMEM={ 
0x00,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT0: BB:     VS 0 ~7
0x80,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT1: BW:     VS 0 ~7
0x40,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT2: WB:     VS 0 ~7
0x00,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT3: WW:     VS 0 ~7
0x00,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT4: VCOM:   VS 0 ~7

0x0A,0x00,0x00,0x00,0x00,                       // TP0 A~D RP0
0x00,0x00,0x00,0x00,0x00,                       // TP1 A~D RP1
0x00,0x00,0x00,0x00,0x00,                       // TP2 A~D RP2
0x00,0x00,0x00,0x00,0x00,                       // TP3 A~D RP3
0x00,0x00,0x00,0x00,0x00,                       // TP4 A~D RP4
0x00,0x00,0x00,0x00,0x00,                       // TP5 A~D RP5
0x00,0x00,0x00,0x00,0x00,                       // TP6 A~D RP6

0x15,0x41,0xA8,0x32,0x30,0x0A,
};       
void EPD_select_LUT(const unsigned char * wave_data)
{        
     unsigned char count;
     Epaper_Write_Command(0x32);
   for(count=0;count<70;count++)
  Epaper_Write_Data(pgm_read_byte(&wave_data[count]));

} 

/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
void EPD_HW_Init(void)
{
  EPD_W21_RST_0;  // Module reset      
  delay(1); //At least 10ms delay 
  EPD_W21_RST_1; 
  delay(1); //At least 10ms delay 

  Epaper_READBUSY();
  Epaper_Write_Command(0x12); // soft reset
  Epaper_READBUSY();

  Epaper_Write_Command(0x74); //set analog block control       
  Epaper_Write_Data(0x54);
  Epaper_Write_Command(0x7E); //set digital block control          
  Epaper_Write_Data(0x3B);

  Epaper_Write_Command(0x01); //Driver output control      
  Epaper_Write_Data(0xF9);
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x00);

  Epaper_Write_Command(0x11); //data entry mode       
  Epaper_Write_Data(0x01);

  Epaper_Write_Command(0x44); //set Ram-X address start/end position   
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x0F);    //0x0C-->(15+1)*8=128

  Epaper_Write_Command(0x45); //set Ram-Y address start/end position          
  Epaper_Write_Data(0xF9);   //0xF9-->(249+1)=250
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x00); 

  Epaper_Write_Command(0x3C); //BorderWavefrom
  Epaper_Write_Data(0x03);  


  Epaper_Write_Command(0x2C);     //VCOM Voltage
  Epaper_Write_Data(0x55);    //

  Epaper_Write_Command(0x03); //      
  Epaper_Write_Data(LUT_DATA[70]);

  Epaper_Write_Command(0x04); //      
  Epaper_Write_Data(LUT_DATA[71]);    
  Epaper_Write_Data(LUT_DATA[72]);    
  Epaper_Write_Data(LUT_DATA[73]);    


  Epaper_Write_Command(0x3A);     //Dummy Line   
  Epaper_Write_Data(LUT_DATA[74]);    
  Epaper_Write_Command(0x3B);     //Gate time 
  Epaper_Write_Data(LUT_DATA[75]);   

  EPD_select_LUT((unsigned char *)LUT_DATA); //LUT

  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(0x00);
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(0xF9);
  Epaper_Write_Data(0x00);
  Epaper_READBUSY();
  
}
//////////////////////////////All screen update////////////////////////////////////////////
void EPD_WhiteScreen_ALL(void)
{
   unsigned int i;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(pgm_read_byte(&gImage_1[i]));
   }
   EPD_Update();   
}

/////////////////////////////////////////////////////////////////////////////////////////
void EPD_Update(void)
{   
  Epaper_Write_Command(0x22); //Display Update Control 
  Epaper_Write_Data(0xC7);   
  Epaper_Write_Command(0x20);  //Activate Display Update Sequence
  Epaper_READBUSY();  

}

void EPD_DeepSleep(void)
{  
  Epaper_Write_Command(0x10); //enter deep sleep
  Epaper_Write_Data(0x01); 
  delay(100);
}
void Epaper_READBUSY(void)
{ 
  while(1)
  {   //=1 BUSY
     if(isEPD_W21_BUSY==0) break;
  }  
}
///////////////////////////Part update//////////////////////////////////////////////
void EPD_SetRAMValue_BaseMap( const unsigned char * datas)
{
  unsigned int i;   
  const unsigned char  *datas_flag;   
  datas_flag=datas;
  Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(pgm_read_byte(&datas[i]));
   }
   datas=datas_flag;
  Epaper_Write_Command(0x26);   //Write Black and White image to RAM
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(pgm_read_byte(&datas[i]));
   }
   EPD_Update();   
}

void EPD_Part_Init(void)
{
  Epaper_Write_Command(0x2C);     //VCOM Voltage
  Epaper_Write_Data(0x26);  
  
  Epaper_READBUSY();  
  EPD_select_LUT(( unsigned char *)LUT_DATA_part);
  Epaper_Write_Command(0x37); 
  Epaper_Write_Data(0x00);  
  Epaper_Write_Data(0x00);  
  Epaper_Write_Data(0x00);  
  Epaper_Write_Data(0x00);  
  Epaper_Write_Data(0x40);  
  Epaper_Write_Data(0x00);  
  Epaper_Write_Data(0x00);   

  Epaper_Write_Command(0x22); //Display Update Control 
  Epaper_Write_Data(0xC0);   
  Epaper_Write_Command(0x20);  //Activate Display Update Sequence
  Epaper_READBUSY();  
  
  Epaper_Write_Command(0x3C); //BorderWavefrom
  Epaper_Write_Data(0x01);

}
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE)
{
  unsigned int i;  
  unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
  x_start=x_start/8;//
  x_end=x_start+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_start;
  if(y_start>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_start+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_start);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    // ????=0 


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_start); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(pgm_read_byte(&datas[i]));
   } 
   EPD_Part_Update();

}

void EPD_Part_Update(void)
{
  Epaper_Write_Command(0x22); //Display Update Control 
  Epaper_Write_Data(0x0C);   
  Epaper_Write_Command(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();    
}


/////////////////////////////////Single display////////////////////////////////////////////////

void EPD_WhiteScreen_Black(void)

{
   unsigned int i,k;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
  for(k=0;k<250;k++)
  {
    for(i=0;i<16;i++)
    {
      Epaper_Write_Data(0x00);
      }
  }
    EPD_Update();
}

void EPD_WhiteScreen_White(void)

{
   unsigned int i,k;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
  for(k=0;k<250;k++)
  {
    for(i=0;i<16;i++)
    {
      Epaper_Write_Data(0xff);
      }
  }
  EPD_Update();
}



//////////////////////////////////END//////////////////////////////////////////////////

