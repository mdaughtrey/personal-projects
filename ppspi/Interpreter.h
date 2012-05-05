#ifndef INCLUDED_INTERPRETER_H
#define INCLUDED_INTERPRETER_H

#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <programmer.h>
#include <buildlogger.h>

class Interpreter;

typedef bool (Interpreter::* Handler)(std::string &);

class Interpreter
{
private:
  log4cpp::Category & log;
  Programmer & programmer;
  std::map<std::string, Handler> cmdMap;

public:

  bool onQuit(std::string &);
  bool onPower(std::string &);
  bool onReset(std::string &);
  bool onSS(std::string &);
  bool onReadByte(std::string &);
  bool onWriteByte(std::string &);
  bool onReadChar(std::string &);
  bool onWriteChar(std::string &);
  bool onHelp(std::string &);
  bool onPassThrough(std::string &);
  bool onInitCycle(std::string &);
  bool onTextLoop(std::string &);
  bool onSCK(std::string &);
  bool onMOSI(std::string &);
  bool onInterfaceEnable(std::string &);
  
  Interpreter(Programmer & prog) :
    log(buildLogger("Interpreter")),
    programmer(prog)
  {
    cmdMap["q"] = &Interpreter::onQuit;
    cmdMap["p"] = &Interpreter::onPower;
    cmdMap["r"] = &Interpreter::onReset;
    cmdMap["rb"] = &Interpreter::onReadByte;
    cmdMap["wb"] = &Interpreter::onWriteByte;
    cmdMap["rc"] = &Interpreter::onReadChar;
    cmdMap["wc"] = &Interpreter::onWriteChar;
    cmdMap["h"] = &Interpreter::onHelp;
    cmdMap["ss"] = &Interpreter::onSS;
    cmdMap["pt"] = &Interpreter::onPassThrough;
    cmdMap["i"] = &Interpreter::onInitCycle;
    cmdMap["tl"] = &Interpreter::onTextLoop;
    cmdMap["sck"] = &Interpreter::onSCK;
    cmdMap["mosi"] = &Interpreter::onMOSI;
    cmdMap["ie"] = &Interpreter::onInterfaceEnable;
  }
  bool process(std::istream & cmdIn);
  //  bool process(std::ifstream & streamIn);
};

#endif // INCLUDED_INTERPRETER_H
