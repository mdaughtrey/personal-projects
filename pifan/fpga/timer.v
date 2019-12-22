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
