// inputs are regs
// outputs are wires
module MySpi
  (
   // Control/Data Signals,
   input wire sysclk,      // FPGA Clock
   output wire oRxReady,    // Data Valid pulse (1 clock cycle)
   output wire [7:0] oRx,  // Byte received on MOSI
   input wire txReady,
   input wire [7:0] tx,
   // SPI Interface
   input wire iSPIClk,  // External pin 22 (2)
   input wire iSPIMOSI, // External pin 20 (45)
   output wire oSPIMISO, // 
   input wire iSPICS, // External pin 19 (47)
   output wire [15:0] probe
   );

reg [2:0] rxBit;
reg [7:0] rxAccum;
reg [7:0] rxFinal;
reg [7:0] txShift;
reg [7:0] txBuffer;
reg rxReady;
//reg spiMiso;
reg txEnable;
//reg txDone;
wire txDone;
reg [1:0] misoState;
reg [2:0] misoIndex;

//assign probe = {3'b0, rxReady, 3'b0, txReady, misoState, 1'b0, rxBit};
//jassign probe = {txDone, 2'b0,  txEnable, 3'b0, txReady, 1'b0, misoIndex, 1'b0, rxBit};
assign probe = {rxAccum, rxFinal};
assign oRxReady = rxReady;
assign oRx = rxFinal;
//assign oSPIMISO = spiMiso;

//assign iSPIMOSI = rxAccum[rxBit];

//always @(posedge rxReady) rxFinal <= rxAccum;

initial begin
    rxBit = 0;
    rxReady = 0;
    rxAccum = 0;
end

// Rx
always @(posedge iSPIClk or posedge iSPICS)
begin
    if (iSPICS)
    begin
        rxBit <= 0;
        rxReady <= 0;
        rxAccum <= 0;
    end else if (iSPIClk) begin
        if (rxBit == 3'd7) begin                    // if we're on the last bit
            rxBit <= 3'd0;
            rxFinal <= {rxAccum[6:0], iSPIMOSI};    
            rxReady <= 1'b1;                        // signal ready
        end else begin                              // else gathering bits
            rxBit <= rxBit + 1'b1;
            rxAccum <= {rxAccum[6:0], iSPIMOSI};
            rxReady <= 1'b0;
        end
    end
end

always @(posedge txReady or posedge sysclk)
begin
    if (txReady & !txEnable) begin
        txEnable <= 1'b1;
        //txShift <= tx;
        txBuffer <= tx;
    //end  else if (txEnable & txDone & misoState == 3'd0)
    end  else if (txEnable & txDone)
        txEnable <= 1'b0;
end

//assign oSPIMISO = txEnable ? txShift[misoState] : 1'bZ;
assign oSPIMISO = iSPICS ? 1'bZ : txShift[rxBit];
assign txDone = (!iSPICS & rxBit > 3'd5);
//assign txShift = txEnable ? txBuffer : 8'b0;
// tx

always @(posedge iSPIClk) //  or posedge iSPICS)
begin
//    if (iSPICS)
//    begin
//        misoIndex <= 3'd7;
//        misoState <= 0;
    if (rxBit == 3'b0) txShift <= txBuffer;
    //end else if (rxBit == 3'b0) txShift <= txBuffer;
//        csCount <= csCount + 1'b1;
//    end else begin
//        case (misoState)
//            //2'd0: if (rxBit == 3'b0 && txEnable)
//            2'd0: if (rxBit == 3'b0)
//            begin
//                //txShift <= txBuffer;
//                txShift <= 8'h55;
//                misoState <= 1;
//                misoIndex <= 3'd6;
//            end
//            2'd1: begin misoIndex <= misoIndex - 1; if (misoIndex == 3'd0) misoState <= 2'd2; end
//            2'd2: begin misoState <= 2'd3; end
//            2'd3: begin misoIndex <= 3'd7; misoState <= 2'd0; end
//        endcase
//    end
end

endmodule 
