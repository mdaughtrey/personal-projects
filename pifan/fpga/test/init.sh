cat >our.v <<'EOF'
  module our;
     initial begin $display("Hello World"); $finish; end
  endmodule
EOF

cat >sim_main.cpp <<'EOF'
  #include "Vour.h"
  #include "verilated.h"
  int main(int argc, char** argv, char** env) {
      Verilated::commandArgs(argc, argv);
      Vour* top = new Vour;
      while (!Verilated::gotFinish()) { top->eval(); }
      delete top;
      exit(0);
  }
EOF
