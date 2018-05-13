#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
//#include <wiringPiSPI.h>
#include <wiringPiI2C.h>
#include <unistd.h>
#include <errno.h>
//#include "arducam.h"
#include <ov5642_regs.h>
#include <linux/i2c-dev.h>
#define OV5642_CHIPID_HIGH 0x300a
#define OV5642_CHIPID_LOW 0x300b

#define BUF_SIZE (384*1024)
uint8_t buffer[BUF_SIZE] = {0xFF};

uint8_t arducam_i2c_word_write(uint16_t fd, uint16_t regID, uint8_t regDat)
{
	uint8_t reg_H,reg_L;
	uint16_t value;
	reg_H = (regID >> 8) & 0x00ff;
	reg_L = regID & 0x00ff;
	value =  regDat << 8 | reg_L;
	if(fd != -1)
	{
        int rc;
		rc = i2c_smbus_write_word_data(fd, reg_H, value);
		//printf("regID:0x%x%x\t\tregDat:0x%02x rc %02x\n",reg_H,reg_L,regDat,rc);
		return(rc);
	}
	return 0;
}

uint8_t arducam_i2c_write(uint16_t fd, uint8_t regID, uint8_t regDat)
{
	if(fd != -1)
	{
        union i2c_smbus_data data ;
        data.byte = regDat;
        i2c_smbus_access (fd, I2C_SMBUS_WRITE, regID, I2C_SMBUS_BYTE_DATA, &data) ;
        //printf("i2c_smbus_access %02x\n", rc);
		//wiringPiI2CWriteReg8(fd,regID,regDat);
//		return(1);
	}
	return 0;
}

int arducam_i2c_write_regs(uint16_t fd, const struct sensor_reg reglist[])
{
	uint16_t reg_addr = 0;
	uint16_t reg_val = 0;
	const struct sensor_reg *next = reglist;

	while ((reg_addr != 0xffff) | (reg_val != 0xff))
	{
		reg_addr = next->reg;
		reg_val = next->val;
    
		//reg_addr = pgm_read_word(&next->reg);
		//reg_val = pgm_read_word(&next->val);
        int rc = arducam_i2c_write(fd, reg_addr, reg_val);
        //printf("reg_addr %04x reg_val %04x rc %02x\n", reg_addr, reg_val, rc);
		if (rc)
			return 1;
	   	next++;
	}

	return 1;
}

uint8_t arducam_i2c_word_read(uint16_t fd, uint16_t regID, uint8_t* regDat)
{
	uint8_t reg_H,reg_L;
	int r;
	reg_H = (regID >> 8) & 0x00ff;
	reg_L = regID & 0x00ff;
	if(fd != -1)
	{
		r = i2c_smbus_write_byte_data(fd,reg_H,reg_L);
		if(r<0)
			return 0;
		*regDat = i2c_smbus_read_byte(fd);
		return(1);
	}
	return 0;
}

