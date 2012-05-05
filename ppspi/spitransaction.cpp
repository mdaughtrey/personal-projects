#include <spitransaction.h>


SpiTransaction::SpiTransaction(std::vector<char> & vCharIn)
{
  int ii;
  std::vector<char>::iterator ivCharIn;
  

  for (ivCharIn = vCharIn.begin();
       ivCharIn != vCharIn.end();
       ivCharIn++)
    {
      for (ii = 0; ii < 8; ii++)
	{
	  inBits.push_back((*ivCharIn) & 0x01 ? true: false);
	  (*ivCharIn) >>= 1;
	}
    }
  iInBits = inBits.begin();
}

SpiTransaction::SpiTransaction(const std::string strIn, std::string & strOut)
{
}

SpiTransaction::~SpiTransaction()
{
}

void SpiTransaction::getBytes(std::vector<char> & vCharOut)
{
  std::vector<bool>::iterator iOutBits;
  int ii = 0;
  char assemble = 0;

  for (iOutBits = outBits.begin();
       iOutBits != outBits.end();
       iOutBits++)
    {
      assemble <<= 1;
      assemble |= (*iOutBits ? 1 : 0 );
      if (++ii == 8)
	{
	  vCharOut.push_back(assemble);
	  assemble = 0;
	  ii = 0;
	}
    }

}
