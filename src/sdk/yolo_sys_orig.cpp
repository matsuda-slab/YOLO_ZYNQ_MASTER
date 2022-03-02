/*
 * Empty C++ Application
 */

// Pcam
#include "platform/platform.h"
#include "ov5640/OV5640.h"
#include "ov5640/ScuGicInterruptController.h"
#include "ov5640/PS_GPIO.h"
#include "ov5640/AXI_VDMA.h"
#include "ov5640/PS_IIC.h"

#include "MIPI_D_PHY_RX.h"
#include "MIPI_CSI_2_RX.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include "xil_printf.h"
#include "xparameters.h"
#include "xgpiops.h"      // LED
#include <xtime_l.h>
#include "PSPL_if.h"      // SPI

//include IP head files
#include "xyolo_conv_top.h"
#include "xyolo_max_pool_top.h"
#include "xyolo_yolo_top.h"
#include "xyolo_acc_top.h"
#include "xyolo_upsamp_top.h"
#include "xaxidma.h"
#include "xaxis_switch.h"

#include "visualizer.h"
#include "img_converter_nnlbox.h"
#include <vector>


#define IRPT_CTL_DEVID 		XPAR_PS7_SCUGIC_0_DEVICE_ID
#define GPIO_DEVID			  XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_IRPT_ID			XPAR_PS7_GPIO_0_INTR
#define CAM_I2C_DEVID		  XPAR_PS7_I2C_0_DEVICE_ID
#define CAM_I2C_IRPT_ID		XPAR_PS7_I2C_0_INTR
#define VDMA_DEVID			  XPAR_AXIVDMA_0_DEVICE_ID
#define VDMA_MM2S_IRPT_ID	XPAR_FABRIC_AXI_VDMA_0_MM2S_INTROUT_INTR
#define VDMA_S2MM_IRPT_ID	XPAR_FABRIC_AXI_VDMA_0_S2MM_INTROUT_INTR
#define CAM_I2C_SCLK_RATE	100000

#define DDR_BASE_ADDR		  XPAR_DDR_MEM_BASEADDR
#define MEM_BASE_ADDR_W		(DDR_BASE_ADDR + 0x09000000)
#define MEM_BASE_ADDR_R		(DDR_BASE_ADDR + 0x0B000000)

#define GAMMA_BASE_ADDR   XPAR_AXI_GAMMACORRECTION_0_BASEADDR

#define RESOLUTION        Resolution::R1280_720_60_PP
#define CAM_MODE          OV5640_cfg::mode_t::MODE_720P_1280_720_60fps
#define FRAME_W           1280
#define FRAME_H           720
#define FRAME_SIZE        1280 * 720 * 3
//#define RESOLUTION        Resolution::R1920_1080_60_PP
//#define CAM_MODE          OV5640_cfg::mode_t::MODE_1080P_1920_1080_30fps
//#define FRAME_W           1920
//#define FRAME_H           1080
//#define FRAME_SIZE        FRAME_W * FRAME_H * 3

#define PSPL_BASEADDR XPAR_PSPL_IF_0_S00_AXI_BASEADDR
#define REG0 PSPL_IF_S00_AXI_SLV_REG0_OFFSET
#define REG1 PSPL_IF_S00_AXI_SLV_REG1_OFFSET
#define REG2 PSPL_IF_S00_AXI_SLV_REG2_OFFSET
#define REG3 PSPL_IF_S00_AXI_SLV_REG3_OFFSET

using namespace digilent;


//declare IP instances
XYolo_conv_top yolo_conv_top;
XYolo_conv_top_Config *yolo_conv_top_cfg;
XYolo_max_pool_top yolo_max_pool_top;
XYolo_max_pool_top_Config *yolo_max_pool_top_cfg;
XYolo_yolo_top yolo_yolo_top;
XYolo_yolo_top_Config *yolo_yolo_top_cfg;
XYolo_acc_top yolo_acc_top;
XYolo_acc_top_Config *yolo_acc_top_cfg;
XYolo_upsamp_top yolo_upsamp_top;
XYolo_upsamp_top_Config *yolo_upsamp_top_cfg;
XAxiDma axiDMA_0, axiDMA_1;
XAxiDma_Config *axiDMA_cfg_0, *axiDMA_cfg_1;
XAxis_Switch axis_switch_0, axis_switch_1, axis_switch_2;
XAxis_Switch_Config *axis_switch_cfg_0, *axis_switch_cfg_1, *axis_switch_cfg_2;


