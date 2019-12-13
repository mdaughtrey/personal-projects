// @MAP_IO SPI0_CLK 22
// @MAP_IO SPI0_MISO 21
// @MAP_IO SPI0_MOSI 20
// @MAP_IO SPI0_CS 19
// @MAP_IO MYLED 14

module fpga_top(
    input wire WF_CLK,
    input wire SPI0_CLK,
    input wire SPI0_MOSI,
    input wire SPI0_CS,
    output wire SPI0_MISO,
    output wire MYLED
);

wire spireset = 1'b1;
wire [0:0] spiRxReady;
wire [7:0] spiRx;
//wire [0:0] spiTxReady;
//wire [7:0] spiTx;
reg [7:0] regRx;
reg [7:0] regTx = 8'b11110000;
//assign spiTx = regTx;
reg [0:0] regTxReady = 1;
reg [0:0] regRxReady = 0;
//assign spiTxReady = regTxReady;


SPI_Slave spi1(.i_Rst_L(spireset), .i_Clk(WF_CLK),
    .o_RX_DV(spiRxReady), .o_RX_Byte(spiRx),
    .i_TX_DV(regTxReady), .i_TX_Byte(regTx),
    .i_SPI_Clk(SPI0_CLK),
    .o_SPI_MISO(SPI0_MISO),
    .i_SPI_MOSI(SPI0_MOSI),
    .i_SPI_CS_n(SPI0_CS));

reg [3:0] rxIndex = 0;
reg [7:0] memory [0:7];
reg [0:0] newTransaction;

assign MYLED = spiRxReady;

// Reset index on new SPI transaction
always @(posedge SPI0_CS)
begin
    newTransaction <= 1'b0;
end

// Read SPI data into memory
always @(posedge regRxReady)
begin
    if (newTransaction)
    begin
        newTransaction <= 0;
        rxIndex <= 0;
    end
    memory[rxIndex] <= spiRx;
    rxIndex <= rxIndex + 1;
end

//always @(rxIndex)
//begin
//    if ((rxIndex > 0) && (memory[0][7])) //  & 8'h80)) // high bit set means read op 
//    begin
//        regTx <= memory[rxIndex];
//        regTxReady <= 1;
//    end
//end
//
//always @(negedge spiRxReady)
//begin
//    rxIndex <= rxIndex + 1'b1;
//    if ((rxIndex == 1) && memory[0][7]) // high bit set means read op
//    begin
//        regTx <= memory[1];
//        regTxReady <= 1'b1;
//    end
//end

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

