module MySpi
  (
   // Control/Data Signals,
	input reset,
   input sysclk,      // FPGA Clock
   output oRxReady,    // Data Valid pulse (1 clock cycle)
    output [7:0] oRx,  // Byte received on MOSI
   // SPI Interface
   input iSPIClk,  // External pin 22 (2)
   input iSPIMOSI, // External pin 20 (45)
   input iSPICS, // External pin 19 (47)
    output [7:0] probe
   );

reg [2:0] rxBit;
reg [7:0] rxAccum;
reg [7:0] rxFinal;
reg rxReady;

assign oRxReady = rxReady;
assign oRx = rxFinal;
//assign probe = {3'b000, iSPIClk, 1'b0, rxBit};
//assign probe = {3'b000, iSPICS, 1'b0,  rxBit}; 
assign probe = rxAccum;

always @(posedge iSPIClk or posedge iSPICS) // or negedge reset)
begin
   if (iSPICS)
	begin
		rxBit <= 0;
        rxReady <= 0;
	end else begin
		rxBit <= rxBit + 1;
		rxAccum <= {rxAccum[6:0], iSPIMOSI};
        if (rxBit == 3'b111)
        begin   
            rxFinal = {rxAccum[6:0], iSPIMOSI};
            rxReady = 1'b1;
        end
        else if (rxBit == 3'b010)
        begin
            rxReady = 1'b0;
        end
	end
end

endmodule // MySpi