#include "custom_headers/car/group_0_weight.h"
#include "custom_headers/car/group_1_weight.h"
#include "custom_headers/car/group_2_weight.h"
#include "custom_headers/car/group_3_weight_it.h"
#include "custom_headers/car/group_4_weight_it.h"
#include "custom_headers/car/group_6_weight_it.h"
#include "custom_headers/car/group_7_weight_it.h"
#include "custom_headers/car/group_8_weight_it.h"
#include "custom_headers/car/group_9_weight_it.h"
#include "custom_headers/car/group_10_weight_it.h"
#include "custom_headers/car/group_11_weight_it.h"
#include "custom_headers/car/group_12_weight_it.h"
#include "custom_headers/car/group_13_weight_it.h"

#include "origin_headers/group_0_input.h"
//#include "group_13_output.h"

void init_yolo_conv()
{
  printf("Initializing yolo_conv_top\n");
  yolo_conv_top_cfg = XYolo_conv_top_LookupConfig(XPAR_YOLO_CONV_TOP_0_DEVICE_ID);
  if(yolo_conv_top_cfg)
  {
    int status = XYolo_conv_top_CfgInitialize(&yolo_conv_top,yolo_conv_top_cfg);
    if(status != XST_SUCCESS)
    {
      printf("Error initializing yolo_conv_top core\n");
    }
  }
}

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
    int status = XYolo_acc_top_CfgInitialize(&yolo_acc_top,yolo_acc_top_cfg);
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
    int status = XAxiDma_CfgInitialize(&axiDMA_0,axiDMA_cfg_0);
    if(status != XST_SUCCESS)
    {
      printf("Error initializing AxiDMA core 0\n");
    }
  }
  axiDMA_cfg_1 = XAxiDma_LookupConfig(XPAR_AXIDMA_1_DEVICE_ID);
  if(axiDMA_cfg_1)
  {
    int status = XAxiDma_CfgInitialize(&axiDMA_1,axiDMA_cfg_1);
    if(status != XST_SUCCESS)
    {
      printf("Error initializing AxiDMA core 1\n");
    }
  }

  XAxiDma_IntrDisable(&axiDMA_0,XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);
  XAxiDma_IntrDisable(&axiDMA_0,XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
  XAxiDma_IntrDisable(&axiDMA_1,XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);
  XAxiDma_IntrDisable(&axiDMA_1,XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
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
}

void initPeripherals()
{
  init_yolo_conv();
  init_yolo_max_pool();
  init_yolo_yolo();
  init_yolo_acc();
  init_yolo_upsamp();
  init_axi_dma();
  init_axis_switch();
}

void set_yolo_conv(u32 output_ch, u32 input_ch, u32 fold_output_ch, u32 fold_input_ch,
    u32 input_h, u32 input_w, u32 real_input_h,
    u32 fold_win_area)
{
  XYolo_conv_top_Set_output_ch_V(&yolo_conv_top, output_ch);
  XYolo_conv_top_Set_input_ch_V(&yolo_conv_top, input_ch);
  XYolo_conv_top_Set_fold_output_ch_V(&yolo_conv_top, fold_output_ch);
  XYolo_conv_top_Set_fold_input_ch_V(&yolo_conv_top, fold_input_ch);
  XYolo_conv_top_Set_input_h_V(&yolo_conv_top, input_h);
  XYolo_conv_top_Set_input_w_V(&yolo_conv_top, input_w);
  XYolo_conv_top_Set_real_input_h_V(&yolo_conv_top, real_input_h);
  XYolo_conv_top_Set_fold_win_area_V(&yolo_conv_top, fold_win_area);
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

void set_yolo_yolo(u32 activate_en,u32 input_h,u32 input_w)
{
  XYolo_yolo_top_Set_activate_en_V(&yolo_yolo_top,activate_en);
  XYolo_yolo_top_Set_input_h_V(&yolo_yolo_top,input_h);
  XYolo_yolo_top_Set_input_w_V(&yolo_yolo_top,input_w);
}

void set_yolo_acc(u32 input_h,u32 input_w,u32 fold_input_ch,u32 leaky,u32 bias_en)
{
  XYolo_acc_top_Set_input_h_V(&yolo_acc_top,input_h);
  XYolo_acc_top_Set_input_w_V(&yolo_acc_top,input_w);
  XYolo_acc_top_Set_fold_input_ch_V(&yolo_acc_top,fold_input_ch);
  XYolo_acc_top_Set_leaky_V(&yolo_acc_top,leaky);
  XYolo_acc_top_Set_bias_en_V(&yolo_acc_top,bias_en);
}

void set_axis_switch(u8 switch_0_s,u8 switch_0_m,
    u8 switch_1_s,u8 switch_1_m,
    u8 switch_2_s,u8 switch_2_m)
{
  XAxisScr_RegUpdateDisable(&axis_switch_0);
  XAxisScr_RegUpdateDisable(&axis_switch_1);
  XAxisScr_RegUpdateDisable(&axis_switch_2);

  XAxisScr_MiPortDisableAll(&axis_switch_0);
  XAxisScr_MiPortDisableAll(&axis_switch_1);
  XAxisScr_MiPortDisableAll(&axis_switch_2);

  XAxisScr_MiPortEnable(&axis_switch_0,switch_0_m,switch_0_s);
  XAxisScr_MiPortEnable(&axis_switch_1,switch_1_m,switch_1_s);
  XAxisScr_MiPortEnable(&axis_switch_2,switch_2_m,switch_2_s);

  XAxisScr_RegUpdateEnable(&axis_switch_0);
  XAxisScr_RegUpdateEnable(&axis_switch_1);
  XAxisScr_RegUpdateEnable(&axis_switch_2);
}

void pipeline_mode_change(AXI_VDMA<ScuGicInterruptController>& vdma_driver, OV5640& cam, VideoOutput& vid, Resolution res, OV5640_cfg::mode_t mode)
{
  //Bring up input pipeline back-to-front
  {
    vdma_driver.resetWrite();
    MIPI_CSI_2_RX_mWriteReg(XPAR_MIPI_CSI_2_RX_0_S_AXI_LITE_BASEADDR, CR_OFFSET, (CR_RESET_MASK & ~CR_ENABLE_MASK));
    MIPI_D_PHY_RX_mWriteReg(XPAR_MIPI_D_PHY_RX_0_S_AXI_LITE_BASEADDR, CR_OFFSET, (CR_RESET_MASK & ~CR_ENABLE_MASK));
    cam.reset();
  }

  {
    vdma_driver.configureWrite(timing[static_cast<int>(res)].h_active, timing[static_cast<int>(res)].v_active);
    Xil_Out32(GAMMA_BASE_ADDR, 3); // Set Gamma correction factor to 1/1.8
    //TODO CSI-2, D-PHY config here
    cam.init();
  }

  {
    vdma_driver.enableWrite();
    MIPI_CSI_2_RX_mWriteReg(XPAR_MIPI_CSI_2_RX_0_S_AXI_LITE_BASEADDR, CR_OFFSET, CR_ENABLE_MASK);
    MIPI_D_PHY_RX_mWriteReg(XPAR_MIPI_D_PHY_RX_0_S_AXI_LITE_BASEADDR, CR_OFFSET, CR_ENABLE_MASK);
    cam.set_mode(mode);
    cam.set_awb(OV5640_cfg::awb_t::AWB_ADVANCED);
  }

  //Bring up output pipeline back-to-front
  {
    vid.reset();
    vdma_driver.resetRead();
  }

  {
    vid.configure(res);
    vdma_driver.configureRead(timing[static_cast<int>(res)].h_active, timing[static_cast<int>(res)].v_active);
  }

  {
    vid.enable();
    vdma_driver.enableRead();
  }
}


typedef enum{
  LINEAR, LEAKY
} ACTIVATION;

typedef enum{
  NONE, MAX_POOL, YOLO, UPSAMPLE
} POST_PROCESS;

#define CH_FOLD_FACTOR 4
#define MAX_KERNEL_NUM 32

u32 activate_en[8]={0xfffffff3,0xffffffff,0xfe7fffff,0xffffffff,0xffffffff,0xffffcfff,0xffffffff,0x7fffffff};

typedef struct layer_group
{
  int input_width;
  int input_height;
  int input_ch;
  int input_fold_ch;
  int input_size;
  int input_fold_factor;

  int acc_size;

  int output_width;
  int output_height;
  int output_ch;
  int output_fold_ch;
  int output_size;
  int output_fold_factor;
  int pooling_stride;

  short *inputs;
  short *outputs;
  short *weights;
  short *biases;

  ACTIVATION activate_type;
  POST_PROCESS post_process_type;
  int conv_disable;

} layer_group;


layer_group make_layer_group(int input_w, int input_h, int input_ch, int input_fold_factor,
    int output_w, int output_h, int output_ch, int output_fold_factor,
    bool conv_disable, ACTIVATION activate_type, POST_PROCESS post_process_type, int pooling_stride)
{
  layer_group inst;

  inst.input_width = input_w;
  inst.input_height = input_h;
  inst.input_ch = input_ch;
  inst.input_fold_ch = (input_ch+3)/4;
  inst.input_size = input_w*input_h*inst.input_fold_ch*CH_FOLD_FACTOR;
  inst.input_fold_factor = input_fold_factor;


  inst.output_width = output_w;
  inst.output_height = output_h;
  inst.output_ch = output_ch;
  inst.output_fold_ch = (output_ch+3)/4;
  inst.output_size = output_w*output_h*inst.output_fold_ch*CH_FOLD_FACTOR;
  inst.output_fold_factor = output_fold_factor;


  inst.acc_size = input_w*input_h*inst.output_fold_ch*CH_FOLD_FACTOR;

  inst.conv_disable = conv_disable;
  inst.activate_type = activate_type;
  inst.post_process_type = post_process_type ;

  inst.pooling_stride = pooling_stride;

  //printf("%d,%d,%d,%d,%d,%d\n",inst.input_width,inst.input_height,inst.input_ch,inst.input_fold_ch,inst.input_size,inst.input_fold_factor);
  //printf("%d,%d,%d,%d,%d,%d\n",inst.output_width,inst.output_height,inst.output_ch,inst.output_fold_ch,inst.output_size,inst.output_fold_factor);
  //printf("%d,%d,%d,%d,%d\n",inst.acc_size,inst.conv_disable,inst.activate_type,inst.post_process_type,inst.pooling_stride);
  return inst;
}


XTime tEnd, tStart;
int time_used_hw;
void forward_layer_group(layer_group l)
{
  short * mid_ptr;

  for(int i = 0; i < l.output_fold_factor; i++)
  {
    short * acc_output_buff = (short *)calloc(l.acc_size,sizeof(short));
    short * acc_input_buff = (short *)calloc(l.acc_size,sizeof(short));
    for(int j = 0; j < l.input_fold_factor; j++)
    {

      if(j==l.input_fold_factor-1)
      {
        //set IP parameters
        if(!l.conv_disable)
        {
          set_yolo_conv(l.output_ch, l.input_ch, l.output_fold_ch, l.input_fold_ch, l.input_height+2, l.input_width+2, l.input_height+2, 3);
          set_yolo_acc(l.input_height, l.input_width, l.output_fold_ch, l.activate_type, 1);
        }
        if(l.post_process_type==MAX_POOL)
        {
          if(l.pooling_stride==2)
          {
            set_yolo_max_pool(l.output_height, l.output_width, l.input_height, l.input_width, l.output_fold_ch, l.pooling_stride);
          }
          else
          {
            set_yolo_max_pool(l.output_height+1, l.output_width+1, l.input_height, l.input_width, l.output_fold_ch, l.pooling_stride);
          }
        }
        if(l.post_process_type==YOLO)
        {
          set_yolo_yolo(activate_en[i], l.input_height, l.input_width);
        }

        set_axis_switch(0, l.conv_disable, l.conv_disable, l.post_process_type, l.post_process_type, 0);

        //start IP
        if(!l.conv_disable)
        {
          XYolo_conv_top_Start(&yolo_conv_top);
          XYolo_acc_top_Start(&yolo_acc_top);
        }
        if(l.post_process_type == MAX_POOL)
        {
          XYolo_max_pool_top_Start(&yolo_max_pool_top);
        }
        if(l.post_process_type == YOLO)
        {
          XYolo_yolo_top_Start(&yolo_yolo_top);
        }
        if(l.post_process_type == UPSAMPLE)
        {
          XYolo_upsamp_top_Start(&yolo_upsamp_top);
        }
      }
      else
      {
        set_yolo_conv(l.output_ch, l.input_ch, l.output_fold_ch, l.input_fold_ch, l.input_height+2, l.input_width+2, l.input_height+2, 3);
        set_yolo_acc(l.input_height, l.input_width, l.output_fold_ch, 0, 0);
        set_axis_switch(0,0,0,0,0,0);
        XYolo_conv_top_Start(&yolo_conv_top);
        XYolo_acc_top_Start(&yolo_acc_top);
      }

      //weight_stream
      if(!l.conv_disable)
      {
        Xil_DCacheFlushRange((u32)(&l.weights[12*l.input_ch*l.output_ch*l.output_fold_factor*j+12*l.input_ch*l.output_ch*i]), 12*l.input_ch*l.output_ch*sizeof(short));
        XAxiDma_SimpleTransfer(&axiDMA_0, (u32)&(l.weights[12*l.input_ch*l.output_ch*l.output_fold_factor*j+12*l.input_ch*l.output_ch*i]), 12*l.input_ch*l.output_ch*sizeof(short), XAXIDMA_DMA_TO_DEVICE);
        while(XAxiDma_Busy(&axiDMA_0, XAXIDMA_DMA_TO_DEVICE));
      }

      if(j==l.input_fold_factor-1)
      {
        if(!l.conv_disable)
        {
          Xil_DCacheFlushRange((u32)(&l.biases[l.output_ch*i]), l.output_ch*sizeof(short));
          XAxiDma_SimpleTransfer(&axiDMA_1, (u32)&(l.biases[l.output_ch*i]), l.output_ch*sizeof(short), XAXIDMA_DMA_TO_DEVICE);
          while(XAxiDma_Busy(&axiDMA_1, XAXIDMA_DMA_TO_DEVICE));
          Xil_DCacheFlushRange((u32)(&l.inputs[l.input_size*j]), l.input_size*sizeof(short));
        }
        else
        {
          Xil_DCacheFlushRange((u32)(&l.inputs[l.input_size*i]), l.input_size*sizeof(short));
        }

        Xil_DCacheFlushRange((u32)(&l.outputs[l.output_size*i]), l.output_size*sizeof(short));
        if(!l.conv_disable)
        {
          Xil_DCacheFlushRange((u32)acc_input_buff,l.acc_size*sizeof(short));
          XAxiDma_SimpleTransfer(&axiDMA_1, (u32)acc_input_buff, l.acc_size*sizeof(short), XAXIDMA_DMA_TO_DEVICE);
          XAxiDma_SimpleTransfer(&axiDMA_0, (u32)&l.inputs[l.input_size*j], l.input_size*sizeof(short), XAXIDMA_DMA_TO_DEVICE);
        }
        else
        {
          XAxiDma_SimpleTransfer(&axiDMA_0,(u32)&l.inputs[l.input_size*i],l.input_size*sizeof(short),XAXIDMA_DMA_TO_DEVICE);
        }

        XAxiDma_SimpleTransfer(&axiDMA_0, (u32)(&l.outputs[l.output_size*i]), l.output_size*sizeof(short), XAXIDMA_DEVICE_TO_DMA);
        while(XAxiDma_Busy(&axiDMA_0, XAXIDMA_DEVICE_TO_DMA));
        Xil_DCacheInvalidateRange((u32)(&l.outputs[l.output_size*i]), l.output_size*sizeof(short));

        if(l.post_process_type == NONE)
        {
          while(!XYolo_acc_top_IsDone(&yolo_acc_top));
        }
        if(l.post_process_type == MAX_POOL)
        {
          while(!XYolo_max_pool_top_IsDone(&yolo_max_pool_top));
        }
        if(l.post_process_type == YOLO)
        {
          while(!XYolo_yolo_top_IsDone(&yolo_yolo_top));
        }
        if(l.post_process_type == UPSAMPLE)
        {
          while(!XYolo_upsamp_top_IsDone(&yolo_upsamp_top));
        }

      }

      else
      {
        Xil_DCacheFlushRange((u32)(&l.inputs[l.input_size*j]), l.input_size*sizeof(short));
        Xil_DCacheFlushRange((u32)acc_output_buff, l.acc_size*sizeof(short));
        Xil_DCacheFlushRange((u32)acc_input_buff, l.acc_size*sizeof(short));

        XAxiDma_SimpleTransfer(&axiDMA_0, (u32)&l.inputs[l.input_size*j], l.input_size*sizeof(short), XAXIDMA_DMA_TO_DEVICE);
        XAxiDma_SimpleTransfer(&axiDMA_1, (u32)acc_input_buff, l.acc_size*sizeof(short), XAXIDMA_DMA_TO_DEVICE);
        XAxiDma_SimpleTransfer(&axiDMA_0, (u32)acc_output_buff, l.acc_size*sizeof(short), XAXIDMA_DEVICE_TO_DMA);

        while(XAxiDma_Busy(&axiDMA_0, XAXIDMA_DEVICE_TO_DMA));

        Xil_DCacheInvalidateRange((u32)acc_output_buff, l.acc_size*sizeof(short));

        while(!XYolo_acc_top_IsDone(&yolo_acc_top));
      }
      mid_ptr = acc_input_buff;

      acc_input_buff = acc_output_buff;
      acc_output_buff = mid_ptr;
      //memcpy(acc_input_buff,acc_output_buff,l.acc_size*sizeof(short));
    }
    free(acc_input_buff);
    free(acc_output_buff);

  }


}

void set_PSPL_reg(u8 byte1, u8 byte2, u8 byte3, u8 byte4) {
    u32 reg_val;
    u32 val_tmp;

    // reg[7:0]
    reg_val = byte1;

    // reg[15:8]
    val_tmp = byte2 << 8;
    reg_val = reg_val | val_tmp;

    // reg[23:16]
    val_tmp = byte3 << 16;
    reg_val = reg_val | val_tmp;

    // reg[31:24]
    val_tmp = byte4 << 24;
    reg_val = reg_val | val_tmp;
    
    PSPL_IF_mWriteReg(PSPL_BASEADDR, REG0, reg_val);
}


int main()
{

  //initialise IPs
  initPeripherals();

  // Pcam, VDMA init
  ScuGicInterruptController irpt_ctl(IRPT_CTL_DEVID);
  PS_GPIO<ScuGicInterruptController> gpio_driver(GPIO_DEVID, irpt_ctl, GPIO_IRPT_ID);
  PS_IIC<ScuGicInterruptController> iic_driver(CAM_I2C_DEVID, irpt_ctl, CAM_I2C_IRPT_ID, 100000);

  OV5640 cam(iic_driver, gpio_driver);
  AXI_VDMA<ScuGicInterruptController> vdma_driver(VDMA_DEVID, MEM_BASE_ADDR_W, MEM_BASE_ADDR_R, irpt_ctl,
      VDMA_MM2S_IRPT_ID,
      VDMA_S2MM_IRPT_ID);
  VideoOutput vid(XPAR_VTC_0_DEVICE_ID, XPAR_VIDEO_DYNCLK_DEVICE_ID);


  /* Setting GPIO for LED */
  XGpioPs instXGpioPs;
  XGpioPs_Config *configXGpioPs;

  configXGpioPs = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
  XGpioPs_CfgInitialize(&instXGpioPs, configXGpioPs, configXGpioPs->BaseAddr);

  XGpioPs_SetDirectionPin(&instXGpioPs, 7, 1);
  XGpioPs_SetOutputEnablePin(&instXGpioPs, 7, 1);

  pipeline_mode_change(vdma_driver, cam, vid, RESOLUTION, CAM_MODE);

  xil_printf("Video init done.\r\n");

  /* Get image from VDMA buffer */
  bgr_t* img = (bgr_t*)calloc(FRAME_SIZE/3, sizeof(bgr_t));
  int* buf_addr_w = (int*)vdma_driver.get_buffer_addr_w();
  int* buf_addr_r = (int*)vdma_driver.get_buffer_addr_r();
  xil_printf("buffer_address (WRITE): %x\r\n", buf_addr_w);
  xil_printf("buffer_address (READ): %x\r\n", buf_addr_r);

  bgr_t* target_img = (bgr_t*)calloc(416*416, sizeof(bgr_t));

  layer_group group_0 = make_layer_group(416,416,3,1,           // in_c  = 3
      208,208,16,1,          // out_c = 16
      0,LEAKY,MAX_POOL,2);
  layer_group group_1 = make_layer_group(208,208,16,1,          // in_c  = 16
      104,104,32,1,          // out_c = 32
      0,LEAKY,MAX_POOL,2);
  layer_group group_2 = make_layer_group(104,104,32,1,          // in_c  = 32
      52,52,32,2,            // out_c = 64
      0,LEAKY,MAX_POOL,2);
  layer_group group_3 = make_layer_group(52,52,32,2,            // in_c  = 64
      26,26,32,4,            // out_c = 128
      0,LEAKY,MAX_POOL,2);
  layer_group group_4 = make_layer_group(26,26,32,4,            // in_c  = 128
      26,26,32,8,            // out_c = 256
      0,LEAKY,NONE,2);
  layer_group group_5 = make_layer_group(26,26,32,1,            // in_c  = 256
      13,13,32,8,            // out_c = 256
      1,LINEAR,MAX_POOL,2);
  layer_group group_6 = make_layer_group(13,13,32,8,            // in_c  = 256
      13,13,32,16,           // out_c = 512
      0,LEAKY,MAX_POOL,1);
  layer_group group_7 = make_layer_group(13,13,32,16,           // in_c  = 512
      13,13,32,32,           // out_c = 1024
      0,LEAKY,NONE,2);
  layer_group group_8 = make_layer_group(13,13,32,32,           // in_c  = 1024
      13,13,32,8,            // out_c = 256
      0,LEAKY,NONE,2);
  layer_group group_9 = make_layer_group(13,13,32,8,            // in_c  = 256
      13,13,32,16,           // out_c = 512
      0,LEAKY,NONE,2);
  layer_group group_10 = make_layer_group(13,13,32,16,          // in_c  = 512
      13,13,32,8,            // out_c = 255
      0,LINEAR,YOLO,2);
  layer_group group_11 = make_layer_group(13,13,32,8,           // in_c  = 256
      26,26,32,4,            // out_c = 128
      0,LEAKY,UPSAMPLE,2);
  layer_group group_12 = make_layer_group(26,26,32,12,          // in_c  = 384
      26,26,32,8,            // out_c = 256
      0,LEAKY,NONE,2);
  layer_group group_13 = make_layer_group(26,26,32,8,           // in_c  = 256
      26,26,32,8,            // out_c = 255
      0,LINEAR,YOLO,2);

  short* resized_img = (short*)calloc(416 * 416 * 4, sizeof(short));
  short* input_img   = (short*)calloc(416 * 416 * 4, sizeof(short));
  short* layer_output_hls;
  group_0.inputs = resized_img;
  //group_0.inputs = input_img;

  group_0.weights = group_0_weights;
  group_0.biases = group_0_biases;
  group_1.weights = group_1_weights;
  group_1.biases = group_1_biases;
  group_2.weights = group_2_weights;
  group_2.biases = group_2_biases;
  group_3.weights = group_3_weights;
  group_3.biases = group_3_biases;
  group_4.weights = group_4_weights;
  group_4.biases = group_4_biases;
  group_6.weights = group_6_weights;
  group_6.biases = group_6_biases;
  group_7.weights = group_7_weights;
  group_7.biases = group_7_biases;
  group_8.weights = group_8_weights;
  group_8.biases = group_8_biases;
  group_9.weights = group_9_weights;
  group_9.biases = group_9_biases;
  group_10.weights = group_10_weights;
  group_10.biases = group_10_biases;
  group_11.weights = group_11_weights;
  group_11.biases = group_11_biases;
  group_12.weights = group_12_weights;
  group_12.biases = group_12_biases;
  group_13.weights = group_13_weights;
  group_13.biases = group_13_biases;

  group_0.outputs = (short *)calloc(group_0.output_size*group_0.output_fold_factor,sizeof(short));
  group_1.outputs = (short *)calloc(group_1.output_size*group_1.output_fold_factor,sizeof(short));
  group_2.outputs = (short *)calloc(group_2.output_size*group_2.output_fold_factor,sizeof(short));
  group_3.outputs = (short *)calloc(group_3.output_size*group_3.output_fold_factor,sizeof(short));
  group_4.outputs = (short *)calloc(group_4.output_size*group_4.output_fold_factor,sizeof(short));
  group_5.outputs = (short *)calloc(group_5.output_size*group_5.output_fold_factor,sizeof(short));
  group_6.outputs = (short *)calloc(group_6.output_size*group_6.output_fold_factor,sizeof(short));
  group_7.outputs = (short *)calloc(group_7.output_size*group_7.output_fold_factor,sizeof(short));
  group_8.outputs = (short *)calloc(group_8.output_size*group_8.output_fold_factor,sizeof(short));
  group_9.outputs = (short *)calloc(group_9.output_size*group_9.output_fold_factor,sizeof(short));
  group_10.outputs = (short *)calloc(group_10.output_size*group_10.output_fold_factor,sizeof(short));   // yolo_output_1
  group_11.outputs = (short *)calloc(group_11.output_size*group_11.output_fold_factor,sizeof(short));
  group_12.inputs = (short *)calloc(group_12.input_size*group_12.input_fold_factor,sizeof(short));
  group_12.outputs = (short *)calloc(group_12.output_size*group_12.output_fold_factor,sizeof(short));
  group_13.outputs = (short *)calloc(group_13.output_size*group_13.output_fold_factor,sizeof(short));

  while (1) {
    /* Get Pcam image from VDMA write buffer */
    XTime_GetTime(&tStart);
    memcpy(img, buf_addr_w, FRAME_SIZE);
    img_convert(resized_img, img, FRAME_W, FRAME_H);
    for(int i = 0; i < 416 * 416 * 4; i = i + 4) {
      target_img[i/4].r = resized_img[i];        // R
      target_img[i/4].g = resized_img[i + 1];    // G
      target_img[i/4].b = resized_img[i + 2];    // B
    }
    //for(int i = 0; i < 416 * 416 * 4; i = i + 4) {
    //  input_img[i]     = resized_img[i + 2];    // R
    //  input_img[i + 1] = resized_img[i];        // G
    //  input_img[i + 2] = resized_img[i + 1];    // B
    //}
    XTime_GetTime(&tEnd);
    time_used_hw = 1.0*((tEnd-tStart)*1000000)/(COUNTS_PER_SECOND);
    xil_printf("preprocess : %d(us)***\n\r", time_used_hw);

    XTime_GetTime(&tStart);

    forward_layer_group(group_0);
    group_1.inputs = group_0.outputs;
    forward_layer_group(group_1);
    group_2.inputs = group_1.outputs;
    forward_layer_group(group_2);
    group_3.inputs = group_2.outputs;
    forward_layer_group(group_3);
    group_4.inputs = group_3.outputs;
    forward_layer_group(group_4);
    group_5.inputs = group_4.outputs;
    forward_layer_group(group_5);

    group_6.inputs = group_5.outputs;
    forward_layer_group(group_6);
    group_7.inputs = group_6.outputs;
    forward_layer_group(group_7);
    group_8.inputs = group_7.outputs;
    forward_layer_group(group_8);
    group_9.inputs = group_8.outputs;
    forward_layer_group(group_9);

    group_10.inputs = group_9.outputs;
    forward_layer_group(group_10);

    group_11.inputs = group_8.outputs;
    forward_layer_group(group_11);

    // concat
    memcpy(group_12.inputs, group_11.outputs, group_11.output_size*group_11.output_fold_factor*sizeof(short));
    memcpy(&group_12.inputs[group_11.output_size*group_11.output_fold_factor], group_4.outputs, group_4.output_size*group_4.output_fold_factor*sizeof(short));

    forward_layer_group(group_12);
    group_13.inputs = group_12.outputs;
    forward_layer_group(group_13);
    XTime_GetTime(&tEnd);


    layer_output_hls = group_13.outputs;

    printf("Calculation complete\n");

    time_used_hw = 1.0*((tEnd-tStart)*1000000)/(COUNTS_PER_SECOND);
    xil_printf("***%d(us)***\n\r", time_used_hw);
    ////int count =0;

    /* Convert YOLO result to coordinates */
    XTime_GetTime(&tStart);
    std::vector<point_t> point_list = postprocess(layer_input, group_10.outputs, group_13.outputs);
    XTime_GetTime(&tEnd);
    time_used_hw = 1.0*((tEnd-tStart)*1000000)/(COUNTS_PER_SECOND);
    xil_printf("postprocess : %d(us)***\n\r", time_used_hw);

    /* Draw rectangle using coordinates */
    //XGpioPs_WritePin(&instXGpioPs, 7, 0);

    /* Output num of detection to SPI (reg of PSPL) */
    XTime_GetTime(&tStart);
    set_PSPL_reg((u8)point_list.size(), 5, 8, 13);
    XTime_GetTime(&tEnd);
    time_used_hw = 1.0*((tEnd-tStart)*1000000)/(COUNTS_PER_SECOND);
    xil_printf("send SPI reg : %d(us)***\n\r", time_used_hw);

    XTime_GetTime(&tStart);
    for(int idx = 0; idx < point_list.size(); idx++) {
      /* Write Low to MIO7 */
      //XGpioPs_WritePin(&instXGpioPs, 7, 1);
      //xil_printf("pt : (%d, %d), (%d, %d)\r\n", point_list[idx].x1, point_list[idx].y1, point_list[idx].x2, point_list[idx].y2);
      for(int i = point_list[idx].y1; i < point_list[idx].y2; i++) {
        for(int j = point_list[idx].x1; j < point_list[idx].x2; j++) {
          if ((((point_list[idx].x1 < j && j < point_list[idx].x1+5) || (point_list[idx].x2-5 < j && j < point_list[idx].x2))) 
              || ((point_list[idx].y1 < i && i < point_list[idx].y1+5) || (point_list[idx].y2-5 < i && i < point_list[idx].y2))) {
            target_img[i*416+j].r = 0;
            target_img[i*416+j].g = 255;
            target_img[i*416+j].b = 0;
          }
        }
      }
    }

    /* Transfer the drawn image to VDMA read buffer */
    for(int i = 0; i < 416; i++) {
      for(int j = 0; j < 416; j++) {
        img[(300+i)*FRAME_W + (400+j)].g = target_img[i*416+j].r;
        img[(300+i)*FRAME_W + (400+j)].b = target_img[i*416+j].g;
        img[(300+i)*FRAME_W + (400+j)].r = target_img[i*416+j].b;
      }
    }
    memcpy(buf_addr_r, img, FRAME_SIZE);
    memcpy(buf_addr_r + FRAME_SIZE, img, FRAME_SIZE);
    memcpy(buf_addr_r + FRAME_SIZE*2, img, FRAME_SIZE);
    XTime_GetTime(&tEnd);
    time_used_hw = 1.0*((tEnd-tStart)*1000000)/(COUNTS_PER_SECOND);
    xil_printf("drawing : %d(us)***\n\n\r", time_used_hw);
  }
  //
  //for(int idx=0;idx<group_13.output_size*group_13.output_fold_factor;idx++)
  //{
  //  short ref = layer_output_ref[idx];
  //  short get = layer_output_hls[idx];

  //  if(abs(ref-get)<64)
  //  {
  //    continue;
  //  }
  //  else
  //  {count++;

  //    //printf("Recv[%d]=%hd,Ref=%hd\n",idx,get,ref);
  //  }
  //  //printf("%hd,\n",get);

  //}
  //free(layer_output_hls);
  //printf("End,%d/%d\n",count,group_13.output_size*group_13.output_fold_factor);
  return 0;
}
