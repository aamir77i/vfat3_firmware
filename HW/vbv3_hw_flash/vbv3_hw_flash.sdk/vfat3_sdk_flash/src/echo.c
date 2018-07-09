/******************************************************************************
*
* Copyright (C) 2016 Xilinx,* Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "lwip/sockets.h"
#include "netif/xadapter.h"
#include "lwipopts.h"
#include "xil_printf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "xparameters.h"
#include "xgpio_l.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "xllfifo.h"
#include "xstreamer.h"
#include <xiic.h>
#include "register_map_vfat3.h"
//#include<cstdlib.h>
#define THREAD_STACKSIZE 1024

#define HDLC_FS                	0X7E
#define HDLC_ADDRESS		   	0X00
#define HDLC_CONTROL           	0x03

#define SC0           			0x96
#define SC1           			0x99


    #define TRANSACTION_ID 		    0x05

    #define TRANS_TYPE_RD  		0x0	// Read
	#define TRANS_TYPE_WR  		0x1		// Write
	#define TRANS_TYPE_RDN  	0x2		// Non-incrementing Read
	#define TRANS_TYPE_WRN  	0x3		// Non-incrementing Write
	#define TRANS_TYPE_RMWB  	0x4		// Read/Modify/Write Bits
	#define TRANS_TYPE_RMWS  	0x5		// Read/Modify/Write Sum

	#define TRANS_TYPE_RSVD  	0xe		// Get Reserved Address Information
	#define TRANS_TYPE_INFO  	0xf		// Byte-order/Idle

	// IPBUS Info Codes
	#define INFO_CODE_OK  		0x0		// Request handled successfully by host
	#define INFO_CODE_BAD_HDR  	0x1		// Bad header
	#define INFO_CODE_RBE  		0x2		// Bus error on read
	#define INFO_CODE_WBE  		0x3		// Bus error on write
	#define INFO_CODE_RTO  		0x4		// Bus timeout on read
	#define INFO_CODE_WTO  		0x5		// Bus timeout on write
	#define INFO_CODE_OVERFLOW 	0x6		// TX Buffer overflow
	#define INFO_CODE_UNDERFLOW 0x7		// RX Buffer underflow
	#define INFO_CODE_REQ 	 	0xf		// Outbound request

	// IPBUS Current Header version
	#define HEADER_VERSION 		0x2
#define DEPTH				    0X1
u16_t echo_port = 7;
#define READ 0X00
#define FIFO_DEV_ID	   	XPAR_AXI_FIFO_0_DEVICE_ID
//#define FIFO_DEV_ID_rx	   	XPAR_AXI_FIFO_0_DEVICE_ID

#define WORD_SIZE 4			/* Size of words in bytes */

#define MAX_PACKET_LEN 4

#define NO_OF_PACKETS 16000//16250

#define MAX_DATA_BUFFER_SIZE NO_OF_PACKETS*MAX_PACKET_LEN

#define CALIB_ADC		0X49
#define POWER_ADC		0X48

//#define FIFO_DEV_ID	   	XPAR_AXI_FIFO_0_DEVICE_ID // receiver fifo
//#define FIFO_DEV_ID	   	XPAR_AXI_FIFO_1_DEVICE_ID // transmitter fifo

/************************** Function Prototypes ******************************/
#ifdef XPAR_UARTNS550_0_BASEADDR
static void Uart550_Setup(void);
#endif
/************************** Variable Definitions *****************************/
/*
 * Device instance definitions
 */
XLlFifo FifoInstance;
XIic Iic;
/*****************************************************************************/
void check_fifo(u32  *SourceAddr,u32 data_len,u8 verbose_mode);
int XLlFifoPollingExample(XLlFifo *InstancePtr, u16 DeviceId,u32  *SourceAddr,u32 data_len,u8 verbose_mode);
int TxSend(XLlFifo *InstancePtr, u32 *SourceAddr,u32 data_len,u8 verbose_mode);
u32 RxReceive(XLlFifo *InstancePtr, u32 *DestinationAddr,u8 verbose_mode);
int send_sync_command(XLlFifo *InstancePtr, u16 DeviceId,u32  *SourceAddr,int sd,u8 no_reply);
int Initialize_fifo(XLlFifo *InstancePtr, u16 DeviceId);
void Initialize_everything();
int decode_receive_packet(u8 *recv_buf,u32 *SourceAddr,int sd);
unsigned int crc16(char *data_p, unsigned short length);
void HDLC_Tx(u32 register_address, u16 register_data,u8 mode,u32* SourceAddr,u8 verbose_mode);
u32 HDLC_Rx(u32* Destination_Addr,u8 mode,u8 verbose_mode);
int CalibrateADC(u32 *SourceAddr,u32 *DestinationBuffer,int sd,u8 start,u8 step , u8 stop,u8 calibration_mode);

int CalibrateCAL_DAC(u32 *SourceAddr,u32 *DestinationBuffer,int sd,u8 start,u8 step , u8 stop,u8 calibration_mode);
int CalibrateCAL_DAC_extended(u32 *SourceAddr,u32 *DestinationBuffer,int sd, u8 start,u8 step , u8 stop,u8 calibration_mode);

signed configureADC(u8 ADC_address,u8 channel);
//signed short ReadADC();
signed short ReadADC(u8 ADC_address,u8 channel);
u16 AdjustIref(u32 *SourceAddr,u32 *DestinationBuffer,int sd);
void Initialize_Calpulse_LV1As(u32  *SourceAddr,u16 data_len, u8 CAL_DAC,u16 Latency,u16 num_of_triggers);
u16 send_Calpulse_LV1As(u8 channel,u16 num_of_triggers,u8 delay,u16 D1,u16 D2);//

int Scurve(u32 *SourceAddr,u32 *DestinationBuffer,u8 start_channel,u8 stop_channel, u8 step_channel,u8 start_CALDAC,u8 stop_CALDAC,u8 step_CALDAC,u16 Latency, u16 num_of_triggers,int sd,u8 arm_dac,u8 delay,u16 D1,u16 D2);
int Scurve_extended(u32 *SourceAddr,u32 *DestinationBuffer, u8 start_channel,u8 stop_channel, u8 step_channel,u16 Latency, u16 num_of_triggers,int sd,u8 arm_dac,u8 delay,u16 D1,u16 D2,u8 points,u8 *dac_arr,u8 calpulse);

int DAC_SCANS(u32 *SourceAddr,u32 *DestinationBuffer,int sd,u32 Mon_sel, u8 start,u8 step , u8 stop);
int DecodeDataPacket(u8 channel, u8 CAL_DAC,u16 Latency,u16 num_of_triggers,u8 verbose_mode);
int Transmit_fast_command(XLlFifo *InstancePtr, u32  *SourceAddr,u16 data_len,u8 verbose_mode);
int Receive_fast_command(XLlFifo *InstancePtr, u8 verbose_mode);
void SendReply(int sd,u32 *Buffer, u32 length);
void Print_Buffer( u32 *Buffer,int length ,char * string);
void write_SC( u32 address, u16 data, u32* SourceAddr,u32* DestinationBuffer, u8 verbose_mode );
u32 read_SC( u32 address, u16 data, u32* SourceAddr,u32* DestinationBuffer, u8 verbose_mode );
void test_controller(void);
int BIST(int sd);
void ext_reset(u8 value,int sd);
void assert_reset(void);
void Set_front_end_Nominal(u32 *SourceAddr,u32 *DestinationBuffer);
void Set_front_end(u32 *SourceAddr,u32 *DestinationBuffer,u8 PRE_I_BLCC_reg,u8 PRE_VREF_reg,u8 PRE_I_BSF_reg,u8 PRE_I_BIT_reg,u8 SH_I_BFCAS_reg,u8 SH_I_BDIFF_reg,u8 SD_I_BDIFF_reg,u8 SD_I_BSF_reg,u8 SD_I_BFCAS_reg);
/***************** Macros (Inline Functions) Definitions *********************/
//int send_sync_command(XLlFifo *InstancePtr_tx);

//volatile u32 SourceBuffer[MAX_DATA_BUFFER_SIZE * WORD_SIZE];
//volatile u32 FastBuffer[MAX_DATA_BUFFER_SIZE * WORD_SIZE];
volatile u32  SourceBuffer[MAX_DATA_BUFFER_SIZE * WORD_SIZE];
volatile u32  DestinationBuffer[MAX_DATA_BUFFER_SIZE * WORD_SIZE];
 volatile u32 ReceiveLength;
u16 MON_SEL_BITS	=	0;
u16 MON_GAIN_BITS	=	0;
u16 VREF_ADC_BITS	=	0;
u16 CAL_DAC;
//u8* datapacket = XPAR_BRAM_0_BASEADDR;
u8 datapacket[27];
u8 Error_Dpkt;
volatile u16   Hit_array[128][256]={0};
volatile u8    mask_array[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
u32 DATA_GBL_CFG_CAL_0;
//XLlFifo *InstancePtr;
XLlFifo_Config *Config;
char TYPE_ID;
short LUT_CAL_DAC[512*4];
volatile u16 scurve_arr[256];
static u32 frame_len;//USED IN RXRECEIVE FUNCTION
volatile char	h0,h1,h2,h3;
volatile u32 register_address;
volatile u16 register_data;
volatile u16 read_data;
//int sd;
#define RECV_BUF_SIZE  2048
u8 recv_buf[RECV_BUF_SIZE];
u8 array_size;
u8 dac_arr[256];
	int n, nwrote;
u32 i=0,j=0;
//volatile u32  __attribute__((section (".base_var_sec")))  i,j;
//u32* try =  XPAR_MICROBLAZE_0_LOCAL_MEMORY_ILMB_BRAM_IF_CNTLR_BASEADDR;

//unsigned short DEPTH=0X01;
union{
			unsigned short crc;

			struct{
			u8 crc_lsb;
			u8 crc_msb;
				  }st1;
			}ut1;

//IP_BUS_HEADER

			//u8 TxMsgPtr []={0x01,0x81,0x00};//default data rate
			u8 TxMsgPtr []={0x01,0xc5,0xE3};//max data rate   byte 1 = 0xc4 ch0, 0xd4 ch1
				u8 TxMsgPtr_conversion []={0x00};
				u8 TxMsgPtr_config[]={0x01};
				u8 RcvBuffer [2]={0x00,0x00};
				unsigned Num_Bytes_received;
				u16 config_register;
				//XIic * 	InstancePtr;

				int ByteCount=sizeof(TxMsgPtr);

int     crc_ok = 0x470F;

u8 reverse(u8 b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}
u8 flag=0;

int BIST(int sd)

{

	int Status = XST_FAILURE;
	u8 BIST_END;
	u32 BIST_cnt;
	u32* p = &BIST_cnt;
#ifdef	XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR
	XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,4 * WORD_SIZE, 0);    //ensure bist is not running
	//XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_BIST_CYCLES_BIST_CYCLES_FROM_TX_CONTROLLER_BASEADDR,4,1);//As input
	//XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_DONE_BIST_BIST_END_FROM_TX_CONTROLLER_BASEADDR,4,1);//As input





		BIST_cnt = XGpio_ReadReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,8 *WORD_SIZE);
	     //xil_printf("\n\LAST VALUE OF COUNT = %d \n\r", BIST_cnt);


		XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,5 * WORD_SIZE, 1);//CLEAR COUNTER
		XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,5 * WORD_SIZE, 0);//deassert reset
		BIST_cnt = XGpio_ReadReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,8 *WORD_SIZE);
			     //xil_printf("\n\current value after a rest  = %d \n\r", BIST_cnt);
			     xil_printf("\n\starting test \n\r");

			     i=0;
		XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,4 * WORD_SIZE, 1);    //start BIST test


		do{

			BIST_END = XGpio_ReadReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,10 * WORD_SIZE);// read bist end gpio pin
			//xil_printf("bist_end = %d \n\r",BIST_END);
		}while(BIST_END==0 && ++i<  2160604);
		XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,4 * WORD_SIZE, 0);
		//usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);
		//usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);
		//usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);
		if(i>=2160604)
		{
			BIST_cnt=-1;
			//SendReply(sd,p,4);
			xil_printf("bist_end = %d   ERROR, \n\r",BIST_END);

		}
		else{
		xil_printf("bist_end = %d , \n\r",BIST_END);
		XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,4 * WORD_SIZE, 0);    //stop BIST test
		BIST_cnt = XGpio_ReadReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,8* WORD_SIZE);
		xil_printf("\n\Final value BIST cycles  = %d , test finished\n\r", BIST_cnt);
		//SendReply(sd,p,4);
		}
		SendReply(sd,p,4);
		XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,5 * WORD_SIZE, 1);//CLEAR bist_end
		XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,5 * WORD_SIZE, 0);//deassert reset

		if(BIST_END)Status = XST_SUCCESS;


	#endif



	return Status;








}
void ext_reset(u8 value,int sd)
{
	u32 ack;
#ifdef 	XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR


		XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,28, value);//ext_rst is 0
	ack = 0;


#endif

#ifndef 	XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR
	ack = -1;

#endif

	SendReply(sd,DestinationBuffer,4);

}


void assert_reset(void)
{

#ifdef 	XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR

		XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,6*4, 0x00);//it should be zero its bits inverse signal for transceiver

		XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,7*4, 1);//ext_rst is 1 active = RESET VFAT3


		XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,7*4, 0);//ext_rst is 0
		//XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,28, 1);//ext_rst is 1
	//	usleep(10000);

	//	usleep(10000);usleep(10000);usleep(10000);usleep(10000);usleep(10000);
						//usleep(10000);usleep(10000);usleep(10000);usleep(10000);
	//	usleep(10000);usleep(10000);usleep(10000);

		//XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,28, 0x00);//ext_rst is 0






		//


		//XGpio_WriteReg((XPAR_REVERSE_TXD_TX_REVERSE_BASEADDR),0, 1);

		xil_printf("\r\n transceiver direction reversed and inverted \r\n");

#endif




}

int Initialize_FIFO(u16 DeviceId)

{
	int Status;
xil_printf("\n\r--- Initializing FIFO ---\n\r");

		Status = Initialize_fifo(&FifoInstance, FIFO_DEV_ID);
		if (Status != XST_SUCCESS) {
			xil_printf("Axi Streaming TX FIFO Initialization failed\n\r");
			xil_printf("--- Exiting main() ---\n\r");
			return XST_FAILURE;
		}

		xil_printf("Successfully Initialized Axi Streaming FIFO \n\r");
		//xil_printf("--- Exiting check_fifo() ---\n\r");

		//return XST_SUCCESS;
/* Initialize the Device Configuration Interface driver */
Config = XLlFfio_LookupConfig(DeviceId);
if (!Config) {
	xil_printf("No config found for %d\r\n", DeviceId);
	return XST_FAILURE;
}



/*
 * This is where the virtual address would be used, this example
 * uses physical address.
 */
Status = XLlFifo_CfgInitialize(&FifoInstance, Config, Config->BaseAddress);
if (Status != XST_SUCCESS) {
	xil_printf("Initialization failed\n\r");
	return Status;
}




/* Check for the Reset value */
Status = XLlFifo_Status(&FifoInstance);
XLlFifo_IntClear(&FifoInstance,0xffffffff);
Status = XLlFifo_Status(&FifoInstance);
if(Status != 0x0) {
	xil_printf("\n ERROR : Reset value of ISR0 : 0x%x\t"
		    "Expected : 0x0\n\r",
		    XLlFifo_Status(&FifoInstance));
	return XST_FAILURE;
}


XLlFifo_Reset(&FifoInstance);

//xil_printf("vacancy= %d\r\n",XLlFifo_iTxVacancy(&FifoInstance));

}
int send_sync_command(XLlFifo *InstancePtr, u16 DeviceId,u32  *SourceAddr,int sd,u8 no_reply)// sends 03 sync commands to the chip hard sync

{
	int Status;

	u32 i=0;
	u8 success;
	u32 Rcv_len=0;
	u32 data_len=3;
//u8 verbose_mode
/*

#ifdef 	XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR

		XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,6*4, 0x00);//it should be zero its bits inverse signal for transceiver

		//XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,7*4, 1);//ext_rst is 1 active = RESET VFAT3


	//	XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,7*4, 0);//ext_rst is 0
		//XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,28, 1);//ext_rst is 1
	//	usleep(10000);

	//	usleep(10000);usleep(10000);usleep(10000);usleep(10000);usleep(10000);
						//usleep(10000);usleep(10000);usleep(10000);usleep(10000);
	//	usleep(10000);usleep(10000);usleep(10000);

		//XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,28, 0x00);//ext_rst is 0






		//


		//XGpio_WriteReg((XPAR_REVERSE_TXD_TX_REVERSE_BASEADDR),0, 1);

		xil_printf("\r\n transceiver direction reversed and inverted \r\n");

#endif
		*/
//////////////////////end setting vbrad board reqirements////////////////////////

/*
#ifdef 		XPAR_BIT_SLIP_BASEADDR
			#ifdef 		XPAR_SUCCESS_BASEADDR
				//ch=!ch;
			//xil_printf("\r\n Aligning bits . ");
			XGpio_WriteReg((XPAR_BIT_SLIP_BASEADDR),	4, 0);//set as OUTPUT
						XGpio_WriteReg((XPAR_SUCCESS_BASEADDR),	4, 1);//set as INPUT
			do{




				//if(recv_buf[1]==0xaa)//bit slip pulse
				{
				XGpio_WriteReg((XPAR_BIT_SLIP_BASEADDR),0, 0);//
				XGpio_WriteReg((XPAR_BIT_SLIP_BASEADDR),0, 1);//rising edge of bitslip signal from microblaze
				XGpio_WriteReg((XPAR_BIT_SLIP_BASEADDR),0, 0);//

				xil_printf(".");
				}
						//xil_printf("Hi Henri i receive ur message\r\n %x,%x %x",recv_buf[0],recv_buf[1],recv_buf[2],recv_buf[3]);
				usleep(1000);usleep(1000);usleep(1000);
				//usleep(1000);usleep(1000);usleep(1000);

				success = XGpio_ReadReg((XPAR_SUCCESS_BASEADDR),0);
				//XLlFifo_Reset(&FifoInstance);
				xil_printf(" success =%d, i= %d\n\r", success, i);
			}while(success==0 && i++ <  50);

		#endif
	#endif

*/

	Initialize_FIFO(DeviceId);
		xil_printf("\n\r--- Entering send_sync_command() ---\n\r");
		//xil_printf("\n\r sourceBuffer  address = %p \n\r",(void*)SourceBuffer);
		//xil_printf("\n\r source_address = %p \n\r",(void*)SourceAddr);

		//xil_printf("\n\r source address = %p \n\r",(void *)SourceAddr);
		//XLlFifo_Config *Config;

			//int Status;
		//	int i;
			int Error;
			Status = XST_SUCCESS;

			/* Initial setup for Uart16550 */
/*
		#ifdef XPAR_UARTNS550_0_BASEADDR

			Uart550_Setup();

		#endif
*/





#ifdef 		XPAR_BIT_SLIP_BASEADDR
			#ifdef 		XPAR_SUCCESS_BASEADDR
				//ch=!ch;
			//xil_printf("\r\n Aligning bits . ");
			XGpio_WriteReg((XPAR_BIT_SLIP_BASEADDR),	4, 0);//set as OUTPUT
						XGpio_WriteReg((XPAR_SUCCESS_BASEADDR),	4, 1);//set as INPUT
			do{




				//if(recv_buf[1]==0xaa)//bit slip pulse
				{
				XGpio_WriteReg((XPAR_BIT_SLIP_BASEADDR),0, 0);//
				XGpio_WriteReg((XPAR_BIT_SLIP_BASEADDR),0, 1);//rising edge of bitslip signal from microblaze
				XGpio_WriteReg((XPAR_BIT_SLIP_BASEADDR),0, 0);//

				xil_printf(".");
				}
						//xil_printf("Hi Henri i receive ur message\r\n %x,%x %x",recv_buf[0],recv_buf[1],recv_buf[2],recv_buf[3]);
				usleep(1000);usleep(1000);usleep(1000);
				//usleep(1000);usleep(1000);usleep(1000);
				success = XGpio_ReadReg((XPAR_SUCCESS_BASEADDR),0);
				//XLlFifo_Reset(&FifoInstance);
				xil_printf(" success =%d, i= %d\n\r", success, i);
			}while(success==0 && i++ <  50);

		#endif
	#endif

			///XLlFifo_Reset(&FifoInstance);
			/* Transmit the Data Stream */

			for (i=0;i<data_len;i++)
							{
								 *(SourceAddr + i) = 0x17;

							}
			XLlFifo_Reset(&FifoInstance);
			Status = TxSend(&FifoInstance, SourceAddr,data_len,0);
			if (Status != XST_SUCCESS){
				xil_printf("Transmisson of Data failed\n\r");
				return XST_FAILURE;
			}





			/* Revceive the Data Stream */
			Rcv_len = RxReceive(&FifoInstance, DestinationBuffer,0);

			for(i=0;i<Rcv_len;i++)
				xil_printf("receive(%d)=%x \r\n", i,*(DestinationBuffer+i));


			if (Status != XST_SUCCESS){
				xil_printf("Receiving data failed");
				return XST_FAILURE;
			}

			Error = 0;
			u16 sync_ok=0;
			if(Rcv_len<MAX_DATA_BUFFER_SIZE)
			{
				xil_printf("receivelength = %d\r\n",Rcv_len);
			for(i=0;i<Rcv_len;i++)
				if((*(DestinationBuffer+i) & 0x000000ff)== 0x3a ) sync_ok=1; else sync_ok=0;
			if(sync_ok==1)
			xil_printf("Sync Ok\r\n ");
			else
			xil_printf("NO Sync \r\n ");
			}
			else
			{
				xil_printf("No sync\r\n");
				XLlFifo_Reset(&FifoInstance);
			}
			if(no_reply==0)
			SendReply(sd,DestinationBuffer,1);
			/* handle request
					if ((nwrote = write(sd, DestinationBuffer, Rcv_len)) < 0) {
						xil_printf("%s: ERROR responding to client echo request. received = %d, written = %d\r\n",
								__FUNCTION__, n, nwrote);
						xil_printf("Closing socket %d\r\n", sd);
					}*/
			/* Compare the data send with the data received */
			//xil_printf(" destination buffer[0] = %x \n\r",DestinationBuffer[0]);
			//for( i=0 ; i<MAX_DATA_BUFFER_SIZE ; i++ ){
			//	if ( *(SourceBuffer + i) != *(DestinationBuffer + i) ){
			//		Error = 1;
			//		break;
			//	}
			//return Status;
		/*	///////////////////ALIGN LVDS RECEIVER BLOCK/////////////////

		#ifdef 		XPAR_BIT_SLIP_BASEADDR
			#ifdef 		XPAR_SUCCESS_BASEADDR
				//ch=!ch;
			//xil_printf("\r\n Aligning bits . ");
			do{


			XGpio_WriteReg((XPAR_BIT_SLIP_BASEADDR),	4, 0);//set as OUTPUT
			XGpio_WriteReg((XPAR_SUCCESS_BASEADDR),	4, 1);//set as INPUT

				//if(recv_buf[1]==0xaa)//bit slip pulse
				{
				XGpio_WriteReg((XPAR_BIT_SLIP_BASEADDR),0, 1);//rising edge of bitslip signal from microblaze
				XGpio_WriteReg((XPAR_BIT_SLIP_BASEADDR),0, 0);//

				xil_printf(".");
				}
						//xil_printf("Hi Henri i receive ur message\r\n %x,%x %x",recv_buf[0],recv_buf[1],recv_buf[2],recv_buf[3]);
				usleep(1000);usleep(1000);usleep(1000);
				success = XGpio_ReadReg((XPAR_SUCCESS_BASEADDR),0);

			}while(success!=1);
			xil_printf("\r\n Aligning  done successfully\r\n");
			#endif
		#endif
		/////////////////END BIT ALIGNMENT BLOCK////////////////////*/


			XLlFifo_Reset(&FifoInstance);
			configureADC(CALIB_ADC,0);
			//SendReply(sd,DestinationBuffer,Rcv_len);
		return Rcv_len;

}

