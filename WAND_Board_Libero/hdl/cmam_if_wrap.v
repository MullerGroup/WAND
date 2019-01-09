////////////////////////////////////////////////////////////////////////////////
// File: nmic_if_wrap.v
// Description: NMIC interface wrapper
////////////////////////////////////////////////////////////////////////////////
module cmam_if_wrap(
	input 	PCLK,	
			PENABLE,
  			PSEL,
  			PRESETN,
  			PWRITE,
  	output	PREADY,
  			PSLVERR,
  	input [6:0]		PADDR,
  	input [31:0] 	PWDATA,
  	output [31:0] 	PRDATA,
  	input [1:0]	N2C_DATA,
  	output [1:0] C2N_DATA, C2N_VALID,
    output DEBUG,
    output C2A_VALID, C2A_DATA, A2C_DATA
);

wire rd_enable;
wire wr_enable;

assign wr_enable = (PENABLE &&  PWRITE && PSEL);
assign rd_enable = (!PENABLE && !PWRITE && PSEL);

cmam	cmam_if(.clk(PCLK), 
				.rstb(PRESETN), 
				.n2c_data(~N2C_DATA),       //becasue of the NM board inverter
				.c2n_data(C2N_DATA),
				.c2n_vld(C2N_VALID),
				.addr(PADDR),
				.data_wr(PWDATA),
				.data_rd(PRDATA),
				.wren(wr_enable),
				.rden(rd_enable),
                .debug(DEBUG),
                .c2a_valid(C2A_VALID),
                .c2a_data(C2A_DATA),
                .a2c_data(A2C_DATA)
);
        
assign PREADY = 1'b1; 
assign PSLVERR = 1'b0;      
    
endmodule 
