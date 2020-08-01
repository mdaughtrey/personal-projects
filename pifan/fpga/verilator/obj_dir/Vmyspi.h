// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Primary design header
//
// This header should be included by all source files instantiating the design.
// The class here is then constructed to instantiate the design.
// See the Verilator manual for examples.

#ifndef _VMYSPI_H_
#define _VMYSPI_H_  // guard

#include "verilated.h"

//==========

class Vmyspi__Syms;

//----------

VL_MODULE(Vmyspi) {
  public:
    
    // PORTS
    // The application code writes and reads these signals to
    // propagate new values into/out from the Verilated model.
    VL_IN8(sysclk,0,0);
    VL_IN8(txReady,0,0);
    VL_IN8(iSPIClk,0,0);
    VL_IN8(iSPICS,0,0);
    VL_OUT8(oRxReady,0,0);
    VL_OUT8(oRx,7,0);
    VL_IN8(tx,7,0);
    VL_IN8(iSPIMOSI,0,0);
    VL_OUT8(oSPIMISO,0,0);
    VL_OUT(probe,23,0);
    
    // LOCAL SIGNALS
    // Internals; generally not touched by application code
    CData/*2:0*/ MySpi__DOT__rxBit;
    CData/*7:0*/ MySpi__DOT__rxAccum;
    CData/*7:0*/ MySpi__DOT__rxFinal;
    CData/*7:0*/ MySpi__DOT__txShift;
    CData/*7:0*/ MySpi__DOT__txBuffer;
    CData/*0:0*/ MySpi__DOT__rxReady;
    CData/*0:0*/ MySpi__DOT__txEnable;
    
    // LOCAL VARIABLES
    // Internals; generally not touched by application code
    CData/*2:0*/ __Vdly__MySpi__DOT__rxBit;
    CData/*0:0*/ __Vdly__MySpi__DOT__txEnable;
    CData/*0:0*/ __Vclklast__TOP__sysclk;
    CData/*0:0*/ __Vclklast__TOP__txReady;
    CData/*0:0*/ __Vclklast__TOP__iSPIClk;
    CData/*0:0*/ __Vclklast__TOP__iSPICS;
    
    // INTERNAL VARIABLES
    // Internals; generally not touched by application code
    Vmyspi__Syms* __VlSymsp;  // Symbol table
    
    // CONSTRUCTORS
  private:
    VL_UNCOPYABLE(Vmyspi);  ///< Copying not allowed
  public:
    /// Construct the model; called by application code
    /// The special name  may be used to make a wrapper with a
    /// single model invisible with respect to DPI scope names.
    Vmyspi(const char* name = "TOP");
    /// Destroy the model; called (often implicitly) by application code
    ~Vmyspi();
    
    // API METHODS
    /// Evaluate the model.  Application must call when inputs change.
    void eval() { eval_step(); }
    /// Evaluate when calling multiple units/models per time step.
    void eval_step();
    /// Evaluate at end of a timestep for tracing, when using eval_step().
    /// Application must call after all eval() and before time changes.
    void eval_end_step() {}
    /// Simulation complete, run final blocks.  Application must call on completion.
    void final();
    
    // INTERNAL METHODS
  private:
    static void _eval_initial_loop(Vmyspi__Syms* __restrict vlSymsp);
  public:
    void __Vconfigure(Vmyspi__Syms* symsp, bool first);
  private:
    static QData _change_request(Vmyspi__Syms* __restrict vlSymsp);
  public:
    static void _combo__TOP__9(Vmyspi__Syms* __restrict vlSymsp);
  private:
    void _ctor_var_reset() VL_ATTR_COLD;
  public:
    static void _eval(Vmyspi__Syms* __restrict vlSymsp);
  private:
#ifdef VL_DEBUG
    void _eval_debug_assertions();
#endif  // VL_DEBUG
  public:
    static void _eval_initial(Vmyspi__Syms* __restrict vlSymsp) VL_ATTR_COLD;
    static void _eval_settle(Vmyspi__Syms* __restrict vlSymsp) VL_ATTR_COLD;
    static void _initial__TOP__5(Vmyspi__Syms* __restrict vlSymsp) VL_ATTR_COLD;
    static void _sequent__TOP__1(Vmyspi__Syms* __restrict vlSymsp);
    static void _sequent__TOP__2(Vmyspi__Syms* __restrict vlSymsp);
    static void _sequent__TOP__3(Vmyspi__Syms* __restrict vlSymsp);
    static void _sequent__TOP__6(Vmyspi__Syms* __restrict vlSymsp);
    static void _sequent__TOP__8(Vmyspi__Syms* __restrict vlSymsp);
    static void _settle__TOP__4(Vmyspi__Syms* __restrict vlSymsp) VL_ATTR_COLD;
    static void _settle__TOP__7(Vmyspi__Syms* __restrict vlSymsp) VL_ATTR_COLD;
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);

//----------


#endif  // guard