int Transmit_fast_command(XLlFifo *InstancePtr, u32  *SourceAddr,u16 data_len,u8 verbose_mode)
{
	u8 Status;
	/* Transmit the Data Stream */
				Status = TxSend(&FifoInstance, SourceAddr,data_len,verbose_mode);
				if (Status != XST_SUCCESS){
					xil_printf("Transmisson of Data failed\n\r");
					return XST_FAILURE;
				}


return 0;
}

int Receive_fast_command(XLlFifo *InstancePtr, u8 verbose_mode)
{

	u32 i,j;
	u32 Rcv_len;
	u8 Header;
	u8 loop;
	u8 indexx;
	u8 PKT_LEN=22;
	//u32* Rcv;
	//Rcv = (u32)xil_calloc(27000,sizeof(u32));
	//if(Rcv==NULL){
	//		xil_printf("memory allocation fail\r\n");
	//		return 0;
	//		}


	Rcv_len = RxReceive(&FifoInstance, DestinationBuffer,verbose_mode);
	//xil_printf("Rcv_len=%d\r\n", Rcv_len);
	   							//	if (Status != XST_SUCCESS){
	   							//		xil_printf("Receiving data failed");
	   							//		return XST_FAILURE;
	   							//	}

	   								//Error = 0;

	   								if(Rcv_len>0 && Rcv_len<MAX_DATA_BUFFER_SIZE)
	   								{
	   									xil_printf("rcv_len=%d\r\n",Rcv_len);
	   									return Rcv_len;
	   								//for(i=0;i<Rcv_len;i++)
	   									/////////////////find header of data packet///////////



	   								}
	   								else
	   								{
	   									xil_printf("No data received\r\n");
	   									XLlFifo_Reset(&FifoInstance);
	   									return 0;
	   								}




}
void Print_Buffer(u32 *Buffer , int length ,char *string)
{
for(int i=0 ; i<length; i++)
	xil_printf("%s  %d = %08x\r\n",string,i, *(Buffer+i));



}

/*void Initialize_Calpulse_LV1As(u32  *SourceAddr,u16 data_len, u8 CAL_DAC,u16 Latency,u16 num_of_triggers)
{

	u32 i,j;
	////////////fill the tx buffer with cal pulse and lv1as


				for(i=0;i<num_of_triggers;i++)
							{
							for(j=0;j<data_len;j++)
											{
												//	if(j==0)
												//	*(SourceAddr + j) = RUN_MODE;//FAST COMMAND
												 if(j==0)
													*(SourceAddr + j + i*(Latency+2))  = CAL_PULSE;//FAST COMMAND

												else if(j==6)
													*(SourceAddr + j+ i*(Latency+2)) = LV1A;

												else if (j % 2==0)
													*(SourceAddr + j+i*(Latency+2)) = 0x00;

												else
													*(SourceAddr + j+i*(Latency+2))  = 0xff;

										//	xil_printf("j= %d, i= %d, index=%d SourceBuffer=%x\r\n",j,i,(j + i*(Latency+2)),*((SourceAddr + j) + i*(Latency+2)) );

											}
							}







}*/

/*void Initialize_Calpulse_LV1As(u32  *SourceAddr,u16 data_len, u8 CAL_DAC,u16 Latency,u16 num_of_triggers)
{

	u32 i,j;
	////////////fill the tx buffer with cal pulse and lv1as
#ifdef XPAR_TX_CONTROLLER_0_BASEADDR

	XGpio_WriteReg(XPAR_TX_CONTROLLER_0_BASEADDR,1, 10);//lv1a
	XGpio_WriteReg(XPAR_TX_CONTROLLER_0_BASEADDR,2, 500);    //delay2
	XGpio_WriteReg(XPAR_TX_CONTROLLER_0_BASEADDR,3, 5);  //delay1
	XGpio_WriteReg(XPAR_TX_CONTROLLER_0_BASEADDR,0, 1);// start
	XGpio_WriteReg(XPAR_TX_CONTROLLER_0_BASEADDR,0, 0);
	usleep(1000);
#endif

}*/

void test_controller()

{
#ifdef XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR



	XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,4, 10);//lv1a
	XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,8, 500);    //delay2
	XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,12, 5);  //delay1
	XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,0,0x800);// start
	//usleep(1000);usleep(1000);
	//XGpio_WriteReg(XPAR_TX_CONTROLLER_0_BASEADDR,0, 0);
	//usleep(1000);

	u32 lv1a, delay1,delay2,start;
	lv1a = XGpio_ReadReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,4);//lv1a
	delay1 = XGpio_ReadReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,12);//lv1a
	delay2 = XGpio_ReadReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,8);//lv1a
	start = XGpio_ReadReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,0);//lv1a

	xil_printf("lv1a %d  delay1 %d delay2  %d  start %d\r\n",lv1a,delay1,delay2,start);

#endif


}


/*int send_Calpulse_LV1As(u32  *SourceAddr,u16 data_len,u8 channel, u8 CAL_DAC,u16 Latency,u16 num_of_triggers, u8 verbose_mode)//

{
	int Status;
	Status = XST_SUCCESS;







			Status = TxSend(&FifoInstance, SourceAddr,data_len*num_of_triggers,verbose_mode);
			if (Status != XST_SUCCESS)
			{
				xil_printf("Transmisson of Data failed\n\r");
				return XST_FAILURE;
			}




		return XST_SUCCESS;

}
*/

u16 send_Calpulse_LV1As(u8 channel,u16 num_of_triggers,u8 delay, u16 D1, u16 D2)//

{
	int Status;
	Status = XST_SUCCESS;

#ifdef XPAR_RX_CONTROLLER_0_S00_AXI_BASEADDR
#ifdef XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR


	XGpio_WriteReg(XPAR_RX_CONTROLLER_0_S00_AXI_BASEADDR,2*4, num_of_triggers);//lv1a
	XGpio_WriteReg(XPAR_RX_CONTROLLER_0_S00_AXI_BASEADDR,3*4,channel);    //channel number
	 u32 busy=XGpio_ReadReg(XPAR_RX_CONTROLLER_0_S00_AXI_BASEADDR,1*4);  //busy or not
	// xil_printf("\n\r initial status:busy = %d\n\r",busy);

	XGpio_WriteReg(XPAR_RX_CONTROLLER_0_S00_AXI_BASEADDR,0, 1);// start ip
	XGpio_WriteReg(XPAR_RX_CONTROLLER_0_S00_AXI_BASEADDR,0, 0);//
//	do{
//	busy=XGpio_ReadReg(XPAR_RX_CONTROLLER_0_S00_AXI_BASEADDR,1*4);
//	//xil_printf("\n\r busy = %d\n\r",busy);
//	}while (busy!=1);//ensure ip is started
	//usleep(1);

	//usleep(delay);
	//xil_printf("\n\r Inside rx_controller executed ---\n\r");







	XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,4, num_of_triggers);//lv1a
	XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,12, D1);    //delay1
	XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,8, D2);  //delay2
	XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,0, 1);// start
	//usleep(1);
	XGpio_WriteReg(XPAR_TX_CONTROLLER_HIER_TX_CONTROLLER_0_BASEADDR,0, 0);
	//usleep(delay);
	//xil_printf("\n\rsend calpulse executed ---\n\r");


	do{
		busy=XGpio_ReadReg(XPAR_RX_CONTROLLER_0_S00_AXI_BASEADDR,1* WORD_SIZE);
		//xil_printf("\n\r busy = %d\n\r",busy);
		}while (busy!=0);//ensure ip has finished its task

	u16 hit_cnt = XGpio_ReadReg(XPAR_RX_CONTROLLER_0_S00_AXI_BASEADDR,4*WORD_SIZE);
	//xil_printf("%d\t",hit_cnt);

#endif
#endif


return (hit_cnt);

}


void Initialize_everything()

{
	//int i=0;
	int Status;
	u8 success;
flag=1;

		xil_printf("\n\r--- Initializing fifo ---\n\r");

		Status = Initialize_fifo(&FifoInstance, FIFO_DEV_ID);
		if (Status != XST_SUCCESS) {
			xil_printf("Axi Streaming TX FIFO Initialization failed\n\r");
			xil_printf("--- Exiting main() ---\n\r");
			return XST_FAILURE;
		}

		xil_printf("Successfully Initialized Axi Streaming FIFO \n\r");
		//xil_printf("--- Exiting check_fifo() ---\n\r");

		//return XST_SUCCESS;







/*
	/////////////////////////
#ifdef 		XPAR_TRANSMITTER_LOGIC_OUT_MUX_CONTROL_BASEADDR
		//ch=!ch;
		XGpio_WriteReg((XPAR_TRANSMITTER_LOGIC_OUT_MUX_CONTROL_BASEADDR),
				4, 0);

		XGpio_WriteReg((XPAR_TRANSMITTER_LOGIC_OUT_MUX_CONTROL_BASEADDR),
						0, 0);
		//xil_printf("\n\r receive packet %x %x %x %x\n \r",recv_buf[0],recv_buf[1],recv_buf[2],recv_buf[3],recv_buf[4],recv_buf[5]);
#endif
*/

////////////////////////////////////////////////////////////////////////

//Initialize_TX_fifo(XLlFifo *InstancePtr_tx, u16 DeviceId_tx);
//Initialize_RX_fifo();
///////////////// set transceivers according to vbrad board////////////////////////////

#ifdef 	XPAR_REVERSE_RXD_RX_REVERSE_BASEADDR
	#ifdef	XPAR_INVERT_RXD_RX_INVERSE_BASEADDR
		#ifdef XPAR_INVERT_TXD_TX_INVERT_BASEADDR
			#ifdef XPAR_REVERSE_TXD_TX_REVERSE_BASEADDR
		XGpio_WriteReg((XPAR_REVERSE_RXD_RX_REVERSE_BASEADDR),4, 0);
		XGpio_WriteReg((XPAR_REVERSE_RXD_RX_REVERSE_BASEADDR),0, 1);

		XGpio_WriteReg((XPAR_INVERT_RXD_RX_INVERSE_BASEADDR),4, 0);
		XGpio_WriteReg((XPAR_INVERT_RXD_RX_INVERSE_BASEADDR),0, 0);

		XGpio_WriteReg((XPAR_INVERT_TXD_TX_INVERT_BASEADDR),4, 0);
		XGpio_WriteReg((XPAR_INVERT_TXD_TX_INVERT_BASEADDR),0, 1);

		XGpio_WriteReg((XPAR_REVERSE_TXD_TX_REVERSE_BASEADDR),4, 0);
		XGpio_WriteReg((XPAR_REVERSE_TXD_TX_REVERSE_BASEADDR),0, 1);

		xil_printf("\r\n transceiver direction reversed and inverted \r\n");
			#endif
		#endif
	#endif
#endif
//////////////////////end setting vbrad board reqirements////////////////////////

	///////////////////ALIGN LVDS RECEIVER BLOCK/////////////////

#ifdef 		XPAR_BIT_SLIP_BASEADDR
	#ifdef 		XPAR_SUCCESS_BASEADDR
		//ch=!ch;
	xil_printf("\r\n Aligning bits . ");
	do{


	XGpio_WriteReg((XPAR_BIT_SLIP_BASEADDR),	4, 0);//set as OUTPUT
	XGpio_WriteReg((XPAR_SUCCESS_BASEADDR),	4, 1);//set as INPUT

		//if(recv_buf[1]==0xaa)//bit slip pulse
		{
		XGpio_WriteReg((XPAR_BIT_SLIP_BASEADDR),0, 1);//rising edge of bitslip signal from microblaze
		XGpio_WriteReg((XPAR_BIT_SLIP_BASEADDR),0, 0);//

		xil_printf(".");
		}
				//xil_printf("Hi Henri i receive ur message\r\n %x,%x %x",recv_buf[0],recv_buf[1],recv_buf[2],recv_buf[3]);
		usleep(1000);usleep(1000);usleep(1000);
		success = XGpio_ReadReg((XPAR_SUCCESS_BASEADDR),0);

	}while(success!=1);
	xil_printf("\r\n Aligning  done successfully\r\n");
	#endif
#endif
/////////////////END BIT ALIGNMENT BLOCK////////////////////


	return XST_SUCCESS;


}
unsigned short swapBytes(unsigned short x)
{
    return ( (x & 0x00ff)<<8 | (x & 0xff00)>>8 );
}


