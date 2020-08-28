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