int initCam()
{
    int camFd;
    int rc;
    camFd = wiringPiI2CSetup(0x3c);
//    rc = arducam_i2c_word_write(camFd, 0x3008, 0x80); // system control 00 - sw reset
    //rc = arducam_i2c_write_regs(camFd, OV5642_QVGA_Preview);
    usleep(1e5);
    rc = arducam_i2c_write_regs(camFd, OV5642_JPEG_Capture_QSXGA);
    //rc = arducam_i2c_write_regs(camFd, ov5642_320x240);
//const struct sensor_reg ov5642_320x240[] PROGMEM =
    usleep(1e5);
    // thumbnail enable - 1010 1000 - compression,vflip
    //rc = arducam_i2c_word_write(camFd, 0x3818, 0xa8); 
    // array control 01 - 0001 0000 
    //rc = arducam_i2c_word_write(camFd, 0x3621, 0x10);
    // timing control hs href horz start point LB
    //rc = arducam_i2c_word_write(camFd, 0x3801, 0xb0);
    // compress control 07 - 0000 1100 - quantization scale
    //rc = arducam_i2c_word_write(camFd, 0x4407, 0x0c);

    // 0x300e 0001 1010 0x1a enable DVP
 //   rc = arducam_i2c_word_write(camFd, 0x300e, 0b00011010);
    usleep(1e5);
    // HERE
    if (rc < 0)
    {
        printf("errno %u\n", errno);
    }
    return camFd;
}
//
//    		sensor_addr = 0x3c;
//    	#else
//      	sensor_addr = 0x78;
//#if ( defined(OV5642_CAM) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP_PLUS) )
//        wrSensorReg16_8(0x3008, 0x80);
//       if (m_fmt == RAW){
//       	//Init and set 640x480;
//         wrSensorRegs16_8(OV5642_1280x960_RAW);	
//		     wrSensorRegs16_8(OV5642_640x480_RAW);	
//      }else{	
//      	wrSensorRegs16_8(OV5642_QVGA_Preview);
//        #if defined (RASPBERRY_PI) 
//			  arducam_delay_ms(100);
//				#else
//        delay(100);
//        #endif
//        if (m_fmt == JPEG)
//        {
//        	#if defined (RASPBERRY_PI) 
//				  arducam_delay_ms(100);
//					#else
//	        delay(100);
//	        #endif
//          wrSensorRegs16_8(OV5642_JPEG_Capture_QSXGA);
//          wrSensorRegs16_8(ov5642_320x240);
//          #if defined (RASPBERRY_PI) 
//			  arducam_delay_ms(100);
//				#else
//        delay(100);
//        #endif
//          wrSensorReg16_8(0x3818, 0xa8);
//          wrSensorReg16_8(0x3621, 0x10);
//          wrSensorReg16_8(0x3801, 0xb0);
//          #if (defined(OV5642_MINI_5MP_PLUS) || (defined ARDUCAM_SHIELD_V2))
//          wrSensorReg16_8(0x4407, 0x04);
//          #else
//          wrSensorReg16_8(0x4407, 0x0C);
//           HERE
//          #endif
//        }
//        else
//        {
//        	
//        	byte reg_val;
//          wrSensorReg16_8(0x4740, 0x21);
//          wrSensorReg16_8(0x501e, 0x2a);
//          wrSensorReg16_8(0x5002, 0xf8);
//          wrSensorReg16_8(0x501f, 0x01);
//          wrSensorReg16_8(0x4300, 0x61);
//          rdSensorReg16_8(0x3818, &reg_val);
//          wrSensorReg16_8(0x3818, (reg_val | 0x60) & 0xff);
//          rdSensorReg16_8(0x3621, &reg_val);
//          wrSensorReg16_8(0x3621, reg_val & 0xdf);
//        }
//        }
//
//#endif
//        break;
//      }
//

uint8_t chipID(uint16_t fd)
{
    uint8_t hi[4];
    uint8_t lo[4];
    arducam_i2c_word_read(fd, 0x300a, (uint8_t*)&hi); // chip ID MSB
    arducam_i2c_word_read(fd, 0x300b, (uint8_t*)&lo); // chip ID LSB
    int id = (hi[0] << 8) | lo[0];
    printf("Chip ID %04x\n", id);
    return 0;
}

void setup()
{
	int fd;
	fd = wiringPiI2CSetup (0x3c);
    printf("%u\n", fd);
    chipID(fd);
//    uint8_t vid,pid;
//    uint8_t temp;
//    wiring_init();
//    arducam(smOV5642,CAM1_CS,-1,-1,-1);
//     // Check if the ArduCAM SPI bus is OK
//    arducam_write_reg(ARDUCHIP_TEST1, 0x55, CAM1_CS);
//    temp = arducam_read_reg(ARDUCHIP_TEST1, CAM1_CS);
//    //printf("temp=%x\n",temp);  //  debug
//    if(temp != 0x55) {
//        printf("SPI interface error!\n");
//        //exit(EXIT_FAILURE);
//    }
//     else{
//    	   printf("SPI interface OK!\n");
//    	}
//    	
//    // Change MCU mode
//    arducam_write_reg(ARDUCHIP_MODE, 0x00, CAM1_CS);
//    
//    // Check if the camera module type is OV5642
//    arducam_i2c_word_read(OV5642_CHIPID_HIGH, &vid);
//    arducam_i2c_word_read(OV5642_CHIPID_LOW, &pid);
//    if((vid != 0x56) || (pid != 0x42)) {
//        printf("Can't find OV5642 module!\n");
//        exit(EXIT_FAILURE);
//    } else {
//        printf("OV5642 detected\n");
//    }
}

void grabFrame(int camFd)
{
    int rc;
    rc = arducam_i2c_word_write(camFd, 0x4741, 0b00000100); // enable test pattern
    printf("rc %02x\n", rc);
    //rc = arducam_i2c_word_write(camFd, 0x471b, 0b00000001); // HSYNC mode enable
    printf("rc %02x\n", rc);
}

