#include <programmer.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>

extern "C"
{
#include <unistd.h>
#include <time.h>
}

const int POWER = 1;
const int MOSI = 0;
const int MISO = 3;
const int SCK = 2;
const int SS = 4;
const int RESET = 3;
const int IENABLE = 5;

#define _BV(x) (1 << x)

namespace
{
  double diffus(struct timespec start, struct timespec now)
  {
    double diff = (now.tv_sec - start.tv_nsec) * 1e9;
    
    if (now.tv_nsec < start.tv_nsec)
      {
	diff += now.tv_nsec;
	diff += (1e9 - start.tv_nsec);
	
      }
    else
      {
	diff += now.tv_nsec - start.tv_nsec;
      }
    return diff;
    
  }
  void usleep_timed(bool sck, bool mosi, int timeus, ParallelPort & port)
  {
    struct timespec start;
    struct timespec now;

    std::ofstream logfile("./usleep_timed.txt", std::ios::app);
    logfile << "time       sck mosi miso" << std::endl;
    clock_gettime(CLOCK_REALTIME, &start);

    do
      {
	clock_gettime(CLOCK_REALTIME, &now);
	logfile << std::setw(9)
		<< now.tv_nsec
		<< " "
		<< sck 
		<< " "
		<< mosi
		<< " "
		<< (port.getStatus() & 0x08 ? 1 : 0)
		<< std::endl;
	
      }
    while (diffus(start, now) < timeus);
  }
}

Programmer::Programmer(const std::string device) :
  port(device),
  log(buildLogger("Programmer"))
{
  LOG4CPP_DEBUG << "Constructor" << LOG4CPP_END;
//   sck(false);
   slaveSelect(true);
}

Programmer::~Programmer ()
{
  LOG4CPP_DEBUG << "Destructor" << LOG4CPP_END;
}

void Programmer::power(bool state)
{
  port.setDataBit(POWER, state);
}

void Programmer::slaveSelect(bool state)
{
  port.setDataBit(SS, state);
}

void Programmer::mosi(bool state)
{
  port.setDataBit(MOSI, state);
}

void Programmer::sck(bool state)
{
  port.setDataBit(SCK, state);
}

void Programmer::reset(bool state)
{
  port.setDataBit(RESET, state);
}

void Programmer::interfaceEnable(bool state)
{
  port.setDataBit(IENABLE, !state);
}

#define DATAHOLD 10
void Programmer::spiTransaction(SpiTransaction & transaction)
{
  int ii = 0;
  
  transaction.iInBits = transaction.inBits.begin();

  while (transaction.iInBits != transaction.inBits.end())
    {
      sck(false);
      //      port.setDataBit(MOSI, 1);
      slaveSelect(false);

      for (ii = 0; ii < 8; ii++)
	{
	  port.setDataBit(MOSI, *transaction.iInBits);
	  sck(true);
	  sck(false);
	  transaction.iInBits++;
	}	  
      //      port.setDataBit(MOSI, 1);
      slaveSelect(true);
    }
}

// void Programmer::spiExchange(char in, char & out)
// {
//   int ii;
//   out = 0;
  
//   for (ii = 0; ii < 7; ii++)
//     {
//       in & 0x01 ? 
//     }
  
// }

// void Programmer::spiExchange(std::vector<char> in, std::vector<char> & out)
// {
//   int ii;
//   std::vector<char>::iterator iIn;
//   std::vector<char>::iterator iOut;

//   for (iIn = in.begin(), iOut = out.begin();
//        iIn != in.end() || iOut != out.end();
//        iIn++, iOut++)
//     {
//       if (iIn != in.end())
// 	{
// 	  //	  spiExchange
// 	}
//     }
// }