int decode_receive_packet(u8 *recv_buf,u32  *SourceAddr,int sd)
{
	u8 start, step,stop;
	u8 start_channel,stop_channel,step_channel;
	u8 start_CALDAC,stop_CALDAC,step_CALDAC;
	u32 Error_code=0;
	int Status;int i;
	u8 verbose_mode=0;
	Status= XST_SUCCESS;
	u32 mon_sel;
	char data_crc[]={0x00,0x03,0x1f,0x06,0x01,0x20,0x81,0x00,0x00,0x00,0x07,0x20,0x00,0x00};
	//char data_crc[]={0x00,0x03,0x0f,0x05,0x01,0x20,0x81,0x00,0x00,0x00};
		unsigned int crc;
		signed short RESULT;
		u16 Latency=6,num_of_triggers=100;
		u32 Read_data;
		u32 *RD ;
		u8 arm_dac;
		u8 delay;
		u16 D1,D2;
		u8 calpulse;
	//int RECV_BUF_SIZE = 2048;
	//u8 *recv_buf;
//xil_printf("Inside decode packet\r\n");
//xil_printf("%x , %x , %x, %x ", *recv_buf ,*(recv_buf+1),*(recv_buf+2),*(recv_buf+3));
//send_sync_command(&FifoInstance, FIFO_DEV_ID,SourceAddr,sd,0);
//if(flag==0)
//{send_sync_command(&FifoInstance, FIFO_DEV_ID,SourceAddr);
//send_sync_command(&FifoInstance, FIFO_DEV_ID,SourceAddr);
//send_sync_command(&FifoInstance, FIFO_DEV_ID,SourceAddr);
//flag=1;


//}
if(*recv_buf==0xca){
	if (*(recv_buf+1)==0x00 && *(recv_buf+2)==0x00)
	{
		xil_printf("In Fast command block \r\n");
		u16 length_FC= (*(recv_buf+3)<<8)| *(recv_buf+4);
		for(i=0;i< length_FC;i++)
			*(SourceAddr+i)= (u32)*(recv_buf+i+5);
		char* string= "Tx_data";

			Print_Buffer(SourceAddr,length_FC,string);




		Transmit_fast_command(&FifoInstance,SourceAddr ,length_FC,verbose_mode);
		int Rcv_length= Receive_fast_command(&FifoInstance,verbose_mode);
		string= "Rx_data";
		Print_Buffer( DestinationBuffer,Rcv_length, string );
		SendReply(sd,DestinationBuffer,Rcv_length*4);
	}
	else if (*(recv_buf+1)==0x00 && *(recv_buf+2)==0x01){
		xil_printf("In Slow control mode\r\n");
		u32 register_address;
			u32 register_data;
			//u8 verbose_mode=0;
			u8 mode=0;
			//u32 read_data;
			//u32 adc_0,adc_1;
			//double imon_adc,vmon_adc;
			/////////////write to GBL_CFG_RUN register for RUN bit=1////////////
		/*	     register_address = GBL_CFG_RUN;
			     register_data    = 0X00000001;//SLEEP/RUN BIT=1;
			     mode             = 1;//write transaction on hdlc slow control
			     HDLC_Tx(register_address , register_data,mode,SourceAddr,verbose_mode);//transmit of sc data
			     //usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);
			     //usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);
			     HDLC_Rx(DestinationBuffer,mode,verbose_mode);//receive acknowledge of sc data
			////////////////////////////////////////////////////////////////////*/

		 register_address = *(recv_buf+7) | (*(recv_buf+6)<<8) | (*(recv_buf+5)<<16) | (*(recv_buf+4)<<24);
		 register_data = *(recv_buf+11) | (*(recv_buf+10)<<8) | (*(recv_buf+9)<<16) | (*(recv_buf+8)<<24);
		 mode = *(recv_buf+3);
		xil_printf("register_address=%d\r\n",register_address);
		xil_printf("write data=%d\r\n",register_data);
		xil_printf("mode =%x\r\n",mode);
		//xil_printf("write_data =%x :: %d,\r\n",mode);
		verbose_mode=0;
		if(mode ==0)
		Read_data = read_SC( register_address, register_data, SourceAddr,DestinationBuffer, verbose_mode );
		else if(mode ==1)	write_SC( register_address, register_data, SourceAddr,DestinationBuffer,verbose_mode );
		//HDLC_Tx(register_address , register_data,mode,SourceAddr,verbose_mode);
		//usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);
		//usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);
		//Read_data = HDLC_Rx(DestinationBuffer,mode,verbose_mode);

		RD = &Read_data;
		xil_printf("HDLC_ read_data= %x\r\n" ,Read_data),
		SendReply(sd, RD,1 * 4);
	}


	else if (*(recv_buf+1)==0x00 && *(recv_buf+2)==0x02){
			xil_printf("\n\rHard reset vfat3\r\n");
			assert_reset();
			send_sync_command(&FifoInstance, FIFO_DEV_ID,SourceAddr,sd,1);
			send_sync_command(&FifoInstance, FIFO_DEV_ID,SourceAddr,sd,0);
			//send_sync_command(&FifoInstance, FIFO_DEV_ID,SourceAddr,verbose_mode);
			//send_sync_command(&FifoInstance, FIFO_DEV_ID,SourceAddr,verbose_mode);


	}
	else if (*(recv_buf+1)==0x00 && *(recv_buf+2)==0x03)
		{
			xil_printf("\n\rIn external ADC Read \r\n");
			//i=0;
			//do{
				u8 adc_address = *(recv_buf+3);
				u8 channel= *(recv_buf+4);
				if(adc_address == 0x48 || adc_address == 0x49 || channel ==0 || channel == 1 || channel == 2 || channel == 3   )
				{
				//usleep(1000);usleep(1000);usleep(1000);
			RESULT=ReadADC(adc_address,channel);
				}
				else {
					RESULT = -1;
					xil_printf("Wrong address passed\r\n");
				}
			Read_data = RESULT;
			RD = &Read_data;
			SendReply(sd, RD,2);
			xil_printf("Result = %x mv\r\n",RESULT);
				//i++;
			//}while(i<1);
		}

	else if (*(recv_buf+1)==0x00 && *(recv_buf+2)==0x04){
				xil_printf("\n\rEntering VFAT3 EXT-RESET\r\n");
				xil_printf("\n\rreset >> %d\r\n", *(recv_buf+3));
				ext_reset(*(recv_buf+3),sd);

		}

	else if (*(recv_buf+1)==0x00 && *(recv_buf+2)==0x05){
					xil_printf("\n\rEntering in Iref adjustment mode function\r\n");
					AdjustIref(SourceAddr,DestinationBuffer,sd);
					//RD = &Read_data;
					//SendReply(sd, RD,1 * 4);


		}

	else if (*(recv_buf+1)==0x00 && *(recv_buf+2)==0x06){
						xil_printf("%c[2J",27);//clear terminal
						//clrscr();
						xil_printf("\n\rEntering in INTERNAL ADC calibration routine\r\n");
						 start = *(recv_buf+3);
													step = *(recv_buf+4);
													stop = *(recv_buf+5);
													xil_printf("start = %d :: step_size = %d :: stop = %d :: \r\n",start,step,stop);
													if(  start >=0 &&  start < stop  && step<=(stop-start) &&  step  > 0    )
														CalibrateADC(SourceAddr,DestinationBuffer,sd,start,step , stop,0);
													else
													{
														Error_code = -1;
														RD = &Error_code;
														SendReply(sd, RD,1 * 4);
														xil_printf("Error in setting of scan steps , Please adjust values\r\n");
													}



			}
	else if (*(recv_buf+1)==0x00 && *(recv_buf+2)==0x07){
							//xil_printf("%c[2J",27);//clear terminal
							//clrscr();
							xil_printf("\n\rEntering in cal_dac calibration routine\r\n");
							//u32 register_address = *(recv_buf+7) | (*(recv_buf+6)<<8) | (*(recv_buf+5)<<16) | (*(recv_buf+4)<<24);
							 start = *(recv_buf+3);
							step = *(recv_buf+4);
							stop = *(recv_buf+5);
							xil_printf("start = %d :: step_size = %d :: stop = %d :: \r\n",start,step,stop);
							if(  start >=0 &&  start < stop  && step<=(stop-start) &&  step  > 0    )
							CalibrateCAL_DAC(SourceAddr,DestinationBuffer,sd,start,step,stop,0);
							else
							{
								Error_code = -1;
								RD = &Error_code;
								SendReply(sd, RD,1 * 4);
								xil_printf("Error in setting of scan steps , Please adjust values\r\n");
							}

				}


	else if (*(recv_buf+1)==0x00 && *(recv_buf+2)==0x09){
								//xil_printf("%c[2J",27);//clear terminal
								//clrscr();
								xil_printf("\n\rEntering in DAC SCAN calibration routine\r\n");
								//u32 register_address = *(recv_buf+7) | (*(recv_buf+6)<<8) | (*(recv_buf+5)<<16) | (*(recv_buf+4)<<24);
								mon_sel =  *(recv_buf+9) | (*(recv_buf+8)<<8) | (*(recv_buf+7)<<16) | (*(recv_buf+6)<<24);
								start = *(recv_buf+3);
								step = *(recv_buf+4);
								stop = *(recv_buf+5);

								xil_printf("start = %d :: step_size = %d :: stop = %d :: Mon_sel = %d\r\n",start,step,stop,mon_sel);
								if(  start >=0 &&  start < stop  && step<=(stop-start) &&  step  > 0    )
								DAC_SCANS(SourceAddr,DestinationBuffer,sd,mon_sel,start,step,stop);
								else
								{
									Error_code = -1;
									RD = &Error_code;
									SendReply(sd, RD,1 * 4);
									xil_printf("Error in setting of scan steps , Please adjust values\r\n");
								}

					}


	else if (*(recv_buf+1)==0x00 && *(recv_buf+2)==0x08)
				{
							//xil_printf("%c[2J",27);//clear terminal
							//clrscr();
							xil_printf("\n\rEntering in scurve routine\r\n");
							start_channel = *(recv_buf+3);
							stop_channel  = *(recv_buf+4);
							step_channel  = *(recv_buf+5);

							start_CALDAC = *(recv_buf+6);
							stop_CALDAC  = *(recv_buf+7);
							step_CALDAC  = *(recv_buf+8);

							Latency = ( *(recv_buf+9) <<8) | *(recv_buf+10);

							num_of_triggers = ( *(recv_buf+11) <<8) | *(recv_buf+12);
							arm_dac        =  *(recv_buf+13);
							delay          =  *(recv_buf+14);
							D1 = ( *(recv_buf+15) <<8) | *(recv_buf+16);
							D2 = ( *(recv_buf+17) <<8) | *(recv_buf+18);

							xil_printf("start_CH = %d :: stop_CH = %d :: step_CH = %d :: \r\n",start_channel,stop_channel,step_channel);
							xil_printf("start_CALDAC = %d :: stop_CALDAC = %d :: step_CALDAC = %d :: \r\n",start_CALDAC,stop_CALDAC,step_CALDAC);
							xil_printf("Latency= %d, Num_of_triggers LV1As = %d\r\n",Latency, num_of_triggers );
							xil_printf("arm_dac= %d, delay = %d D1 = %d    D2 = %d\r\n",arm_dac,delay,D1,D2 );
							Scurve(SourceAddr,DestinationBuffer,start_channel,stop_channel,step_channel,start_CALDAC,stop_CALDAC,step_CALDAC,Latency,num_of_triggers,sd,arm_dac,delay,D1,D2);
							//Scurve(SourceAddr,DestinationBuffer,Latency,num_of_triggers,0);


				}

	else if (*(recv_buf+1)==0x00 && *(recv_buf+2)==10)
					{
								//xil_printf("%c[2J",27);//clear terminal
								//clrscr();
								xil_printf("\n\rEntering in BIST test\r\n");
								BIST(sd);
					}


	else if (*(recv_buf+1)==0xff && *(recv_buf+2)==0x00)
					{

test_controller();
					}


	else if (*(recv_buf+1)==0xFF && *(recv_buf+2)==0x07){
								//xil_printf("%c[2J",27);//clear terminal
								//clrscr();
								xil_printf("\n\rEntering in EXTENDED CAL_DAC calibration routine\r\n");
								//u32 register_address = *(recv_buf+7) | (*(recv_buf+6)<<8) | (*(recv_buf+5)<<16) | (*(recv_buf+4)<<24);
								 start = *(recv_buf+3);
								step = *(recv_buf+4);
								stop = *(recv_buf+5);
								xil_printf("start = %d :: step_size = %d :: stop = %d :: \r\n",start,step,stop);
								if(  start >=0 &&  start < stop  && step<=(stop-start) &&  step  > 0    )
								CalibrateCAL_DAC_extended(SourceAddr,DestinationBuffer,sd,start,step,stop,0);
	}



	else if (*(recv_buf+1)==0xff && *(recv_buf+2)==0x08)
					{
								//xil_printf("%c[2J",27);//clear terminal
								//clrscr();
								//xil_printf("\n\rEntering in SCURVE extended  routine\r\n");
								start_channel = *(recv_buf+3);
								stop_channel  = *(recv_buf+4);
								step_channel  = *(recv_buf+5);

								//start_CALDAC = *(recv_buf+6);
								//stop_CALDAC  = *(recv_buf+7);
								//step_CALDAC  = *(recv_buf+8);

								Latency = ( *(recv_buf+9) <<8) | *(recv_buf+10);

								num_of_triggers = ( *(recv_buf+11) <<8) | *(recv_buf+12);
								arm_dac        =  *(recv_buf+13);
								delay          =  *(recv_buf+14);
								D1 = ( *(recv_buf+15) <<8) | *(recv_buf+16);
								D2 = ( *(recv_buf+17) <<8) | *(recv_buf+18);
								calpulse = 	*(recv_buf+19);
							    array_size = *(recv_buf+20);
								//dac_arr[256];
							   // xil_printf("dac_arr: ");
								for (i=0; i< array_size; i++)
								{
									dac_arr[i] = *(recv_buf+21 + i);
									xil_printf("dac_arr(%d) =  %d\t", i, dac_arr[i]);


								}
								xil_printf("\n\r ");



								xil_printf("start_CH\t= %d :: stop_CH = %d :: step_CH = %d :: \r\n",start_channel,stop_channel,step_channel);

								xil_printf("Latency\t= %d, Num_of_triggers LV1As = %d\r\n",Latency, num_of_triggers );
								xil_printf("D1\t= %d :: D2 = %d ::  delay = %d :: calpulse= %d :: \r\n",D1,D2,delay,calpulse);
								xil_printf("arm_dac\t= %d\r\n",arm_dac );
								xil_printf("array_size\t= %d\r\n",array_size);
								Scurve_extended(SourceAddr,DestinationBuffer,start_channel,stop_channel,step_channel,Latency,num_of_triggers,sd,arm_dac,delay,D1,D2,array_size,dac_arr,calpulse);



					}

	else if (*(recv_buf+1)==0xff && *(recv_buf+2)==0x09)
	{

		Set_front_end_Nominal(SourceAddr,DestinationBuffer);
		xil_printf("frontend default settings sent\r\n");
		Error_code = 0;
		RD = &Error_code;
		SendReply(sd, RD,1 * 4);

	}

	else if (*(recv_buf+1)==0xff && *(recv_buf+2)==0x0a)
	{
		xil_printf("desired frontend settings loaded\r\n");
		Set_front_end(SourceAddr,DestinationBuffer,*(recv_buf+3),*(recv_buf+4),*(recv_buf+5),*(recv_buf+6),*(recv_buf+7),*(recv_buf+2),*(recv_buf+2),*(recv_buf+2),*(recv_buf+2));
		Error_code = 0;
		RD = &Error_code;
		SendReply(sd, RD,1 * 4);
	}
	else {
		xil_printf("no match\r\n");
		xil_printf("%c\r\n",0x0c);
		xil_printf("data = %x %x %x %x\r\n",*recv_buf,*(recv_buf+1),*(recv_buf+2),*(recv_buf+3));
	}

}
return Status;
}
int Initialize_fifo(XLlFifo *InstancePtr, u16 DeviceId)
{
	int Status;
	//XLlFifo_Config *Config;
	//	XLlFifo_Config *Config_rx;
		//int Status;
		int i;
		int Error;
		Status = XST_SUCCESS;

	/* Initialize the Device Configuration Interface driver */
		Config = XLlFfio_LookupConfig(DeviceId);
		if (!Config) {
			xil_printf("No config found for %d\r\n", DeviceId);
			return XST_FAILURE;
		}




		/*
		 * This is where the virtual address would be used, this example
		 * uses physical address.
		 */
		Status = XLlFifo_CfgInitialize(&FifoInstance, Config, Config->BaseAddress);
		if (Status != XST_SUCCESS) {
			xil_printf("Initialization failed\n\r");
			return Status;
		}
		/* Check for the Reset value */
			Status = XLlFifo_Status(&FifoInstance);
			XLlFifo_IntClear(&FifoInstance,0xffffffff);
			Status = XLlFifo_Status(&FifoInstance);
			if(Status != 0x0) {
				xil_printf("\n ERROR : Reset value of ISR0 : 0x%x\t"
					    "Expected : 0x0\n\r",
					    XLlFifo_Status(&FifoInstance));
				return XST_FAILURE;
			}
			return XST_SUCCESS;
}
u32 RxReceive (XLlFifo *InstancePtr, u32* DestinationAddr,u8 verbose_mode)
{

	//int i;
	int Status;
	//u32 RxWord;
	//u32 ReceiveLength;
	//u8 verbose_mode=1;

	//if(verbose_mode)xil_printf(" Receiving data ....\n\r");
//usleep(50000);
	 //while (XLlFifo_RxOccupancy(&FifoInstance)) {
	                frame_len = XLlFifo_RxGetLen(&FifoInstance)/WORD_SIZE;
	                //ReceiveLength=XLlFifo_RxOccupancy(&FifoInstance);
	               // xil_printf(" frame_len=%d\n\r",frame_len);

	              //  while (frame_len) {
	                       // unsigned bytes = frame_len;//min(sizeof(DestinationAddr), frame_len);
	                        XLlFifo_Read(&FifoInstance, DestinationAddr, frame_len *WORD_SIZE);
	                 //       for (i=0;i< frame_len;i++)xil_printf("%x\r\n",*(DestinationAddr+i));

	                        XLlFifo_Reset(&FifoInstance);


	return frame_len;
}


/*
u32 RxReceive_quick (XLlFifo *InstancePtr, u32* DestinationAddr,u32 Receivelength,u8 verbose_mode)
{

	int i;
	int Status;
	u32 RxWord1,RxWord2;
	//u32 ReceiveLength;
	//u8 verbose_mode=1;
	//static u32 frame_len;
	//if(verbose_mode)xil_printf(" Receiving data ....\n\r");



	unsigned length=0;





//	 Read Recieve Length
//	ReceiveLength = (XLlFifo_iRxGetLen(&FifoInstance))/WORD_SIZE;
//	length        = ReceiveLength;
	//if(verbose_mode)
	//xil_printf("hdlc_rx :: receive length =%d \r\n",ReceiveLength);
	 //Start Receiving
	for ( i=0; i < ReceiveLength; i++){
		RxWord1 = 0;
		RxWord1 = XLlFifo_RxGetWord(&FifoInstance);
		//xil_printf("rxword1  =%x \r\n",RxWord);
		if(XLlFifo_iRxOccupancy(&FifoInstance)){
		//	RxWord2 = XLlFifo_RxGetWord(&FifoInstance);
		//	xil_printf("rxword1 %x ,, rxword2 %x  \r\n",RxWord1,RxWord2);
		//}
		*(DestinationAddr+i) = RxWord1;
	}
	 //for (i=0;i< 352;i++)xil_printf("%x\r\n",*(DestinationAddr+i));
///	if(verbose_mode){
//	for(i=0;i<length;i++)
		//if(i==(length-1))
//	xil_printf("ReceiveBuffer(%d) = %x \r\n ",i,*(DestinationAddr+i));
//	}
	//Status = XLlFifo_IsRxDone(InstancePtr);
	//if(Status != TRUE){
	//	xil_printf("Failing in receive complete ... \r\n");
	//return XST_FAILURE;
//	}
	//XLlFifo_Reset(&FifoInstance);//added on 02-03-2018

	return ReceiveLength;
}

*/



int XLlFifoPollingExample(XLlFifo *InstancePtr, u16 DeviceId,u32  *SourceAddr,u32 data_len,u8 verbose_mode)
{
	XLlFifo_Config *Config;

	int Status;
	int i;
	int Error;
	Status = XST_SUCCESS;

	/* Initial setup for Uart16550 */
/*
#ifdef XPAR_UARTNS550_0_BASEADDR

	Uart550_Setup();

#endif
*/

/*
	 Initialize the Device Configuration Interface driver
	Config = XLlFfio_LookupConfig(DeviceId);
	if (!Config) {
		xil_printf("No config found for %d\r\n", DeviceId);
		return XST_FAILURE;
	}




	 * This is where the virtual address would be used, this example
	 * uses physical address.

	Status = XLlFifo_CfgInitialize(InstancePtr, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("Initialization failed\n\r");
		return Status;
	}




	 Check for the Reset value
	Status = XLlFifo_Status(InstancePtr);
	XLlFifo_IntClear(InstancePtr,0xffffffff);
	Status = XLlFifo_Status(InstancePtr);
	if(Status != 0x0) {
		xil_printf("\n ERROR : Reset value of ISR0 : 0x%x\t"
			    "Expected : 0x0\n\r",
			    XLlFifo_Status(InstancePtr));
		return XST_FAILURE;
	}
*/



	/* Transmit the Data Stream */
	Status = TxSend(&FifoInstance, SourceAddr,data_len,verbose_mode);
	if (Status != XST_SUCCESS){
		xil_printf("Transmisson of Data failed\n\r");
		return XST_FAILURE;
	}





	/* Revceive the Data Stream */
		ReceiveLength = RxReceive(&FifoInstance, DestinationBuffer,verbose_mode);
	//if (Status != XST_SUCCESS){
	//	xil_printf("Receiving data failed");
	//	return XST_FAILURE;
//	}

	Error = 0;

	/* Compare the data send with the data received */
	//xil_printf(" destination buffer[0] = %x \n\r",DestinationBuffer[0]);
	//for( i=0 ; i<MAX_DATA_BUFFER_SIZE ; i++ ){
	//	if ( *(SourceBuffer + i) != *(DestinationBuffer + i) ){
	//		Error = 1;
	//		break;
	//	}
	return Status;
	}

	//if (Error != 0){
	//	return XST_FAILURE;
	//}

//	return Status;
//}


/*****************************************************************************/
/**
*
* TxSend routine, It will send the requested amount of data at the
* specified addr.
*
* @param	InstancePtr is a pointer to the instance of the
*		XLlFifo component.
*
* @param	SourceAddr is the address where the FIFO stars writing
*
* @return
*		-XST_SUCCESS to indicate success
*		-XST_FAILURE to indicate failure
*
* @note		None
*
******************************************************************************/
int TxSend(XLlFifo *InstancePtr, u32  *SourceAddr,u32 data_len,u8 verbose_mode)
{

	u32 i;
	u32 j;



	if( XLlFifo_iTxVacancy(&FifoInstance)> data_len)
	{
	XLlFifo_Write
		(
			&FifoInstance,
			SourceAddr,
			data_len * WORD_SIZE
		);



	/* Start Transmission by writing transmission length into the TLR */
	XLlFifo_iTxSetLen(&FifoInstance, data_len * WORD_SIZE);
	}
//	for(i=0;i< data_len ; i++)
//		xil_printf("%x\r\n", *(SourceAddr+i));
	/* Check for Transmission completion */
	while( !(XLlFifo_IsTxDone(&FifoInstance)) ){

	}

	/* Transmission Complete */
	//if(verbose_mode)xil_printf(" End Transmition.. \r\n");
	return XST_SUCCESS;
}



/*****************************************************************************/
/**
*
* RxReceive routine.It will receive the data from the FIFO.
*
* @param	InstancePtr is a pointer to the instance of the
*		XLlFifo instance.
*
* @param	DestinationAddr is the address where to copy the received data.
*
* @return
*		-XST_SUCCESS to indicate success
*		-XST_FAILURE to indicate failure
*
* @note		None
*
******************************************************************************/
void check_fifo(u32  *SourceAddr,u32 data_len,u8 verbose_mode)
{
int Status;
u32 i;
//u8 verbose_mode=0;
//if(verbose_mode)xil_printf("\n\r--- Entering check_fifo() ---\n\r");
//if(verbose_mode)	xil_printf("\n\r source address_static = %p \n\r",(void*)SourceBuffer);
//if(verbose_mode)	xil_printf("\n\r source address = %p \n\r",(void*)SourceAddr);
	/*for (i=0;i<MAX_DATA_BUFFER_SIZE;i++)
			{
				if(i>=0 && i< 3)*(SourceAddr + i) = 0x17;
				else if(i==(MAX_DATA_BUFFER_SIZE-1))*(SourceAddr + i) = 0xe8;
				else if( i % 2 == 0 )
				*(SourceAddr + i) = 0x00;//

			else
					*(SourceAddr + i) = 0xff;//
		//if(i>=0 && i< 3){*(SourceAddr + i) = 0x17;}//sync
		 //if (i>=500)
	//	 {*(SourceAddr + i) = 0xe8;}// syncack
		//else {*(SourceAddr + i) = 0xe8;}
			}*/
	//xil_printf("\n\r source address = %p \n\r",(void *)SourceAddr);
	Status = XLlFifoPollingExample(&FifoInstance, FIFO_DEV_ID,SourceAddr,data_len,verbose_mode);
	if (Status != XST_SUCCESS) {
		xil_printf("Axi Streaming FIFO Polling Example Test Failed\n\r");
		xil_printf("--- Exiting main() ---\n\r");
		return XST_FAILURE;
	}

	//xil_printf("Successfully ran Axi Streaming FIFO Polling Example\n\r");
	//xil_printf("--- Exiting check_fifo() ---\n\r");

	return XST_SUCCESS;
}



/**************************************************************************
//
// crc16.c - generate a ccitt 16 bit cyclic redundancy check (crc)
//
//      The code in this module generates the crc for a block of data.
//
**************************************************************************/

/*
//                                16  12  5
// The CCITT CRC 16 polynomial is X + X + X + 1.
// In binary, this is the bit pattern 1 0001 0000 0010 0001, and in hex it
//  is 0x11021.
// A 17 bit register is simulated by testing the MSB before shifting
//  the data, which affords us the luxury of specifiy the polynomial as a
//  16 bit value, 0x1021.
// Due to the way in which we process the CRC, the bits of the polynomial
//  are stored in reverse order. This makes the polynomial 0x8408.
*/
#define POLY 0x8408

