/*
 * Empty C++ Application
 */
#include <stdio.h>
#include <unistd.h>
#include "xparameters.h"
//#include "tb_sample.h"
//#include "xyolo_conv_top.h"
#include "xyolo_conv_dw_top.h"
//#include "xyolo_conv_pw_top.h"
#include "xyolo_max_pool_top.h"
#include "xyolo_yolo_top.h"
#include "xyolo_acc_top.h"
#include "xyolo_upsamp_top.h"
#include "xaxidma.h"
#include "xaxis_switch.h"

//#include "group_0_0_input.h"
#include "group_0_input.h"
//#include "group_0_0_weights.h"
#include "group_0_weight.h"
//#include "group_0_0_biases.h"

#define INPUT_W 416
#define INPUT_H 416
#define OUTPUT_W INPUT_W
#define OUTPUT_H INPUT_H
#define IN_CHANNEL 4
#define OUT_CHANNEL 4

//XYolo_conv_top yolo_conv_top;
//XYolo_conv_top_Config *yolo_conv_top_cfg;
XYolo_conv_dw_top yolo_conv_dw_top;
XYolo_conv_dw_top_Config *yolo_conv_dw_top_cfg;
//XYolo_conv_pw_top yolo_conv_pw_top;
//XYolo_conv_pw_top_Config *yolo_conv_pw_top_cfg;
XYolo_max_pool_top yolo_max_pool_top;
XYolo_max_pool_top_Config *yolo_max_pool_top_cfg;
XYolo_yolo_top yolo_yolo_top;
XYolo_yolo_top_Config *yolo_yolo_top_cfg;
XYolo_acc_top yolo_acc_top;
XYolo_acc_top_Config *yolo_acc_top_cfg;
XYolo_upsamp_top yolo_upsamp_top;
XYolo_upsamp_top_Config *yolo_upsamp_top_cfg;
XAxiDma axiDMA_0;
XAxiDma_Config *axiDMA_cfg_0;
XAxiDma axiDMA_1;
XAxiDma_Config *axiDMA_cfg_1;
XAxis_Switch axis_switch_0, axis_switch_1, axis_switch_2;
XAxis_Switch_Config *axis_switch_cfg_0, *axis_switch_cfg_1, *axis_switch_cfg_2;

//void init_yolo_conv()
//{
//	printf("Initializing yolo_conv_top\n");
//	yolo_conv_top_cfg = XYolo_conv_top_LookupConfig(XPAR_YOLO_CONV_TOP_0_DEVICE_ID);
//	if(yolo_conv_top_cfg)
//	{
//		int status = XYolo_conv_top_CfgInitialize(&yolo_conv_top, yolo_conv_top_cfg);
//		if(status != XST_SUCCESS)
//		{
//			printf("Error initializing yolo_conv_top core\n");
//		}
//	}
//}

void init_yolo_conv_dw()
{
  printf("Initializing yolo_conv_dw_top\n");
  yolo_conv_dw_top_cfg = XYolo_conv_dw_top_LookupConfig(XPAR_YOLO_CONV_DW_TOP_0_DEVICE_ID);
  if(yolo_conv_dw_top_cfg)
  {
    int status = XYolo_conv_dw_top_CfgInitialize(&yolo_conv_dw_top,yolo_conv_dw_top_cfg);
    if(status != XST_SUCCESS)
    {
      printf("Error initializing yolo_conv_dw_top core\n");
    }
  }
}

//void init_yolo_conv_pw()
//{
//  printf("Initializing yolo_conv_pw_top\n");
//  yolo_conv_pw_top_cfg = XYolo_conv_pw_top_LookupConfig(XPAR_YOLO_CONV_PW_TOP_0_DEVICE_ID);
//  if(yolo_conv_pw_top_cfg)
//  {
//    int status = XYolo_conv_pw_top_CfgInitialize(&yolo_conv_pw_top,yolo_conv_pw_top_cfg);
//    if(status != XST_SUCCESS)
//    {
//      printf("Error initializing yolo_conv_pw_top core\n");
//    }
//  }
//}
//
void init_yolo_max_pool()
{
  printf("Initializing yolo_max_pool_top\n");
  yolo_max_pool_top_cfg = XYolo_max_pool_top_LookupConfig(XPAR_YOLO_MAX_POOL_TOP_0_DEVICE_ID);
  if(yolo_max_pool_top_cfg)
  {
    int status = XYolo_max_pool_top_CfgInitialize(&yolo_max_pool_top,yolo_max_pool_top_cfg);
    if(status != XST_SUCCESS)
    {
      printf("Error initializing yolo_max_pool_top core\n");
    }
  }
}

