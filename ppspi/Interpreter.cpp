
#include <Interpreter.h>
#include <buildlogger.h>

bool Interpreter::onQuit(std::string &)
{
  throw std::runtime_error("quit");
  return false;
}

bool Interpreter::onPower(std::string & cmdIn)
{
  if (cmdIn == "on")
    {
      programmer.power(true);
    }
  else if (cmdIn == "off")
    {
      programmer.power(false);
    }
  else
    {
      return false;
    }

  return true;
}

bool Interpreter::onInterfaceEnable(std::string & cmdIn)
{
  if (cmdIn == "off")
    {
      programmer.interfaceEnable(false);
    }
  else if (cmdIn == "on")
    {
      programmer.interfaceEnable(true);
    }
  else
    {
      return false;
    }
  return true;
}

bool Interpreter::onReset(std::string & cmdIn)
{
  if (cmdIn == "high")
    {
      programmer.reset(true);
    }
  else if (cmdIn == "low")
    {
      programmer.reset(false);
    }
  else
    {
      return false;
    }

  return true;
}

bool Interpreter::onTextLoop(std::string & cmdIn)
{
  int it = strtol(cmdIn.c_str(), NULL, 10);
  std::string text("abcdefghijklmnopqrstuvwxyz");
  while (it--)
    {
      LOG4CPP_DEBUG << it << " passes remaining" << LOG4CPP_END;
      onPassThrough(text);
      sleep(1);
    }
  return true;
}

bool Interpreter::onSS(std::string & cmdIn)
{
  if (cmdIn == "high")
    {
      programmer.slaveSelect(true);
    }
  else if (cmdIn == "low")
    {
      programmer.slaveSelect(false);
    }
  else
    {
      return false;
    }

  return true;
}

bool Interpreter::onMOSI(std::string & cmdIn)
{
  if (cmdIn == "high")
    {
      programmer.mosi(true);
    }
  else if (cmdIn == "low")
    {
      programmer.mosi(false);
    }
  else
    {
      return false;
    }

  return true;
}

bool Interpreter::onSCK(std::string & cmdIn)
{
  if (cmdIn == "high")
    {
      programmer.sck(true);
    }
  else if (cmdIn == "low")
    {
      programmer.sck(false);
    }
  else
    {
      return false;
    }

  return true;
}

bool Interpreter::onPassThrough(std::string & cmdIn)
{
  std::vector<char> command(cmdIn.begin(), cmdIn.end());
  std::vector<char> result;

  try
    {
      SpiTransaction transaction(command);
      programmer.spiTransaction(transaction);
      transaction.getBytes(result);
      LOG4CPP_DEBUG << std::string(result.begin(), result.end()).c_str() << LOG4CPP_END;
    }
  catch (std::exception e)
    {
      LOG4CPP_ERROR << e.what() << LOG4CPP_END;
      return false;
    }

  return true;
}

bool Interpreter::onReadByte(std::string &)
{
  return true;
}

bool Interpreter::onWriteByte(std::string &)
{
  return true;
}

bool Interpreter::onReadChar(std::string &)
{
  return true;
}

bool Interpreter::onWriteChar(std::string &)
{
  return true;
}

bool Interpreter::onInitCycle(std::string &)
{
  programmer.power(false);
  programmer.slaveSelect(true);
  sleep(1);
  programmer.power(true);
  programmer.reset(false);
  programmer.reset(true);
  
  return true;
}

bool Interpreter::onHelp(std::string &)
{
  std::map<std::string, Handler>::const_iterator iMap;
  std::string helpString("commands: ");

  for (iMap = cmdMap.begin(); iMap != cmdMap.end(); iMap++)
    {
      helpString += iMap->first + std::string(", ");
    }
  LOG4CPP_INFO << helpString.c_str() << LOG4CPP_END;

  return true;
}

// bool Interpreter::process(std::ifstream & streamIn)
// {
//   char command[256];

//   while (!streamIn.eof())
//     {
//       streamIn.getline(command, 256);
//     }
// }

bool Interpreter::process(std::istream & cmdIn)
{
  std::map<std::string, Handler>::const_iterator iMap;
  std::string command;
  
  if (cmdIn == std::cin)
    {
      LOG4CPP_INFO << "Interactive Input, Ready" << LOG4CPP_END;
      std::getline(cmdIn, command);
    }
  else
    {
      std::getline(cmdIn, command);
      if (command.empty())
	{
	  return false;
	}
    }
  
  std::string test(command, 0,
		   command.find(' ') == std::string::npos ? command.length() : command.find(' '));
//   LOG4CPP_DEBUG << "test "
// 		<< test.c_str()
// 		<< LOG4CPP_END;
  
  iMap = cmdMap.find(std::string(command, 0,
				 command.find(' ') == std::string::npos ? command.length() : command.find(' ')));
  if (iMap == cmdMap.end())
    {
      LOG4CPP_ERROR << "Unknown command "
		    << command.c_str()
		    << LOG4CPP_END;
      return true;
    }
  else
    {
      int pos;
      Handler h = iMap->second;
      pos = command.find(' ');
      if (pos != std::string::npos)
	{
	  command.erase(0, pos + 1);
	}
      try
	{
	  if (((*this).*h)(command) == false)
	    {
	      LOG4CPP_WARN << "Unknown command "
			   << command.c_str()
			   << LOG4CPP_END;
	    }
	}
      catch (std::exception e)
	{
	  return false;
	}
	      return true;
    }
}
