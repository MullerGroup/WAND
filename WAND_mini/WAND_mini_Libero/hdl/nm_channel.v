////////////////////////////////////////////////////////////////////////////////
// File: nm_channel.v
// Description: Single NM channel together with registers
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
module nm_channel(	input clk, rstb,
					input tx_start,		// transmitter start command
					input tx_mode,		// transmit cmd/reg (from status reg)
					input n2a_data,		// NM received data
					output a2n_valid, a2n_data,
					input rx_strobe, tx_strobe,
					input [3:0] addr,		// register address
					input [31:0] data_wr,	// write bus
					output reg [31:0] data_rd,	// read bus
					input wr_stb, rd_stb,	// read/write strobes
					output has_adcdata,		// ADC data indicator
					output has_ackdata,		// register data indicator
					output reg txbusy,		// tx busy indicator
                    output debug
				);

wire [15:0] rx_data, ackfifo_data_in, ackfifo_data_out, adcfifo_data_in, adcfifo_data_out;

reg [33:0] tx_data;

assign debug = wr_adc_fifo;
					
nmic_rx nrx(
	.clk(clk), .rstb(rstb), 
	.data_stb(rx_strobe), 
	.n2a_data(n2a_data),
	.adc_full(adc_full), 
	.reg_full(ack_full),
	.data(rx_data),
	.wr_adc(wr_adc_fifo), 
	.wr_reg(wr_ack_fifo) );

nmic_tx ntx(	.clk(clk), 
				.rstb(rstb), 
				.data_stb(tx_strobe),
				.data(tx_data),
				.cmd_mode(tx_mode),
				.start(tx_start),
				.a2n_valid(a2n_valid), 
				.a2n_data(a2n_data),
				.done(tx_done));

ACKFIFO ackfifo(	.CLK(clk),
					.RESET(rstb),
					.DATA(rx_data),
					.WE(wr_ack_fifo),
					.RE(rd_stb && (addr == 4'h8)),
					.AFULL(ack_full),
					.AEMPTY(ack_empty),
					.Q(ackfifo_data_out)	);
					
ADCFIFO adcfifo(	.CLK(clk),
					.RESET(rstb),
					.DATA(rx_data),
					.WE(wr_adc_fifo),
					.RE(rd_stb && (addr == 4'hC)),
					.AFULL(adc_full),
					.AEMPTY(adc_empty),
					.Q(adcfifo_data_out)	);

assign has_adcdata = ~adc_empty;
assign has_ackdata = ~ack_empty;					

// data_rd mux
always @(*) begin
	case(addr)
		4'h0:
		begin
			data_rd = {30'b0, tx_data[33:32]};	
		end
		4'h4:
		begin
			data_rd = tx_data[31:0];
		end
		4'h8:
		begin
			data_rd = {16'b0, ackfifo_data_out};
		end
		4'hC:
		begin
			data_rd = {16'b0, adcfifo_data_out};
		end
        default:
        begin  
            data_rd = 32'bX;
        end
	endcase
end

always @(posedge clk or negedge rstb) begin
	if (!rstb) begin
		txbusy <= 0;
	end else begin
		txbusy <= (txbusy || tx_start) && (!tx_done);
		if (wr_stb) begin
			if (addr == 4'h0) begin	
				tx_data[33:32] <= data_wr[1:0];
			end else if (addr == 4'h04) begin
				tx_data[31:0] <= data_wr;
			end
		end
	end
end
				
endmodule

