////////////////////////////////////////////////////////////////////////////////
// File: nmic_tx.v
// Description: NMIC transmitter
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
module nmic_tx(	input clk, rstb, data_stb,
					input [33:0] data,		// left justified (MSB is first bit transmitted, LSBs ignored in cmd mode)
					input cmd_mode,			// 1: send command; 0: send reg op
					input start,
					output reg a2n_valid, a2n_data,
					output reg done
					);


parameter 	STATE_IDLE = 0, 
			STATE_XMIT = 1,
			STATE_CRC = 2;
reg start_latch;
reg [1:0] state;
reg [5:0] count;
reg [4:0] crc;

wire [5:0] length;
wire dbit;

assign length = cmd_mode ? 6'd11 : 6'd34;
assign dbit = data[(cmd_mode ? 10 : 33)-count];

initial begin
	state = STATE_IDLE;
end

always @(posedge clk or negedge rstb) begin
	if (!rstb) begin
		a2n_valid <= 0;
		a2n_data <= 0;
		start_latch <= 0;
		done <= 0;
		state <= STATE_IDLE;
	end
	else begin
		start_latch <= start | start_latch;
		case(state)
			STATE_IDLE:
			begin
				a2n_valid <= 0;
				count <= 0;
				done <= 0;
				if (data_stb && start_latch) begin
					state <= STATE_XMIT;
					start_latch <= 0;
					crc <= 5'b01100;	// equivalent to initializing verifier with 11111
				end
			end
			STATE_XMIT:
			begin
				if (data_stb) begin
					a2n_valid <= 1;
					a2n_data <= dbit;
					count <= count + 1;
					if (count == length) begin
						state <= STATE_CRC;
						a2n_data <= crc[4];
						count <= 1;
					end else begin
						crc <= {crc[3], crc[2], (crc[1] ^ (crc[4] ^ dbit)), crc[0], crc[4] ^ dbit};
					end
				end
			end
			STATE_CRC:
			begin
				if (data_stb) begin
					count <= count + 1;
					if (count == 5) begin
						state <= STATE_IDLE;
						done <= 1;
						a2n_valid <= 0;
					end else begin
						a2n_valid <= 1;
						a2n_data <= crc[4-count[2:0]];
					end
				end
			end
		endcase
	end
end
					
endmodule
