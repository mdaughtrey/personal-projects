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

