
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
    output [15:0] probe
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
assign probe = {4'b0, txIndex, misoState, 1'b0, rxBit};
assign oRxReady = rxReady;
assign oRx = rxFinal;
assign oSPIMISO = spiMiso;
  
// Rx
always @(posedge iSPIClk or negedge iSPIClk or posedge iSPICS)
begin
    if (iSPICS)
    begin
        rxBit <= 0;
        rxReady <= 0;
        misoState <= 0;
        txIndex <= 0;
        spiMiso <= 0;
        txShift <= 0;
    end else begin
        // Rx handler
        if (iSPIClk) begin
            rxAccum <= {rxAccum[6:0], iSPIMOSI};
            if (rxBit == 3'b111) begin
                rxFinal <= {rxAccum[6:0], iSPIMOSI};
                rxReady <= 1'b1;
            end
            else if (rxBit == 3'b010) begin
                rxReady <= 1'b0;
            end
            // Tx handler
            case (misoState)
            // Idle and waiting for txready
            0: if (txReady) begin
                txShift <= tx;
                misoState <= 1;
            end
            1: if (!rxBit) begin
              //  spiMiso <= txShift[7];
                txIndex <= 6;
                misoState <= 2;
            end
            // Sending bits
            default: begin
                //spiMiso <= txShift[txIndex];
                misoState <= misoState + 1;
            end
            // Last bit sent
            9: misoState <= 0;
            endcase
    //      spiMiso <= txShift[0];
    //      txIndex <= 1;
    ////      txShift <= txShift >> 1;
        end
        else begin
            rxBit <= rxBit + 1;
           //if (txIndex) txIndex <= txIndex - 1;
        end
    end
end

endmodule 
