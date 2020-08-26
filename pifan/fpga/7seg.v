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