int main(int argc, char *argv[])
{
//    if (argc == 1) {
//        printf("Usage: %s [-s <resolution>] | [-c <filename]", argv[0]);
//        printf(" -s <resolution> Set resolution, valid resolutions are:\n");      
//        printf("                   320x240\n");
//        printf("                   640x480\n");
//        printf("                   800x600\n");
//        //printf("                   1280x720\n");
//        printf("                   1920x1080\n");
//        printf("                   2048x1563\n");
//        printf("                   2592x1944\n");
//        printf(" -c <filename>   Capture image\n");
//        exit(EXIT_SUCCESS);
//    }
//    
//    printf("argv1 %s argc %u\n", argv[1], argc);
//    if (strcmp(argv[1], "-c") == 0 && argc == 4) {
        int fd = initCam();
        chipID(fd);
        grabFrame(fd);
//        arducam_set_format(fmtJPEG);
//        arducam_init();
//        
//        // Change to JPEG capture mode and initialize the OV2640 module   
//        if (strcmp(argv[3], "320x240") == 0) arducam_OV5642_set_jpeg_size(OV5642_320x240);
//        else if (strcmp(argv[3], "640x480") == 0) arducam_OV5642_set_jpeg_size(OV5642_640x480);
//        //else if (strcmp(argv[2], "1280x720") == 0) arducam_OV5642_set_jpeg_size(OV5642_1280x720);
//        else if (strcmp(argv[3], "1920x1080") == 0) arducam_OV5642_set_jpeg_size(OV5642_1920x1080);
//        else if (strcmp(argv[3], "2048x1536") == 0) arducam_OV5642_set_jpeg_size(OV5642_2048x1536);
//        else if (strcmp(argv[3], "2592x1944") == 0) arducam_OV5642_set_jpeg_size(OV5642_2592x1944);
//      
//        else {
//            printf("Unknown resolution %s\n", argv[3]);
//            exit(EXIT_FAILURE);
//        }
//        sleep(1); // Let auto exposure do it's thing after changing image settings
//        printf("Changed resolution1 to %s\n", argv[3]);
//        
//        delay(1000);
//        
//        arducam_write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK,CAM1_CS);		//VSYNC is active HIGH   
//			  
//        // Flush the FIFO
//        arducam_flush_fifo(CAM1_CS);    
//        // Clear the capture done flag
//        arducam_clear_fifo_flag(CAM1_CS);
//        // Start capture
//        printf("Start capture\n");  
//        arducam_start_capture(CAM1_CS);
//        
//       while (!(arducam_read_reg(ARDUCHIP_TRIG,CAM1_CS) & CAP_DONE_MASK)) ;
//        
//        printf(" CAM1 Capture Done\n");
//                 
//        // Open the new file
//        FILE *fp1 = fopen(argv[2], "w+");
//        
//        if (!fp1) {
//            printf("Error: could not open %s\n", argv[2]);
//            exit(EXIT_FAILURE);
//        }
//          
//        printf("Reading FIFO\n");
//        
//        size_t len = read_fifo_length(CAM1_CS);
//	      if (len >= 393216){
//			   printf("Over size.");
//			    exit(EXIT_FAILURE);
//			  }else if (len == 0 ){
//			    printf("Size is 0.");
//			    exit(EXIT_FAILURE);
//			  } 
//			  digitalWrite(CAM1_CS,LOW);  //Set CS low       
//	      set_fifo_burst(BURST_FIFO_READ);
//	      arducam_spi_transfers(buffer,1);//dummy read  
//	      int32_t i=1;
//	      while(len>4096)
//	      {	 
//	      	arducam_transfers(&buffer[i],4096);
//	      	len -= 4096;
//	      	i += 4096;
//	      }
//	      arducam_spi_transfers(&buffer[i],len); 
//	
//	      fwrite(buffer, len+i, 1, fp1);
//	      digitalWrite(CAM1_CS,HIGH);  //Set CS HIGH
//	       //Close the file
//	      delay(100);
//	      fclose(fp1);  
//	      // Clear the capture done flag
//	      arducam_clear_fifo_flag(CAM1_CS);
//    
//    } else {
//        printf("Error: unknown or missing argument.\n");
//        exit(EXIT_FAILURE);
//    }
    exit(EXIT_SUCCESS);
}