/*
// note: when the crc is included in the message, the valid crc is:
//      0xF0B8, before the compliment and byte swap,
//      0x0F47, after compliment, before the byte swap,
//      0x470F, after the compliment and the byte swap.
*/

extern  crc_ok;
/**************************************************************************
//
// crc16() - generate a 16 bit crc
//
//
// PURPOSE
//      This routine generates the 16 bit remainder of a block of
//      data using the ccitt polynomial generator.
//
// CALLING SEQUENCE
//      crc = crc16(data, len);
//
// PARAMETERS
//      data    <-- address of start of data block
//      len     <-- length of data block
//
// RETURNED VALUE
//      crc16 value. data is calcuated using the 16 bit ccitt polynomial.
//
// NOTES
//      The CRC is preset to all 1's to detect errors involving a loss
//        of leading zero's.
//      The CRC (a 16 bit value) is generated in LSB MSB order.
//      Two ways to verify the integrity of a received message
//        or block of data:
//        1) Calculate the crc on the data, and compare it to the crc
//           calculated previously. The location of the saved crc must be
//           known.
/         2) Append the calculated crc to the end of the data. Now calculate
//           the crc of the data and its crc. If the new crc equals the
//           value in "crc_ok", the data is valid.
//
// PSEUDO CODE:
//      initialize crc (-1)
//      DO WHILE count NE zero
//        DO FOR each bit in the data byte, from LSB to MSB
//          IF (LSB of crc) EOR (LSB of data)
//            crc := (crc / 2) EOR polynomial
//          ELSE
//            crc := (crc / 2)
//          FI
//        OD
//      OD
//      1's compliment and swap bytes in crc
//      RETURN crc
//
**************************************************************************/
unsigned int crc16(char *data_p, unsigned short length)
//char *data_p;
//unsigned short length;
{
       unsigned char i;
       unsigned int data;
       unsigned int crc;

       crc = 0xffff;

       if (length == 0)
              return (~crc);

       do {
              for (i = 0, data = (unsigned int)0xff & *data_p++;
                  i < 8;
                  i++, data >>= 1) {
                    if ((crc & 0x0001) ^ (data & 0x0001))
                           crc = (crc >> 1) ^ POLY;
                    else
                           crc >>= 1;
              }
       } while (--length);

       crc = ~crc;

       data = crc;
       crc = (crc << 8) | (data >> 8 & 0xFF);
       crc=swapBytes(crc);
       return (crc);
}


u32 HDLC_Rx(u32* DestinationAddr,u8 mode,u8 verbose_mode)
{

	u8 fcs          = 0xff;
	u8 hdlc_address = 0xff;
	u8 hdlc_control = 0xff;
	u32 read_data=-1;




u8 type_id=0xaa;
	u8 NUM_OF_ONES=0;
	u8 tmp;
	u8 rcv_8;
	u32 i=0;
	u32 j=0;
	u8 rcv_arr[15];
	u16 crc_calculated;
	u16 crc_received;
	u8 error=0;

//usleep(1000);
//usleep(1000);
	ReceiveLength = RxReceive(&FifoInstance, DestinationBuffer,verbose_mode);

	//ReceiveLength=ReceiveLength/4;

	//u8 verbose_mode=1;
if(verbose_mode)
xil_printf("hdlc_rx::rcv_len=%d, mode is %d\r\n",ReceiveLength,mode);
	do{
		rcv_8 = *(DestinationAddr+i);
		tmp=(rcv_8==SC0)?0:1;
		fcs <<=1;
		fcs|=tmp;
	//	if(verbose_mode)xil_printf("i=%d    rcv_8= %x  ,,     fcs=%x\r\n",i,rcv_8,fcs);
		i++;

}while(!(fcs==0x7e || i>ReceiveLength));//replaced rcv_len with ReceiveLength on 28th feb


	if(verbose_mode)
{
int ii,iii;
	xil_printf(" HDLC_RX ::\r\n");
		for (ii=(i-8),iii=0;ii<ReceiveLength;ii++,iii++){
			if(iii % 8 == 0)xil_printf("\r\n");

				xil_printf(" %02x,",(u8)*(DestinationBuffer+ii));
		}
}


	if(i<ReceiveLength)//replaced rcv_len with ReceiveLength on 28th feb
	{
	///////////////HDLC ADDRESS DECODING///////////////////
		j=0;
		int k=0;
		do{

				rcv_8 = *(DestinationAddr+i);
				if(rcv_8 == SC0 || rcv_8== SC1)
				{


					if(rcv_8 == SC1 ){NUM_OF_ONES++;}else if(NUM_OF_ONES<5 && rcv_8==SC0) NUM_OF_ONES=0;//ZERO REMOVAL

					if(NUM_OF_ONES == 5 && rcv_8 == SC0){
						NUM_OF_ONES=0;
						i++;
					//	if(verbose_mode)xil_printf("discarded extra 0\r\n");
					}//discard extra 0
					else if(NUM_OF_ONES>5 && rcv_8 == SC1)
					{
						NUM_OF_ONES=0;
						i++;
						xil_printf("frame error 6 ones received in packet\r\n");
						error = 1;
					}//error handling
					else
					{
					tmp=(rcv_8==SC0)?0:1;
					rcv_arr[k] <<=1;
					rcv_arr[k]|=tmp;
					//if(verbose_mode)	xil_printf("i=%d    rcv_8= %x  ,,     hdlc_address=%x\r\n",i,rcv_8,rcv_arr[k]);
					i++;j++;
					}
				//	if(rcv_8==SC1){NUM_OF_ONES++;}else NUM_OF_ONES=0;//ZERO REMOVAL
				}
				else{xil_printf("frame error 1 rcv_8 = %x\r\n",rcv_8);error=1;}
				//j++;
		}while(j<8 && error == 0);
		rcv_arr[k]=reverse(rcv_arr[k]);
		if(verbose_mode)xil_printf("\r\nHDLC_FCS RECEIVED= %02x\r\n",fcs);
		if(verbose_mode)xil_printf("HDLC_ADDRESS RECEIVED= %02x\r\n",rcv_arr[k]);
//////////////////////////////////////////////////////////////////////////
///////////////HDLC CONTROL DECODING//////////////////////////
		j=0;k++;//NUM_OF_ONES=0;
				do{
						rcv_8 = *(DestinationAddr+i);
						if(rcv_8 == SC0 || rcv_8== SC1)
						{
							if(rcv_8 == SC1 ){
								NUM_OF_ONES++;}else if(NUM_OF_ONES<5 && rcv_8==SC0) NUM_OF_ONES=0;//ZERO REMOVAL
							if(NUM_OF_ONES == 5 && rcv_8 == SC0)
							{
								NUM_OF_ONES=0;
								i++;
							//	if(verbose_mode)xil_printf("discarded extra 0\r\n");
							}//discard extra 0
							else if(NUM_OF_ONES>5 && rcv_8 == SC1)
							{
								NUM_OF_ONES=0;
								i++;
								//xil_printf("frame error 6 ones received in packet\r\n");
								}//error handling
							else{
								tmp=(rcv_8==SC0)?0:1;
							rcv_arr[k] <<= 1;
							rcv_arr[k]|= tmp;
							//if(verbose_mode)xil_printf("i=%d    rcv_8= %x  ,,     hdlc_control=%x\r\n",i,rcv_8,rcv_arr[k]);
							i++;j++;
							}
							//if(rcv_8==SC1){NUM_OF_ONES++;}else NUM_OF_ONES=0;//ZERO REMOVAL
						}
						else{xil_printf("frame error 2 rcv_8 = %x\r\n",rcv_8);error = 1;}
						//j++;
				}while(j<8 && error == 0);
				rcv_arr[k]=reverse(rcv_arr[k]);
				if(verbose_mode)xil_printf("HDLC_CONTROL RECEIVED= %02x\r\n",rcv_arr[k]);
///////////////////////////////////////////////////////////////////////////////////////
				///////////////IPBUS HEADER DECODING//////////////////////////
						j=0;k++;//NUM_OF_ONES=0;
								do{
										rcv_8 = *(DestinationAddr+i);
										if(rcv_8 == SC0 || rcv_8== SC1)
										{
											if(rcv_8 == SC1 ){NUM_OF_ONES++;}else if(NUM_OF_ONES<5 && rcv_8==SC0) NUM_OF_ONES=0;//ZERO REMOVAL
											if(NUM_OF_ONES == 5 && rcv_8 == SC0)
											{
												NUM_OF_ONES=0;
												i++;
											//	if(verbose_mode)xil_printf("discarded extra 0\r\n");
											}//discard extra 0
											else if(NUM_OF_ONES>5 && rcv_8 == SC1)
											{
												NUM_OF_ONES=0;
												i++;
												xil_printf("frame error 6 ones received in packet\r\n");error = 1;}//error handling
											else{
											tmp=(rcv_8==SC0)?0:1;
											rcv_arr[k] <<=1;
											rcv_arr[k]|=tmp;
											//if(verbose_mode)xil_printf("i=%d    rcv_8= %x  ,,     hdlc_control=%x\r\n",i,rcv_8,rcv_arr[k]);
											i++;j++;
											}
											//if(rcv_8==SC1){NUM_OF_ONES++;}else NUM_OF_ONES=0;//ZERO REMOVAL
										}
										else{xil_printf("frame error 3 rcv_8 = %x\r\n",rcv_8);error = 1;}
										//j++;
								}while(j<8 && error == 0);
								rcv_arr[k]=reverse(rcv_arr[k]);//uncommented this 22/04/2018
								if(verbose_mode)	xil_printf("IP bus header_0 RECEIVED= %02x\r\n",rcv_arr[k]);


							h0 = rcv_arr[k];
				///////////////////////////////////////////////////////////////////////////////////////
								j=0;k++;//NUM_OF_ONES=0;
									do{
										rcv_8 = *(DestinationAddr+i);
										if(rcv_8 == SC0 || rcv_8== SC1)
										{
											if(rcv_8 == SC1 ){NUM_OF_ONES++;}else if(NUM_OF_ONES<5 && rcv_8==SC0) NUM_OF_ONES=0;//ZERO REMOVAL
											if(NUM_OF_ONES == 5 && rcv_8 == SC0)
											{NUM_OF_ONES=0;
											i++;
											//if(verbose_mode)xil_printf("discarded extra 0\r\n");
											}//discard extra 0
											else if(NUM_OF_ONES>5 && rcv_8 == SC1)
											{
												NUM_OF_ONES=0;
												i++;
												xil_printf("frame error 6 ones received in packet\r\n");error = 1;
											}//error handling
											else{
											tmp=(rcv_8==SC0)?0:1;
											rcv_arr[k] <<=1;
											rcv_arr[k]|=tmp;
											//if(verbose_mode)xil_printf("i=%d    rcv_8= %x  ,,     ipbus_header_1=%x\r\n",i,rcv_8,rcv_arr[k]);
											i++;j++;
												}
										//	if(rcv_8==SC1){NUM_OF_ONES++;}else NUM_OF_ONES=0;//ZERO REMOVAL
										}
										else{xil_printf("frame error  4 rcv_8 = %x\r\n",rcv_8);error = 1;}
										//j++;
										}while(j<8 && error == 0);
									rcv_arr[k]=reverse(rcv_arr[k]);//uncommented this 22/04/2018
										if(verbose_mode)	xil_printf("IP bus header_1 RECEIVED= %02x\r\n",rcv_arr[k]);
										h1 = rcv_arr[k];
										///////////////////////////////////////////////////////////////////////////////////////
							j=0;k++;//NUM_OF_ONES=0;
								do{
										rcv_8 = *(DestinationAddr+i);
										if(rcv_8 == SC0 || rcv_8== SC1)
										{

											if(rcv_8 == SC1 ){NUM_OF_ONES++;}else if(NUM_OF_ONES<5 && rcv_8==SC0) NUM_OF_ONES=0;//ZERO REMOVAL
											if(NUM_OF_ONES == 5 && rcv_8 == SC0)
											{
												NUM_OF_ONES=0;
												i++;
											//	if(verbose_mode)xil_printf("discarded extra 0\r\n");
											}//discard extra 0
											else if(NUM_OF_ONES>5 && rcv_8 == SC1){NUM_OF_ONES=0;i++;xil_printf("frame error 6 ones received in packet\r\n");error = 1;}//error handling
											else{
										tmp=(rcv_8==SC0)?0:1;
										rcv_arr[k] <<=1;
										rcv_arr[k]|=tmp;
										//if(verbose_mode)xil_printf("i=%d    rcv_8= %x  ,,     ipbus_header_2=%x\r\n",i,rcv_8,rcv_arr[k]);
										i++;j++;
											}
										//if(rcv_8==SC1){NUM_OF_ONES++;}else NUM_OF_ONES=0;//ZERO REMOVAL
										}
										else{xil_printf("frame error 5 rcv_8 = %x\r\n",rcv_8);error = 1;}
										//j++;
								}while(j<8 && error == 0);
								rcv_arr[k]=reverse(rcv_arr[k]);//uncommented this 22/04/2018
								if(verbose_mode)	xil_printf("IP bus header_2 RECEIVED= %02x\r\n",rcv_arr[k]);
								h2 = rcv_arr[k];
//////////////////////////////////////////////////////////////////////////////////////////
								j=0;k++;//NUM_OF_ONES=0;
								do{
									rcv_8 = *(DestinationAddr+i);
									if(rcv_8 == SC0 || rcv_8== SC1)
									{
										if(rcv_8 == SC1 ){NUM_OF_ONES++;}else if(NUM_OF_ONES<5 && rcv_8==SC0) NUM_OF_ONES=0;//ZERO REMOVAL
										if(NUM_OF_ONES == 5 && rcv_8 == SC0){
											NUM_OF_ONES=0;
											i++;
										//	if(verbose_mode)xil_printf("discarded extra 0\r\n");
										}//discard extra 0
										else if(NUM_OF_ONES>5 && rcv_8 == SC1)
										{
											NUM_OF_ONES=0;
											i++;
											xil_printf("frame error 6 ones received in packet\r\n");error = 1;
										}//error handling
										else{
										tmp=(rcv_8==SC0)?0:1;
										rcv_arr[k] <<=1;
										rcv_arr[k]|=tmp;
										//if(verbose_mode)	xil_printf("i=%d    rcv_8= %x  ,,     ipbus_header_3=%x\r\n",i,rcv_8,rcv_arr[k]);
										i++;j++;
										}
									//if(rcv_8==SC1){NUM_OF_ONES++;}else NUM_OF_ONES=0;//ZERO REMOVAL
									}
								else{xil_printf("frame error 6 rcv_8 = %x\r\n",rcv_8);error = 1;}
									//j++;
								}while(j<8 && error == 0);
								rcv_arr[k]=reverse(rcv_arr[k]);//uncommented this 22/04/2018
								if(verbose_mode)xil_printf("IP bus header_3 RECEIVED= %02x\r\n",rcv_arr[k]);
								h3 = rcv_arr[k];


////////////////////////////////////////////////////////////////////////////////////////////////////////////
								/////////////////////data//////////////////////////////////////////////

										if(mode == READ)//read transaction read data
										{
																	j=0;k++;//NUM_OF_ONES=0;
																do{
																rcv_8 = *(DestinationAddr+i);
																if(rcv_8 == SC0 || rcv_8== SC1)
																{

																if(rcv_8 == SC1 ){NUM_OF_ONES++;}else if(NUM_OF_ONES<5 && rcv_8==SC0) {NUM_OF_ONES=0;}//ZERO REMOVAL
																if(NUM_OF_ONES == 5 && rcv_8 == SC0)
																{NUM_OF_ONES=0;
																i++;
																//if(verbose_mode)xil_printf("discarded extra 0\r\n");
																}//discard extra 0
																else if(NUM_OF_ONES>5 && rcv_8 == SC1){NUM_OF_ONES=0;i++;xil_printf("frame error 6 ones received in packet\r\n");error = 1;}//error handling
																else{
																	//xil_printf("last else\t");
																tmp=(rcv_8 == SC0)?0:1;
																rcv_arr[k] <<=1;
																rcv_arr[k]|=tmp;
																//if(verbose_mode)	xil_printf("i=%d    rcv_8= %x  ,,     read_data_0=%x\r\n",i,rcv_8,rcv_arr[k]);
																i++;j++;
																}
																//xil_printf("num_of_ones= %d ,rcv_8 = %x ,i= %d\r\n",NUM_OF_ONES,rcv_8,i-1);
																}
																else{xil_printf("frame error  7 rcv_8 = %x\r\n",rcv_8);error = 1;}

															}while(j<8 && error == 0);
																rcv_arr[k]=reverse(rcv_arr[k]);
															if(verbose_mode)xil_printf("k=%d,read_data_0 RECEIVED= %02x\r\n",k,rcv_arr[k]);

////////////////////////////////////////////////data_1////////////////////////
															j=0;k++;//NUM_OF_ONES=0;
														do{
														rcv_8 = *(DestinationAddr+i);
														if(rcv_8 == SC0 || rcv_8== SC1)
														{
															if(rcv_8 == SC1 ){NUM_OF_ONES++;}else if(NUM_OF_ONES<5 && rcv_8==SC0) NUM_OF_ONES=0;//ZERO REMOVAL
															if(NUM_OF_ONES == 5 && rcv_8 == SC0){NUM_OF_ONES=0;i++;if(verbose_mode)xil_printf("discarded extra 0\r\n");}//discard extra 0
															else if(NUM_OF_ONES>5 && rcv_8 == SC1){NUM_OF_ONES=0;i++;xil_printf("frame error 6 ones received in packet\r\n");error = 1;}//error handling
															else{
																tmp=(rcv_8==SC0)?0:1;
																rcv_arr[k] <<=1;
																rcv_arr[k]|=tmp;
																//if(verbose_mode)	xil_printf("i=%d    rcv_8= %x  ,,     read_data_1=%x\r\n",i,rcv_8,rcv_arr[k]);
																i++;j++;
																}

																}
																else{xil_printf("frame error 8 rcv_8 = %x\r\n",rcv_8);error = 1;}
														//j++;
															}while(j<8 && error == 0);
														rcv_arr[k]=reverse(rcv_arr[k]);
																if(verbose_mode)xil_printf("k= %d,read_data_1 RECEIVED= %02x\r\n",k,rcv_arr[k]);


																////////////////////////////////////////////////data_2////////////////////////
																															j=0;k++;//NUM_OF_ONES=0;
																														do{
																														rcv_8 = *(DestinationAddr+i);
																														if(rcv_8 == SC0 || rcv_8== SC1)
																														{
																															if(rcv_8 == SC1 ){NUM_OF_ONES++;}else if(NUM_OF_ONES<5 && rcv_8==SC0) NUM_OF_ONES=0;//ZERO REMOVAL
																																															if(NUM_OF_ONES == 5 && rcv_8 == SC0){NUM_OF_ONES=0;i++;if(verbose_mode)xil_printf("discarded extra 0\r\n");}//discard extra 0
																																															else if(NUM_OF_ONES>5 && rcv_8 == SC1){NUM_OF_ONES=0;i++;xil_printf("frame error 6 ones received in packet\r\n");}//error handling
																																															else{
																																tmp=(rcv_8==SC0)?0:1;
																																rcv_arr[k] <<=1;
																																rcv_arr[k]|=tmp;
																																//if(verbose_mode)	xil_printf("i=%d    rcv_8= %x  ,,     read_data_2=%x\r\n",i,rcv_8,rcv_arr[k]);
																																i++;j++;
																																}
																																//if(rcv_8==SC1){NUM_OF_ONES++;}else NUM_OF_ONES=0;//ZERO REMOVAL
																																}
																																else{xil_printf("frame error  9 rcv_8 = %x\r\n",rcv_8);error = 1;}
																														//j++;
																															}while(j<8 && error == 0);
																														rcv_arr[k]=reverse(rcv_arr[k]);
																														//rcv_arr[k]=0;///added on 22/04/2018
																																if(verbose_mode)xil_printf("k = %d,  read_data_2 RECEIVED= %02x\r\n",k,rcv_arr[k]);

																																////////////////////////////////////////////////data_3////////////////////////
																														j=0;k++;//NUM_OF_ONES=0;
																														do{
																														rcv_8 = *(DestinationAddr+i);
																														if(rcv_8 == SC0 || rcv_8== SC1)
																														{
																															if(rcv_8 == SC1 ){NUM_OF_ONES++;}else if(NUM_OF_ONES<5 && rcv_8==SC0) NUM_OF_ONES=0;//ZERO REMOVAL
																																															if(NUM_OF_ONES == 5 && rcv_8 == SC0){NUM_OF_ONES=0;i++;if(verbose_mode)xil_printf("discarded extra 0\r\n");}//discard extra 0
																																															else if(NUM_OF_ONES>5 && rcv_8 == SC1){NUM_OF_ONES=0;i++;xil_printf("frame error 6 ones received in packet\r\n");}//error handling
																																															else{
																														tmp=(rcv_8==SC0)?0:1;
																														rcv_arr[k]<<=1;
																														rcv_arr[k]|=tmp;
																														//if(verbose_mode)	xil_printf("i=%d    rcv_8= %x  ,,     read_data_3=%x\r\n",i,rcv_8,rcv_arr[k]);
																														i++;j++;
																														}
																														//if(rcv_8==SC1){NUM_OF_ONES++;}else NUM_OF_ONES=0;//ZERO REMOVAL
																														}
																														else{xil_printf("frame error 10 rcv_8 = %x\r\n",rcv_8);error = 1;}
																														//j++;
																														}while(j<8 && error == 0);
																														rcv_arr[k]=reverse(rcv_arr[k]);
																														//rcv_arr[k]=0;///added on 22/04/2018

																														if(verbose_mode)xil_printf("k = %d, read_data_3 RECEIVED= %02x\r\n",k,rcv_arr[k]);


																}//END MODE = READ
										////////////////////////////crc calculation/////////////////


																	j=0;//NUM_OF_ONES=0;
																									do{
																										rcv_8 = *(DestinationAddr+i);
																										if(rcv_8 == SC0 || rcv_8== SC1)
																										{
																											if(rcv_8 == SC1 ){NUM_OF_ONES++;}else if(NUM_OF_ONES<5 && rcv_8==SC0) NUM_OF_ONES=0;//ZERO REMOVAL
																																											if(NUM_OF_ONES == 5 && rcv_8 == SC0){NUM_OF_ONES=0;i++;if(verbose_mode)xil_printf("discarded extra 0\r\n");}//discard extra 0
																																											else if(NUM_OF_ONES>5 && rcv_8 == SC1){NUM_OF_ONES=0;i++;xil_printf("frame error 6 ones received in packet\r\n");error = 1;}//error handling
																																											else{
																											tmp=(rcv_8==SC0)?0:1;
																											rcv_arr[k+1] <<=1;
																											rcv_arr[k+1]|=tmp;
																											//if(verbose_mode)	xil_printf("i=%d    rcv_8= %x  ,,     crc_lsb=%x\r\n",i,rcv_8,rcv_arr[k+1]);
																											i++;j++;
																											}
																										//if(rcv_8==SC1){NUM_OF_ONES++;}else NUM_OF_ONES=0;//ZERO REMOVAL
																										}
																									else{xil_printf("frame error 11 rcv_8 = %x\r\n",rcv_8);error = 1;}
																										//j++;
																									}while(j<8 && error == 0);
																									rcv_arr[k+1]=reverse(rcv_arr[k+1]);
																									if(verbose_mode)xil_printf("crc_lsb RECEIVED= %02x\r\n",rcv_arr[k+1]);

																	////////////////////crc msb/////////////////////////////////////////
																									j=0;//k++;//NUM_OF_ONES=0;
																									do{
																										rcv_8 = *(DestinationAddr+i);
																										if(rcv_8 == SC0 || rcv_8== SC1)
																										{
																											if(rcv_8 == SC1 ){NUM_OF_ONES++;}else if(NUM_OF_ONES<5 && rcv_8==SC0) NUM_OF_ONES=0;//ZERO REMOVAL
																																											if(NUM_OF_ONES == 5 && rcv_8 == SC0){NUM_OF_ONES=0;i++;if(verbose_mode)xil_printf("discarded extra 0\r\n");}//discard extra 0
																																											else if(NUM_OF_ONES>5 && rcv_8 == SC1){NUM_OF_ONES=0;i++;xil_printf("frame error 6 ones received in packet\r\n");error = 1;}//error handling
																																											else{
																										tmp=(rcv_8==SC0)?0:1;
																										rcv_arr[k+2] <<=1;
																										rcv_arr[k+2]|=tmp;
																										//if(verbose_mode)	xil_printf("i=%d    rcv_8= %x  ,,     crc_msb=%x\r\n",i,rcv_8,rcv_arr[k+2]);
																										i++;j++;
																											}
																										//if(rcv_8==SC1){NUM_OF_ONES++;}else NUM_OF_ONES=0;//ZERO REMOVAL
																											}
																										else{xil_printf("frame error 12  rcv_8 = %x\r\n",rcv_8);error = 1;}
																										//j++;
																									}while(j<8 && error == 0);
																									rcv_arr[k+2]=reverse(rcv_arr[k+2]);
																									if(verbose_mode)xil_printf("crc_msb RECEIVED= %02x\r\n",rcv_arr[k+2]);

																									//type_id=(un.st.ipbus_header_0 >> 4) & 0x0f;


																									///////////////////FCS :TERMINATING FRAME

																									j=0;//k++;//NUM_OF_ONES=0;
																									do{
																										rcv_8 = *(DestinationAddr+i);
																										if(rcv_8 == SC0 || rcv_8== SC1)
																										{
																										//if(NUM_OF_ONES==5 && rcv_8 == SC0){NUM_OF_ONES=0;i++;xil_printf("discarded extra 0\r\n");}//discard extra 0
																										//else if(NUM_OF_ONES==5 && rcv_8 == SC1){NUM_OF_ONES=0;i++;xil_printf("frame error 6 ones received in packet\r\n");}//error handling
																										//else{
																											tmp=(rcv_8==SC0)?0:1;
																											rcv_arr[k+3] <<=1;
																											rcv_arr[k+3]|=tmp;
																										//	if(verbose_mode)	xil_printf("i=%d    rcv_8= %x  ,,     fcs_end=%x\r\n",i,rcv_8,rcv_arr[k+3]);
																											i++;j++;
																											//}
																											//if(rcv_8==SC1){NUM_OF_ONES++;}else NUM_OF_ONES=0;//ZERO REMOVAL
																											}
																										else{xil_printf("frame error 13 rcv_8 = %x\r\n",rcv_8);error = 1;}
																										//j++;
																										}while(j<8 && error == 0);
																										//rcv_arr[k]=reverse(rcv_arr[k]);
																										//if(verbose_mode)xil_printf("FCS_END  RECEIVED= %02x\r\n",rcv_arr[k+3]);
																																													//type_id=(un.st.ipbus_header_0 >> 4) & 0x0f;
																									////////////////////////////////////////////////////////////////////////


																										if(mode==READ)crc_received=(rcv_arr[11]<<8)|rcv_arr[10];
																										else crc_received=(rcv_arr[7]<<8)|rcv_arr[6];
																										crc_calculated= crc16(rcv_arr,k+1);




																											if(verbose_mode)
																											{xil_printf("k=%d\r\n",k);
																										xil_printf("crc_calculated = %x :: crc_received = %x\r\n", crc_calculated, crc_received);
																											}
																										//u8 loop=0;do{xil_printf("RCV_ARR[loop]=%02x\r\n",rcv_arr[loop++]);}while(loop<(k-1));




//for(i = 0,i<)
																			//rcv_arr[0] 		= 	hdlc_address;
																			//rcv_arr[1] 		=    hdlc_control;
																			//rcv_arr[2]	=   un.st.ipbus_header_0;
																			//rcv_arr[3]	=   un.st.ipbus_header_1;
																			//rcv_arr[4]	=   un.st.ipbus_header_2;
																			//rcv_arr[5]	=   un.st.ipbus_header_3;



			//	if(verbose_mode){xil_printf("IP bus header RECEIVED= %08x\r\n",(u32)(rcv_arr[5]<<24)|(rcv_arr[4]<<16)|(rcv_arr[3]<<8)|rcv_arr[2]);
			//					xil_printf("Info code= %01x\r\n",(rcv_arr[2] & 0x0f));
			//					xil_printf("Type_id= %01x\r\n",((rcv_arr[2] >> 4) & 0x0f));
			//					xil_printf("Transaction id= %02x\r\n",rcv_arr[3]);
			//					xil_printf("Depth = %03x\r\n",(u16)(( (rcv_arr[5] &0x0f)<<8)|rcv_arr[4]));
			//					xil_printf("Header version = %01x\r\n",( (un.st.ipbus_header_3 >>4)  & 0x0f) );
			///////					//xil_printf("Header version = %01x\r\n",( (un.st.ipbus_header_3 >>4)  & 0x0f) );

						//		if(mode==READ)xil_printf("READ_DATA= %04x\r\n", (u32)(rcv_arr[9]<<24)|(rcv_arr[8]<<16)|(rcv_arr[7]<<8)|rcv_arr[6] );
						//		xil_printf("CRC = %04x\r\n", crc_received );
					//			//for()
						//		xil_printf("i= %d, length =%d\r\n",i,rcv_len);
				//}
	///////////////////crc calculation//////////////////
									}//end if rcv_len
if(mode==READ)
	 read_data =  ((u32)rcv_arr[9]<<24) |  ((u32)rcv_arr[8]<<16)  | ((u32)rcv_arr[7]<<8)  |   (u32)rcv_arr[6];
else
	read_data  = h3<<24 | h2<<16 | h1<<8 | h0;

return read_data;

}



