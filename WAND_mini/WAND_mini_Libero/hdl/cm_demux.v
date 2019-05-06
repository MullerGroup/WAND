////////////////////////////////////////////////////////////////////////////////
// File: cm_demux.v
// Description: CM demultiplexer
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

module cm_demux
(
	input clk,					// 20 MHz master clock
	input rstb,					// active low reset
	input a2c_data,
	output reg c2a_data,
	output reg c2a_valid,
	output reg [7:0] rx_data,	// received bits for NMs 7..0
	output rx_data_stb,		// NM received data strobe
	input [7:0] tx_data,		// data bits to transmit for NMs 7..0
	input [7:0] tx_valid,		// valid bits to transmit for NMs 7..0
	input [7:0] rst_valid,		// valid bits to transmit during reset
	input [7:0] rst_data,		// data bits to transmit during reset
	output tx_data_stb,		// NM transmit data strobe (drivers should update bits on this strobe)
	input [1:0] link_rst_type,	// 11: reset AM; 10: reset NMs, 00: switch NM power
	input link_rst,				// initiate link reset
	output reg rst_busy
);

reg [3:0] cm_rx_ctr;
reg [3:0] cm_tx_ctr;
reg [1:0] state;
reg [7:0] rx_frame;
reg link_rst_latch;

parameter 	STATE_DATA = 0,
			STATE_BLANK = 1,
			STATE_RST = 2;

// CM master counters
// There are two offset counters to simplify the demux logic
// Counter values 0-7 correspond to data slots for the rx/tx
// pilot is #8, reset bit is #9
always @(posedge clk or negedge rstb) begin
	if (!rstb) begin
		cm_tx_ctr <= 0;
		cm_rx_ctr <= 6;
	end else begin
		cm_rx_ctr <= (cm_rx_ctr == 4'd9) ? 0 : cm_rx_ctr + 1;
		cm_tx_ctr <= (cm_tx_ctr == 4'd9) ? 0 : cm_tx_ctr + 1;
	end
end

assign tx_data_stb = (cm_tx_ctr == 4'd8);
assign rx_data_stb = (cm_rx_ctr == 4'd9);	// rx_frame gets latched to rx_data on counter == 8

// The AM's counter gets aligned when the AM receives a frame where none of the 
// valid bits are high.
// Thus, any reset frame must be preceded by a blank frame to ensure a false counter reset
// does not occur.

// transmitter mux logic
always @(*) begin
	case(state)
		STATE_DATA:
		begin
			rst_busy = 0;
			if (cm_tx_ctr >= 4'd8) begin	// pilot/reset slot
				c2a_valid = 1;
				c2a_data = 1;
			end else begin
				c2a_valid = tx_valid[cm_tx_ctr];
				c2a_data = tx_data[cm_tx_ctr];
			end
		end
		STATE_BLANK:
		begin
			rst_busy = 1;
			if (cm_tx_ctr >= 4'd8) begin	// pilot/reset slot
				c2a_valid = 1;
				c2a_data = 1;
			end else begin
				c2a_valid = 0;
				c2a_data = 0;
			end
		end
		STATE_RST:
		begin
			rst_busy = 1;
			if (cm_tx_ctr == 4'd8) begin	// pilot slot
				c2a_valid = 1;
				c2a_data = 0;
			end else if (cm_tx_ctr == 4'd9) begin // reset slot
				{c2a_valid, c2a_data}  = link_rst_type;
			end else begin
				c2a_valid = rst_valid[cm_tx_ctr];
				c2a_data = rst_data[cm_tx_ctr];
			end
		end
        default:
        begin
            rst_busy = 1'bx;
            c2a_valid = 1'bx;
            c2a_data = 1'bx;
        end
	endcase
end

// transmitter state machine
always @(posedge clk or negedge rstb) begin
	if (!rstb) begin
		state <= STATE_DATA;
		link_rst_latch <= 0;
	end else begin
		if (link_rst)
			link_rst_latch <= 1;
		case(state)
			STATE_DATA:
			begin
				if (cm_tx_ctr == 4'd7 && link_rst_latch) begin
					state <= STATE_BLANK;
					link_rst_latch <= 0;
				end
			end
			STATE_BLANK:
			begin
				if (cm_tx_ctr == 4'd7)
					state <= STATE_RST;
			end
			STATE_RST:
			begin
				if (cm_tx_ctr == 4'd7)
					state <= STATE_DATA;
			end
            default:
            begin
                state <= STATE_DATA;
            end 
		endcase
	end
end

// receiver logic
always @(negedge clk or negedge rstb) begin
	if (!rstb) begin
		rx_data <= 0;
        rx_frame <= 0;
	end else begin
		if (cm_rx_ctr <= 4'd7) begin
			rx_frame[cm_rx_ctr] <= a2c_data;
		end else if (cm_rx_ctr == 4'd8) begin
			rx_data <= rx_frame;
		end
	end
end

endmodule

