
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
reg rxReady;
reg spiMiso;
reg [3:0] misoState;
//reg [2:0] txIndex;

//assign probe = {5'b0, rxBit};
//assign probe = rxAccum;
//assign probe = {1'b0, rxBit, misoState};
assign probe = {rxFinal, misoState, 1'b0, rxBit};
//assign probe = {4'b0,  misoState, txShift};
assign oRxReady = rxReady;
assign oRx = rxFinal;
assign oSPIMISO = spiMiso;

//jinitial begin
//j    spiMiso <= 0;
//jend
  
//always @(posedge sysclk)
//begin
//    if (iSPIClk)
//    spiMiso <= !spiMiso;
//end
//always @(posedge iSPIClk)
//begin
//    if (iSPICS) spiMiso <= 0;
//    else begin
//        spiMiso <= !spiMiso;
//        rxBit <= rxBit + 1;
//end
//end
// Rx
always @(posedge iSPIClk or posedge iSPICS)
begin
    if (iSPICS)
    begin
        rxBit <= 0;
        rxReady <= 0;
    end else  begin
        rxBit <= rxBit + 1;
        rxAccum <= {rxAccum[6:0], iSPIMOSI};
        if (rxBit == 3'b111) begin
            rxFinal <= {rxAccum[6:0], iSPIMOSI};
            rxReady <= 1;
        end
        else if (rxBit == 3'b010) begin
            rxReady <= 0;
        end 
    end
end

// tx
always @(posedge iSPIClk or posedge iSPICS)
begin
    if (iSPICS)
    begin
        misoState <= 8;
        spiMiso <= 0;
        txShift <= 0;
    end else begin
            // Tx handler
            case (misoState)
            // Idle and waiting for txready
            8: if (txReady) begin
            //4'b1000: begin
//                txShift <= tx;
                misoState <= 7;
            end
            7: if (!rxBit) begin
 //               spiMiso <= txShift[7];
//                spiMiso <= 1'b1;
                misoState <= 6;
            end

            6: begin
//                spiMiso <= txShift[6];
//                spiMiso <= 1'b0;
                misoState <= 5;
            end
            5: begin
//                spiMiso <= txShift[5];
//                spiMiso <= 1'b1;
                misoState <= 4;
            end
            4: begin
//                spiMiso <= txShift[4];
//                spiMiso <= 1'b0;
                misoState <= 3;
            end
            3: begin
//                spiMiso <= txShift[3];
//                spiMiso <= 1'b1;
                misoState <= 2;
            end
            2: begin
//                spiMiso <= txShift[2];
//                spiMiso <= 1'b0;
                misoState <= 1;
            end
            1: begin
//                spiMiso <= txShift[1];
                misoState <= 0;
            end
            0: begin
 //               spiMiso <= txShift[0];
//                misoState <= 8;
            end
            endcase
    end
end

endmodule 
