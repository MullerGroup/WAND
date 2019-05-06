module am
(
	input clk_am,
	input c2a_data,
	input c2a_valid,
	input [1:0] n2a_data,
	output reg a2c_data = 1'b0,
	output reg [1:0] a2n_data = 2'd0,
	output reg [1:0] a2n_valid = 2'd0,
	output reg [1:0] nm_switch = 2'd0
);

	reg reset = 1'b1;
	reg [3:0] am_cntr = 4'd0;
	reg isReset = 1'b0, nm_isReset = 1'b0, nm_isPSwitch = 1'b0;
	reg [1:0] nm_rst_flag = 2'd0;
	reg [1:0] nm_onoff = 2'b11;

	reg c2a_valid_reg = 1'b0, c2a_data_reg = 1'b0;

	reg [9:0] frame;
	reg [12:0] training_cntr = 13'd0;

//---------------AM Counter:----------------

	always @(posedge clk_am)
	begin			
		if (reset == 1'b0)		//synchronous reset
		begin
			am_cntr <= 4'd2;	
		end
		else if (frame == 10'b0110000000)
		begin
			am_cntr <= 4'd0;
		end
		else
		begin
			if (am_cntr == 4'd9)
				am_cntr <= 4'd0;
			else
				am_cntr <= am_cntr + 4'd1;
		end
	end
	
//------Reset Counter by sending empty frames:
	always @(posedge clk_am)
	begin
		frame[0] <= c2a_valid_reg;
		frame[9:1] <= frame[8:0];
	end

//------------Sampling on negedge of clk:----

	always @(negedge clk_am)
	begin		
		if (reset == 1'b0)
		begin
			c2a_valid_reg <= 1'b0;
			c2a_data_reg <= 1'b0;
			nm_rst_flag <= 2'b0;
			nm_onoff <= 2'b11;
			isReset <= 1'b0;
			nm_isReset <= 1'b0;
			nm_isPSwitch <= 1'b0;
			if (training_cntr > 13'd0)
			begin
				training_cntr <= training_cntr - 13'd1;
			end
			else
			begin
				reset <= 1'b1;
			end
		end
		else
		begin
			c2a_valid_reg <= c2a_valid;
			c2a_data_reg <= c2a_data;
			if (am_cntr == 4'd0)
			begin
				nm_isReset <= 1'b0;
				nm_isPSwitch <= 1'b0;
				nm_onoff <= 2'b11;
				if (c2a_data == 1'b0 && c2a_valid == 1'b1)	//inverted because of the inverters on the hardware
					isReset <= 1'b1;
				else
					isReset <= 1'b0;
				nm_rst_flag <= 2'd0;
			end
	
			if (isReset == 1'b1)
			begin
				case (am_cntr)
					4'd1:		//reset type
					begin
						case ({c2a_valid, c2a_data})
							2'b11:
							begin
								reset <= 1'b0;	//reset AM
								training_cntr <= 13'd8000;
							end

							2'b10:
							begin
								nm_isReset <= 1'b1;	//reset NMs interface
							end

							2'b00:
							begin
								nm_isPSwitch <= 1'b1;	//Power switch NMs
							end
						endcase						

					end
					4'd2:
					begin
						nm_rst_flag[0] <= c2a_valid;
						if (nm_isPSwitch == 1'b1)
							nm_onoff[0] <= c2a_data;
					end
					4'd3:
					begin
						nm_rst_flag[1] <= c2a_valid;
						if (nm_isPSwitch == 1'b1)
							nm_onoff[1] <= c2a_data;
					end
				endcase
			end
		end
	end

//------------Sending data & reset AM => NMs:----

	always @(posedge clk_am)
	begin
		if (reset == 1'b0)
		begin
			a2n_valid <= 2'b0;
			a2n_data  <= 2'b0;
			nm_switch <= 2'b0;	
		end
		else
		begin
			if (isReset == 1'b0)			//Sending data AM => NMs:
			begin	
				case (am_cntr)
					4'd2:
					begin
						a2n_valid[0] <= c2a_valid_reg;
						//if (c2a_valid_reg == 1'b1)
							a2n_data[0] <= c2a_data_reg;
					end
	
					4'd3:
					begin
						a2n_valid[1] <= c2a_valid_reg;
						//if (c2a_valid_reg == 1'b1)
							a2n_data[1] <= c2a_data_reg;
					end

				endcase 
			end

			else if (nm_isReset == 1'b1)		//Sending soft reset AM => NMs
			begin
				case (am_cntr)
					4'd2:
					begin
						if (nm_rst_flag[0] == 1'b1)						
							a2n_valid[0] <= 1'b0;
					end
	
					4'd3:
					begin
						if (nm_rst_flag[0] == 1'b1)						
							a2n_valid[0] <= 1'b1;						
						if (nm_rst_flag[1] == 1'b1)
							a2n_valid[1] <= 1'b0;
					end

					4'd4:
					begin
						if (nm_rst_flag[0] == 1'b1)						
							a2n_valid[0] <= 1'b1;
						if (nm_rst_flag[1] == 1'b1)
							a2n_valid[1] <= 1'b1;
					end
	
					4'd5:
					begin
						if (nm_rst_flag[0] == 1'b1)						
							a2n_valid[0] <= 1'b0;
						if (nm_rst_flag[1] == 1'b1)
							a2n_valid[1] <= 1'b1;
					end

					4'd6:
					begin
						if (nm_rst_flag[1] == 1'b1)
							a2n_valid[1] <= 1'b0;
					end

				endcase 
			end

			else if (nm_isPSwitch == 1'b1)
			begin
				case (am_cntr)
					4'd2:
					begin
						if (nm_rst_flag[0] == 1'b1)						
							nm_switch[0] <= nm_onoff[0];
					end
	
					4'd3:
					begin
						if (nm_rst_flag[1] == 1'b1)						
							nm_switch[1] <= nm_onoff[1];
					end
				endcase 
			end
		end
	end

//------------sending frames AM => CM:----

	always @(posedge clk_am)
	begin
		if (reset == 1'b0)
		begin
			if(training_cntr > 13'd4000)
				a2c_data <= 1'b0;
			else
				a2c_data <= 1'b1;
		end
		else
		begin	
			case (am_cntr)
				4'd3:
				begin
					a2c_data <= 1'b1;
				end

				4'd4:
				begin
					a2c_data <= 1'b1;
				end

				4'd5:
				begin
					a2c_data <= n2a_data[0];
				end

				4'd6:
				begin
					a2c_data <= n2a_data[1];
				end
				
				default:
					a2c_data <= 0;

			endcase 
		end
	end

//------------------------
endmodule
