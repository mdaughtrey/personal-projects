// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vmyspi.h for the primary calling header

#include "Vmyspi.h"
#include "Vmyspi__Syms.h"

//==========

VL_CTOR_IMP(Vmyspi) {
    Vmyspi__Syms* __restrict vlSymsp = __VlSymsp = new Vmyspi__Syms(this, name());
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Reset internal values
    
    // Reset structure values
    _ctor_var_reset();
}

void Vmyspi::__Vconfigure(Vmyspi__Syms* vlSymsp, bool first) {
    if (0 && first) {}  // Prevent unused
    this->__VlSymsp = vlSymsp;
}

Vmyspi::~Vmyspi() {
    delete __VlSymsp; __VlSymsp=NULL;
}

void Vmyspi::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vmyspi::eval\n"); );
    Vmyspi__Syms* __restrict vlSymsp = this->__VlSymsp;  // Setup global symbol table
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
#ifdef VL_DEBUG
    // Debug assertions
    _eval_debug_assertions();
#endif  // VL_DEBUG
    // Initialize
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) _eval_initial_loop(vlSymsp);
    // Evaluate till stable
    int __VclockLoop = 0;
    QData __Vchange = 1;
    do {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Clock loop\n"););
        _eval(vlSymsp);
        if (VL_UNLIKELY(++__VclockLoop > 100)) {
            // About to fail, so enable debug to see what's not settling.
            // Note you must run make with OPT=-DVL_DEBUG for debug prints.
            int __Vsaved_debug = Verilated::debug();
            Verilated::debug(1);
            __Vchange = _change_request(vlSymsp);
            Verilated::debug(__Vsaved_debug);
            VL_FATAL_MT("myspi.v", 3, "",
                "Verilated model didn't converge\n"
                "- See DIDNOTCONVERGE in the Verilator manual");
        } else {
            __Vchange = _change_request(vlSymsp);
        }
    } while (VL_UNLIKELY(__Vchange));
}

void Vmyspi::_eval_initial_loop(Vmyspi__Syms* __restrict vlSymsp) {
    vlSymsp->__Vm_didInit = true;
    _eval_initial(vlSymsp);
    // Evaluate till stable
    int __VclockLoop = 0;
    QData __Vchange = 1;
    do {
        _eval_settle(vlSymsp);
        _eval(vlSymsp);
        if (VL_UNLIKELY(++__VclockLoop > 100)) {
            // About to fail, so enable debug to see what's not settling.
            // Note you must run make with OPT=-DVL_DEBUG for debug prints.
            int __Vsaved_debug = Verilated::debug();
            Verilated::debug(1);
            __Vchange = _change_request(vlSymsp);
            Verilated::debug(__Vsaved_debug);
            VL_FATAL_MT("myspi.v", 3, "",
                "Verilated model didn't DC converge\n"
                "- See DIDNOTCONVERGE in the Verilator manual");
        } else {
            __Vchange = _change_request(vlSymsp);
        }
    } while (VL_UNLIKELY(__Vchange));
}

VL_INLINE_OPT void Vmyspi::_sequent__TOP__1(Vmyspi__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_sequent__TOP__1\n"); );
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->__Vdly__MySpi__DOT__txEnable = vlTOPp->MySpi__DOT__txEnable;
}

VL_INLINE_OPT void Vmyspi::_sequent__TOP__2(Vmyspi__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_sequent__TOP__2\n"); );
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    if ((0U == (IData)(vlTOPp->MySpi__DOT__rxBit))) {
        vlTOPp->MySpi__DOT__txShift = vlTOPp->MySpi__DOT__txBuffer;
    }
}

VL_INLINE_OPT void Vmyspi::_sequent__TOP__3(Vmyspi__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_sequent__TOP__3\n"); );
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Variables
    CData/*7:0*/ __Vdly__MySpi__DOT__rxAccum;
    // Body
    __Vdly__MySpi__DOT__rxAccum = vlTOPp->MySpi__DOT__rxAccum;
    vlTOPp->__Vdly__MySpi__DOT__rxBit = vlTOPp->MySpi__DOT__rxBit;
    if (vlTOPp->iSPICS) {
        vlTOPp->__Vdly__MySpi__DOT__rxBit = 0U;
        vlTOPp->MySpi__DOT__rxReady = 0U;
        __Vdly__MySpi__DOT__rxAccum = 0U;
        __Vdly__MySpi__DOT__rxAccum = 0U;
    } else {
        if (vlTOPp->iSPIClk) {
            if ((7U == (IData)(vlTOPp->MySpi__DOT__rxBit))) {
                vlTOPp->MySpi__DOT__rxFinal = ((0xfeU 
                                                & ((IData)(vlTOPp->MySpi__DOT__rxAccum) 
                                                   << 1U)) 
                                               | (IData)(vlTOPp->iSPIMOSI));
                vlTOPp->MySpi__DOT__rxReady = 1U;
                vlTOPp->__Vdly__MySpi__DOT__rxBit = 0U;
            } else {
                vlTOPp->__Vdly__MySpi__DOT__rxBit = 
                    (7U & ((IData)(1U) + (IData)(vlTOPp->MySpi__DOT__rxBit)));
                __Vdly__MySpi__DOT__rxAccum = ((0xfeU 
                                                & ((IData)(vlTOPp->MySpi__DOT__rxAccum) 
                                                   << 1U)) 
                                               | (IData)(vlTOPp->iSPIMOSI));
                vlTOPp->MySpi__DOT__rxReady = 0U;
            }
        }
    }
    vlTOPp->MySpi__DOT__rxAccum = __Vdly__MySpi__DOT__rxAccum;
    vlTOPp->oRxReady = vlTOPp->MySpi__DOT__rxReady;
    vlTOPp->oRx = vlTOPp->MySpi__DOT__rxFinal;
}

