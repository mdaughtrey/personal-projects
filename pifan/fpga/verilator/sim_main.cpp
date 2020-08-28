#include "Vmyspi.h"
#include "verilated.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <unistd.h>
#include <sstream>
#include <bitset>

// VL_IN8(sysclk,0,0);
// VL_IN8(txReady,0,0);
// VL_IN8(iSPIClk,0,0);
// VL_IN8(iSPICS,0,0);
// VL_OUT8(oRxReady,0,0);
// VL_OUT8(oRx,7,0);
// VL_IN8(tx,7,0);
// VL_IN8(iSPIMOSI,0,0);
// VL_OUT8(oSPIMISO,0,0);
// VL_OUT16(probe,15,0);

//assign probe = {13'b0, rxBit, rxAccum, misoIndex};
//assign probe = {9'b0, spimiso, misoIndex, rxBit, rxAccum, rxFinal};

typedef struct
{
    unsigned int rxFinal : 8;
    unsigned int rxAccum : 8;
    unsigned int rxBit : 3;
    unsigned int misoIndex : 3;
    unsigned int spimiso : 1;
    unsigned int dummy : 9;
} Probe;

vluint64_t main_time = 0;
double sc_time_stamp()
{
    return main_time;
}

// Sends SPI signal to Verilog
class MasterOut {
public:
    MasterOut(unsigned char);
    bool eval(Vmyspi &, double);
private:
    const std::vector<int> clkHigh = { 40, 80, 120, 160, 200, 240, 280, 320 };
    const std::vector<int> clkLow = { 60, 100, 140, 180, 220, 260, 300, 340 };
    int state;
    int sendbit;
    unsigned char tosend;
};

MasterOut::MasterOut(unsigned char _tosend)
{
    this->tosend = _tosend;
    this->sendbit = 7;
    this->state = 0;
    std::cout << "tosend is " << std::hex << (int)this->tosend << std::endl;
}

bool MasterOut::eval(Vmyspi & top, double main_time)
{
    if (0 == this->state)
    {
        top.iSPICS = 1;
        top.iSPIClk = 0;
        std::cout << "MasterOut: " << state << ": init" << std::endl;
    } else if (this->state == this->clkHigh.front() - 20)
    {
        top.iSPICS = 0;
        std::cout << "MasterOut: " << state << ": CS Low" << std::endl;
    } else if (clkHigh.end() !=
        std::find(this->clkHigh.begin(), this->clkHigh.end(), this->state))
    {
        std::cout << "MasterOut: " << "Sending bit " << sendbit << " of " << std::hex << (int)this->tosend << std::endl;
//        std::cout << "tosend: " << (this->tosend >> sendbit++) << std::endl;
        top.iSPIMOSI = (this->tosend >> sendbit) & 0x01;
        sendbit--;
        std::cout << "MasterOut: " << "iSPIMOSI :" << (int) top.iSPIMOSI << std::endl;
        top.iSPIClk = 1;
        std::cout << "MasterOut: " << state << ": CLK High" << std::endl;
    } else if (clkLow.end() !=
        std::find(this->clkLow.begin(), this->clkLow.end(), this->state))
    {
        top.iSPIClk = 0;
        std::cout << "MasterOut: " << state << ": CLK Low" << std::endl;
    } else if (this->state == this->clkLow.back() + 40)
    {
        top.iSPICS = 1;
        std::cout << "MasterOut: " << state << ": CS High" << std::endl;
    }
    if (this->state++ > this->clkLow.back() + 60)
    {
        std::cout << "MasterOut: " << state << ": Done" << std::endl;
//        return false;
    }
    if (this->state > this->clkLow.back() + 61)
    {
        return true;
    }
    this->state %= this->clkLow.back() + 70;
    return true;
}

class MasterIn {
public:
    MasterIn(){};
    bool eval(Vmyspi &);
private:
    int state;
    int rxbit;
    unsigned char rxaccum;
    unsigned char rxfinal;
};

bool MasterIn::eval(Vmyspi & top)
{
    std::cout << "MasterIn state " << this->state
        << " rxbit " << (int)this->rxbit
        << " MISO " << (int)top.oSPIMISO 
        << " rxAccum " << std::hex << (int)this->rxaccum
        << " rxFinal " << std::hex << (int)this->rxfinal
        << std::endl;
    if (1 == top.iSPICS)
    {
        this->rxbit = 0;
        this->state = 0;
        this->rxaccum = 0;
        this->rxfinal = 0;
        return true;
    }
    switch (this->state)
    {
    case 0: if (0 == top.iSPICS) state++; break;
    case 1: if (1 == top.iSPIClk)
        {
            if (0 == rxbit)
            {
                std::cout << "Reset m_in" << std::endl;
                this->rxbit = 0;
                this->rxaccum = 0;
            }
            this->state++;
        }
        break;
    case 2: if (0 == top.iSPIClk)
        {
            this->rxaccum |= top.oSPIMISO;
            this->rxaccum <<= 1;
            std::cout << "MasterIn: Received bit " << (int)this->rxbit << ", rxaccum " << std::bitset<8>(this->rxaccum) << std::endl;
            this->rxbit++;
            if (8 == this->rxbit)
            {
                this->rxfinal = this->rxaccum;
                std::cout << "MasterIn: Received 8 bits rxfinal:" << std::bitset<8>(this->rxfinal) <<  std::endl;
                this->rxbit = 0;
            }
            this->state = 1;
        }
    default:
        break;
    }
    return true; 
}

std::string dumpProbe(Probe * probe)
{
    std::stringstream os;

    os << "rxFinal " << std::bitset<8>(probe->rxFinal)
        << " rxAccum " << std::bitset<8>(probe->rxAccum )
        << " rxBit " << std::bitset<3>(probe->rxBit)
        << " misoIndex " << std::bitset<3>(probe->misoIndex)
        << " spimiso" << std::bitset<1>(probe->spimiso);
   
    return os.str();
}

int main(int argc, char** argv, char** env) {
    Verilated::commandArgs(argc, argv);
    Vmyspi top;
    top.tx = 0xaa;
    top.txReady = 1;
    MasterOut mout(0x55);
    MasterIn m_in;
    std::cout << "start" << std::endl;
    unsigned int probe = 0;
    Probe * probePtr(reinterpret_cast<Probe *>(&top.probe));
    std::cout << sizeof(unsigned int) << std::endl;
    top.tx = 0xaa;
    top.txReady = 1;
    while (!Verilated::gotFinish() && main_time < 1000)
    {
        if (main_time % 10 == 1) top.sysclk = 1;
        if (main_time % 10 == 6) top.sysclk = 0;
        if (!mout.eval(top, main_time))
        {
            break;
        }
        top.eval();
        m_in.eval(top);
        if (probe != top.probe)
        {
            probe = top.probe;
            std::cout << dumpProbe(probePtr) << std::endl;
        }
//        if (top.oRxReady)
//        {
//            std::cout << "Received " << std::hex << top.oRx << std::endl;
//            top.tx = 0xaa;
//            top.txReady = 1;
//        }
        main_time++;
        //sleep(1);
    }
    exit(0);
}