void write_SC( u32 address, u16 data, u32* SourceAddr,u32* DestinationBuffer, u8 verbose_mode )
{
	HDLC_Tx(address , data,1,SourceAddr,verbose_mode);           //transmit of sc data
	HDLC_Rx(DestinationBuffer,1,verbose_mode);                   //receive acknowledge of sc data


}

u32 read_SC( u32 address, u16 data, u32* SourceAddr,u32* DestinationBuffer, u8 verbose_mode )
{
	u32 read_data;
	HDLC_Tx(address , data,0,SourceAddr,verbose_mode);           //transmit of sc data
read_data = HDLC_Rx(DestinationBuffer,0,verbose_mode);                   //receive acknowledge of sc data
return read_data;

}


void HDLC_Tx(u32 register_address, u16 register_data,u8 mode,u32* SourceAddr,u8 verbose_mode)
{


		int i;


u8 Status;
		unsigned short length;

		if (mode == READ)
			 TYPE_ID = TRANS_TYPE_RD;
		else TYPE_ID = TRANS_TYPE_WR;
		u32 IP_BUS_HEADER  = (u32) ((HEADER_VERSION<<28) | (DEPTH<<16) | (TRANSACTION_ID<<8)|(TYPE_ID<<4) | INFO_CODE_REQ);

//if(verbose_mode)xil_printf("header= %08x\r\n",IP_BUS_HEADER);


	u8 hdlc_tx[] =  	{
								//HDLC_FS,
								HDLC_ADDRESS,
								HDLC_CONTROL,
								IP_BUS_HEADER ,//& 0X000000FF,
								((IP_BUS_HEADER >>8) ),// & 0X000000FF),
								((IP_BUS_HEADER >>16) ),//& 0X000000FF),
								((IP_BUS_HEADER >>24) ),//& 0X000000FF),

								((register_address )   ),// & 0X000000FF),
								((register_address>>8)  ),//& 0X000000FF),
								((register_address>>16) ),//& 0X000000FF),
								((register_address>>24) ),//& 0X000000FF),


								((register_data ) ),//&    0X000000FF),
								((register_data>>8) ),//&  0X000000FF),
								0,//((register_data>>16) ),//& 0X000000FF),
								0,//((register_data>>24) )//& 0X000000FF)

								//(u8)ut1.st1.crc_lsb,
								//(u8)ut1.st1.crc_msb
								};





				if(mode ==READ)	length= (sizeof(hdlc_tx) - 4);//exclude register_data in crc calculation
				else 			length= sizeof(hdlc_tx);//write mode ,include register _data in string

		//		xil_printf("mode  %d  length of hdlc_tx araay %d\r\n" ,mode, length);
		ut1.crc=crc16(hdlc_tx, length);

		//xil_printf("        FCS = %x\r\n",HDLC_FS);

u8 bits[(length+4)*8];// (+4 means fcs+crc+crc+fcs)
u8 tmp;
char j;
		for(int i=0;i<(length+4);i++){
			if(i==0 || (i== (length+3))){
						//if(verbose_mode)xil_printf("        FCS = %x\r\n",HDLC_FS);
						tmp = HDLC_FS;
						for(j=0;j<8;j++){
							bits[8*i+j]= ((tmp & 0x01)==0x01)? SC1:SC0;
							tmp>>=1;
							//xil_printf("%d\t",bits[i][j]);
						}
						/*if(i==length+3)
						{
							tmp = 0x00;
													for(j=0;j<8;j++){
														bits[8*i+j]= ((tmp & 1)==1)? 0x00:0x00;
														tmp>>=1;
														//xil_printf("%d\t",bits[i][j]);
													}


						}
*/

					//	if(verbose_mode)for(j=7;j>=0;j--){
					//		xil_printf("%02X\t",bits[8*i+j]);
					//									}
					//	if(verbose_mode)xil_printf("\r\n");
					}
			else if(i>0 && i<=length)
			{
			//	if(verbose_mode)xil_printf("hdlc_tx(%02d) = %02x\r\n",i-1,hdlc_tx[i-1]);

				tmp = hdlc_tx[i-1];
				for(j=0;j<8;j++){
				bits[8*i+j]= ((tmp & 1)==1)? SC1:SC0;
				tmp>>=1;
			//xil_printf("%d\t",bits[i][j]);
						}
		//if(verbose_mode)for(j=7;j>=0;j--){
		//	xil_printf("%02x\t",bits[8*i+j]);
		//		}


		//if(verbose_mode)xil_printf("\r\n");
		}

		else if(i>length && i<=length+2)//crc
		{
				if(i==(length+1))
				    {
					//if(verbose_mode)xil_printf("crc_lsb = %02x\r\n",ut1.st1.crc_lsb);

					tmp = ut1.st1.crc_lsb;
					for(j=0;j<8;j++){
						bits[8*i+j]= ((tmp & 1)==1)? SC1:SC0;
						tmp>>=1;
						//xil_printf("%d\t",bits[i][j]);
					}
					//if(verbose_mode)for(j=7;j>=0;j--){
					//	xil_printf("%02X\t",bits[8*i+j]);
					//		}


					//if(verbose_mode)xil_printf("\r\n");
					}
				else{
			//		if(verbose_mode)xil_printf("crc_msb = %02x\r\n",ut1.st1.crc_msb);
						tmp = (ut1.st1.crc_msb);
						for(j=0;j<8;j++){
						bits[8*i+j]= ((tmp & 1)==1)? SC1:SC0;
						tmp>>=1;
						//xil_printf("%d\t",bits[i][j]);
						}
				//	for(j=7;j>=0;j--){
				//		if(verbose_mode)xil_printf("%02X\t",bits[8*i+j]);
				//		}


				//	if(verbose_mode)xil_printf("\r\n");
					}

	}


}
//	j=0;for(i=0;i<sizeof(bits);i++)
//	{

//		if(verbose_mode)xil_printf("%02x\t",bits[i]);
//		if(j==7){
//			if(verbose_mode)xil_printf("\r\n");
//			j=0;}
//		else j++;
//	}


	/////////bit stuffing determination/////////////////
	///////////findind penta 5 1s///////////////
		j=0;
		u8 ZeroInsertion_location[50];
		u8 NUM_OF_ONES=0;
		u8 NUM_OF_ZEROS=0;
	for(i=8;i<(sizeof(bits)-8);i++)
		{

			if(bits[i]==SC1)
			{NUM_OF_ONES++;

			}
			else
			NUM_OF_ONES=0;

		//	ZeroInsertion_location[j]=i+1;


		if(NUM_OF_ONES==5)
		{
			//ZeroInsertion_location[j]=i+1;
			ZeroInsertion_location[j]=i;
			j++;
			NUM_OF_ONES=0;
		}

		}
	NUM_OF_ZEROS=j;
	if(verbose_mode){
	xil_printf("hdlc_tx::NUM_OF_ZEROS inserted =%d\r\n",NUM_OF_ZEROS);
//	for(i=0;i<NUM_OF_ZEROS;i++)xil_printf("ADDRESS OF_ZEROS (%d)=%d\r\n",i,ZeroInsertion_location[i]);
	}
	/////////////////////////////////////////////////
	//stuffing zeros
	if(verbose_mode)
	{
	for(int var=0; var< NUM_OF_ZEROS;var++)
		xil_printf("loc[%d]: %d\r\n ",var,ZeroInsertion_location[var]);
	}


	u8 bits_stuffed[sizeof(bits)+NUM_OF_ZEROS];
	u32 data_len = sizeof(bits)+NUM_OF_ZEROS;//sizeof(bits_stuffed);
	u8 k=0;
	u32 addr;
/*
	for(addr=0;addr<sizeof(bits);addr++)
	{
		if((addr==ZeroInsertion_location[k]) && (NUM_OF_ZEROS>0) && (k< NUM_OF_ZEROS))
		{
			bits_stuffed[addr+k]=SC0;
			bits_stuffed[addr+k+1]=bits[addr];//SC0;


		//	bits_stuffed[addr+k+2]=bits[addr+1];//SC0;
		//	bits_stuffed[addr+k+1]=SC0;

			k++;

		}
		else bits_stuffed[addr+k]=bits[addr];
	}

*/

	for(addr=0;addr<sizeof(bits);addr++)
		{
			bits_stuffed[addr+k]=bits[addr];
			if((addr==ZeroInsertion_location[k]) && (NUM_OF_ZEROS>0) && (k< NUM_OF_ZEROS))
			{
				bits_stuffed[addr+k+1]=SC0;
				//bits_stuffed[addr+k+1]=bits[addr];//SC0;


			//	bits_stuffed[addr+k+2]=bits[addr+1];//SC0;
			//	bits_stuffed[addr+k+1]=SC0;

				k++;

			}

		}





	if(verbose_mode)
	{

		xil_printf("BITS ARRAY HDLC_TX\r\n");
	j=0;for(i=0;i<sizeof(bits);i++)
		{

	if(verbose_mode)xil_printf("%02x,",bits[i]);
		if(j==7){
			if(verbose_mode)	xil_printf("\r\n");
			j=0;}
		else j++;
	}
   }


 data_len = sizeof(bits_stuffed);
for (i=0;i<data_len;i++){
	*(SourceAddr+i)=bits_stuffed[i];

	//if(verbose_mode)xil_printf("\r\nSourceBuffer(%d)=%x  bits_stuffed(%d)=%x\r\n",i,*(SourceAddr+i),i,*(bits_stuffed+i));
}
	/* Transmit the Data Stream */
		Status = TxSend(&FifoInstance, SourceAddr,data_len,verbose_mode);
		if (Status != XST_SUCCESS){
			xil_printf("Transmisson of Data failed\n\r");
			return XST_FAILURE;
		}

	//check_fifo( SourceAddr,data_len,verbose_mode);//bits_stuffed);


	/*if(mode==READ){


				p = calloc(10, sizeof(u8) );  // Make u8 array of 10 elements


			}

			else {


						p = calloc(14, sizeof(u8) );  // Make u8 array of 14 elements


					}
*/



//usleep(1000);usleep(1000);
//		XLlFifo_Reset(&FifoInstance);//added on 02-03-2018


//	xil_printf("\r\nsizeof(bits)=%d\r\n",sizeof(bits));
//	xil_printf("\r\nsizeof(bits_stuffed)=%d\r\n",sizeof(bits_stuffed));
}


signed configureADC(u8 ADC_address,u8 channel)
{
	unsigned  status;
	ByteCount=3;//sizeof(TxMsgPtr);
		if (channel == 0) 			 TxMsgPtr[1] = 0xc5;//ch0
  else  if (channel == 1)            TxMsgPtr[1] = 0xd5;//ch1
  else  if (channel == 2)            TxMsgPtr[1] = 0xe5;//ch2
  else  if (channel == 3)            TxMsgPtr[1] = 0xf5;//ch3
		////////write to config register//////////////////
		status=  XIic_Send	(	XPAR_AXI_IIC_0_BASEADDR,
				ADC_address,
				TxMsgPtr,
				ByteCount,
				0
				);
		return status;
}
signed short ReadADC(u8 ADC_address, u8 channel)
{
	unsigned  status;
	configureADC(ADC_address,channel);

		//u8 RcvBuffer [2]={0x00,0x00};

		//			u16 config_register;
					//XIic * 	InstancePtr;



	////////write to config register//////////////////
	/*ByteCount=3;//sizeof(TxMsgPtr);
	status=  XIic_Send	(	XPAR_AXI_IIC_0_BASEADDR,
			0x49,
			TxMsgPtr,
			ByteCount,
			0
			);
*/



	//////////////////////write to adress pointer register to point config register////////////////
/////////////////////////////////////TO MONITOR BUSY BIT MSB///////////////
	do{
		 config_register=0;
	ByteCount=1;//sizeof(TxMsgPtr_config);
	//while(1){
	status=  XIic_Send	(	XPAR_AXI_IIC_0_BASEADDR,
					ADC_address,
					TxMsgPtr_config,
					1,
					0
					);
	//xil_printf("trapped\r\n ");
	 //        }
	//////////////read config register//////////////////


	Num_Bytes_received=XIic_Recv(
									XPAR_AXI_IIC_0_BASEADDR,
									ADC_address,
									RcvBuffer,
									2,//byte count
									0
									);
	config_register=(RcvBuffer[0] <<8)|RcvBuffer[1];
	//xil_printf("config_register=%x\r\n ",config_register);
	config_register=config_register & 0x8000;

	}while(	config_register!=0x8000);
//////////////////////write to adress pointer register to point conversion register////////////////
	//xil_printf("out of while\r\n ");
	ByteCount=1;//sizeof(TxMsgPtr_conversion);
		status=  XIic_Send	(	XPAR_AXI_IIC_0_BASEADDR,
				ADC_address,
				TxMsgPtr_conversion,
				ByteCount,
				0
				);

	//usleep(2500);usleep(2500);usleep(2500);usleep(2500);
	//usleep(2500);usleep(2500);usleep(2500);usleep(2500);
	//usleep(2500);usleep(2500);usleep(2500);usleep(2500);


	/////////////////////////read conversion register//////////////////
	Num_Bytes_received=XIic_Recv(
								XPAR_AXI_IIC_0_BASEADDR,
								ADC_address,
								RcvBuffer,
								2,//sizeof(RcvBuffer),
								0
								);


	//xil_printf(" MSB=%x :   LSB=  %x\t", RcvBuffer[0], RcvBuffer[1]);
	//xil_printf(" %x\r\n",(RcvBuffer[0]<<8 |RcvBuffer[1]));
	//printf(" %f\r\n",(float)((signed short)(RcvBuffer[0]<<8 |RcvBuffer[1])*0.0000625));
return (signed short)(RcvBuffer[0]<<8 |RcvBuffer[1]);
}



