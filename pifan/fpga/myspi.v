
module MySpi
  (
   // Control/Data Signals,
   input wire  iClk,      // FPGA Clock
   output reg oRxReady,    // Data Valid pulse (1 clock cycle)
   output reg [7:0] oRx,  // Byte received on MOSI
//   input            iTxReady,    // Data Valid pulse to register i_TX_Byte
//   input  [7:0]     iTx,  // Byte to serialize to MISO.

   // SPI Interface
   input wire iSPIClk,  // External pin 22 (2)
   //output reg o_SPI_MISO, // External pin 21 (48)
//   output     oSPIMISO, // External pin 21 (48)
   input wire iSPIMOSI, // External pin 20 (45)
   input wire iSPICS // External pin 19 (47)
   );

reg [2:0] rxBit;
reg [7:0] rxAccum;

always @(posedge iSPIClk, negedge iSPICS)
begin
    if (iSPICS) 
    begin
        rxBit <= 3'b0;
    end else begin
        if (iSPIClk)
        begin
            rxAccum <= {rxAccum[7:1], iSPIMOSI};
            if (rxBit == 3'b111)
            begin 
                oRx <= rxAccum;
                oRxReady <= 1'b1;
            end else begin
                oRxReady <= 1'b0;
            end
            rxBit <= rxBit + 1;
        end
    end
end

endmodule // MySpi
