////////////////////////////////////////////////////////////////////////////////
// File: nmic_if.v
// Description: NMIC interface
//
// CORTERA NEUROTECHNOLOGIES INC. CONFIDENTIAL
// Unpublished Copyright (C) 2015 CORTERA NEUROTECHNOLOGIES INC.
// All rights reserved.
//
// NOTICE:  All information contained herein is the property of 
// CORTERA NEUROTECHNOLOGIES INC. The intellectual and technical concepts 
// contained herein are proprietary to CORTERA NEUROTECHNOLOGIES INC. and may
// be covered by U.S. and foreign patents, patents in process, and are protected
// by trade secret and/or copyright law.  Dissemination of this information or 
// reproduction of this material is strictly forbidden unless prior written 
// permission is obtained from CORTERA NEUROTECHNOLOGIES INC.
//
// The copyright notice above does not evidence any actual or intended publication
// or disclosure of this source code, which includes information that is confidential
// and/or proprietary, and is a trade secret, of CORTERA NEUROTECHNOLOGIES INC.   
// ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
// OR PUBLIC DISPLAY OF OR THROUGH USE OF THIS SOURCE CODE WITHOUT 
// THE EXPRESS WRITTEN CONSENT OF CORTERA NEUROTECHNOLOGIES INC. IS STRICTLY PROHIBITED, AND 
// IN VIOLATION OF APPLICABLE LAWS AND INTERNATIONAL TREATIES.  THE 
// RECEIPT OR POSSESSION OF THIS SOURCE CODE AND/OR RELATED INFORMATION
// DOES NOT CONVEY OR IMPLY ANY RIGHTS TO REPRODUCE, DISCLOSE OR DISTRIBUTE
// ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT MAY 
// DESCRIBE, IN WHOLE OR IN PART.                
//////////////////////////////////////////////////////////////////////////////////
module nmic_if(	input clk, rstb, 
				input a2c_data,
				output c2a_data, c2a_vld,
					input [6:0] addr,		// register address
					input [31:0] data_wr,	// write data in
					input wren,				// write enable (active high)
					input rden,
					output reg [31:0] data_rd,	// read data out
                output reg debug
					);
					

wire [3:0] tx_start, chan_dbg;
wire [3:0] nm_has_adc, nm_has_ack, tx_busy;
wire [31:0] data_rd_n0, data_rd_n1, data_rd_n2, data_rd_n3;
reg [7:0] status;
reg [7:0] rst_valid, rst_data;
wire [7:0] cm_txd, cm_txvalid, rx_data;
wire rst_start, rst_busy;
reg [3:0] nmic_sel;
reg [31:0] debug_reg;