void init_yolo_yolo()
{
  printf("Initializing yolo_yolo_top\n");
  yolo_yolo_top_cfg = XYolo_yolo_top_LookupConfig(XPAR_YOLO_YOLO_TOP_0_DEVICE_ID);
  if(yolo_yolo_top_cfg)
  {
    int status = XYolo_yolo_top_CfgInitialize(&yolo_yolo_top,yolo_yolo_top_cfg);
    if(status != XST_SUCCESS)
    {
      printf("Error initializing yolo_yolo_top core\n");
    }
  }
}

void init_yolo_acc()
{
	printf("Initializing yolo_acc_top\n");
	yolo_acc_top_cfg = XYolo_acc_top_LookupConfig(XPAR_YOLO_ACC_TOP_0_DEVICE_ID);
	if(yolo_acc_top_cfg)
	{
		int status = XYolo_acc_top_CfgInitialize(&yolo_acc_top, yolo_acc_top_cfg);
		if(status != XST_SUCCESS)
		{
			printf("Error initializing yolo_acc_top core\n");
		}
	}
}

void init_yolo_upsamp()
{
  printf("Initializing yolo_upsamp_top\n");
  yolo_upsamp_top_cfg = XYolo_upsamp_top_LookupConfig(XPAR_YOLO_UPSAMP_TOP_0_DEVICE_ID);
  if(yolo_upsamp_top_cfg)
  {
    int status = XYolo_upsamp_top_CfgInitialize(&yolo_upsamp_top,yolo_upsamp_top_cfg);
    if(status != XST_SUCCESS)
    {
      printf("Error initializing yolo_upsamp_top core\n");
    }
  }
}

