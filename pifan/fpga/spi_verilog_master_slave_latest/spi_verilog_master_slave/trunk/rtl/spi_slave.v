////////////////////////////////////////////////////////////////////////////////
////                                                                        ////
//// Project Name: SPI (Verilog)                                            ////
////                                                                        ////
//// Module Name: spi_slave                                                ////
////                                                                        ////
////                                                                        ////
////  This file is part of the Ethernet IP core project                     ////
////  http://opencores.com/project,spi_verilog_master_slave                 ////
////                                                                        ////
////  Author(s):                                                            ////
////      Santhosh G (santhg@opencores.org)                                 ////
////                                                                        ////
////  Refer to Readme.txt for more information                              ////
////                                                                        ////
////////////////////////////////////////////////////////////////////////////////
////                                                                        ////
//// Copyright (C) 2014, 2015 Authors                                       ////
////                                                                        ////
//// This source file may be used and distributed without                   ////
//// restriction provided that this copyright statement is not              ////
//// removed from the file and that any derivative work contains            ////
//// the original copyright notice and the associated disclaimer.           ////
////                                                                        ////
//// This source file is free software; you can redistribute it             ////
//// and/or modify it under the terms of the GNU Lesser General             ////
//// Public License as published by the Free Software Foundation;           ////
//// either version 2.1 of the License, or (at your option) any             ////
//// later version.                                                         ////
////                                                                        ////
//// This source is distributed in the hope that it will be                 ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied             ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR                ////
//// PURPOSE.  See the GNU Lesser General Public License for more           ////
//// details.                                                               ////
////                                                                        ////
//// You should have received a copy of the GNU Lesser General              ////
//// Public License along with this source; if not, download it             ////
//// from http://www.opencores.org/lgpl.shtml                               ////
////                                                                        ////
////////////////////////////////////////////////////////////////////////////////
/* SPI MODE 3
		CHANGE DATA (SPI_MISO) @ NEGEDGE SPI_SCK
		read data (SPI_MOSI) @posedge SPI_SCK
*/		
module spi_slave (reset,txEnable,txData,msb_lsb,SPI_SS,SPI_SCK,SPI_MOSI, SPI_MISO,done,rxData, diag);
  input reset,SPI_SS,SPI_SCK,SPI_MOSI,txEnable,msb_lsb;
  input [7:0] txData;
  output SPI_MISO;           //slave out   master in 
  output reg done;
  output reg [7:0] rxData;
  output reg [3:0] diag;

  reg [7:0] txReg,rxReg;
  reg [3:0] nb;
  wire serOut;
  
  assign serOut=msb_lsb?txReg[7]:txReg[0];
  assign SPI_MISO=( (!SPI_SS)&&txEnable )?serOut:1'bz; //if 1=> send data  else TRI-STATE SPI_MISO


//read from SPI_MOSI
always @(posedge SPI_SCK or negedge reset)
  begin
    if (reset==0)
		begin rxReg <= 8'h00;  rxData <= 8'h00; done <= 0; nb <= 0; end   //
	else if (!SPI_SS) begin 
		diag <= nb;
			if(msb_lsb==0)  //LSB first, in@msb -> right shift
				begin rxReg <={SPI_MOSI,rxReg[7:1]}; end
			else     //MSB first, in@lsb -> left shift
				begin rxReg <={rxReg[6:0],SPI_MOSI}; end  
		//increment bit count
			nb<=nb+1;
			if(nb!=8) done<=0;
			else  begin rxData<=rxReg; done<=1; nb<=0; end
		end	 //if(!SPI_SS)_END  if(nb==8)
  end

//send to  SPI_MISO
always @(negedge SPI_SCK or negedge reset)
  begin
	if (reset==0)
		begin txReg = 8'hFF; end
	else begin
		if(!SPI_SS) begin			
			if(nb==0) txReg=txData;
			else begin
			   if(msb_lsb==0)  //LSB first, out=lsb -> right shift
					begin txReg = {1'b1,txReg[7:1]}; end
			   else     //MSB first, out=msb -> left shift
					begin txReg = {txReg[6:0],1'b1}; end			
			end
		end //!SPI_SS
	 end //reset	
  end //always

endmodule
      
/*
			if(msb_lsb==0)  //LSB first, out=lsb -> right shift
					begin txReg = {txReg[7],txReg[7:1]}; end
			else     //MSB first, out=msb -> left shift
					begin txReg = {txReg[6:0],txReg[0]}; end	
*/
