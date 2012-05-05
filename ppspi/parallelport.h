#ifndef INCLUDED_PARALLELPORT_H
#define INCLUDED_PARALLELPORT_H

#include <buildlogger.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>

extern "C"
{
#include <linux/ppdev.h>
}

const int DATA = 0;

const int CTL_SELECT = 3;	// pin 17
const int CTL_INIT = 2;		// pin 16
const int CTL_AUTOFWD = 1;	// pin 14
const int CTL_STROBE = 0;	// pin 1

const int STATUS_BUSY = 7;	// pin 11 
const int STATUS_ACK = 6;	// pin 10
const int STATUS_PAPEREND = 5;	// pin 12
const int STATUS_SELECTIN = 4;	// pin 13
const int STATUS_ERRO = 3;	// pin 15

class ParallelPort
{
 private:
  int ppFD;
  log4cpp::Category & log;
  unsigned char shadowData;
  unsigned char shadowControl;
  
 public:
  ParallelPort(std::string portName);
  virtual ~ParallelPort();

  void setData(unsigned char data);
  unsigned char getData(void);

  void setDataBit(int bitNumber, bool state);
  bool getDataBit(int bitNumber);

  void setControlBit(int bitNumber, bool state);
  bool getControlBit(int bitNumber);

  unsigned char getStatus(void);
};

#endif // INCLUDED_PARALLELPORT_H
