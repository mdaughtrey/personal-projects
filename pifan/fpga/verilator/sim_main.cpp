#include "Vmyspi.h"
#include "verilated.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <unistd.h>
#include <sstream>

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

//assign probe = {5'b0, rxBit, rxAccum, rxFinal};
typedef struct
{
    unsigned int rxFinal : 8;
    unsigned int rxAccum : 8;
    unsigned int rxBit : 3;
    unsigned int dummy : 13;
} Probe;

vluint64_t main_time = 0;
double sc_time_stamp()
{
    return main_time;
}

class SpiSender {
public:
    SpiSender(unsigned char);
    bool eval(Vmyspi &, double);
private:
    const std::vector<int> clkHigh = { 40, 80, 120, 160, 200, 240, 280, 320 };
    const std::vector<int> clkLow = { 60, 100, 140, 180, 220, 260, 300, 340 };
    int state;
    int sendbit;
    unsigned char tosend;
};

SpiSender::SpiSender(unsigned char _tosend)
{
    this->tosend = tosend;
    this->sendbit = 7;
    this->state = 0;
}

bool SpiSender::eval(Vmyspi & top, double main_time)
{
    if (0 == this->state)
    {
        top.iSPICS = 1;
        top.iSPIClk = 0;
        std::cout << state << ": init" << std::endl;
    } else if (this->state == this->clkHigh.front() - 20)
    {
        top.iSPICS = 0;
        std::cout << state << ": CS Low" << std::endl;
    } else if (clkHigh.end() !=
        std::find(this->clkHigh.begin(), this->clkHigh.end(), this->state))
    {
        std::cout << "tosend: " << (this->tosend >> sendbit++) << std::endl;
        top.iSPIMOSI = (this->tosend >> sendbit++) & 0x01;
        std::cout << "iSPIMOSI :" << (int) top.iSPIMOSI << std::endl;
        top.iSPIClk = 1;
        std::cout << state << ": CLK High" << std::endl;
        this->tosend--;
    } else if (clkLow.end() !=
        std::find(this->clkLow.begin(), this->clkLow.end(), this->state))
    {
        top.iSPIClk = 0;
        this->tosend--;
        std::cout << state << ": CLK Low" << std::endl;
    } else if (this->state == this->clkLow.back() + 40)
    {
        top.iSPICS = 1;
        std::cout << state << ": CS High" << std::endl;
    }
    if (this->state++ > this->clkLow.back() + 60)
    {
        std::cout << state << ": Done" << std::endl;
        return false;
    }
    this->state %= this->clkLow.back() + 70;
    return true;
}

std::string dumpProbe(Probe * probe)
{
    //assign probe = {5'b0, rxBit, rxAccum, rxFinal};
    std::stringstream os;
    os << "rxBit " << std::hex << probe->rxBit
        << " rxAccum " << std::hex << probe->rxAccum
        << " rxFinal " << std::hex << probe->rxFinal;
    return os.str();
}

int main(int argc, char** argv, char** env) {
    Verilated::commandArgs(argc, argv);
    Vmyspi top;
    SpiSender sender(0x55);
    std::cout << "start" << std::endl;
    unsigned int probe = 0;
    Probe * probePtr(reinterpret_cast<Probe *>(&top.probe));
    std::cout << sizeof(unsigned int) << std::endl;
    while (!Verilated::gotFinish())
    {
        if (main_time % 10 == 1) top.sysclk = 1;
        if (main_time % 10 == 6) top.sysclk = 0;
        if (!sender.eval(top, main_time))
        {
            break;
        }
        top.eval();
        if (probe != top.probe)
        {
            probe = top.probe;
            std::cout << dumpProbe(probePtr) << std::endl;
        }
        main_time++;
        //sleep(1);
    }
    exit(0);
}
