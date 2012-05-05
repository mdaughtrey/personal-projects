#ifndef INCLUDED_PROGRAMMER_H
#define INCLUDED_PROGRAMMER_H

#include <buildlogger.h>
#include <parallelport.h>
#include <spitransaction.h>
#include <iostream>
#include <vector>

class Programmer
{
private:
  log4cpp::Category & log;
  ParallelPort port;
public:
  Programmer (const std::string device);
  ~Programmer ();
  void power(bool state);
  void reset(bool state);
  void slaveSelect(bool state);
  void sck(bool state);
  void mosi(bool state);
  void interfaceEnable(bool state);
  void spiTransaction(SpiTransaction & transaction);
/*   void spiExchange(std::vector<char> in, std::vector<char> & out); */
/*   void spiExchange(char in, char & out); */
/*   void spiStart(void); */
/*   void spiEnd(void); */
};

#endif // INCLUDED_PROGRAMMER_H
