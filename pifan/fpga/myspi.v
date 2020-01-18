// Code your design here
module MySpi
  (
   // Control/Data Signals,
   input wire  sysclk,      // FPGA Clock
   output reg oRxReady,    // Data Valid pulse (1 clock cycle)
   output reg [7:0] oRx,  // Byte received on MOSI
//   input            iTxReady,    // Data Valid pulse to register i_TX_Byte
//   input  [7:0]     iTx,  // Byte to serialize to MISO.

   // SPI Interface
   input wire iSPIClk,  // External pin 22 (2)
   //output reg o_SPI_MISO, // External pin 21 (48)
//   output     oSPIMISO, // External pin 21 (48)
   input wire iSPIMOSI, // External pin 20 (45)
   input wire iSPICS, // External pin 19 (47)
   output reg[3:0] digit0,
   output reg[3:0] digit1,
   output reg[3:0] digit2,
   output reg[3:0] digit3
   );

reg [3:0] rxBit;
reg [7:0] rxAccum;
  
initial begin
    rxBit <= 0;
    oRxReady <= 1'b0;
    rxAccum <= 'h00;
end

always @(posedge sysclk)
begin
    digit0 <= rxAccum[3:0];
    digit1 <= rxAccum[7:4];
    digit2 <= rxBit;
end

always @(rxBit)
begin
    rxAccum <= {rxAccum[6:0], iSPIMOSI};
//    oRxReady <= rxBit[3];
//    if (rxBit[3]) begin
//        oRx <= rxAccum;
//    end
end

always @(posedge iSPIClk or posedge iSPICS)
begin
    if (iSPICS) 
        rxBit <= 0;
    else
        rxBit <= rxBit + 1;
end

endmodule // MySpi