void Vmyspi::_settle__TOP__4(Vmyspi__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_settle__TOP__4\n"); );
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->oRx = vlTOPp->MySpi__DOT__rxFinal;
}

void Vmyspi::_initial__TOP__5(Vmyspi__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_initial__TOP__5\n"); );
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->MySpi__DOT__rxBit = 0U;
    vlTOPp->MySpi__DOT__rxReady = 0U;
    vlTOPp->MySpi__DOT__rxAccum = 0U;
}

VL_INLINE_OPT void Vmyspi::_sequent__TOP__6(Vmyspi__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_sequent__TOP__6\n"); );
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    if (((IData)(vlTOPp->txReady) & (~ (IData)(vlTOPp->MySpi__DOT__txEnable)))) {
        vlTOPp->__Vdly__MySpi__DOT__txEnable = 1U;
        vlTOPp->MySpi__DOT__txBuffer = vlTOPp->tx;
    } else {
        if (((IData)(vlTOPp->MySpi__DOT__txEnable) 
             & ((~ (IData)(vlTOPp->iSPICS)) & (5U < (IData)(vlTOPp->MySpi__DOT__rxBit))))) {
            vlTOPp->__Vdly__MySpi__DOT__txEnable = 0U;
        }
    }
    vlTOPp->MySpi__DOT__txEnable = vlTOPp->__Vdly__MySpi__DOT__txEnable;
}

void Vmyspi::_settle__TOP__7(Vmyspi__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_settle__TOP__7\n"); );
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->oRxReady = vlTOPp->MySpi__DOT__rxReady;
    vlTOPp->oSPIMISO = (1U & ((((~ (IData)(vlTOPp->iSPICS)) 
                                & ((IData)(vlTOPp->MySpi__DOT__txShift) 
                                   >> (IData)(vlTOPp->MySpi__DOT__rxBit))) 
                               & (~ (IData)(vlTOPp->iSPICS))) 
                              & (~ (IData)(vlTOPp->iSPICS))));
    vlTOPp->probe = (((IData)(vlTOPp->MySpi__DOT__rxBit) 
                      << 0x10U) | (((IData)(vlTOPp->MySpi__DOT__rxAccum) 
                                    << 8U) | (IData)(vlTOPp->MySpi__DOT__rxFinal)));
}

VL_INLINE_OPT void Vmyspi::_sequent__TOP__8(Vmyspi__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_sequent__TOP__8\n"); );
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->MySpi__DOT__rxBit = vlTOPp->__Vdly__MySpi__DOT__rxBit;
    vlTOPp->probe = (((IData)(vlTOPp->MySpi__DOT__rxBit) 
                      << 0x10U) | (((IData)(vlTOPp->MySpi__DOT__rxAccum) 
                                    << 8U) | (IData)(vlTOPp->MySpi__DOT__rxFinal)));
}

VL_INLINE_OPT void Vmyspi::_combo__TOP__9(Vmyspi__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_combo__TOP__9\n"); );
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->oSPIMISO = (1U & ((((~ (IData)(vlTOPp->iSPICS)) 
                                & ((IData)(vlTOPp->MySpi__DOT__txShift) 
                                   >> (IData)(vlTOPp->MySpi__DOT__rxBit))) 
                               & (~ (IData)(vlTOPp->iSPICS))) 
                              & (~ (IData)(vlTOPp->iSPICS))));
}

