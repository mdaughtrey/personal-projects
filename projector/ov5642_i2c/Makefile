all :  ov5642_cap
INC=-I. -I/home/mattd/Arduino/ArduCAM  -I../RaspberryPi/ArduCAM4Pi
DEFINES=-DRASPBERRY_PI
CC=g++ --std=c++11

#objects = arducam.o arducam_arch_raspberrypi.o
objects=

ov5642_cap : $(objects) ov5642_cap.o 
	$(CC) -o ov5642_cap $(objects) ov5642_cap.o -lwiringPi -Wall 
	
#arducam.o : arducam.c
#	$(CC) -c -g arducam.c -lwiringPi -Wall
#arducam_arch_raspberrypi.o : arducam_arch_raspberrypi.c
#	$(CC) -c -g arducam_arch_raspberrypi.c -lwiringPi -Wall


ov5642_cap.o : ov5642_cap.c
	$(CC) -c -g ov5642_cap.c -lwiringPi -Wall $(INC) $(DEFINES)

clean : 
	rm -f ov5642_ $(objects) *.o

run:
	sudo ./ov5642_cap -c out 640x480

debug:
	sudo gdb --args ./ov5642_cap -c out 640x480
