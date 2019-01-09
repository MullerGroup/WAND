////////////////////////////////////////////////////////////////////////////////
// File: deserializer.v
// Description: NMIC deserializer
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
module deserializer(
					input clk, rstb, bit_data, bit_valid, flush, done, output_full, crc_ok,
					output reg out_valid, 
					output reg [15:0] word	);

reg [3:0] bitPos;
reg [15:0] tmpWord;
reg dropFrame; 		// indicates next frame should be dropped because the output FIFO is full
reg first;

always @(posedge clk or negedge rstb) begin
	if (!rstb) begin
		bitPos <= 4'hF;
		tmpWord <= 0;
		out_valid <= 0;
		dropFrame <= 0;
		first <= 1;
	end else begin
		out_valid <= 0;
		if ((bit_valid && (bitPos == 0)) || done || flush) begin
			// flush/done occurs when the bit is not valid, so don't clock in another bit
			word <= done ? {tmpWord[7:0], 7'b0, crc_ok} : (flush ? tmpWord : {tmpWord[14:0], bit_data});
			// we drop the frame if the output is full
			// when we try to write the first word
			if (first)
				dropFrame <= output_full;
			first <= done;
			tmpWord <= 0;
			bitPos <= 4'hF;
			out_valid <= first ? ~output_full : ~dropFrame;
		end else if (bit_valid) begin
			tmpWord <= {tmpWord[14:0], bit_data};
			bitPos <= bitPos - 1;
		end
	end
end

endmodule 
