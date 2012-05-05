#ifndef INCLUDED_SPITRANSACTION_H
#define INCLUDED_SPITRANSACTION_H

#include <vector>
#include <string>

class Programmer;

class SpiTransaction
{
private:
  friend class Programmer;

  std::vector<bool> inBits;
  std::vector<bool> outBits;
  std::vector<bool>::iterator iInBits;

public:
  SpiTransaction(std::vector<char> & vCharIn);
  SpiTransaction(const std::string strIn, std::string & strOut);
  ~SpiTransaction();
  void getBytes(std::vector<char> & vCharOut);
};


#endif // INCLUDED_SPITRANSACTION_H
