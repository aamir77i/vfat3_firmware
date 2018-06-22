
module data_decode
( input wire  clk,
input wire rst_n,
input wire [7:0]data_in,
input wire start,
input wire [15:0]LV1As,
input wire [7:0]channel,
output wire [7:0]data_out,
output wire [15:0]hit_cnt,
output reg busy,
output reg done,
output reg [7:0]buffer_3
);

wire [7:0]mux_16_1_out;
reg [4:0] byte_cnt;
reg [15:0] trigger_cnt;
reg en1;reg flag;
reg soft_rst;
wire channel_1_bit;
//reg [7:0] buffer[25:0];
reg [7:0]buffer_0;
reg [7:0]buffer_1;
reg [7:0]buffer_2;
//reg [7:0]buffer_3;
reg [7:0]buffer_4;
reg [7:0]buffer_5;
reg [7:0]buffer_6;
reg [7:0]buffer_7;
reg [7:0]buffer_8;
reg [7:0]buffer_9;
reg [7:0]buffer_10;
reg [7:0]buffer_11;
reg [7:0]buffer_12;
reg [7:0]buffer_13;
reg [7:0]buffer_14;
reg [7:0]buffer_15;
reg [7:0]buffer_16;
reg [7:0]buffer_17;
reg [7:0]buffer_18;
reg [7:0]buffer_19;
reg [7:0]buffer_20;
reg [7:0]buffer_21;
reg [7:0]buffer_22;
reg [7:0]buffer_23;






reg[7:0] toggle_out;
////////////fsm //////////

   parameter IDLE            = 6'b000001;
   parameter LOOK_HEADER     = 6'b000010;
   parameter POPULATE_DATA   = 6'b000100;
   parameter DECODE_DATA     = 6'b001000;
   parameter FINISH_DECODING = 6'b010000;
   parameter HOLD_DONE       = 6'b100000;

   reg [5:0] state = IDLE;

   always @(posedge clk)
      if (~rst_n) begin
         state <= IDLE;
         en1 <= 0;// disable hit counting while in idle state
	     soft_rst<=1;//enable soft reset
         byte_cnt<=0;
         trigger_cnt<=0;
         done<=0;
	     //data_out<=data_in;
         busy<=0;
         
      end
      else begin
      //defaults
                        en1 <= 0;// disable hit counting while in idle state
                        soft_rst<=0;//
                        //byte_cnt<=0;
                        //trigger_cnt<=0;
                        done<=0;
                     //   busy<=1;
         case (state)
         
            IDLE : begin
             busy<=0;
               if (start)
               begin
                  state <= LOOK_HEADER;
                 trigger_cnt<= LV1As;
		
               end
               else
		         begin
                  state <= IDLE;
                  byte_cnt<=0;
                  trigger_cnt<=0;
                  soft_rst<=1;
                  end
                end
             LOOK_HEADER :
              begin
               busy<=1;
				 if (data_in == 8'h1E)
                  state<= POPULATE_DATA;
                else
                  state <= LOOK_HEADER;
              end
            
               POPULATE_DATA : begin
                busy<=1;
		if (byte_cnt == 20)// full data packet is received now 
                  begin
                   state <= DECODE_DATA;
                   byte_cnt<=0;
                  end
               else begin
                  state <= POPULATE_DATA;
               case(byte_cnt)
               0:begin buffer_0 <= data_in;end
               1:begin buffer_1 <= data_in;end
               2:begin buffer_2 <= data_in;end
               3:begin buffer_3 <= data_in;end
               4:begin buffer_4 <= data_in;end
               5:begin buffer_5 <= data_in;end
               6:begin buffer_6 <= data_in;end
               7:begin buffer_7 <= data_in;end
               8:begin buffer_8 <= data_in;end
               9:begin buffer_9 <= data_in;end
               10:begin buffer_10 <= data_in;end
               11:begin buffer_11 <= data_in;end
               12:begin buffer_12 <= data_in;end
               13:begin buffer_13 <= data_in;end
               14:begin buffer_14 <= data_in;end
               15:begin buffer_15 <= data_in;end
               16:begin buffer_16 <= data_in;end
               17:begin buffer_17 <= data_in;end
               18:begin buffer_18 <= data_in;end
               19:begin buffer_19 <= data_in;end
               20:begin buffer_20 <= data_in;end
               //21:buffer_21 <= data_in;
               //0:buffer_22 <= data_in;
               //0:buffer_23 <= data_in;
               //0:buffer_24 <= data_in;
                     
               endcase 
               //buffer[byte_cnt] <= data_in;
               byte_cnt <= byte_cnt +1;
                    end
               end
            
                      
            DECODE_DATA : begin
                  state <= FINISH_DECODING;//NEXT STATE
                  en1 <= 1;     //activate hit cnt module
                  trigger_cnt <= trigger_cnt - 1;
                   busy<=1;
            end
           
           
           
           FINISH_DECODING : begin
                  busy<=1;
                      
               if (trigger_cnt== 0)//  
                  begin
                   state <= HOLD_DONE;// 
                   //soft_rst <= 1;//activate rst
                   done<=1;
                  end
               else
                  state <= LOOK_HEADER;
               
                            end

           HOLD_DONE:
           begin
           done<=1;
           state <= IDLE;
           end

            default: begin  // Fault Recovery
               state <= IDLE;
               soft_rst <= 1;//activate  soft rst
               en1 <= 0;
               busy<=0;
	           end
         endcase
     end							
							

//////////////////////////////



 Mux_16_1 mux_16_1_inst(
.clk(clk),
.sel(channel[6:3]),
.datain_1(buffer_3),
.datain_2(buffer_4),
.datain_3(buffer_5),
.datain_4(buffer_6),
.datain_5(buffer_7),
.datain_6(buffer_8),
.datain_7(buffer_9),
.datain_8(buffer_10),
.datain_9(buffer_11),
.datain_10(buffer_12),
.datain_11(buffer_13),
.datain_12(buffer_14),
.datain_13(buffer_15),
.datain_14(buffer_16),
.datain_15(buffer_17),
.datain_16(buffer_18),
.dataout(mux_16_1_out)

);


Mux_8_1  mux_8_1_inst(
.clk(clk),
.sel(channel[2:0]),
.datain(mux_16_1_out),
.dataout(channel_1_bit)

);




hit_counter hit_cnt_inst(
	.clk(clk),
	.rst_n(rst_n),
	.soft_rst(soft_rst),
	.en1(en1),
	.en2(channel_1_bit),
	.hit_cnt(hit_cnt) 
	);


always @(posedge clk)
if(~rst_n)
begin
toggle_out <= 0;
flag      <=0;
end
else begin
     flag   <=~flag;
     if (flag==0)toggle_out <=  8'b01111110;
      else     toggle_out   <=  8'b10000001;
end


//always@(posedge clk)
//    if(rst_n==0)
//        data_out<=0;
//    else if(busy==1)
//        data_out <= toggle_out;
//        else 
//        data_out <= data_in;
        
assign data_out = (busy)? toggle_out : data_in;

endmodule
