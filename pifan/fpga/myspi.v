
module MySpi
  (
   // Control/Data Signals,
   input sysclk,      // FPGA Clock
   output oRxReady,    // Data Valid pulse (1 clock cycle)
   output [7:0] oRx,  // Byte received on MOSI
   input txReady,
    input [7:0] tx,
   // SPI Interface
   input iSPIClk,  // External pin 22 (2)
   input iSPIMOSI, // External pin 20 (45)
   output oSPIMISO, // 
   input iSPICS, // External pin 19 (47)
    output [7:0] probe
   );

reg [2:0] rxBit;
reg [7:0] rxAccum;
reg [7:0] rxFinal;
reg [7:0] txShift;
reg rxReady;
reg spiMiso;
  reg [3:0] misoState;

  assign probe = {1'b0, rxBit, misoState};
//  assign probe = {5'b000000, rxBit};
assign oRxReady = rxReady;
assign oRx = rxFinal;
assign oSPIMISO = spiMiso;
  
// Tx
  always @(posedge iSPIClk or posedge txReady or posedge iSPICS) //  or posedge sysclk)
begin
  if (iSPICS)
    misoState <= 0;
  else
    case (misoState)
      // Idle and waiting for txready
      0: if (txReady) 
        begin
          txShift <= tx;
          misoState <= 1;
        end
      // Loaded and waiting for sync with rx
      1: if ((rxBit == 0) && (iSPIClk))//  && (~iSPICS)) 
        begin
          spiMiso <= txShift & 1'b1;
          txShift <= txShift >> 1;
          misoState <= 2;
        end
      
      // Last bit sent
      9: misoState <= 0;
      
      // Sending bits
      default:
        if (iSPIClk) //  && (~iSPICS)) 
        begin
          spiMiso <= txShift & 1'b1;
          txShift <= txShift >> 1;
          misoState <= misoState + 1;
        end
    endcase
end
  
// Rx
always @(posedge iSPIClk or posedge iSPICS)
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
        rxFinal <= {rxAccum[6:0], iSPIMOSI};
        rxReady <= 1'b1;
      end
      else if (rxBit == 3'b010)
      begin
        rxReady <= 1'b0;
      end
    end
end

endmodule 
