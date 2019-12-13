`define SCREEN_WIDTH 360
`define SCREEN_HEIGHT 360

///////* DON'T CHANGE THIS PART *///////
module DE0_NANO(
	CLOCK_50,
	GPIO_0_D,
	GPIO_1_D,
	KEY
);

//=======================================================
//  PARAMETER declarations
//=======================================================
localparam RED = 8'b111_000_00;
localparam GREEN = 8'b000_111_00;
localparam BLUE = 8'b000_000_11;

//=======================================================
//  PORT declarations
//=======================================================

//////////// CLOCK - DON'T NEED TO CHANGE THIS //////////
input 		          		CLOCK_50;

//////////// GPIO_0, GPIO_0 connect to GPIO Default //////////
output 		    [33:0]		GPIO_0_D;
//////////// GPIO_0, GPIO_1 connect to GPIO Default //////////
input 		    [33:20]		GPIO_1_D;
input 		     [1:0]		KEY;

///// PIXEL DATA /////
reg [7:0]	pixel_data_RGB332 = 8'd0;

///// READ/WRITE ADDRESS /////
reg [14:0] X_ADDR;
reg [14:0] Y_ADDR;
wire [16:0] WRITE_ADDRESS;
reg  [16:0] READ_ADDRESS; 

assign WRITE_ADDRESS = X_ADDR + Y_ADDR*(`SCREEN_WIDTH);

///// VGA INPUTS/OUTPUTS /////
wire 			VGA_RESET;
wire [7:0]	VGA_COLOR_IN;
wire [9:0]	VGA_PIXEL_X;
wire [9:0]	VGA_PIXEL_Y;
wire [7:0]	MEM_OUTPUT;
wire			VGA_VSYNC_NEG;
wire			VGA_HSYNC_NEG;
reg			VGA_READ_MEM_EN;

assign GPIO_0_D[5] = VGA_VSYNC_NEG;
assign VGA_RESET = ~KEY[0];

///// I/O for Img Proc /////
wire [8:0] RESULT;

/* WRITE ENABLE */
reg W_EN;

/*Our wires*/
reg [7:0] displayColor;
//reg [7:0] MEM_OUTPUT;


///////* CREATE ANY LOCAL WIRES YOU NEED FOR YOUR PLL *///////
wire c0_sig;
wire c1_sig;
wire c2_sig;

reg W_DONE;
//reg mcuVLD;
reg [7:0] mcuDIN;
reg mcuVLDedge = 1'b0;

wire writeVALID;
assign writeVALID = (~mcuVLDedge && mcuVLD);

///////* INSTANTIATE YOUR PLL HERE *///////

upsPLL	upsPLL_inst (
	.inclk0 ( CLOCK_50 ),
	.c0 ( c0_sig ),
	.c1 ( c1_sig ),
	.c2 ( c2_sig )
	);

///////* M9K Module *///////

Dual_Port_RAM_M9K mem(
	.input_data(pixel_data_RGB332),
	.w_addr(WRITE_ADDRESS),
	.r_addr(READ_ADDRESS),
	.w_en(W_EN),
	.clk_W(c2_sig),
	.clk_R(c1_sig), // DO WE NEED TO READ SLOWER THAN WRITE??
	.output_data(MEM_OUTPUT)
);

wire [7:0] mcuDINw;
wire mcuVLD;

assign mcuVLD = GPIO_1_D[33];
assign mcuDINw = {GPIO_1_D[25], GPIO_1_D[26], GPIO_1_D[27], GPIO_1_D[28], GPIO_1_D[29], GPIO_1_D[30], GPIO_1_D[31], GPIO_1_D[32]};

always @ (posedge c2_sig) begin
	if(~KEY[0]) mcuVLDedge <= 1'b0;
	else mcuVLDedge <= mcuVLD;
end

always @ ( posedge c2_sig ) begin
	if (~KEY[0] ) begin
		mcuDIN <= 0;
		
	end else if (mcuVLD) begin
		mcuDIN <= mcuDINw;
	end
end
	/*
always @ (*) begin
	if(~KEY[0]) 
		begin
			mcuVLD = 1'b0;
			mcuDIN = 16'd0;
		end
	else
		begin
			mcuVLD = 1'b1;
			mcuDIN = 16'b1100_0000_0010_0100;
		end
end
*/


///// Decode the incoming bit /////
always @ (posedge c2_sig) begin
	if (~KEY[0]) // On rest
		begin
			X_ADDR <= 0;
			Y_ADDR <= 0;
			W_DONE <= 1'b0; // Not done (haven't started)
		end
	else if (writeVALID) // Latch values on rising clock edge
		begin
			X_ADDR <=  20 + (mcuDIN[5:3] * 40);
			Y_ADDR <=  20 + (mcuDIN[2:0] * 40);
			W_DONE <= 1'b0; //Not done
		end
	else if (~W_DONE && mcuVLD) //Are we done yet?
		begin
		  // Conditional ensures that we are writing in the correct grid offset
			if (X_ADDR >= 20 + 40 * mcuDIN[5:3] && X_ADDR <= 20 + 40 * (mcuDIN[5:3] + 1))
				begin
					case(mcuDIN[7:6])
						2'b00: begin //TOP WALL
									pixel_data_RGB332 <= RED;
									W_EN <= (Y_ADDR == 20 + (mcuDIN[2:0] * 40)) ? 1:0;
								 end
						2'b01: begin //RIGHT WALL
									pixel_data_RGB332 <= RED;
									W_EN <= (X_ADDR == 20 + ((mcuDIN[5:3] + 1) * 40)) ? 1:0;
								 end
						2'b10: begin // BOTTOM WALL
									pixel_data_RGB332 <= RED;
									W_EN <= (Y_ADDR == 20 + ((mcuDIN[2:0] + 1) * 40)) ? 1:0;
								 end
						2'b11: begin // LEFT WALL
									pixel_data_RGB332 <= RED;
									W_EN <= (X_ADDR == 20 + (mcuDIN[5:3] * 40)) ? 1:0;
								 end
					endcase
					X_ADDR <= X_ADDR + 1; //Pixel by pixel write to the dual port ram
					Y_ADDR <= Y_ADDR;
					W_DONE <= 1'b0;
				 end
			else if (Y_ADDR < 20 + ((mcuDIN[2:0] + 1) * 40) && Y_ADDR >= 20 + 40 * mcuDIN[2:0])
				begin
					Y_ADDR<= Y_ADDR + 1;				// Increment Y
					X_ADDR <= 20 + (mcuDIN[5:3] * 40);  // Reset X
					W_DONE <= 1'b0;
					W_EN <= 1'b0;
				end
			else 
				begin
					X_ADDR <= 0;
					Y_ADDR <= 0;
					W_DONE <= 1'b1; //Finally we are done
					W_EN <= 1'b0;
					
				end
		end else begin
			X_ADDR <= 0;
			Y_ADDR <= 0;
			W_DONE <= 1'b1;
			W_EN <= 1'b0;
		end
end



always @ (c1_sig) begin
	if (VGA_READ_MEM_EN) begin
		if (VGA_PIXEL_X == 40 || VGA_PIXEL_X == 80 || VGA_PIXEL_X == 120 || VGA_PIXEL_X == 160 || VGA_PIXEL_X == 200 || VGA_PIXEL_X == 240 || VGA_PIXEL_X == 280 || VGA_PIXEL_X == 320 || VGA_PIXEL_X == 360) begin
			displayColor <= 8'hff;
		end else if (VGA_PIXEL_Y == 40 || VGA_PIXEL_Y == 80 || VGA_PIXEL_Y == 120 || VGA_PIXEL_Y == 160 || VGA_PIXEL_Y == 200 || VGA_PIXEL_Y == 240 || VGA_PIXEL_Y == 280 || VGA_PIXEL_Y == 320 || VGA_PIXEL_Y == 360) begin
			displayColor <= 8'hff;
		end else begin
			displayColor <= MEM_OUTPUT;
		end
	end else begin
		displayColor <= 8'hff;
	end
end


///////* VGA Module *///////
VGA_DRIVER driver (
	.RESET(VGA_RESET),
	.CLOCK(c1_sig),
	.PIXEL_COLOR_IN(displayColor),
	.PIXEL_X(VGA_PIXEL_X),
	.PIXEL_Y(VGA_PIXEL_Y),
	.PIXEL_COLOR_OUT({GPIO_0_D[9],GPIO_0_D[11],GPIO_0_D[13],GPIO_0_D[15],GPIO_0_D[17],GPIO_0_D[19],GPIO_0_D[21],GPIO_0_D[23]}),
   .H_SYNC_NEG(GPIO_0_D[7]),
   .V_SYNC_NEG(VGA_VSYNC_NEG)
);

///////* Image Processor *///////



///////* Update Read Address *///////
always @ (VGA_PIXEL_X, VGA_PIXEL_Y) begin
		READ_ADDRESS = (VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
		if(VGA_PIXEL_X>(`SCREEN_WIDTH - 1) || VGA_PIXEL_Y>(`SCREEN_HEIGHT - 1))begin
				VGA_READ_MEM_EN = 1'b0;
		end
		else begin
				VGA_READ_MEM_EN = 1'b1;
		end
end

	
endmodule 