assign tx_start[3:0] = data_wr[15:12] & ((wren && addr[6:0] == 7'b0) ? 4'b1111 : 4'b0000); 
assign rst_start = data_wr[8] & (wren && addr[6:0] == 7'b0); 

always @(*) begin
    case(debug_reg[2:0])
    3'd0:
        debug = rx_data[debug_reg[5:3]];
    3'd1:
        debug = rx_strobe;
    3'd2:
        debug = chan_dbg[debug_reg[4:3]];
    default:
        debug = 0;
    endcase
end

cm_demux cmd_inst(
	.clk(clk),					// 20 MHz master clock
	.rstb(rstb),					// active low reset
	.a2c_data(a2c_data),
	.c2a_data(c2a_data),
	.c2a_valid(c2a_vld),
	.rx_data(rx_data),	// received bits for NMs 7..0
	.rx_data_stb(rx_strobe),		// NM received data strobe
	.tx_data(cm_txd),		// data bits to transmit for NMs 7..0
	.tx_valid(cm_txvalid),		// valid bits to transmit for NMs 7..0
	.rst_data(rst_data),
	.rst_valid(rst_valid),
	.tx_data_stb(tx_strobe),		// NM transmit data strobe (drivers should update bits on this strobe)
	.link_rst_type(status[1:0]),	// 11: reset AM; 10: reset NMs, 00: switch NM power
	.link_rst(rst_start),				// initiate link reset
	.rst_busy(rst_busy)
);

nm_channel	nmc0(	.clk(clk), .rstb(rstb),
					.tx_start(tx_start[0]),
					.tx_mode(status[4]),
					.rx_strobe(rx_strobe), 
					.tx_strobe(tx_strobe),
					.n2a_data(rx_data[0]),
					.a2n_valid(cm_txvalid[0]),
					.a2n_data(cm_txd[0]),
					.addr(addr[3:0]),
					.data_wr(data_wr),
					.data_rd(data_rd_n0),
					.rd_stb(rden && nmic_sel[0]),
					.wr_stb(wren && nmic_sel[0]),
					.has_adcdata(nm_has_adc[0]),
					.has_ackdata(nm_has_ack[0]),
					.txbusy(tx_busy[0]),
                    .debug(chan_dbg[0]));

nm_channel	nmc1(	.clk(clk), .rstb(rstb),
					.tx_start(tx_start[1]),
					.tx_mode(status[5]),
					.rx_strobe(rx_strobe), 
					.tx_strobe(tx_strobe),
					.n2a_data(rx_data[1]),
					.a2n_valid(cm_txvalid[1]),
					.a2n_data(cm_txd[1]),
					.addr(addr[3:0]),
					.data_wr(data_wr),
					.data_rd(data_rd_n1),
					.rd_stb(rden && nmic_sel[1]),
					.wr_stb(wren && nmic_sel[1]),
					.has_adcdata(nm_has_adc[1]),
					.has_ackdata(nm_has_ack[1]),
					.txbusy(tx_busy[1]),
                    .debug(chan_dbg[1]));
					
nm_channel	nmc2(	.clk(clk), .rstb(rstb),
					.tx_start(tx_start[2]),
					.tx_mode(status[6]),
					.rx_strobe(rx_strobe), 
					.tx_strobe(tx_strobe),
					.n2a_data(rx_data[2]),
					.a2n_valid(cm_txvalid[2]),
					.a2n_data(cm_txd[2]),
					.addr(addr[3:0]),
					.data_wr(data_wr),
					.data_rd(data_rd_n2),
					.rd_stb(rden && nmic_sel[2]),
					.wr_stb(wren && nmic_sel[2]),
					.has_adcdata(nm_has_adc[2]),
					.has_ackdata(nm_has_ack[2]),
					.txbusy(tx_busy[2]),
                    .debug(chan_dbg[2]));
					
nm_channel	nmc3(	.clk(clk), .rstb(rstb),
					.tx_start(tx_start[3]),
					.tx_mode(status[7]),
					.rx_strobe(rx_strobe), 
					.tx_strobe(tx_strobe),
					.n2a_data(rx_data[3]),
					.a2n_valid(cm_txvalid[3]),
					.a2n_data(cm_txd[3]),
					.addr(addr[3:0]),
					.data_wr(data_wr),
					.data_rd(data_rd_n3),
					.rd_stb(rden && nmic_sel[3]),
					.wr_stb(wren && nmic_sel[3]),
					.has_adcdata(nm_has_adc[3]),
					.has_ackdata(nm_has_ack[3]),
					.txbusy(tx_busy[3]),
                    .debug(chan_dbg[3])	);					
					



// data_rd mux
always @(*) begin
	case(addr[6:4])
		3'b000:
		begin
			case(addr[3:0])
				4'b0000:
					data_rd = {nm_has_ack, tx_busy, nm_has_adc, 3'b0, rst_busy, 8'b0, status};
				4'b0100:
					data_rd = {16'b0, rst_valid, rst_data};
				default:
					data_rd = 32'bX;
			endcase
            nmic_sel = 4'b0;
		end
		3'b001:
        begin
			data_rd = data_rd_n0;
            nmic_sel = 4'b0001;
        end
		3'b010:
        begin
			data_rd = data_rd_n1;
            nmic_sel = 4'b0010;
        end
		3'b011:
        begin
			data_rd = data_rd_n2;
            nmic_sel = 4'b0100;
        end
		3'b100:
        begin
			data_rd = data_rd_n3;
            nmic_sel = 4'b1000;
        end
		default:
        begin
			data_rd = 32'bX;
            nmic_sel = 4'b0;
        end
	endcase
end 

always @(posedge clk or negedge rstb) begin
	if (!rstb) begin
		status <= 0;
		rst_valid <= 0;
		rst_data <= 0;
        debug_reg <= 0;
	end else begin
		if (wren) begin
			if (addr == 7'h00) begin
				status <= data_wr[7:0];
			end else if (addr == 7'h04) begin
				{rst_valid, rst_data} <= data_wr[15:0];
			end else if (addr == 7'h08) begin
                debug_reg <= data_wr;
            end
		end
	end
end

endmodule


					
