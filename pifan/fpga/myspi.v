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

reg [3:0] rxBit;
reg [7:0] rxAccum;

assign oRxReady = rxBit[3];
assign oRx = rxAccum;
//assign probe = {7'b0000000, oRxReady};
//assign probe = rxAccum;

always @(posedge iSPIClk or posedge iSPICS or negedge reset)
begin
	if (!reset) begin
		rxBit <= 0;
		rxAccum <= 0;
	end else begin
		if (iSPICS)
		begin
			rxBit <= 0;
		end else begin
		  if (rxBit < 8) begin
			rxBit <= rxBit + 1;
			rxAccum <= {rxAccum[6:0], iSPIMOSI};
		  end else begin
			rxBit <= 0;
		  end
		end
	end
end

endmodule // MySpi
