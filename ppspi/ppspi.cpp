#include <iostream>
#include <iomanip>
#include <fstream>

extern "C"
{
#include <unistd.h>
}
#include <stdexcept>
#include <string>
#include <programmer.h>
#include <parallelport.h>
#include <Interpreter.h>
#include <buildlogger.h>


extern "C"
{
#include <unistd.h>
}

int main(int argc, char * argv[])
{
  log4cpp::Category & log = buildLogger("main");
  std::ifstream commandFile;

  if (argc > 1)
    {
      commandFile.open(argv[1]);
      if (commandFile.fail())
	{
	  LOG4CPP_ERROR << "Cannot open command file "
			<< argv[1]
			<< LOG4CPP_END;
	  exit(1);
	}
    }

  try
    {
      Programmer programmer("/dev/parport0");
      Interpreter interpreter(programmer);

      if (commandFile.is_open())
	{
	  LOG4CPP_INFO << "Reading from command file "
		       << argv[1]
		       << LOG4CPP_END;
	  while (interpreter.process(commandFile) == true);
	}
      else
	{
	  while (interpreter.process(std::cin) == true);
	}
      //      ParallelPort port("/dev/parport0");
//       port.setData(0);
//       exit(0);
//       while (1)
//       { 
// 	  for (int count = startBit; count <= endBit; count++)
// 	    {
// 	      unsigned char status;
// 	      char hexTable[] = "0123456789abcdef";
 // 	      for (int count2 = 0; count2 < 1000; count2++)
// 		{
// 		  if (startBit == endBit)
// 		    {
// 		  port.setData(0);
// 		      usleep(100000);
// 		    }
// 		  port.setData(1 << count);
// 		  //		}
//  	      status = port.getStatus();
	      
//  	      LOG4CPP_INFO << hexTable[(status >> 4) & 0x0f]
//  			   << hexTable[status & 0x0f]
//  			   << LOG4CPP_END;
//	      sleep(1);
// 		  usleep(100000);
// 	    }
// 	}

    }

  catch (std::exception e)
    {
      LOG4CPP_FATAL << e.what() << LOG4CPP_END;
    }
  log4cpp::Category::shutdown();

  return 0;
}