void init_axi_dma()
{
  printf("Initializing AxiDMA\n");
  axiDMA_cfg_0 = XAxiDma_LookupConfig(XPAR_AXIDMA_0_DEVICE_ID);
  if(axiDMA_cfg_0)
  {
    int status = XAxiDma_CfgInitialize(&axiDMA_0, axiDMA_cfg_0);
    if(status != XST_SUCCESS)
    {
      printf("Error initializing AxiDMA core 0\n");
    }
  }
  axiDMA_cfg_1 = XAxiDma_LookupConfig(XPAR_AXIDMA_1_DEVICE_ID);
  if(axiDMA_cfg_1)
  {
    int status = XAxiDma_CfgInitialize(&axiDMA_1, axiDMA_cfg_1);
    if(status != XST_SUCCESS)
    {
      printf("Error initializing AxiDMA core 1\n");
    }
  }

  XAxiDma_IntrDisable(&axiDMA_0, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
  XAxiDma_IntrDisable(&axiDMA_0, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
  //XAxiDma_IntrDisable(&axiDMA_1, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
  XAxiDma_IntrDisable(&axiDMA_1, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
}

void init_axis_switch()
{
  printf("Initializing axis switches\n");
  axis_switch_cfg_0 = XAxisScr_LookupConfig(XPAR_AXIS_SWITCH_0_DEVICE_ID);
  if(axis_switch_cfg_0)
  {
    int status = XAxisScr_CfgInitialize(&axis_switch_0,axis_switch_cfg_0,axis_switch_cfg_0->BaseAddress);
    if(status != XST_SUCCESS)
    {
      printf("Error initializing axis switch 0\n");
    }
  }
  axis_switch_cfg_1 = XAxisScr_LookupConfig(XPAR_AXIS_SWITCH_1_DEVICE_ID);
  if(axis_switch_cfg_1)
  {
    int status = XAxisScr_CfgInitialize(&axis_switch_1,axis_switch_cfg_1,axis_switch_cfg_1->BaseAddress);
    if(status != XST_SUCCESS)
    {
      printf("Error initializing axis switch 1\n");
    }
  }
  axis_switch_cfg_2 = XAxisScr_LookupConfig(XPAR_AXIS_SWITCH_2_DEVICE_ID);
  if(axis_switch_cfg_2)
  {
    int status = XAxisScr_CfgInitialize(&axis_switch_2,axis_switch_cfg_2,axis_switch_cfg_2->BaseAddress);
    if(status != XST_SUCCESS)
    {
      printf("Error initializing axis switch 2\n");
    }
  }

  //axis_switch_cfg_3 = XAxisScr_LookupConfig(XPAR_AXIS_SWITCH_3_DEVICE_ID);
  //if(axis_switch_cfg_3)
  //{
  //  int status = XAxisScr_CfgInitialize(&axis_switch_3,axis_switch_cfg_3,axis_switch_cfg_3->BaseAddress);
  //  if(status != XST_SUCCESS)
  //  {
  //    printf("Error initializing axis switch 3\n");
  //  }
  //}
}

//void set_yolo_conv(u32 output_ch,u32 input_ch,u32 fold_output_ch,u32 fold_input_ch,
//		 	 	   u32 input_h,u32 input_w,u32 real_input_h,
//				   u32 fold_win_area)
//{
//    XYolo_conv_top_Set_output_ch_V(&yolo_conv_top, output_ch);
//    XYolo_conv_top_Set_input_ch_V(&yolo_conv_top, input_ch);
//    XYolo_conv_top_Set_fold_output_ch_V(&yolo_conv_top, fold_output_ch);
//    XYolo_conv_top_Set_fold_input_ch_V(&yolo_conv_top, fold_input_ch);
//    XYolo_conv_top_Set_input_h_V(&yolo_conv_top, input_h);
//    XYolo_conv_top_Set_input_w_V(&yolo_conv_top, input_w);
//    XYolo_conv_top_Set_real_input_h_V(&yolo_conv_top, real_input_h);
//    XYolo_conv_top_Set_fold_win_area_V(&yolo_conv_top, fold_win_area);
//}

void set_yolo_conv_dw(u32 output_ch, u32 input_ch, u32 fold_output_ch, u32 fold_input_ch,
    u32 input_h, u32 input_w, u32 real_input_h,
    u32 fold_win_area)
{
  XYolo_conv_dw_top_Set_output_ch_V(&yolo_conv_dw_top, output_ch);
  XYolo_conv_dw_top_Set_input_ch_V(&yolo_conv_dw_top, input_ch);
  XYolo_conv_dw_top_Set_fold_output_ch_V(&yolo_conv_dw_top, fold_output_ch);
  XYolo_conv_dw_top_Set_fold_input_ch_V(&yolo_conv_dw_top, fold_input_ch);
  XYolo_conv_dw_top_Set_input_h_V(&yolo_conv_dw_top, input_h);
  XYolo_conv_dw_top_Set_input_w_V(&yolo_conv_dw_top, input_w);
  XYolo_conv_dw_top_Set_real_input_h_V(&yolo_conv_dw_top, real_input_h);
  XYolo_conv_dw_top_Set_fold_win_area_V(&yolo_conv_dw_top, fold_win_area);
}

//void set_yolo_conv_pw(u32 output_ch, u32 input_ch, u32 fold_output_ch, u32 fold_input_ch,
//    u32 input_h, u32 input_w)
//{
//  XYolo_conv_pw_top_Set_output_ch_V(&yolo_conv_pw_top, output_ch);
//  XYolo_conv_pw_top_Set_input_ch_V(&yolo_conv_pw_top, input_ch);
//  XYolo_conv_pw_top_Set_fold_output_ch_V(&yolo_conv_pw_top, fold_output_ch);
//  XYolo_conv_pw_top_Set_fold_input_ch_V(&yolo_conv_pw_top, fold_input_ch);
//  XYolo_conv_pw_top_Set_input_h_V(&yolo_conv_pw_top, input_h);
//  XYolo_conv_pw_top_Set_input_w_V(&yolo_conv_pw_top, input_w);
//  //XYolo_conv_pw_top_Set_real_input_h_V(&yolo_conv_pw_top, real_input_h);
//  //XYolo_conv_pw_top_Set_fold_win_area_V(&yolo_conv_pw_top, fold_win_area);
//}
//
void set_yolo_acc(u32 input_h, u32 input_w, u32 fold_input_ch, u32 leaky, u32 bias_en)
{
  XYolo_acc_top_Set_input_h_V(&yolo_acc_top, input_h);
  XYolo_acc_top_Set_input_w_V(&yolo_acc_top, input_w);
  XYolo_acc_top_Set_fold_input_ch_V(&yolo_acc_top, fold_input_ch);
  XYolo_acc_top_Set_leaky_V(&yolo_acc_top, leaky);
  XYolo_acc_top_Set_bias_en_V(&yolo_acc_top, bias_en);
}

void set_yolo_max_pool(u32 output_h,u32 output_w,
    u32 input_h,u32 input_w,
    u32 input_fold_ch,
    u32 stride)
{
  XYolo_max_pool_top_Set_output_h_V(&yolo_max_pool_top,output_h);
  XYolo_max_pool_top_Set_output_w_V(&yolo_max_pool_top,output_w);
  XYolo_max_pool_top_Set_input_h_V(&yolo_max_pool_top,input_h);
  XYolo_max_pool_top_Set_input_w_V(&yolo_max_pool_top,input_w);
  XYolo_max_pool_top_Set_input_fold_ch_V(&yolo_max_pool_top,input_fold_ch);
  XYolo_max_pool_top_Set_stride_V(&yolo_max_pool_top,stride);
}

void set_yolo_yolo(u32 activate_en, u32 input_h, u32 input_w)
{
  XYolo_yolo_top_Set_activate_en_V(&yolo_yolo_top,activate_en);
  XYolo_yolo_top_Set_input_h_V(&yolo_yolo_top,input_h);
  XYolo_yolo_top_Set_input_w_V(&yolo_yolo_top,input_w);
}

void set_axis_switch(u8 switch_0_s, u8 switch_0_m,
                     u8 switch_1_s, u8 switch_1_m,
                     u8 switch_2_s, u8 switch_2_m)
{
  XAxisScr_RegUpdateDisable(&axis_switch_0);
  XAxisScr_RegUpdateDisable(&axis_switch_1);
  XAxisScr_RegUpdateDisable(&axis_switch_2);
  //XAxisScr_RegUpdateDisable(&axis_switch_3);

  XAxisScr_MiPortDisableAll(&axis_switch_0);
  XAxisScr_MiPortDisableAll(&axis_switch_1);
  XAxisScr_MiPortDisableAll(&axis_switch_2);
  //XAxisScr_MiPortDisableAll(&axis_switch_3);

  XAxisScr_MiPortEnable(&axis_switch_0, switch_0_m, switch_0_s);
  XAxisScr_MiPortEnable(&axis_switch_1, switch_1_m, switch_1_s);
  XAxisScr_MiPortEnable(&axis_switch_2, switch_2_m, switch_2_s);
  //XAxisScr_MiPortEnable(&axis_switch_3, switch_3_m, switch_3_s);

  XAxisScr_RegUpdateEnable(&axis_switch_0);
  XAxisScr_RegUpdateEnable(&axis_switch_1);
  XAxisScr_RegUpdateEnable(&axis_switch_2);
  //XAxisScr_RegUpdateEnable(&axis_switch_3);
}

void initPeripherals()
{
	//init_yolo_conv();
  init_yolo_conv_dw();
  //init_yolo_conv_pw();
  init_yolo_max_pool();
  init_yolo_yolo();
	init_yolo_acc();
  init_yolo_upsamp();
  init_axi_dma();
  init_axis_switch();
}

int main()
{
  initPeripherals();

  int acc_size = INPUT_W*INPUT_H*1*4;
  //int acc_size = OUTPUT_W*OUTPUT_H*((OUT_CHANNEL+3)/4)*4;
  //int acc_size = INPUT_W*INPUT_H*4*4;
  int output_size = OUTPUT_W * OUTPUT_H * 1 * 4;

  //short *input = tb_input;
  //short *input = layer_input;
  short *input = input_layer;
  //short *weight = tb_weight;
  //short *weight = group_0_0_weights;
  short *weight = group_0_weights;
  //short *bias = tb_bias;
  //short *bias = group_0_0_biases;
  short *bias = group_0_biases;
  short *output = (short *)calloc(output_size, sizeof(short));
  //short *output = (short *)calloc(OUTPUT_W*OUTPUT_H*4*4, sizeof(short));
  short *acc_input_buf = (short *)calloc(acc_size, sizeof(short));

  set_axis_switch(0, 0, 0, 0, 0, 0);
  //set_yolo_conv(OUT_CHANNEL, IN_CHANNEL, (OUT_CHANNEL+3)/4, (IN_CHANNEL+3)/4, INPUT_H+2, INPUT_W+2, INPUT_H+2, 3);
  //set_yolo_conv(16, 3, 4, 1, INPUT_W+2, INPUT_H+2, INPUT_H+2, 3);
  set_yolo_conv_dw(4, 4, 1, 1, INPUT_W+2, INPUT_H+2, INPUT_H+2, 3);
  //set_yolo_acc(INPUT_W, INPUT_H, (OUT_CHANNEL+3)/4, 1, 1);
  set_yolo_acc(INPUT_W, INPUT_H, 1, 1, 1);
  //set_yolo_max_pool(OUTPUT_H, OUTPUT_W, INPUT_H, INPUT_W, 4, 2);
  //XYolo_conv_top_Start(&yolo_conv_top);
  XYolo_conv_dw_top_Start(&yolo_conv_dw_top);
  XYolo_acc_top_Start(&yolo_acc_top);
  //XYolo_max_pool_top_Start(&yolo_max_pool_top);

  /* weight stream */
  printf("weight stream started\n");
  int size = 12*IN_CHANNEL*sizeof(short);
  //int size = 12*IN_CHANNEL*OUT_CHANNEL*sizeof(short);
  Xil_DCacheFlushRange((u32)(&weight[0]), size);
  XAxiDma_SimpleTransfer(&axiDMA_0, (u32)&(weight[0]), size, XAXIDMA_DMA_TO_DEVICE);
  while(XAxiDma_Busy(&axiDMA_0, XAXIDMA_DMA_TO_DEVICE));

  /* bias stream */
  printf("bias stream started\n");
  //Xil_DCacheFlushRange((u32)(&bias[0]), OUT_CHANNEL*sizeof(short));
  Xil_DCacheFlushRange((u32)(&bias[0]), 4*sizeof(short));
  //XAxiDma_SimpleTransfer(&axiDMA_1, (u32)&(bias[0]), OUT_CHANNEL*sizeof(short), XAXIDMA_DMA_TO_DEVICE);
  XAxiDma_SimpleTransfer(&axiDMA_1, (u32)&(bias[0]), 4*sizeof(short), XAXIDMA_DMA_TO_DEVICE);
  while(XAxiDma_Busy(&axiDMA_1, XAXIDMA_DMA_TO_DEVICE));

  Xil_DCacheFlushRange((u32)(&input[0]), INPUT_W*INPUT_H*1*4*sizeof(short));
  //Xil_DCacheFlushRange((u32)(&output[0]), INPUT_W*INPUT_H*1*4*sizeof(short));
  //Xil_DCacheFlushRange((u32)(&output[0]), OUTPUT_W*OUTPUT_H*((OUT_CHANNEL+3)/4)*4*sizeof(short));
  Xil_DCacheFlushRange((u32)(&output[0]), output_size*sizeof(short));

  int status;

  /* layer input stream */
  printf("input and acc stream started\n");
  Xil_DCacheFlushRange((u32)acc_input_buf, acc_size*sizeof(short));
  status = XAxiDma_SimpleTransfer(&axiDMA_1, (u32)acc_input_buf, acc_size*sizeof(short), XAXIDMA_DMA_TO_DEVICE);
  printf("Status: %d\n", status);
  status = XAxiDma_SimpleTransfer(&axiDMA_0, (u32)&input[0], INPUT_W*INPUT_H*1*4*sizeof(short), XAXIDMA_DMA_TO_DEVICE);
  printf("Status: %d\n", status);

  status = XAxiDma_SimpleTransfer(&axiDMA_0, (u32)(&output[0]), output_size*sizeof(short), XAXIDMA_DEVICE_TO_DMA);
  printf("Status: %d\n", status);
  //XAxiDma_SimpleTransfer(&axiDMA_0, (u32)(&output[0]), OUTPUT_W*OUTPUT_H*1*4*sizeof(short), XAXIDMA_DEVICE_TO_DMA);
  //XAxiDma_SimpleTransfer(&axiDMA_0, (u32)(&output[0]), OUTPUT_W*OUTPUT_H*((OUT_CHANNEL+3)/4)*4*sizeof(short), XAXIDMA_DEVICE_TO_DMA);
  //while(XAxiDma_Busy(&axiDMA_0, XAXIDMA_DMA_TO_DEVICE));
  //printf("input stream done\n");
  //printf("test1\n");
  //sleep(1);
  //printf("test2\n");
  //for(int i = 0; i < OUTPUT_W*OUTPUT_H*1*4; i+=10) {
  //  printf("%d: %d\n", i, output[i]);
  //}
  //printf("test3\n");
  while(XAxiDma_Busy(&axiDMA_0, XAXIDMA_DEVICE_TO_DMA));
  //Xil_DCacheInvalidateRange((u32)(&output[0]), OUTPUT_W*OUTPUT_H*1*4*sizeof(short));
  Xil_DCacheInvalidateRange((u32)(&output[0]), OUTPUT_W*OUTPUT_H*((OUT_CHANNEL+3)/4)*4*sizeof(short));
  printf("Complete\n");

	return 0;
}