u16 AdjustIref(u32 *SourceAddr,u32 *DestinationBuffer,int sd)
{

	//u32 register_address;
	//u16 register_data;
	u8 verbose_mode=0;
	u8 mode=0;
	u16 read_data;
	u8 direction;
	double imon_adc;
	double new_difference;
	double old_difference=1000;
	short adc_value;
	int i=0;


u8 set=0;


	/////////////write to GBL_CFG_RUN register for RUN bit=1////////////

	     read_data=read_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	     register_data    = 0X00000001 ;//| (read_data & 0xfffe);//SLEEP/RUN BIT=1;

	     write_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );

	////////////////////////////////////////////////////////////////////
	     //do{
	/////////////read  GBL_CFG_CTR_4 register ////////////
	      read_data = read_SC( GBL_CFG_CTR_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	      register_data    = (u16)((read_data & 0x0000fc40) | VREF_ADC_3 | MON_GAIN_1 | IREF);//MON SEL=0(imon) MON GAIN=0 VREF_ADC=3
		 //register_data    = ((read_data & 0xfffffc40) | 0X00000300);//MON SEL=0(imon) MON GAIN=0 VREF_ADC=3

		 write_SC( GBL_CFG_CTR_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );




//////////////read iref initial value//////////////////////
		read_data = read_SC( GBL_CFG_CTR_5, register_data, SourceAddr,DestinationBuffer,verbose_mode );
		register_data = read_data & 0x0000003F;
		adc_value = ReadADC(CALIB_ADC,1);
		imon_adc=adc_value*0.0625;//read value from external ADC IN MILLIVOLTS
		xil_printf("imon = %d mv\r\n", imon_adc);
		if(imon_adc == 100.0) direction= 0;
		else if(imon_adc < 100)direction = 1;//
		else  if(imon_adc > 100)direction = 2;



//configureADC(1);//channel 1
	do{
		write_SC( GBL_CFG_CTR_5, register_data, SourceAddr,DestinationBuffer,verbose_mode );

		adc_value = ReadADC(CALIB_ADC,1);
		imon_adc=adc_value*0.0625;//read value from external ADC IN MILLIVOLTS

		xil_printf("IREF = %d , imon =  %dmv ,   ADC = %x \r\n",register_data  , imon_adc * 100, adc_value);



		new_difference = abs(100 - imon_adc);
			if(new_difference < old_difference){if(direction ==1)register_data++;else if(direction ==2)register_data--;}
			else
				{ xil_printf("set=%d ,direction = %d\r\n",set,direction);
				set=1;if(direction ==1)register_data--;else if(direction ==2)register_data++;}


		old_difference= new_difference;

		//printf("%f\r\n",imon_adc-100);
	}while(set==0 && i++ < 62);

	if(i<62)
	{
		write_SC( GBL_CFG_CTR_5, register_data, SourceAddr,DestinationBuffer,verbose_mode );
		adc_value = ReadADC(CALIB_ADC,1);
		imon_adc=adc_value*0.0625;//read value from external ADC IN MILLIVOLTS
	xil_printf("selected Iref= %d , imon= %d mv  adc_value= %x i= %d\r\n",register_data, imon_adc *100,adc_value,i);
	LUT_CAL_DAC[0] = register_data;
	//LUT_CAL_DAC[1] = register_data<<16;
	LUT_CAL_DAC[1] = adc_value;
	//LUT_CAL_DAC[3] = adc_value<<8;

	SendReply(sd,LUT_CAL_DAC,4);
	}

	else {
		xil_printf("Iref not adjusted \r\n");
		LUT_CAL_DAC[0]=0xff;
		LUT_CAL_DAC[1]= 0xff;
		LUT_CAL_DAC[2]=0xff;
		LUT_CAL_DAC[3]= 0xff;

		SendReply(sd,LUT_CAL_DAC,4);

	}
	return register_data;// return iref register value

}

/*u16 AdjustIref_EXTENDED(u32 *SourceAddr,u32 *DestinationBuffer,int sd)
{

	//u32 register_address;
	//u16 register_data;
	u8 verbose_mode=0;
	u8 mode=0;
	u16 read_data;
	u8 direction;
	double imon_adc;
	double new_difference;
	double old_difference=1000;
	short adc_value;
	int i=0;


u8 set=0;


	/////////////write to GBL_CFG_RUN register for RUN bit=1////////////

	     read_data=read_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	     register_data    = 0X00000001 ;//| (read_data & 0xfffe);//SLEEP/RUN BIT=1;

	     write_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );

	////////////////////////////////////////////////////////////////////
	     //do{
	/////////////read  GBL_CFG_CTR_4 register ////////////
	      read_data = read_SC( GBL_CFG_CTR_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	      register_data    = (u16)((read_data & 0x0000fc40) | VREF_ADC_3 | MON_GAIN_1 | IREF);//MON SEL=0(imon) MON GAIN=0 VREF_ADC=3
		 //register_data    = ((read_data & 0xfffffc40) | 0X00000300);//MON SEL=0(imon) MON GAIN=0 VREF_ADC=3

		 write_SC( GBL_CFG_CTR_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );




//////////////read iref initial value//////////////////////
		read_data = read_SC( GBL_CFG_CTR_5, register_data, SourceAddr,DestinationBuffer,verbose_mode );
		register_data = read_data & 0x0000003F;
		adc_value = ReadADC(CALIB_ADC,1);
		imon_adc=adc_value*0.0625;//read value from external ADC IN MILLIVOLTS
		xil_printf("imon = %d mv\r\n", imon_adc);
		if(imon_adc == 100.0) direction= 0;
		else if(imon_adc < 100)direction = 1;//
		else  if(imon_adc > 100)direction = 2;



//configureADC(1);//channel 1
	do{
		write_SC( GBL_CFG_CTR_5, register_data, SourceAddr,DestinationBuffer,verbose_mode );
		LUT_CAL_DAC[i] = DAC;

							LUT_CAL_DAC[i+1]=	ReadADC(CALIB_ADC,0);

		adc_value = ReadADC(CALIB_ADC,1);
		imon_adc=adc_value*0.0625;//read value from external ADC IN MILLIVOLTS

		xil_printf("IREF = %d , imon =  %dmv ,   ADC = %x \r\n",register_data  , imon_adc * 100, adc_value);



		new_difference = abs(100 - imon_adc);
			if(new_difference < old_difference){if(direction ==1)register_data++;else if(direction ==2)register_data--;}
			else
				{ xil_printf("set=%d ,direction = %d\r\n",set,direction);
				set=1;if(direction ==1)register_data--;else if(direction ==2)register_data++;}


		old_difference= new_difference;

		//printf("%f\r\n",imon_adc-100);
	}while(set==0 && i++ < 62);

	if(i<62)
	{
		write_SC( GBL_CFG_CTR_5, register_data, SourceAddr,DestinationBuffer,verbose_mode );
		adc_value = ReadADC(CALIB_ADC,1);
		imon_adc=adc_value*0.0625;//read value from external ADC IN MILLIVOLTS
	xil_printf("selected Iref= %d , imon= %d mv  adc_value= %x i= %d\r\n",register_data, imon_adc *100,adc_value,i);
	LUT_CAL_DAC[0] = register_data;
	//LUT_CAL_DAC[1] = register_data<<16;
	LUT_CAL_DAC[1] = adc_value;
	//LUT_CAL_DAC[3] = adc_value<<8;

	SendReply(sd,LUT_CAL_DAC,4);
	}

	else {
		xil_printf("Iref not adjusted \r\n");
		LUT_CAL_DAC[0]=0xff;
		LUT_CAL_DAC[1]= 0xff;
		LUT_CAL_DAC[2]=0xff;
		LUT_CAL_DAC[3]= 0xff;

		SendReply(sd,LUT_CAL_DAC,4);

	}
	return register_data;// return iref register value

}*/

/*
u16 AdjustIref(u32 *SourceAddr,u32 *DestinationBuffer,int sd)
{

	u32 register_address;
	u16 register_data;
	u8 verbose_mode=0;
	u8 mode=0;
	u16 read_data;

	/////////////write to GBL_CFG_RUN register for RUN bit=1////////////

	     read_data=read_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	     register_data    = 0X00000001 | (read_data & 0xfffe);//SLEEP/RUN BIT=1;

	     write_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );

	////////////////////////////////////////////////////////////////////
	     //do{
	/////////////read  GBL_CFG_CTR_4 register ////////////
	      read_data = read_SC( GBL_CFG_CTR_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	      register_data    = (u16)((read_data & 0xfc40) | VREF_ADC_3 | MON_GAIN_1 | IREF);//MON SEL=0(imon) MON GAIN=0 VREF_ADC=3
		 //register_data    = ((read_data & 0xfffffc40) | 0X00000300);//MON SEL=0(imon) MON GAIN=0 VREF_ADC=3

		 write_SC( GBL_CFG_CTR_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );




//////////////read iref initial value//////////////////////
		read_data = read_SC( GBL_CFG_CTR_5, register_data, SourceAddr,DestinationBuffer,verbose_mode );
		register_data = read_data ;


		double imon_adc;

u8 set=0;
short adc_value;
//configureADC(1);//channel 1
	do{
		write_SC( GBL_CFG_CTR_5, register_data, SourceAddr,DestinationBuffer,verbose_mode );

		adc_value = ReadADC(1);
		imon_adc=adc_value*0.0625;//read value from external ADC IN MILLIVOLTS

		printf("%d ,  %f mv\r\n",register_data , imon_adc);

		if(imon_adc>100){
			if((imon_adc-100)>1.6)register_data--;
			else set=1;
		}
		else
		{
			if((100-imon_adc)>1.6)register_data++;
			else
				set=1;

		}

		//printf("%f\r\n",imon_adc-100);
	}while(set!=1);
	printf("Iref= %x , imon= %f mv  adc_value= %x\r\n",register_data, imon_adc,adc_value);
	LUT_CAL_DAC[0] = register_data;
	//LUT_CAL_DAC[1] = register_data<<16;
	LUT_CAL_DAC[1] = adc_value;
	//LUT_CAL_DAC[3] = adc_value<<8;

	SendReply(sd,LUT_CAL_DAC,4);

	return register_data;// return iref register value

}
*/



int CalibrateADC(u32 *SourceAddr,u32 *DestinationBuffer,int sd,u8 start,u8 step , u8 stop,u8 calibration_mode)
{
	int i=0;

	//u32 register_address;
	//u16 register_data;
	u8 verbose_mode=0;
	//u8 mode=0;

	u16 adc_0,adc_1;
	//double imon_adc,vmon_adc;
	/////////////write to GBL_CFG_RUN register for RUN bit=1////////////

	    //register_address = GBL_CFG_RUN;
	    read_data = read_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    register_data    = 1 | (read_data & 0xfffe);//SLEEP/RUN BIT=1;
	    write_SC(GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );


	////////////////////////////////////////////////////////////////////

	/////////////write to GBL_CFG_CTR_4 register ////////////
		//register_address = GBL_CFG_CTR_4;//vmon=adc)vref
		read_data = read_SC( GBL_CFG_CTR_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );

		register_data    = (VREF_ADC_3 | MON_GAIN_1 | PREAMP_INP_TRAN |  (read_data & 0xFC40) );//MON SEL=0(imon) MON GAIN=0 VREF_ADC=3
		write_SC( GBL_CFG_CTR_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );



		///////loop through different values of PRE I BIT REGISTER  /////////////
				// register_address 	= 	GBL_CFG_CAL_0;
				// register_data		=	0;
				// mode				=	1;
				 xil_printf("wait \r\n");
				 read_data = read_SC( GBL_CFG_BIAS_1, register_data, SourceAddr,DestinationBuffer,verbose_mode );
				 u8 DAC  = start;
				 register_data		= 	(u16)((read_data & 0xFF00) |DAC);// | VREF_ADC_BITS | MON_GAIN_BITS | MON_SEL_BITS;//MON SEL=0(imon) MON GAIN=0 VREF_ADC=3
				////////////////re read to verify write /////////////////////////////////////////////////
				 i=0;int loop=0;

				 int points = ((stop - start)/step) +1;
				 do{


					 write_SC( GBL_CFG_BIAS_1, register_data, SourceAddr,DestinationBuffer,verbose_mode );





					//register_address 	= 	ADC_READ_1;

				 	//if(register_data % 10 == 0)
				 	LUT_CAL_DAC[i] = DAC;

					LUT_CAL_DAC[i+1]=	ReadADC(CALIB_ADC,0);

					adc_0= read_SC( ADC_READ_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
					LUT_CAL_DAC[i+2]=adc_0;

					adc_1= read_SC( ADC_READ_1, register_data, SourceAddr,DestinationBuffer,verbose_mode );
					LUT_CAL_DAC[i+3] = adc_1;


					//xil_printf("i=%d,DAC VALUE = %d, ext_adc = %d ADC_0 = %d,ADC_1 =%d \r\n",i,LUT_CAL_DAC[i],LUT_CAL_DAC[i+1],LUT_CAL_DAC[i+2],LUT_CAL_DAC[i+3]);
				 	DAC+=step;
				 	read_data = read_SC( GBL_CFG_BIAS_1, register_data, SourceAddr,DestinationBuffer,verbose_mode );
				 	register_data		= 	(u16)((read_data & 0xFF00) |DAC);
					//register_data+=1;
				 	loop++;
				 	i=i+4;

				 }while(loop<points);//256  ORIGINAL
				 SendReply(sd,LUT_CAL_DAC,i*2);
				 //xil_printf("%d 16-bit values sent,\r\n",i);
				 	 xil_printf("done_adc calibration\r\n");


	return 0;

}

void print_echo_app_header()
{
    xil_printf("%20s %6d %s\r\n", "echo server",
                        echo_port,
                        "$ telnet <board_ip> 7");

}





int CalibrateCAL_DAC(u32 *SourceAddr,u32 *DestinationBuffer,int sd, u8 start,u8 step , u8 stop,u8 calibration_mode)
{

/* This routine will find fC values corresponding to the DAC values
 * of the CAL_DAC//////////////////////////////////
 */
	//u32 register_address;
	//u32 register_data;
	u8 verbose_mode=0;
	//u8 mode=0;
	u32 read_data;
	//double imon_adc,vmon_adc;
	double step_voltage, base_voltage,charge = 0;

	/////////////write to GBL_CFG_RUN register for RUN bit=1////////////
	     //register_address = GBL_CFG_RUN;
	     read_data = read_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	     //register_address = GBL_CFG_RUN;
	     register_data    = 0X00000001 | read_data;//SLEEP/RUN BIT=1;
	     write_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );

	     ////////////////////////////////////////////////////////////////////

	/////////////write to GBL_CFG_CTR_4 register ////////////
	     ////////////////set MONTORING REGISTER////////////////
		//register_address = GBL_CFG_CTR_4;//vmon=calib vstep


				//VREF_ADC_BITS 	=  	    VREF_ADC_3;
				//MON_SEL_BITS	=		CALIB_V_STEP;
				//MON_GAIN_BITS 	= 		MON_GAIN_1;

				//mode            = 0;//read transaction on hdlc slow control
				//HDLC_Tx(register_address , register_data,mode,SourceAddr,verbose_mode);//transmit of sc data
				//read_data=HDLC_Rx(DestinationBuffer,mode,verbose_mode);//receive acknowledge of sc data

				read_data = read_SC( GBL_CFG_CTR_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );

				register_data   = (u32)(VREF_ADC_3 | CALIB_V_STEP | MON_GAIN_1 | (read_data & 0xFC40) );//MON SEL=CALIB VSTEP MON GAIN=0 VREF_ADC=3

				write_SC( GBL_CFG_CTR_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );
				//double vmon_adc=ReadADC()*0.0625;//read value from external ADC IN MILLIVOLTS

				//xil_printf("cfg_ctr_4 set value , READ_DATA = %x \r\n", read_data);

				////////////////set GBL_CFG_CAL_0 REGISTER SET CAL_SEL_POL =1 ////////////////

				//CAL_DAC            =   start;

				//read_data = read_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
				//register_data       =   (0xFC03 & read_data) | (CAL_DAC<<2);
				//write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );


///////////////////////cal_pol to 1//////////////////////////////////////////////
				read_data = read_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
				register_data       =   (0xBFFF & read_data) | 0x4000;
				write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );

				//usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);
				u16 base_voltage_hex=ReadADC(CALIB_ADC,0);
				//xil_printf("base_voltage hex %04x",base_voltage_hex);

				//base_voltage =  base_voltage_hex*0.0000625;//v
                  /////////////SET CAL POL TO 0
				read_data = read_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
				register_data       =   (0xBFFF & read_data) | 0x0000;
				write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );



		///////SET CAL DAC TO 0 OR START VALUE /////////////
				CAL_DAC            =   start;

				read_data = read_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
				register_data       =   (0xFC03 & read_data) | (CAL_DAC<<2);
				write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );



				 //register_data		=	(u32)(read_data |((u32)CAL_DAC<<2));
				 //mode				=	1;
				 u16 index          =   0;
				 u16 i=0;
				 int points = (stop - start)/step +1;
				 do{


					 write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );

					LUT_CAL_DAC[i]   = CAL_DAC;
				 	LUT_CAL_DAC[i+1] = ReadADC(CALIB_ADC,0);
				///// 	step_voltage = LUT_CAL_DAC[index]*0.0000625;//volts
				///// 	charge       = (step_voltage - base_voltage)*100;
				////    printf("CAL_DAC= %d , GBL_CFG_CAL_0= %x , step_voltage= %fmv, base_voltage= %fmv, charge= %ffC  LUT_CAL_DAC= %04x \r\n",CAL_DAC,register_data,step_voltage*1000,base_voltage*1000,charge,LUT_CAL_DAC[CAL_DAC]);
				 	CAL_DAC+=step;
				 	register_data		=		(u32)(0xFC03 & read_data | ((u32)CAL_DAC<<2));

				 	index++;
				 	i+=2;
				 }while( index < points);
				 //	index++;
				 u16  *BV;
				 BV =&base_voltage_hex;
				 SendReply(sd,BV,2);
				 SendReply(sd,LUT_CAL_DAC,i*2);
				 xil_printf("index = %d points %d \r\n", index,points);
				 	 xil_printf("done_CAL_DAC  calibration\r\n");






	return 0;

}



