// @MAP_IO SPI0_CLK 22
// @MAP_IO SPI0_MISO 21
// @MAP_IO SPI0_MOSI 20
// @MAP_IO SPI0_CS 19
// @MAP_IO MYLED 14
// @MAP_IO CLK_7SEG 23
// @MAP_IO DOUT_7SEG 24
// @MAP_IO LOAD_7SEG 25

module fpga_top(
    input wire WF_CLK,
    input wire SPI0_CLK,
    input wire SPI0_MOSI,
    input wire SPI0_CS,
    output wire SPI0_MISO,
	output wire CLK_7SEG,
	output wire DOUT_7SEG,
	output wire LOAD_7SEG,
	output wire MYLED
);

// SPITEST
wire testclk;
wire testcs;
wire testmosi;
SpiTest spitest
  (
    .clock(two_ms),
   // SPI Interface
   .otestclk(testclk),
   .otestcs(testcs),
   .otestmosi(testmosi)
   );

// -----------------------------------------------------------------------------
// SPI
// -----------------------------------------------------------------------------
reg txReady;
//wire txReady;
//wire [7:0] tx;
reg [7:0] tx;

// Outputs are wires
wire[7:0] spiRx;
wire spiRxReady;
wire [15:0] probe;

reg [3:0] rxCount;

initial begin
    txReady = 0;
    rxCount = 0;
    tx = 0;
end

// spiRx is good
//assign spiRx[7:4] = digit1;
//assign spiRx[3:0] = digit0;
//assign spiRx[7:4] = digit1;
assign probe[15:12] = digit3;
assign probe[11:8] = digit2;
assign probe[7:4] = digit1;
assign probe[3:0] = digit0;

MySpi spi(
   // Control/Data Signals,
   .sysclk(WF_CLK),      
   .oRxReady(spiRxReady),
   .oRx(spiRx),
   .txReady(txReady),
   .tx(tx),

   // SPI Interface
   //.iSPIClk(SPI0_CLK),
   .iSPIClk(testclk),
   //.iSPIMOSI(SPI0_MOSI),
   .iSPIMOSI(testmosi),
   .oSPIMISO(SPI0_MISO),
   //.iSPICS(SPI0_CS),
   .iSPICS(testcs),
   .probe(probe)
   );

//assign txReady = spiRxReady;
//assign tx = 8'h55;
always @(posedge spiRxReady or posedge WF_CLK)
begin
    if (spiRxReady & !txReady)
    begin
//        tx <= spiRx + 1;
        tx <= 8'ha5;
        txReady <= 1'b1;
    end else if (WF_CLK & txReady)
    begin
        txReady <= 1'b0;
    end 
end

// DIGITS 3 2 1 0
// -----------------------------------------------------------------------------
// 7 segment display
// -----------------------------------------------------------------------------
initial begin
    digit0 = 0;
    digit1 = 0;
    digit2 = 0;
    digit3 = 0;
end

wire two_ms;
wire one_s;

// 2msec timer for 7 segment display
WF_timer #(.COUNT(31916), .EVERY_CLK(1'b1)) two_msec(
           .clk(WF_CLK),
           .enable(1'b1),
           .timer_pulse(two_ms));

WF_timer #(.COUNT(500), .EVERY_CLK(1'b1)) one_sec(
           .clk(two_ms),
           .enable(1'b1),
           .timer_pulse(one_s));

//kalways @(posedge one_s)
//kbegin
//k	digit3 <= digit3 + 1;
//kend

reg [3:0] digit0;    // BCD digits, 0 is LSB
reg [3:0] digit1;
reg [3:0] digit2;
reg [3:0] digit3;
reg [1:0] colon = 2'b00;   // 00 colon, 01 decpoint, 11 none

