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

wire [0:0] spiRxReady;
reg [7:0] regTx = 8'b01010101;
//reg [0:0] regTxReady = 1;

reg [3:0] rxIndex = 0;
reg [7:0] memory [0:7];
wire [0:0] two_ms;
wire [0:0] one_s;
wire [7:0] spiRx;


//assign MYLED = spiRxReady;
reg[0:0] spiReset;
initial begin
	spiReset = 0;
end

always @(posedge WF_CLK)
begin
	if (spiReset); begin spiReset <= 1; end
end

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
    if (digit2 == 15)
    begin
    	digit3 <= digit3 + 1;
    end
	digit2 <= digit2 + 1;
end

//reg [0:0] rDiag;
//assign WF_LED = rDiag;
wire [3:0] diag;

spi_slave spiSlave(.reset(spiReset),
    .txEnable(1'b0),
    .txData(regTx),
    .msb_lsb(1'b0),
    .SPI_SS(SPI0_CS),
    .SPI_SCK(SPI0_CLK),
    .SPI_MOSI(SPI0_MOSI), 
    .SPI_MISO(SPI0_MISO),
    .done(spiRxReady),
    .rxData(spiRx),
	.diag(diag));

reg [3:0] digit0;    // BCD digits, 0 is LSD
reg [3:0] digit1;
reg [3:0] digit2 = 3;
reg [3:0] digit3 = 4;
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

always @(negedge SPI0_CS)
begin
    if (digit0 == 15)
    begin
    	digit1 <= digit1 + 1;
    end
	digit0 <= digit0 + 1;
//	digit1 <= spiRx[7:4];
end
//always @(posedge spiRxReady)
//begin
//	digit0 <= spiRx[3:0];
//	digit1 <= spiRx[7:4];
//end

// Reset index on new SPI transaction
//always @(posedge SPI0_CS)
//begin
//    newTransaction <= 1'b0;
//end
//
//// Read SPI data into memory
//always @(posedge regRxReady)
//begin
//    if (newTransaction)
//    begin
//        newTransaction <= 0;
//        rxIndex <= 0;
//    end
//    memory[rxIndex] <= spiRx;
//    rxIndex <= rxIndex + 1;
//end

//always @(rxIndex)
//begin
//    if ((rxIndex > 0) && (memory[0][7])) //  & 8'h80)) // high bit set means read op 
//    begin
//        regTx <= memory[rxIndex];
//        regTxReady <= 1;
//    end
//end
//


//    regTx <= spiRx;
//	digit0 <= spiRx[3:0];
//	digit1 <= spiRx[7:4];
//    memory[rxIndex] <= spiRx;
//    rxIndex <= rxIndex + 1;
//    regTx = rxIndex;
//    rxIndex <= rxIndex + 1'b1;
//    if ((rxIndex == 1) && memory[0][7]) // high bit set means read op
//    begin
//        regTx <= memory[1];
//        regTxReady <= 1'b1;
//    end

// Reset any high pulses
//always @(posedge WF_CLK)
//begin
//    if (spiTxReady)
//    begin
//        regTxReady <= 0;
//    end
//end

//   input            i_Rst_L,    // FPGA Reset
//   input            i_Clk,      // FPGA Clock
//   output reg       o_RX_DV,    // Data Valid pulse (1 clock cycle)
//   output reg [7:0] o_RX_Byte,  // Byte received on MOSI
//   input            i_TX_DV,    // Data Valid pulse to register i_TX_Byte
//   input  [7:0]     i_TX_Byte,  // Byte to serialize to MISO.
//
//   // SPI Interface
//   input      i_SPI_Clk,  // External pin 22 (2)
//   //output reg o_SPI_MISO, // External pin 21 (48)
//   output     o_SPI_MISO, // External pin 21 (48)
//   input      i_SPI_MOSI, // External pin 20 (45)
//   input      i_SPI_CS_n // External pin 19 (47)
endmodule

