all:
	gcc -g -lmmal_core -lmmal_util -lmmal_vc_client -lvcos -lbcm_host -I/opt/vc/include/ -I/opt/vc/include/interface/vctypes/ -L/opt/vc/lib/ -o raspiraw raspiraw.c RaspiCLI.c


run:
	./raspiraw --mode 1 -o out.raw --i2c 2

