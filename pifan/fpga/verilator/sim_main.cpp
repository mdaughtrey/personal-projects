#include "Vmyspi.h"
#include "verilated.h"
#include <iostream>

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
    void eval(Vmyspi *, double);
private:
    int state;
    unsigned char tosend;
};

SpiSender::SpiSender(unsigned char _tosend)
{
    this->tosend = tosend;
}

void SpiSender::eval(Vmyspi * top, double main_time)
{
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
