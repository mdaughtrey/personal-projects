#include <parallelport.h>
#include <stdexcept>

extern "C"
{
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
}


ParallelPort::ParallelPort(std::string portName)
 :
  log(buildLogger("ParallelPort")),
  shadowData(0),
  shadowControl(0)
{
  LOG4CPP_DEBUG << "Constructor" << LOG4CPP_END;

  ppFD = open(portName.c_str(), O_RDWR);
  if (ppFD < 0)
    {
      throw std::runtime_error("error opening parallel port");
    }
  if (ioctl(ppFD, PPCLAIM) < 0)
    {
      throw std::runtime_error("cannot claim port");
    }

}

ParallelPort::~ParallelPort()
{
  LOG4CPP_DEBUG << "Destructor" << LOG4CPP_END;
  ioctl(ppFD, PPRELEASE);
  if (ppFD >= 0)
    {
      close (ppFD);
    }
}

void ParallelPort::setData(unsigned char data)
{
  //  LOG4CPP_DEBUG << "setData before " << (int)data << LOG4CPP_END;
  if (ioctl(ppFD, PPWDATA, &data) < 0)
    {
      int errNum = errno;
      LOG4CPP_ERROR << "PPWDATA fails "
		    << errNum
		    << " ("
		    << strerror(errNum)
		    << ")"
		    << LOG4CPP_END;
    }
  //  LOG4CPP_DEBUG << "setData after " << (int)data << LOG4CPP_END;
  shadowData = data;
}

unsigned char ParallelPort::getData(void)
{
  return 0;
}

void ParallelPort::setDataBit(int bitNumber, bool state)
{
  if (state == true)
    {
      shadowData |= (1 << bitNumber);
    }
  else
    {
      shadowData &= ~(1 << bitNumber);
    }
  setData(shadowData);
}

bool ParallelPort::getDataBit(int bitNumber)
{
  return false;
}

unsigned char ParallelPort::getStatus(void)
{
  unsigned char status;

  if (ioctl(ppFD, PPRSTATUS, &status) < 0)
    {
      int errNum = errno;
      LOG4CPP_ERROR << "PPRSTATUS fails "
		    << errNum
		    << " ("
		    << strerror(errNum)
		    << ")"
		    << LOG4CPP_END;
    }
  return status;
}

void ParallelPort::setControlBit(int bitNumber, bool state)
{
  unsigned char bits;
  
  ioctl(ppFD, PPRCONTROL, &bits);
  if (state == true)
    {
      bits |= (1 << bitNumber);
    }
  else
    {
      bits &= ~(1 << bitNumber);
    }

  if (ioctl(ppFD, PPWCONTROL, &bits) < 0)
    {
      int errNum = errno;
      LOG4CPP_ERROR << "PPWCONTROL fails "
		    << errNum
		    << " ("
		    << strerror(errNum)
		    << ")"
		    << LOG4CPP_END;
    }
}

bool ParallelPort::getControlBit(int bitNumber)
{
  unsigned char bits;
  
  ioctl(ppFD, PPRCONTROL, &bits);

  return (bits & (1 << bitNumber) ? true : false);
}
