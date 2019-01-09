////////////////////////////////////////////////////////////////////////////////
// File: nmic_rx.v
// Description: NMIC receiver / fifo interface
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
module nmic_rx(input clk, rstb, data_stb, n2a_data, adc_full, reg_full,
				output [15:0] data, output wr_adc, wr_reg);


reg [12:0] startseq;
reg in_packet;
reg pktType;		// 1 = ADC; 0 = reg packet

reg [10:0] bitCt;
reg [7:0] crc;
reg flush, done;
wire ds_out_valid;

assign wr_adc = ds_out_valid && pktType;
assign wr_reg = ds_out_valid && !pktType;


deserializer ds(	.clk(clk), .rstb(rstb), .bit_data(n2a_data), .crc_ok(crc == 8'h0),
					.bit_valid(data_stb & in_packet), .flush(flush), .done(done),
					.out_valid(ds_out_valid), 
					.word(data),
					.output_full(pktType ? adc_full : reg_full));

always @(posedge clk or negedge rstb) begin
	if (!rstb) begin
		startseq <= 13'b1111111111111;
		in_packet <= 0;
		done <= 0;
		flush <= 0;
        crc <= 0;
        pktType <= 0;
        bitCt <= 0;
	end else begin
		done <= 0;
		flush <= 0;
		if (data_stb) begin
			startseq <= {startseq[11:0], n2a_data};
			if (!in_packet && startseq == 13'b0000000010101) begin
				pktType <= n2a_data;
				in_packet <= 1;
				//crc <= {1'b0, 1'b1 ^ n2a_data, 1'b1, 1'b0, 1'b0 ^ n2a_data ^ 1'b1, 1'b1 ^ 1'b0 ^ n2a_data, 1'b0, 1'b1 ^ n2a_data};
				// initial is 10001000 --> equiv to initializing generator with 0xFF + 10101 sequence
				crc <= {1'b0, ~n2a_data, 1'b0, 1'b1, ~n2a_data, ~n2a_data, 1'b0, ~n2a_data};
				bitCt <= n2a_data ? 11'd1033 : 11'd41; // 2 header bits; 32/1024 bits payload; 8b CRC; minus one
			end
			else if (in_packet) begin
				bitCt <= bitCt - 1;
				crc <= {crc[6], crc[7] ^ crc[5] ^ n2a_data, crc[4], crc[3], crc[2] ^ n2a_data ^ crc[7], crc[7] ^ crc[1] ^ n2a_data, crc[0], crc[7] ^ n2a_data};
				if ( (pktType && bitCt == 11'd1032) || (!pktType && bitCt == 11'd40)) begin
					flush <= 1;
				end
				if (bitCt == 0) begin
					in_packet <= 0;
					startseq <= {12'b111111111111, n2a_data};
					done <= 1;
				end
			end
		end
	end

end

endmodule

