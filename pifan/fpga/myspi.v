
// inputs are regs
// outputs are wires
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
reg [2:0] txIndex;

//assign probe = {5'b0, rxBit};
//assign probe = rxAccum;
//assign probe = {1'b0, rxBit, misoState};
assign probe = {3'b0, txReady, misoState};
assign oRxReady = rxReady;
assign oRx = rxFinal;
assign oSPIMISO = spiMiso;
  
// Tx
//always @(posedge iSPIClk or posedge txReady or posedge iSPICS) //  or posedge sysclk)
always @(posedge iSPIClk or negedge iSPIClk or posedge txReady or posedge iSPICS) //  or posedge sysclk)
begin
  if (iSPICS)
      begin
    misoState <= 0;
    txIndex <= 0;
    end
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
          spiMiso <= txShift[0];
          txIndex <= 1;
//          txShift <= txShift >> 1;
          misoState <= 2;
        end
      
      // Last bit sent
      9: misoState <= 0;
      
      // Sending bits
      default: if (iSPIClk) //  && (~iSPICS)) 
        begin
          spiMiso <= txShift[txIndex];
          misoState <= misoState + 1;
          end else begin
          txIndex <= txIndex + 1;
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