int CalibrateCAL_DAC_extended(u32 *SourceAddr,u32 *DestinationBuffer,int sd, u8 start,u8 step , u8 stop,u8 calibration_mode)
{

/* This routine will find fC values corresponding to the DAC values
 * of the CAL_DAC//////////////////////////////////
 */
	//u32 register_address;
	//u32 register_data;
	u8 verbose_mode=0;
	//u8 mode=0;
	u32 read_data;
	//double imon_adc,vmon_adc;
	double step_voltage, base_voltage,charge = 0;

	/////////////write to GBL_CFG_RUN register for RUN bit=1////////////
	     //register_address = GBL_CFG_RUN;
	     read_data = read_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	     //register_address = GBL_CFG_RUN;
	     register_data    = 0X00000001 | read_data;//SLEEP/RUN BIT=1;
	     write_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );

	     ////////////////////////////////////////////////////////////////////

	/////////////write to GBL_CFG_CTR_4 register ////////////


				read_data = read_SC( GBL_CFG_CTR_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );

				register_data   = (u32)(VREF_ADC_3 | CALIB_V_STEP | MON_GAIN_1 | (read_data & 0xFC40) );//MON SEL=CALIB VSTEP MON GAIN=0 VREF_ADC=3

				write_SC( GBL_CFG_CTR_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );


				////////////////set GBL_CFG_CAL_0 REGISTER SET CAL_SEL_POL = 1 ////////////////


				read_data = read_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );

				register_data       =   (0xBFFF & read_data) | CAL_POL_1;//negative polarity to select base voltage
				write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );

				//usleep(1000);usleep(1000);usleep(1000);usleep(1000);usleep(1000);
				u16 base_voltage_hex=ReadADC(CALIB_ADC,0);
				base_voltage = base_voltage_hex * 0.0000625;
				              /////////////SET CAL POL TO 0
				read_data = read_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
				register_data       =   (0xBFFF & read_data) | CAL_POL_0;// positive polarity to select cal dac
				write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );



		///////SET CAL DAC TO 0 OR START VALUE /////////////
				CAL_DAC            =   start;

				read_data = read_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
				register_data       =   (0xFC03 & read_data) | (CAL_DAC<<2);
				write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );



				 //register_data		=	(u32)(read_data |((u32)CAL_DAC<<2));
				 //mode				=	1;
				 u16 index          =   0;
				 u16 i=0;
				 int points = (stop - start)/step +1;
				 do{


					 write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );

					LUT_CAL_DAC[i]   = CAL_DAC;
				 	LUT_CAL_DAC[i+1] = ReadADC(CALIB_ADC,0);
				 	//step_voltage = LUT_CAL_DAC[i+1]*0.0000625;//volts
				 	//charge       = (step_voltage - base_voltage)*100;
				    //xil_printf("CAL_DAC= %d , LUT_CAL_DAC= %04x \r\n",CAL_DAC,LUT_CAL_DAC[CAL_DAC]);
				 	CAL_DAC+=step;
				 	register_data		=		(u32)((0xFC03 & read_data) | ((u32)CAL_DAC<<2));

				 	index++;
				 	i+=2;
				 }while( index < points);
				 //	index++;
				 u16  *BV;
				 BV =&base_voltage_hex;
				 SendReply(sd,BV,2);
				 SendReply(sd,LUT_CAL_DAC,i*2);
				 xil_printf("index = %d points %d \r\n", index,points);
				 	 xil_printf("done_CAL_DAC  calibration\r\n");






	return 0;

}

int DAC_SCANS(u32 *SourceAddr,u32 *DestinationBuffer,int sd,u32 Mon_sel, u8 start,u8 step , u8 stop)
{

/* This routine will find fC values corresponding to the DAC values
 * of the CAL_DAC//////////////////////////////////
 */
	//u32 register_address;
	//u32 register_data;
	u32 dac_address;
	u16 DAC;
	u8 verbose_mode=0;
	u8 mode=0;
	u8 SHIFT=0;
	u16 MASK;
	u8 MAX;
	u16 read_data;
	//double imon_adc,vmon_adc;
	double step_voltage, base_voltage,charge = 0;
	int i = 0;

	/////////////write to GBL_CFG_RUN register for RUN bit=1////////////

	/////////////write to GBL_CFG_RUN register for RUN bit=1////////////

		     read_data=read_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );
		     register_data    = 0X00000001 | (read_data & 0xfffe);//SLEEP/RUN BIT=1;

		     write_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	     ////////////////////////////////////////////////////////////////////

	     switch(Mon_sel)
		{
	     case IREF:
	    	 	 dac_address =  GBL_CFG_CTR_5;
	    	 	 SHIFT = 0;
	    	 	 MASK  = 0XFFC0;
	    	 	 MAX = 63;
	    	 	 break;

	     case CALIB_IDC:
	    	 	 dac_address =  GBL_CFG_CAL_0;//????
	    	 	 SHIFT = 2;
	    	 	MASK  = 0XFC03;
	    	 	MAX = 255;
	    	 	 break;
	     case PREAMP_INP_TRAN:
	    	 	 dac_address =  GBL_CFG_BIAS_1;
	    	 	 SHIFT = 0;
	    	 	MASK  = 0XFF00;
	    	 	MAX = 255;
	    	 	 break;
	     case PREAMP_LC:
	    	 	 dac_address =  GBL_CFG_BIAS_2;
	    	 	 SHIFT = 8;
	    	 	MASK  = 0XC0FF;
	    	 	MAX = 63;
	    	 	 break;
	     case PREAMP_SF:
	    	 	 dac_address = GBL_CFG_BIAS_1;
	    	 	 SHIFT = 8;
	    	 	MASK  = 0XC0FF;
	    	 	MAX = 63;
	    	 	 break;
	     case SHAP_FC:
	    	 	 dac_address =  GBL_CFG_BIAS_3;
	    	 	 SHIFT = 8;
	    	 	MASK  = 0X00FF;
	    	 	MAX = 255;
	    	 	 break;
	     case SHAP_INPAIR:
	    	 	 dac_address =  GBL_CFG_BIAS_3;
	    	 	 SHIFT = 0;
	    	 	MASK  = 0XFF00;
	    	 	MAX = 255;
	    	 	 break;
	     case SD_INPAIR:
	    	 	 dac_address =  GBL_CFG_BIAS_4;
	    	 	 SHIFT = 0;
	    	 	MASK  = 0XFF00;
	    	 	MAX = 255;
	     	     	    break;

	     case SD_FC:
	    	 	 dac_address =  GBL_CFG_BIAS_5;
	    	 	 SHIFT = 0;
	    	 	MASK  = 0XFF00;
	    	 	MAX = 255;
	     	     break;
	     case SD_SF:
	    	 	 dac_address =  GBL_CFG_BIAS_5;
	    	 	 SHIFT = 8;
	    	 	MASK  = 0XC0FF;
	    	 	MAX = 63;
	     	     break;
	     case CFD_BIAS_1:
	    	 	 dac_address =  GBL_CFG_BIAS_0;
	    	 	 SHIFT = 0;
	    	 	MASK  = 0XFFC0;
	    	 	MAX = 63;
	     	     break;
	     case CFD_BIAS_2:
	    	 	 dac_address =  GBL_CFG_BIAS_0;
	    	 	 SHIFT = 6;
	    	 	MASK  = 0XF03F;
	    	 	MAX = 63;
	     	     break;
	     case CFD_HYST:
	    	 	 dac_address =  GBL_CFG_HYS;
	    	 	 SHIFT = 0;
	    	 	MASK  = 0XFFC0;
	    	 	MAX = 63;
	     	     break;
	     case CFD_IREF_LOCAL:
	    	 	 dac_address =  IREF;
	    	 	 SHIFT = 0;
	    	 	MASK  = 0XFFFF;
	    	 	MAX = 63;
	     	     break;
	     case CFD_TH_ARM:
	    	 	 dac_address =  GBL_CFG_THR;
	    	 	 SHIFT = 0;
	    	 	MASK  = 0XFF00;
	    	 	MAX = 255;
	     	     break;
	     case CFD_TH_ZCC:
	    	 	 dac_address = GBL_CFG_THR;
	    	 	 SHIFT = 8;
	    	 	MASK  = 0X00FF;
	    	 	MAX = 255;
	     	     break;
	     case SLVS_IBIAS:
	    	 	 dac_address =  GBL_CFG_BIAS_6;
	    	 	 SHIFT = 6;
	    	 	MASK  = 0XF03F;
	    	 	MAX = 63;
	     	     break;
	     case BGR:
	    	 	 dac_address =  IREF;
	    	 	 SHIFT = 0;
	    	 	MASK  = 0XFFFF;
	    	 	MAX = 63;
	     	     break;
	     case CALIB_V_STEP:
	    	 	 dac_address =  GBL_CFG_CAL_0;
	    	 	 SHIFT = 2;
	    	 	MASK  = 0XFC03;
	    	 	MAX = 255;
	    	 //SHIFT = 2;
	     	     break;
	     case PRE_AMP_VREF:
	    	 	 dac_address =  GBL_CFG_BIAS_2;
	    	 	 SHIFT = 0;
	    	 	MASK  = 0XFF00;
	    	 	MAX = 255;
	     	     break;
	     case V_TH_ARM:
	    	 	 dac_address =  GBL_CFG_THR;
	    	 	 SHIFT = 0;
	    	 	MASK  = 0XFF00;
	    	 	MAX = 255;
	     	     break;
	     case V_TH_ZCC:
	    	 	 dac_address =  GBL_CFG_THR;
	    	 	 SHIFT = 8;
	    	 	MASK  = 0X00FF;
	    	 	MAX = 255;
	     	     break;
	   /*  case V_TSENS_INT:
	    	 	 dac_address =  ???;
	    	 	 SHIFT = 2;
	     	     break;
	     case V_TSENS_EXT:
	    	 	 dac_address = ???;
	    	 	 SHIFT = 2;
	     	     break;
	     case ADC_VREF:
	    	 	 dac_address =  ????;
	    	 	 SHIFT = 2;
	     	     break;
	     case ADC_VIN_M:
	    	 	 dac_address =  ?????;
	    	 	 SHIFT = 2;
	     	     break;*/
	     case SLVS_VREF:
	    	 	 dac_address =  GBL_CFG_BIAS_6;
	    	 	 SHIFT = 0;
	    	 	MASK  = 0XFFC0;
	    	 	MAX = 63;
	    	 	 break;
	    default:
	     		dac_address =  IREF;
	     		SHIFT       =  0;
	     		MASK  = 0XFFFF;
	     		MAX = 63;
	     		break;

		}
