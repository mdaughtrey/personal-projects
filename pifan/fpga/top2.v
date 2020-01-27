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
    output reg SPI0_MISO,
	output wire CLK_7SEG,
	output wire DOUT_7SEG,
	output wire LOAD_7SEG,
	output wire MYLED
);

// -----------------------------------------------------------------------------
// SPI
// -----------------------------------------------------------------------------
// Inputs are regs
reg reset;
reg [3:0] rsmState;

// Outputs are wires
wire[7:0] spiRx;
wire spiRxReady;
wire [7:0] probe;

initial begin
	reset <= 1;
	rsmState <= 0;
end

assign spiRx[7:4] = digit1;
assign spiRx[3:0] = digit0;

//always @(probe)
//always @(posedge spiRxReady)
//begin
//    digit2 <= probe;
//	digit2 <= digit2 + 1;
//	digit0 <= spiRx[3:0];
//    digit1 <= spiRx[7:4];
//end

// Reset state machine
//always @(posedge two_ms)
//begin
//	case (rsmState)
//	0: rsmState <= rsmState + 1;
//	1: 	begin
//			reset <= 0;
//			rsmState <= rsmState + 1;
//		end
//	2: 	begin
//			reset <= 1;
//			rsmState <= rsmState + 1;
//		end
//	default: rsmState <= 3;
//	endcase
//end

//wire[3:0] wdigit0;
//wire[3:0] wdigit1;
//wire[3:0] wdigit2;
//wire[3:0] wdigit3;

MySpi spi(
   // Control/Data Signals,
	.reset(reset),
   .sysclk(WF_CLK),      
   .oRxReady(spiRxReady),
   .oRx(spiRx),

   // SPI Interface
   .iSPIClk(SPI0_CLK),
   .iSPIMOSI(SPI0_MOSI),
   .iSPICS(SPI0_CS),
   .probe(probe)
   );

//always @(wdigit0) digit0 <= wdigit0;
//always @(wdigit1) digit1 <= wdigit1;
//always @(wdigit2) digit2 <= wdigit2;
//always @(wdigit3) digit3 <= wdigit3;

// DIGITS 3 2 1 0

// -----------------------------------------------------------------------------
// 7 segment display
// -----------------------------------------------------------------------------
initial begin
    digit0 <= 0;
    digit1 <= 0;
    digit2 <= 0;
    digit3 <= 0;
end

//wire [0:0] two_ms;
//wire [0:0] one_s;
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

always @(posedge one_s)
begin
//    if (digit2 == 15)
//    begin
//    	digit3 <= digit3 + 1;
//    end
	digit3 <= digit3 + 1;
end

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