void Vmyspi::_eval(Vmyspi__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_eval\n"); );
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    if ((((IData)(vlTOPp->sysclk) & (~ (IData)(vlTOPp->__Vclklast__TOP__sysclk))) 
         | ((IData)(vlTOPp->txReady) & (~ (IData)(vlTOPp->__Vclklast__TOP__txReady))))) {
        vlTOPp->_sequent__TOP__1(vlSymsp);
    }
    if (((IData)(vlTOPp->iSPIClk) & (~ (IData)(vlTOPp->__Vclklast__TOP__iSPIClk)))) {
        vlTOPp->_sequent__TOP__2(vlSymsp);
    }
    if ((((IData)(vlTOPp->iSPICS) & (~ (IData)(vlTOPp->__Vclklast__TOP__iSPICS))) 
         | ((IData)(vlTOPp->iSPIClk) & (~ (IData)(vlTOPp->__Vclklast__TOP__iSPIClk))))) {
        vlTOPp->_sequent__TOP__3(vlSymsp);
    }
    if ((((IData)(vlTOPp->sysclk) & (~ (IData)(vlTOPp->__Vclklast__TOP__sysclk))) 
         | ((IData)(vlTOPp->txReady) & (~ (IData)(vlTOPp->__Vclklast__TOP__txReady))))) {
        vlTOPp->_sequent__TOP__6(vlSymsp);
    }
    if ((((IData)(vlTOPp->iSPICS) & (~ (IData)(vlTOPp->__Vclklast__TOP__iSPICS))) 
         | ((IData)(vlTOPp->iSPIClk) & (~ (IData)(vlTOPp->__Vclklast__TOP__iSPIClk))))) {
        vlTOPp->_sequent__TOP__8(vlSymsp);
    }
    vlTOPp->_combo__TOP__9(vlSymsp);
    // Final
    vlTOPp->__Vclklast__TOP__sysclk = vlTOPp->sysclk;
    vlTOPp->__Vclklast__TOP__txReady = vlTOPp->txReady;
    vlTOPp->__Vclklast__TOP__iSPIClk = vlTOPp->iSPIClk;
    vlTOPp->__Vclklast__TOP__iSPICS = vlTOPp->iSPICS;
}

void Vmyspi::_eval_initial(Vmyspi__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_eval_initial\n"); );
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->__Vclklast__TOP__sysclk = vlTOPp->sysclk;
    vlTOPp->__Vclklast__TOP__txReady = vlTOPp->txReady;
    vlTOPp->__Vclklast__TOP__iSPIClk = vlTOPp->iSPIClk;
    vlTOPp->__Vclklast__TOP__iSPICS = vlTOPp->iSPICS;
    vlTOPp->_initial__TOP__5(vlSymsp);
}

void Vmyspi::final() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::final\n"); );
    // Variables
    Vmyspi__Syms* __restrict vlSymsp = this->__VlSymsp;
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
}

void Vmyspi::_eval_settle(Vmyspi__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_eval_settle\n"); );
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->_settle__TOP__4(vlSymsp);
    vlTOPp->_settle__TOP__7(vlSymsp);
}

VL_INLINE_OPT QData Vmyspi::_change_request(Vmyspi__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_change_request\n"); );
    Vmyspi* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    // Change detection
    QData __req = false;  // Logically a bool
    return __req;
}

#ifdef VL_DEBUG
void Vmyspi::_eval_debug_assertions() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((sysclk & 0xfeU))) {
        Verilated::overWidthError("sysclk");}
    if (VL_UNLIKELY((txReady & 0xfeU))) {
        Verilated::overWidthError("txReady");}
    if (VL_UNLIKELY((iSPIClk & 0xfeU))) {
        Verilated::overWidthError("iSPIClk");}
    if (VL_UNLIKELY((iSPIMOSI & 0xfeU))) {
        Verilated::overWidthError("iSPIMOSI");}
    if (VL_UNLIKELY((iSPICS & 0xfeU))) {
        Verilated::overWidthError("iSPICS");}
}
#endif  // VL_DEBUG

void Vmyspi::_ctor_var_reset() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vmyspi::_ctor_var_reset\n"); );
    // Body
    sysclk = VL_RAND_RESET_I(1);
    oRxReady = VL_RAND_RESET_I(1);
    oRx = VL_RAND_RESET_I(8);
    txReady = VL_RAND_RESET_I(1);
    tx = VL_RAND_RESET_I(8);
    iSPIClk = VL_RAND_RESET_I(1);
    iSPIMOSI = VL_RAND_RESET_I(1);
    oSPIMISO = VL_RAND_RESET_I(1);
    iSPICS = VL_RAND_RESET_I(1);
    probe = VL_RAND_RESET_I(24);
    MySpi__DOT__rxBit = VL_RAND_RESET_I(3);
    MySpi__DOT__rxAccum = VL_RAND_RESET_I(8);
    MySpi__DOT__rxFinal = VL_RAND_RESET_I(8);
    MySpi__DOT__txShift = VL_RAND_RESET_I(8);
    MySpi__DOT__txBuffer = VL_RAND_RESET_I(8);
    MySpi__DOT__rxReady = VL_RAND_RESET_I(1);
    MySpi__DOT__txEnable = VL_RAND_RESET_I(1);
    __Vdly__MySpi__DOT__rxBit = VL_RAND_RESET_I(3);
    __Vdly__MySpi__DOT__txEnable = VL_RAND_RESET_I(1);
}