WF_BL_7seg_if seg7(
	.clk(WF_CLK),
	.scan_enable(two_ms),
	.digit0(digit0),    // BCD digits, 0 is LSD
	.digit1(digit1),
	.digit2(digit2),
	.digit3(digit3),
	.colon(colon),   // 00 colon, 01 decpoint, 11 none
	.CLK_OUT(CLK_7SEG),
	.LOAD(LOAD_7SEG),
	.DOUT(DOUT_7SEG)
);
endmodule // top

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
assign probe = {3'b0, iSPICS, 1'b0, rxBit, rxFinal};
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
        rxAccum <= 0;
    end else if (iSPIClk) begin
        if (rxBit == 3'd7) begin                    // if we're on the last bit
            rxFinal <= {rxAccum[6:0], iSPIMOSI};    
            rxReady <= 1'b1;                        // signal ready
            rxBit <= 3'd0;
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
module WF_timer (
    input  wire clk,
    input  wire enable,
    output reg  timer_pulse
);

parameter [16:0] COUNT = 499;
parameter       EVERY_CLK = 1'b0;

 reg [16:0] counter;

 always @ (posedge clk)
     if (enable)
       begin
         if (counter == COUNT)
           begin
             timer_pulse <= 1'b1;
             counter     <= 'b0;
           end
        else
          begin
           counter     <= counter + 'b1;
           if (EVERY_CLK)  // case enable never goes low
            timer_pulse <= 1'b0;
          end

       end
     else
         timer_pulse <= 1'b0;

endmodule
module WF_BL_7seg_if (

	input clk,
	input scan_enable,   // should be >60Hz, 2 ms or better

	input [3:0] digit0,    // BCD digits, 0 is LSD
	input [3:0] digit1,
	input [3:0] digit2,
	input [3:0] digit3,
	input [1:0] colon,   // 00 colon, 01 decpoint, 11 none

	output reg CLK_OUT,
	output     LOAD,
	output     DOUT
);

   reg [4:0] digit;    // rotating bit for which digit is being scanned
                       // scan enables (4 digits, one colon)
   reg       clk_en;   // during DOUT shifting
   reg [4:0] seg_index;
   reg [3:0] current_digit;
   reg [7:0] segments;
   reg [7:0] data_out; // segment data to be shifted

////////////////////////////////////////////////////////////////////////////
// send data out 16 clocks of data, { digits[3:0],3'b000,DIGDP, seg[7:0]}
// scan digits every scan_enable, 
// 4 digits, and colon digit.
//

// digit scaning 
     always @ (posedge clk)
       begin
         if (digit == 5'h0)     // just came out of reset
           digit <= 5'h01;

         if (clk_en && seg_index == 5'd31) // rotate at end of cycle
	   digit <= {digit[3:0],digit[4]};   // rotate the select bit
       end

// shift register bit counter
     always @ (posedge clk)
       begin
         if (scan_enable)
           begin
             clk_en      <= 1'b1;
             seg_index   <= 5'b0;
           end

         if (clk_en)
           begin
	     if (seg_index == 5'd31)   // check for last bit
               clk_en <= 1'b0;

	     seg_index <= seg_index + 5'h1;
           end

       end   // always clk enable


// send data out, shift register 
     always @ (posedge clk)
       if (clk_en )
         begin
           if (seg_index[0]) // only change on falling edge
	     begin
	       if(seg_index == 5'd15) // mid way, send digit enables
                 data_out <= {digit[4],3'b000,digit[3:0]};
               else
                 data_out <= {data_out[6:0],1'b0};
             end
         end
       else
         data_out<= (digit[4]) ? {6'h0,colon} : segments; // load for next cycle

// create external outputs to drive the board
     assign LOAD = ~clk_en;
     assign DOUT = data_out[7];
     // generate clock
     always @ (posedge clk)
       if (clk_en)
         CLK_OUT <= ~CLK_OUT;
       else
         CLK_OUT <= 1'b0;   // make sure it at zero at idle


////////////////////////////////////////////////////////////////////////////
     // encode current digit into segment data
  //   
   always @(*)    // pre-load one digit ahead
      case (digit[4:0])   // synthesis parallel_case
        5'h1 : current_digit = digit0;
        5'h2 : current_digit = digit1;
        5'h4 : current_digit = digit2;
        default: current_digit = digit3;   // 5'h8 case
     endcase
//
//         aaaa  
//       ff    bb
//       ff    bb
//       ff    bb
//         gggg  
//       ee    cc
//       ee    cc
//       ee    cc
//         dddd  
//
//         time colons:  top dot: seg aa
//         time colons:  bot dot: seg bb
//        
//        shift order is  {ee,  dd,  ff,  aa,     gg,  bb,  DP,cc}
//        note: DP does exist on this display, it has its own DIGDP 
//

////////////////////////////////////////////////////////////////////////////
//active low turns on segment LED
   always @(*)
      case (current_digit)// DP   g    f    e    d    c    b    a
        4'h0 : segments = {1'b1,1'b1,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0};
        4'h1 : segments = {1'b1,1'b1,1'b1,1'b1,1'b1,1'b0,1'b0,1'b1};
        4'h2 : segments = {1'b1,1'b0,1'b1,1'b0,1'b0,1'b1,1'b0,1'b0};
        4'h3 : segments = {1'b1,1'b0,1'b1,1'b1,1'b0,1'b0,1'b0,1'b0};
        4'h4 : segments = {1'b1,1'b0,1'b0,1'b1,1'b1,1'b0,1'b0,1'b1};
        4'h5 : segments = {1'b1,1'b0,1'b0,1'b1,1'b0,1'b0,1'b1,1'b0};
        4'h6 : segments = {1'b1,1'b0,1'b0,1'b0,1'b0,1'b0,1'b1,1'b1};
        4'h7 : segments = {1'b1,1'b1,1'b1,1'b1,1'b1,1'b0,1'b0,1'b0};
        4'h8 : segments = {1'b1,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0};
        4'h9 : segments = {1'b1,1'b0,1'b0,1'b1,1'b1,1'b0,1'b0,1'b0};
        4'ha : segments = {1'b1,1'b0,1'b0,1'b0,1'b1,1'b0,1'b0,1'b0};
        4'hb : segments = {1'b1,1'b0,1'b0,1'b0,1'b0,1'b0,1'b1,1'b1};
        4'hc : segments = {1'b1,1'b0,1'b1,1'b0,1'b0,1'b1,1'b1,1'b1};
        4'hd : segments = {1'b1,1'b0,1'b1,1'b0,1'b0,1'b0,1'b0,1'b1};
        4'he : segments = {1'b1,1'b0,1'b0,1'b0,1'b0,1'b1,1'b1,1'b0};
        4'hf : segments = {1'b1,1'b0,1'b0,1'b0,1'b1,1'b1,1'b1,1'b0};
     endcase

endmodule

module SpiTest
  (
    input wire clock,
   // SPI Interface
   output wire otestclk,
   output wire otestcs,
   output wire otestmosi
   );

reg[4:0] state;
reg testclk;
reg testcs;
reg testmosi;
reg [7:0] testdata;

assign otestclk = testclk;
assign otestcs = testcs;
assign otestmosi = testmosi;

initial begin
    state = 5'd0;
    testclk = 1'b0;
    testcs = 1'b1;
    testmosi = 1'b0;
    testdata = 8'h55;
end

always @(posedge clock)
begin
    case (state)
    5'd0: testcs <= 1'b0;

    5'd1: testmosi <= testdata[7]; // 0
    5'd2: testclk <= 1'b1;  // 0

    5'd3: begin testclk <= 1'b0;  testmosi <= testdata[6]; end 
    5'd4: testclk <= 1'b1;

    5'd5: begin testclk <= 1'b0;  testmosi <= testdata[5]; end 
    5'd6: testclk <= 1'b1;

    5'd7: begin testclk <= 1'b0;  testmosi <= testdata[4]; end 
    5'd8: testclk <= 1'b1;

    5'd9: begin testclk <= 1'b0;  testmosi <= testdata[3]; end 
    5'd10: testclk <= 1'b1;

    5'd11: begin testclk <= 1'b0;  testmosi <= testdata[2]; end 
    5'd12: testclk <= 1'b1;

    5'd13: begin testclk <= 1'b0;  testmosi <= testdata[1]; end 
    5'd14: testclk <= 1'b1;

    5'd15: begin testclk <= 1'b0;  testmosi <= testdata[0]; end 
    5'd16: testclk <= 1'b1;
    5'd17: testclk <= 1'b0; 

//    5'd26: testcs <= 1'b1;

//    default: state <= 5'd1; //  testdata <= testdata + 1; end
//    default: begin state <= 0; testdata <= testdata + 1; end

    endcase
    state <= state + 1'b1;
    if (state > 17) state <= 5'd1;
end

endmodule
