#include <stdio.h>
#include <math.h>
#include <iostream>
#include "../src/yolo_conv_dw.h"
#include "include/tb_sample.h"

#define KERNEL_DIM 3
#define PAD 1
#define INPUT_CHANNEL 3
#define INPUT_WIDTH (16+2*PAD)
#define INPUT_HEIGHT (16+2*PAD)
#define REAL_INPUT_HEIGHT (16+2*PAD)
#define OUTPUT_CHANNEL INPUT_CHANNEL
#define OUTPUT_WIDTH 16
#define OUTPUT_HEIGHT 16

int main()
{
	yolo_quad_stream inputStream("in_stream"), outputStream("out_stream");

	bool flag = false;

	//static fp_data_type output_data[OUTPUT_WIDTH*OUTPUT_HEIGHT*OUTPUT_CHANNEL];
	static fp_data_type output_data[OUTPUT_WIDTH*OUTPUT_HEIGHT*4];
  
	int k = 0;

  /* Weight stream */
  for(int i = 0; i < INPUT_CHANNEL; i++)
  {
    for(int j = 0; j < (KERNEL_DIM*KERNEL_DIM+3)/4; j++)
      // for(int j = 0; j < 3; j++) と同じ
    {
      quad_fp_side_channel curr_input;
      short input_data_sub0;
      short input_data_sub1;
      short input_data_sub2;
      short input_data_sub3;

      input_data_sub0 = (short)(tb_weight[k++] * 256);

      if (j == (KERNEL_DIM*KERNEL_DIM+3)/4-1) {
        input_data_sub1 = 0;
        input_data_sub2 = 0;
        input_data_sub3 = 0;
      }
      else {
        input_data_sub1 = (short)(tb_weight[k++] * 256);
        input_data_sub2 = (short)(tb_weight[k++] * 256);
        input_data_sub3 = (short)(tb_weight[k++] * 256);
      }

      fp_data_type *sub0_p = (fp_data_type *)&input_data_sub0;
      fp_data_type *sub1_p = (fp_data_type *)&input_data_sub1;
      fp_data_type *sub2_p = (fp_data_type *)&input_data_sub2;
      fp_data_type *sub3_p = (fp_data_type *)&input_data_sub3;

      curr_input.data.sub_data_0 = *sub0_p;
      curr_input.data.sub_data_1 = *sub1_p;
      curr_input.data.sub_data_2 = *sub2_p;
      curr_input.data.sub_data_3 = *sub3_p;

      curr_input.keep = 1;
      curr_input.strb = 1;
      curr_input.user = 1;
      curr_input.id   = 0;
      curr_input.dest = 0;

      inputStream << curr_input;
    }
  }


  int input_height_max = (INPUT_HEIGHT == REAL_INPUT_HEIGHT) ? INPUT_HEIGHT-2*PAD : INPUT_HEIGHT-PAD;

  k = 0;
  /* Input stream */
  for(int row_idx = 0; row_idx < input_height_max; row_idx++)
  {
    for(int col_idx = 0; col_idx < INPUT_WIDTH-2*PAD; col_idx++)
    {
      for(int input_ch_idx = 0; input_ch_idx < (INPUT_CHANNEL+3)/4; input_ch_idx++)
      {
        quad_fp_side_channel curr_input;

        short input_data_sub0;
        short input_data_sub1;
        short input_data_sub2;
        short input_data_sub3;

        input_data_sub0 = (short)(tb_input[k++] * 256);
        input_data_sub1 = (short)(tb_input[k++] * 256);
        input_data_sub2 = (short)(tb_input[k++] * 256);

        if(!(((INPUT_CHANNEL+3)/4 != (INPUT_CHANNEL)/4) && (input_ch_idx == (INPUT_CHANNEL+3)/4-1)))
          input_data_sub3 = (short)(tb_input[k++] * 256);
        else
          input_data_sub3 = 0;

        fp_data_type *sub0_p = (fp_data_type *)&input_data_sub0;
        fp_data_type *sub1_p = (fp_data_type *)&input_data_sub1;
        fp_data_type *sub2_p = (fp_data_type *)&input_data_sub2;
        fp_data_type *sub3_p = (fp_data_type *)&input_data_sub3;

        curr_input.data.sub_data_0 = *sub0_p;
        curr_input.data.sub_data_1 = *sub1_p;
        curr_input.data.sub_data_2 = *sub2_p;
        curr_input.data.sub_data_3 = *sub3_p;
      
        //std::cout << "input 0: " << curr_input.data.sub_data_0 << std::endl;
        //std::cout << "input 1: " << curr_input.data.sub_data_1 << std::endl;
        //std::cout << "input 2: " << curr_input.data.sub_data_2 << std::endl;
        //std::cout << "input 3: " << curr_input.data.sub_data_3 << std::endl;

        curr_input.keep = 1;
        curr_input.strb = 1;
        curr_input.user = 1;
        curr_input.id   = 0;
        curr_input.dest = 0;

        inputStream << curr_input;
      }
    }
  }


  yolo_conv_dw_top(inputStream, outputStream, 
                   OUTPUT_CHANNEL, INPUT_CHANNEL,
                   (OUTPUT_CHANNEL+3)/4, (INPUT_CHANNEL+3)/4,//KERNEL_DIM,
                   INPUT_HEIGHT, INPUT_WIDTH, REAL_INPUT_HEIGHT,
                   (KERNEL_DIM*KERNEL_DIM+3)/4);

  for(int pix_idx = 0; pix_idx < (OUTPUT_WIDTH*OUTPUT_HEIGHT); pix_idx++)
  {
    for(int ch_idx = 0; ch_idx < (OUTPUT_CHANNEL+3)/4; ch_idx++)
    {
      quad_fp_side_channel curr_output;
      outputStream.read(curr_output);

      output_data[4*(pix_idx*((OUTPUT_CHANNEL+3)/4) + ch_idx)] = curr_output.data.sub_data_0;
      output_data[4*(pix_idx*((OUTPUT_CHANNEL+3)/4) + ch_idx) + 1] = curr_output.data.sub_data_1;
      output_data[4*(pix_idx*((OUTPUT_CHANNEL+3)/4) + ch_idx) + 2] = curr_output.data.sub_data_2;
      output_data[4*(pix_idx*((OUTPUT_CHANNEL+3)/4) + ch_idx) + 3] = curr_output.data.sub_data_3;

      //printf("idx: %d\n", 4*(pix_idx*(OUTPUT_CHANNEL+3)/4 + ch_idx));
      //printf("pix_idx: %d\n", pix_idx);
      //printf("ch_idx: %d\n", ch_idx);
      //std::cout << "output0: " << output_data[4*(pix_idx*(OUTPUT_CHANNEL+3)/4 + ch_idx)] << std::endl;
      //std::cout << "output1: " << output_data[4*(pix_idx*(OUTPUT_CHANNEL+3)/4 + ch_idx) + 1] << std::endl;
      //std::cout << "output2: " << output_data[4*(pix_idx*(OUTPUT_CHANNEL+3)/4 + ch_idx) + 2] << std::endl;
      //std::cout << "output3: " << output_data[4*(pix_idx*(OUTPUT_CHANNEL+3)/4 + ch_idx) + 3] << std::endl;

      /* DEBUG */
      //std::cout << "curr_output[0]: " << curr_output.data.sub_data_0 << std::endl;

      if(curr_output.last == 1)
        printf("%d\n", pix_idx*(OUTPUT_CHANNEL+3)/4 + ch_idx);
    }
  }

  short *ptr = (short *)&output_data[0];
  //for(int i = 0; i < OUTPUT_WIDTH*OUTPUT_HEIGHT*OUTPUT_CHANNEL; i++)
  for(int i = 0; i < OUTPUT_WIDTH*OUTPUT_HEIGHT*4; i++)
  {
    if(abs((short)(tb_output[i] * 256) - ptr[i]) > 5)
    {
      flag = true;
    }
    printf("[%d] sw_output: %d, hls_output: %d\n", i, (short)(tb_output[i] * 256), ptr[i]);
  }

  if (flag)
    return 1;
  else
    return 0;
}
