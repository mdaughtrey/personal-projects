#include "Vmyspi.h"
#include "verilated.h"
#include <iostream>
#include <algorithm>
#include <vector>

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

vluint64_t main_time = 0;
double sc_time_stamp()
{
    return main_time;
}

class SpiSender {
public:
    SpiSender(unsigned char);
    void eval(Vmyspi &, double);
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

void SpiSender::eval(Vmyspi & top, double main_time)
{
    if (0 == state)
    {
        top.iSPICS = 1;
        top.iSPIClk = 0;
    } else if (state == this->clkHigh.front() - 20)
    {
        top.iSPICs = 0;
    } else if (clkHigh.end() !=
        std::find(this->clkHigh.begin(), this->clkHigh.end(), state))
    {
        top.iSPIMOSI = (this->tosend >> sendbit) & 0x01;
        top.iSPICLK = 1;
        this->tosend--;
    } else if (clkLow.end() !=
        std::find(this->clkLow.begin(), this->clkLow.end(), state))
    {
        top.iSPICLK = 0;
        this.tosend--;
    } else if (state == this->clkLow.back() + 20)
    {
        top.iSPICs = 1;
    }
    if (this->state++ > this->clkLow.back() + 40)
    {
        return false;
    }
    this->state %= this->clkLow.back() + 40;
    return true;
}

int main(int argc, char** argv, char** env) {
    Verilated::commandArgs(argc, argv);
    Vmyspi* top = new Vmyspi;
    SpiSender * sender = new SpiSender(0x55);
    std::cout << "start" << std::endl;
    while (!Verilated::gotFinish())
    {
        if (main_time % 10 == 1) top->sysclk = 1;
        if (main_time % 10 == 6) top->sysclk = 0;
        sender->eval(top, main_time);
        
        top->eval(); }
//        std::cout << top->out << std::endl;
        main_time++;
    }
    top->final();
    delete top;
    exit(0);
}
