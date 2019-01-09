module cmam (
	input clk, rstb, 
	input [1:0] n2c_data,
	output [1:0] c2n_data, c2n_vld,
	input [6:0] addr,		// register address
	input [31:0] data_wr,	// write data in
	input wren,				// write enable (active high)
	input rden,
	output [31:0] data_rd,	// read data out
	output debug,
    output c2a_valid, c2a_data, a2c_data
);

nmic_if	nif(	.clk(clk), 
				.rstb(rstb), 
				.a2c_data(a2c_data),
				.c2a_data(c2a_data),
				.c2a_vld(c2a_valid),
				.addr(addr),
				.data_wr(data_wr),
				.data_rd(data_rd),
				.wren(wren),
				.rden(rden),
                .debug(debug)
);

am am_if(
	.clk_am(clk),
	.c2a_data(c2a_data),
	.c2a_valid(c2a_valid),
	.n2a_data(n2c_data),
	.a2c_data(a2c_data),
	.a2n_data(c2n_data),
	.a2n_valid(c2n_vld),
	.nm_switch()
);

endmodule