xil_printf("dac_address  >> %X \r\n", dac_address);
	/////////////write to GBL_CFG_CTR_4 register ////////////
	     ////////////////set MONTORING REGISTER////////////////
		register_address = GBL_CFG_CTR_4;//vmon=calib vstep


				//VREF_ADC_BITS 	=  	    VREF_ADC_3;
				//MON_SEL_BITS	=		Mon_sel;
				//MON_GAIN_BITS 	= 		MON_GAIN_1;
		/////////////read  GBL_CFG_CTR_4 register ////////////
			      read_data = read_SC( GBL_CFG_CTR_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );
			      register_data    = (u16)((read_data & 0xfc40) | VREF_ADC_3 | MON_GAIN_1 | Mon_sel);//MON SEL=0(imon) MON GAIN=0 VREF_ADC=3
				 //register_data    = ((read_data & 0xfffffc40) | 0X00000300);//MON SEL=0(imon) MON GAIN=0 VREF_ADC=3

				 write_SC( GBL_CFG_CTR_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );


		///////loop through dac values /////////////

				register_address = dac_address;//THIS VALUE COMES FROM SWITCH ABOVE
				read_data = read_SC( register_address, register_data, SourceAddr,DestinationBuffer,verbose_mode );
				//xil_printf("Initial value , READ_DATA = %x \r\n", read_data);

				 DAC            =   start;
				 register_data		=	(read_data & MASK) |(DAC<<SHIFT);
				 //mode				=	1;
				 u16 indexx          =   0;
				 int points;
				 if (stop <=  MAX)
					 points = (stop - start)/step +1;
				 else
					 points = (MAX - start)/step +1;

				 i=0;
				 do{

					 //mode				=	1;
					//register_address 	= 	GBL_CFG_CAL_0;
					 //HDLC_Tx(register_address , register_data,mode,SourceAddr,verbose_mode);//transmit of sc data
				 	 //HDLC_Rx(DestinationBuffer,mode,verbose_mode);//receive acknowledge of sc data}
				 	write_SC( register_address, register_data, SourceAddr,DestinationBuffer,verbose_mode );
				 	//////////////////
				 	////////////////read      internal  adcS/////////////

				 	LUT_CAL_DAC[i] = DAC;
				 	LUT_CAL_DAC[i+1]= read_SC( ADC_READ_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
				 	//adc_0=LUT_CAL_DAC[i];


				 	LUT_CAL_DAC[i + 2]= read_SC( ADC_READ_1, register_data, SourceAddr,DestinationBuffer,verbose_mode );
				 	//adc_1=LUT_CAL_DAC[i+2];

				 	DAC+=step;
				 	register_data		=		(read_data & MASK) |(DAC<<SHIFT);

				 	indexx++;
				 	i+=3;

				 }while( indexx < points);
				 //	index++;
				 //u16  *BV;
				 //BV =&base_voltage_hex;
				 //SendReply(sd,BV,2);
				 SendReply(sd,LUT_CAL_DAC,i*2);
				 xil_printf("index = %d points %d  i= %d  \r\n", indexx,points,i);
				 	 xil_printf("done DAC SCAN   calibration\r\n");






	return 0;

}
int Scurve(u32 *SourceAddr,u32 *DestinationBuffer, u8 start_channel,u8 stop_channel, u8 step_channel,u8 start_CALDAC,u8 stop_CALDAC,u8 step_CALDAC,u16 Latency, u16 num_of_triggers,int sd,u8 arm_dac,u8 delay,u16 D1,u16 D2)
{
//u64 p=9;
/* This routine will get the scurves for all the channels
 *//////////////////////////////////

	u32 register_address;
	u32 register_data =0;
	u8 verbose_mode=0;
	u8 mode=0;
	u32 read_data;
	u8 channel;
	int u,v;

	for ( i=0;i<128;i++)
		for (j=0;j<128;j++)
			Hit_array[i][j]=0;

	//double imon_adc,vmon_adc;
	//double step_voltage, base_voltage,charge = 0;
	//u8 CAL_DAC;
	/////////////write to GBL_CFG_RUN register for RUN bit=1////////////

			     read_data=read_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );
			     register_data    = 0X00000001 | (read_data & 0xfffe);//SLEEP/RUN BIT=1;

			     write_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );
		     ////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	     ///////////////send run mode fast command////////////////
	     *SourceAddr =RUN_MODE;

	     Transmit_fast_command(&FifoInstance, SourceAddr,1,verbose_mode);


	     ////////////////////////SET NOMINAL VALUES FOR THE FRONT END//////////////////////////
	     /////////////////PREAMP  settings ,pre_i_bit,pre_i_bsf,pre_i_blcc,pre_vref

	     	 	 register_data       =   (u32)(PRE_I_BLCC<<8 |PRE_VREF);//pre_i_blcc,pre_vref
	    	     write_SC( GBL_CFG_BIAS_2, register_data, SourceAddr,DestinationBuffer,verbose_mode );



	    	     register_data       =   (u32)( (PRE_I_BSF<<8) |PRE_I_BIT);//,pre_i_bit,pre_i_bsf
	    	     write_SC( GBL_CFG_BIAS_1, register_data, SourceAddr,DestinationBuffer,verbose_mode );

	    	    /////NOMINAL VALUES FOR THE SHAPER ///////////////////////////////////


	    	     register_data       =   (u32)( (SH_I_BFCAS<<8) |SH_I_BDIFF);//SH_I_BFCAS, SH_I_BDIFF
	    	     write_SC( GBL_CFG_BIAS_3, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	     /////NOMINAL VALUES FOR THE SD ///////////////////////////////////

	    	     read_data=read_SC( GBL_CFG_BIAS_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	     register_data       =   (u32)( (read_data &  0xff00) | SD_I_BDIFF);//SD_I_BDIFF
	    	     write_SC( GBL_CFG_BIAS_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );




	    	     register_data       =   (u32)( (SD_I_BSF<<8) | SD_I_BFCAS);//SD_I_BSF , SD_I_BFCAS
  	    	     write_SC( GBL_CFG_BIAS_5, register_data, SourceAddr,DestinationBuffer,verbose_mode );


	     /////////////////////////////////////////////////////////////////////////////////////

	     ///////////set cal mode to 1 cal sel pol to 0   (GBL CFG CAL 0 register)/////////////////////////////////////////////////////////


	     register_data       =   0x00000001;//cal_sel_pol=0  cal mode 01 voltage pulse
	     write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	     ///////////set SEL COMP MODE TO 1  (GBL CFG CTR3 register)/////////////////////////////////////////////////////////


	     read_data=read_SC( GBL_CFG_CTR_3, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	     register_data       =  (read_data & 0xfffe) | 0x00000001;//SEL COMP MODE = 01 ARMING (CFD OUTPUT MODE =  ARMING)
	     write_SC( GBL_CFG_CTR_3, register_data, SourceAddr,DestinationBuffer,verbose_mode );

	        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	    ///////////set ARM DAC to 100 /////////////////////////////////////////////////////////

	     	read_data=read_SC( GBL_CFG_THR, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	     	register_data       =  (read_data & 0xff00) | arm_dac;//
	     	write_SC( GBL_CFG_THR, register_data, SourceAddr,DestinationBuffer,verbose_mode );




///////////////SET CAL DUR to 200//////////////


	    	  read_data=read_SC( GBL_CFG_CAL_1, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	  register_data       =  (read_data & 0xFE00) | 200;//200 CAL DUR
	    	  write_SC( GBL_CFG_CAL_1, register_data, SourceAddr,DestinationBuffer,verbose_mode );




	    	  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	    	  ///////////////SET PS to 7//////////////



	    	  	    	  read_data=read_SC( GBL_CFG_CTR_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	  	    	  register_data       =  (read_data & 0x1FFF) | 0XE000;//PS =7
	    	  	    	  write_SC( GBL_CFG_CTR_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );

	    	  	    	  //////////////setting Latency register/////////////////////
	    	  	    	write_SC( GBL_CFG_LAT, Latency, SourceAddr,DestinationBuffer,verbose_mode );
	    	  	    	////////////////////////////////////////////////////////////////////////


	    	  	    	  /////////////////////////////////////loop through all channels//////////////////////////////////////////////////////////////////////////////////////////////


	    	  	    	//////////////////////////////start outer most loop for channels////////////////////////////////////
	    	  	    	  channel=start_channel;
	    	  	    	  u32 length_scurve = (stop_CALDAC - start_CALDAC)+1;//data points to sendback
	    	  	    	xil_printf("\r\nPlease Wait ");
	    	  	    	  do{
//	    	  	    			  xil_printf("\r\nCHANNEL %03d :",channel);

	    	  	    	  ///////////////////////////SET CHANNEL FOR CALIBRATION //////////////////////



	    	  	  read_data=read_SC( channel, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	  	  register_data		   =  (read_data & 0x7fff)| 0x8000;//cal =1
	    	  	  write_SC( channel, register_data, SourceAddr,DestinationBuffer,verbose_mode );




                                     /////////////loop through cal dac values/////////////////////////
	    	  	                     ///////////START 2ND LOOP FOR CAL DAC VALUES///////////////////////////

	    	  	    	 CAL_DAC          =   start_CALDAC;


	    	  	    	read_data=read_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	  	    	register_data		=	(u32)((read_data & 0xfc03) | (CAL_DAC<<2));
	    	  	    	write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );

	    	  	    	short indexx=0;
	///    	  	    	xil_printf("[\t");
	    	  	    	do{

	    	  	    		write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );	    	  	    	 //for (u16 inner_loop=0;inner_loop < num_of_triggers;inner_loop++)
	    	  	    	 //{
	    	  	    	 //send_Calpulse_LV1As(channel,num_of_triggers,delay,D1, D2);//send lv1as with latency
	    	  	    	 //usleep(40);
	    	  	    	scurve_arr[indexx]= send_Calpulse_LV1As(channel,num_of_triggers,delay,D1, D2);//send lv1as with latency
	    	  	   	     //DecodeDataPacket(channel,CAL_DAC,Latency,num_of_triggers,verbose_mode);
	    	  	    	// }
	    	  	   	     //scurve_arr[index]=Hit_array[channel][CAL_DAC];
	   /// 	  	   	    xil_printf("%d\t",scurve_arr[index]);
	    	  	   	     indexx++;
   						//////////////////////////////////////////////////////////////////////////////////////////
   						  CAL_DAC+=step_CALDAC;//if(CAL_DAC>255)CAL_DAC=255;

   						read_data=read_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
   						register_data		=	(u32)((read_data & 0xfc03) | (CAL_DAC<<2));
   						//write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );




	    	  	    	}while(CAL_DAC<=stop_CALDAC);// END SECOND LOOP FOR CAL DAC VALUES
	    	  	    	xil_printf(".");



	    	  	    	read_data=read_SC( channel, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	  	   	    register_data		   =  (read_data & 0x7fff)| 0x0000;//cal =0
	    	  	         write_SC( channel, register_data, SourceAddr,DestinationBuffer,verbose_mode );

	    	  	    	for ( u=0;u<128;u++){
	    	  	    		    	  	    		for( v=0;v<256;v++)
	    	  	    		    	  	    			Hit_array[u][v]=0;
	    	  	    	}
	    	  	    	 SendReply(sd,scurve_arr,length_scurve * 2);
	    	  	    	 channel+= step_channel;//if(channel >127)channel=127;
	    	  	    	  }while(channel<=stop_channel);//128);// END OUTER MOST LOOP FOR CHANNELS





		 	 xil_printf("\r\ndone s-curve calibration\r\n");






	return 0;

}

int Scurve_extended(u32 *SourceAddr,u32 *DestinationBuffer, u8 start_channel,u8 stop_channel, u8 step_channel,u16 Latency, u16 num_of_triggers,int sd,u8 arm_dac,u8 delay,u16 D1,u16 D2,u8 points,u8 *dac_arr,u8 calpulse)
{
//u64 p=9;
/* This routine will get the scurves for all the channels
 *//////////////////////////////////

	//u32 register_address;
	//u32 register_data =0;
	u8 verbose_mode=0;
	u8 mode=0;
	//u32 read_data;
	u8 channel;
	int u,v;


	/////////////write to GBL_CFG_RUN register for RUN bit=1////////////

			     read_data=read_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );
			     register_data    = 0X00000001 | (read_data & 0xfffe);//SLEEP/RUN BIT=1;

			     write_SC( GBL_CFG_RUN, register_data, SourceAddr,DestinationBuffer,verbose_mode );
		     ////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	     ///////////////send run mode fast command////////////////
	     *SourceAddr =RUN_MODE;

	     Transmit_fast_command(&FifoInstance, SourceAddr,1,verbose_mode);




	     ///////////set cal mode to 1 cal sel pol to 0   (GBL CFG CAL 0 register)/////////////////////////////////////////////////////////


  	    	   read_data = read_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
  	    	   register_data       =   (0xBFFC & read_data) | 0x0001;//
  	    	   write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );


	     ///////////set SEL COMP MODE TO 1  (GBL CFG CTR3 register)/////////////////////////////////////////////////////////

	     read_data=read_SC( GBL_CFG_CTR_3, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	     register_data       =  (read_data & 0xfffC) | 0x00000001;//SEL COMP MODE = 01 ARMING (CFD OUTPUT MODE =  ARMING)
	     write_SC( GBL_CFG_CTR_3, register_data, SourceAddr,DestinationBuffer,verbose_mode );

	        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	    ///////////set ARM DAC to 100 /////////////////////////////////////////////////////////

	     	read_data=read_SC( GBL_CFG_THR, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	     	register_data       =  (read_data & 0xff00) | arm_dac;//
	     	write_SC( GBL_CFG_THR, register_data, SourceAddr,DestinationBuffer,verbose_mode );




///////////////SET CAL DUR to 200//////////////


	    	  read_data=read_SC( GBL_CFG_CAL_1, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	  register_data       =  (read_data & 0xFE00) | 200;//200 CAL DUR
	    	  write_SC( GBL_CFG_CAL_1, register_data, SourceAddr,DestinationBuffer,verbose_mode );




	    	  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	    	  ///////////////SET PS to 7//////////////



	    	  	    	  read_data=read_SC( GBL_CFG_CTR_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	  	    	  register_data       =  (read_data & 0x1FFF) | 0XE000;//PS =7
	    	  	    	  write_SC( GBL_CFG_CTR_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );

	    	  	    	  //////////////setting Latency register/////////////////////
	    	  	    	write_SC( GBL_CFG_LAT, Latency, SourceAddr,DestinationBuffer,verbose_mode );
	    	  	    	////////////////////////////////////////////////////////////////////////


	    	  	    	  /////////////////////////////////////loop through all channels//////////////////////////////////////////////////////////////////////////////////////////////


	    	  	    	//////////////////////////////start outer most loop for channels////////////////////////////////////
	    	  	    	  channel=start_channel;
	    	  	    	  xil_printf("\r\nPlease Wait ");
	    	  	    	  do{
	    	  	    			  xil_printf("\r\nCHANNEL %03d :",channel);

	    	  	    	  ///////////////////////////SET CHANNEL FOR CALIBRATION //////////////////////


	    	  	   if(calpulse == 1){
	    	  	  read_data=read_SC( channel, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	  	  register_data		   =  (read_data & 0x7fff)| 0x8000;//cal =1
	    	  	  write_SC( channel, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	  	   }



                                     /////////////loop through cal dac values/////////////////////////
	    	  	                     ///////////START 2ND LOOP FOR CAL DAC VALUES///////////////////////////

	    	  	    	 //CAL_DAC          =   start_CALDAC;


	    	  	    	//read_data=read_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	  	    	//register_data		=	(u32)((read_data & 0xfc03) | (CAL_DAC<<2));
	    	  	    	//write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );

	    	  	    	short indexx=0;
	    	  	    	xil_printf("[\t");
	    	  	    	do{
	    	  	    		CAL_DAC= dac_arr[indexx];
	    	  	    		read_data=read_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	  	    		register_data		=	(u32)((read_data & 0xfc03) | (CAL_DAC<<2));
	    	  	    		write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );	    	  	    	 //for (u16 inner_loop=0;inner_loop < num_of_triggers;inner_loop++)
	    	  	    	 //{
	    	  	    		scurve_arr[indexx]= send_Calpulse_LV1As(channel,num_of_triggers,delay,D1, D2);//send lv1as with latency
	    	  	    	 //usleep(40);
	    	  	   	     //DecodeDataPacket(channel,CAL_DAC,Latency,num_of_triggers,verbose_mode);
	    	  	    	// }
	    	  	   	    // scurve_arr[indexx]=Hit_array[channel][CAL_DAC];
	    	  	   	    xil_printf("%d,",scurve_arr[indexx]);
	    	  	   	     indexx++;
   						//////////////////////////////////////////////////////////////////////////////////////////
   						//  CAL_DAC+=step_CALDAC;//if(CAL_DAC>255)CAL_DAC=255;


   						//write_SC( GBL_CFG_CAL_0, register_data, SourceAddr,DestinationBuffer,verbose_mode );




	    	  	    	}while(indexx<points);// END SECOND LOOP FOR CAL DAC VALUES
	    	  	    	//xil_printf(".");
	    	  	    //	xil_printf("]\r\n");

	    	  	    	if(calpulse==1){
	    	  	    	read_data=read_SC( channel, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	  	   	    register_data		   =  (read_data & 0x7fff)| 0x0000;//cal =0
	    	  	         write_SC( channel, register_data, SourceAddr,DestinationBuffer,verbose_mode );
	    	  	    	}
//	    	  	    	for ( u=0;u<128;u++){
//	    	  	    		    	  	    		for( v=0;v<256;v++)
//	    	  	    		    	  	    			Hit_array[u][v]=0;
//	    	  	    	}
	    	  	    	 SendReply(sd,scurve_arr,indexx * 2);
	    	  	    	 channel+= step_channel;//if(channel >127)channel=127;
	    	  	    	  }while(channel<=stop_channel);//128);// END OUTER MOST LOOP FOR CHANNELS





		 	 xil_printf("\r\ndone s-curve calibration\r\n");






	return 0;

}

void Set_front_end_Nominal(u32 *SourceAddr,u32 *DestinationBuffer)
{
	u8 verbose_mode=0;
	////////////////////////SET NOMINAL VALUES FOR THE FRONT END//////////////////////////
		     /////////////////PREAMP  settings ,pre_i_bit,pre_i_bsf,pre_i_blcc,pre_vref

		     	 	 register_data       =   (u32)(PRE_I_BLCC<<8 |PRE_VREF);//pre_i_blcc,pre_vref
		    	     write_SC( GBL_CFG_BIAS_2, register_data, SourceAddr,DestinationBuffer,verbose_mode );



		    	     register_data       =   (u32)( (PRE_I_BSF<<8) |PRE_I_BIT);//,pre_i_bit,pre_i_bsf
		    	     write_SC( GBL_CFG_BIAS_1, register_data, SourceAddr,DestinationBuffer,verbose_mode );

		    	    /////NOMINAL VALUES FOR THE SHAPER ///////////////////////////////////


		    	     register_data       =   (u32)( (SH_I_BFCAS<<8) |SH_I_BDIFF);//SH_I_BFCAS, SH_I_BDIFF
		    	     write_SC( GBL_CFG_BIAS_3, register_data, SourceAddr,DestinationBuffer,verbose_mode );
		    	     /////NOMINAL VALUES FOR THE SD ///////////////////////////////////

		    	     read_data=read_SC( GBL_CFG_BIAS_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );
		    	     register_data       =   (u32)( (read_data &  0xff00) | SD_I_BDIFF);//SD_I_BDIFF
		    	     write_SC( GBL_CFG_BIAS_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );




		    	     register_data       =   (u32)( (SD_I_BSF<<8) | SD_I_BFCAS);//SD_I_BSF , SD_I_BFCAS
	  	    	     write_SC( GBL_CFG_BIAS_5, register_data, SourceAddr,DestinationBuffer,verbose_mode );


		     /////////////////////////////////////////////////////////////////////////////////////


}

void Set_front_end(u32 *SourceAddr,u32 *DestinationBuffer,u8 PRE_I_BLCC_reg,u8 PRE_VREF_reg,u8 PRE_I_BSF_reg,u8 PRE_I_BIT_reg,u8 SH_I_BFCAS_reg,u8 SH_I_BDIFF_reg,u8 SD_I_BDIFF_reg,u8 SD_I_BSF_reg,u8 SD_I_BFCAS_reg)
{
	u8 verbose_mode = 0;
	////////////////////////SET NOMINAL VALUES FOR THE FRONT END//////////////////////////
		     /////////////////PREAMP  settings ,pre_i_bit,pre_i_bsf,pre_i_blcc,pre_vref

		     	 	 register_data       =   (u32)(PRE_I_BLCC_reg<<8 |PRE_VREF_reg);//pre_i_blcc,pre_vref
		    	     write_SC( GBL_CFG_BIAS_2, register_data, SourceAddr,DestinationBuffer,verbose_mode );



		    	     register_data       =   (u32)( (PRE_I_BSF_reg<<8) |PRE_I_BIT_reg);//,pre_i_bit,pre_i_bsf
		    	     write_SC( GBL_CFG_BIAS_1, register_data, SourceAddr,DestinationBuffer,verbose_mode );

		    	    /////NOMINAL VALUES FOR THE SHAPER ///////////////////////////////////


		    	     register_data       =   (u32)( (SH_I_BFCAS_reg<<8) |SH_I_BDIFF_reg);//SH_I_BFCAS, SH_I_BDIFF
		    	     write_SC( GBL_CFG_BIAS_3, register_data, SourceAddr,DestinationBuffer,verbose_mode );
		    	     /////NOMINAL VALUES FOR THE SD ///////////////////////////////////

		    	     read_data=read_SC( GBL_CFG_BIAS_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );
		    	     register_data       =   (u32)( (read_data &  0xff00) | SD_I_BDIFF_reg);//SD_I_BDIFF
		    	     write_SC( GBL_CFG_BIAS_4, register_data, SourceAddr,DestinationBuffer,verbose_mode );




		    	     register_data       =   (u32)( (SD_I_BSF_reg<<8) | SD_I_BFCAS_reg);//SD_I_BSF , SD_I_BFCAS
	  	    	     write_SC( GBL_CFG_BIAS_5, register_data, SourceAddr,DestinationBuffer,verbose_mode );


		     /////////////////////////////////////////////////////////////////////////////////////


}

//int ProcessDataPackets(XLlFifo *InstancePtr, u16 DeviceId,u16 Latency, u16 num_of_triggers,u8 verbose_mode)
//{
//
//
//}

int DecodeDataPacket(u8 channel, u8 CAL_DAC,u16 Latency, u16 num_of_triggers,u8 verbose_mode)
{

	//u32 i,j;
	u32 Rcv_len;
	u8 Header;
	u8 loop;
	u8 indexx;
	u8 PKT_LEN=22;

////	xil_printf("\r\nENTERING DECODE DATA PACKET\r\n");
	Rcv_len = RxReceive(&FifoInstance, DestinationBuffer,verbose_mode);
	//xil_printf("Rcv_len=%d\r\n", Rcv_len);
	   							//	if (Status != XST_SUCCESS){
	   							//		xil_printf("Receiving data failed");
	   							//		return XST_FAILURE;
	   							//	}

	   								//Error = 0;

	   								if(Rcv_len>0 && Rcv_len<MAX_DATA_BUFFER_SIZE)
	   								{
	   								//	xil_printf("rcv_len=%d\r\n",Rcv_len);
	   								//for(i=0;i<Rcv_len;i++)
	   									/////////////////find header of data packet///////////
	   									j=0;i=0;


	   								for(i=0;i<num_of_triggers;i++)
	   								{
										//#pragma HLS UNROLL
	   									//j=0;
	   									do
	   									{
										//#pragma HLS UNROLL
	   									datapacket[0] = *(DestinationBuffer+j);
	   									//*(datapacket+0) = *(DestinationBuffer+j);
	   									j++;
	   									}while(!(datapacket[0]== 0x1E || j>=Rcv_len));
	   								//}while(!(*(datapacket)== 0x1E || j>=Rcv_len));

	   								if(j>=Rcv_len)
	   									{
	   									 //Error_Dpkt = 1;
	   									 xil_printf("Error while decoding datapacket, possible not enough data\r\n");
	   									return 0;
   									}


	   								//int flag=0;
	   								for( indexx = 1;indexx <= 22;indexx++,j++)
	   								{
	   								//xil_printf("indexx= %d,j=%d\r\n", indexx,j);
	   									//#pragma HLS UNROLL
	   									datapacket[indexx]= *(DestinationBuffer+j);
	   									//*(datapacket +indexx)= *(DestinationBuffer+j);
	   									//xil_printf("datapacket[%d]= %x:: destination buffer[%d]=%x\r\n", indexx,datapacket[indexx],j,*(DestinationBuffer+j));
	   								}
	   								//flag=1;
	   								/////////////////loop unrolling///////////////

	   								//indexx=1;
	   								//datapacket[indexx]= *(DestinationBuffer+j);



	   								//loop++;
	   								//xil_printf("\t\t/////////////////////////////////data packet %d\r\n",i+1);
	   								//xil_printf("\r\nHeader = %02x\r\n",datapacket[0]);
	   								//xil_printf("EC= %02x\r\n",datapacket[1]);
	   								//xil_printf("BC= %04x\r\n",datapacket[2]<<8 | datapacket[3]  );
	   					///			xil_printf("%02x%02x%02x%02x%02x%02x%02x%02x", datapacket[4],datapacket[5],datapacket[6],datapacket[7],datapacket[8],datapacket[9],datapacket[10],datapacket[11]);
	   					///			xil_printf("%02x%02x%02x%02x%02x%02x%02x%02x\t\t",datapacket[12],datapacket[13],datapacket[14],datapacket[15],datapacket[16],datapacket[17],datapacket[18],datapacket[19]);
	   								Hit_array[channel][CAL_DAC] += (datapacket [19-channel/8] & mask_array[channel & 0x7]) ? 1:0;
	   								//Hit_array[channel][CAL_DAC] += ( *(datapacket + 19-channel/8) & mask_array[channel & 0x7]) ? 1:0;
	   								//Hit_array[channel][CAL_DAC] += (19-channel/8] & mask_array[channel & 0x7]) ? 1:0;
	   								//scurve[i]+=(datapacket [19-channel/8] & mask_array[channel & 0x7]) ? 1:0;
	   								//xil_printf("\r\nchecksum= %02x%02x\r\n",datapacket[20],datapacket[21]);
	   								//u16 chk_sum = crc16(datapacket, 20);
  									   								//loop++;
	   								//xil_printf("checksum calculated = %x\r\n",chk_sum);
	   									   								//xil_printf("Loop= %d\r\n",loop);


	   									   								//	xil_printf("rcv length=%d :: data_packet[%d]=%x \r\n ",Rcv_len,i,*(DestinationBuffer+i));
	   								}
	   								//Error_Dpkt=0;





	   								}
	   								else
	   								{
	   									xil_printf("No data received\r\n");
	   									XLlFifo_Reset(&FifoInstance);
	   								}



return 0;
}


/*
int DecodeDataPacket(u8 channel, u8 CAL_DAC,u16 Latency, u16 num_of_triggers,u8 verbose_mode)
{

	u32 i,j;
	u32 Rcv_len;
	u8 Header;
	u8 loop;
	u8 indexx;
	u8 PKT_LEN=22;
	u32 RxWord;




	   									/////////////////find header of data packet///////////
	   									j=0;i=0;


	   								for(i=0;i<num_of_triggers;i++)
	   								{


	   									//j=0;
	   									do
	   									{
	   										//xil_printf("In do while only\r\n");
	   										//if(XLlFifo_iRxOccupancy(&FifoInstance))
	   									{
	   										RxWord = XLlFifo_RxGetWord(&FifoInstance);
	   										datapacket[0] = RxWord;
	   									    j++;
	   									 //xil_printf("stucked in first do while\r\n");
	   									}
	   									}while(!(datapacket[0]== 0x1E || j>num_of_triggers*27));

	   								if(j>=(num_of_triggers*27))
	   									{
	   									 //Error_Dpkt = 1;
	   									 xil_printf("Error while decoding datapacket, possible not enough data\r\n");
	   									return 0;
   									}


	   								//int flag=0;
	   								for( indexx = 1;indexx <= 22;indexx++,j++)
	   								{
	   								//xil_printf("indexx= %d,j=%d\r\n", indexx,j);
	   									//#pragma HLS UNROLL
	   								//	if(XLlFifo_iRxOccupancy(&FifoInstance))
	   									{
	   									datapacket[indexx]= XLlFifo_RxGetWord(&FifoInstance);
	   									//xil_printf("datapacket[%d]= %x:: destination buffer[%d]=%x\r\n", indexx,datapacket[indexx],j,*(DestinationBuffer+j));
	   									}
	   								}


	   								Hit_array[channel][CAL_DAC] += (datapacket [19-channel/8] & mask_array[channel & 0x7]) ? 1:0;
	   								//Hit_array[channel][CAL_DAC] += (19-channel/8] & mask_array[channel & 0x7]) ? 1:0;
	   								//scurve[i]+=(datapacket [19-channel/8] & mask_array[channel & 0x7]) ? 1:0;
	   								//xil_printf("\r\nchecksum= %02x%02x\r\n",datapacket[20],datapacket[21]);
	   								//u16 chk_sum = crc16(datapacket, 20);
  									   								//loop++;
	   								//xil_printf("checksum calculated = %x\r\n",chk_sum);
	   									   								//xil_printf("Loop= %d\r\n",loop);


	   									   								//	xil_printf("rcv length=%d :: data_packet[%d]=%x \r\n ",Rcv_len,i,*(DestinationBuffer+i));

	   								//Error_Dpkt=0;





	   								}
	   						//		else
	   							//	{
	   								//	xil_printf("No data received\r\n");
	   								//	XLlFifo_Reset(&FifoInstance);
	   								//}



return 0;
}


*/
void SendReply(int sd, u32 *Buffer, u32 length)
{
	/* handle request */
			if ((nwrote = write(sd, Buffer, length)) < 0) {
				//for(int i=0;i<5*4;i++)nwrote = write(sd, DestinationBuffer, 5*4);
				xil_printf("%s: ERROR responding to client echo request. received = %d, written = %d\r\n",
						__FUNCTION__, n, nwrote);
				xil_printf("Closing socket %d\r\n", sd);
				//break;
			}
}

/* thread spawned for each connection */
void process_echo_request(void *p,XLlFifo *InstancePtr_tx)
{
	int  sd = (int)p;
	//int RECV_BUF_SIZE = 2048;
	//u8 recv_buf[RECV_BUF_SIZE];
	//int n, nwrote;
	//XLlFifo FifoInstance_tx;
	//XLlFifo FifoInstance_rx;

	while (1) {
		/* read a max of RECV_BUF_SIZE bytes from socket */
		if ((n = read(sd, recv_buf, RECV_BUF_SIZE)) < 0) {
			xil_printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
			break;
		}






		/* break if the recved message = "quit"
		if (!strncmp(recv_buf, "quit", 4)){xil_printf("break strncmp\r\n");
			break;}*/

		/* break if client closed connection */
		if (n <= 0){xil_printf("break n<=0\r\n");
			break;}

		u32 *SourceAddr;
		SourceAddr= SourceBuffer;
		xil_printf("entering decode receive packet\r\n");
		decode_receive_packet(recv_buf,SourceAddr,sd);
        //for(int i=0;i<5*4;i++)xil_printf("destination buffer%x \r\n",*(DestinationBuffer+i));
		/* handle request */
/////		if ((nwrote = write(sd, DestinationBuffer, 5*4)) < 0) {
			//for(int i=0;i<5*4;i++)nwrote = write(sd, DestinationBuffer, 5*4);
//////			xil_printf("%s: ERROR responding to client echo request. received = %d, written = %d\r\n",
///////					__FUNCTION__, n, nwrote);
////////			xil_printf("Closing socket %d\r\n", sd);
///////			break;
///////		}



	}
//if(flag==0)
//	Initialize_everything();
//xil_printf("source buffer %x  %x  %x  %x\r\n",SourceBuffer[0],SourceBuffer[1],SourceBuffer[2],SourceBuffer[3]);
//xil_printf("destination buffer %x  %x  %x  %x\r\n",DestinationBuffer[0],DestinationBuffer[1],DestinationBuffer[2],DestinationBuffer[3]);




//	check_fifo();//transmit some data





	/* close connection */
	close(sd);
	vTaskDelete(NULL);
}

void echo_application_thread()
{
	int sock, new_sd;
	struct sockaddr_in address, remote;
	int size;

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return;

	address.sin_family = AF_INET;
	address.sin_port = htons(echo_port);
	address.sin_addr.s_addr = INADDR_ANY;

	if (lwip_bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
		return;

	lwip_listen(sock, 0);

	size = sizeof(remote);

	while (1) {
		if ((new_sd = lwip_accept(sock, (struct sockaddr *)&remote, (socklen_t *)&size)) > 0) {
			sys_thread_new("echos", process_echo_request,
				(void*)new_sd,
				THREAD_STACKSIZE,
				DEFAULT_THREAD_PRIO);
		}
	}
}