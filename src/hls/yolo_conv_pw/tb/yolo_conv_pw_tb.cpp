//hls test bench
//if error : return 1
#include <stdio.h>
#include <math.h>
#include "../src/yolo_conv_pw.h"
#include "include/conv_tb.h"

/*
#define INPUT_CHANNEL 4
#define OUTPUT_CHANNEL 4

#define IN_IMG_IN_CHANNEL 1

#define IMG_WIDTH 2
#define IMG_HEIGHT 2

#define ERROR 5

int main()
{
  //入力, 出力 ストリーム
  yolo_quad_stream inputStream("in_stream"), outputStream("out_stream");


  //for(int i=0;i<OUTPUT_CHANNEL*INPUT_CHANNEL/4;i++){
  //  short input_data_sub0 = (i*4  ) * 256;
  //  short input_data_sub1 = (i*4+1) * 256;
  //  short input_data_sub2 = (i*4+2) * 256;
  //  short input_data_sub3 = (i*4+3) * 256;
  //
  //  fp_data_type *sub0_p = (fp_data_type *)&input_data_sub0;
  //  fp_data_type *sub1_p = (fp_data_type *)&input_data_sub1;
  //  fp_data_type *sub2_p = (fp_data_type *)&input_data_sub2;
  //  fp_data_type *sub3_p = (fp_data_type *)&input_data_sub3;
  //
  //  quad_fp_side_channel curr_input;
  //  curr_input.data.sub_data_0  = *sub0_p;
  //  curr_input.data.sub_data_1  = *sub1_p;
  //  curr_input.data.sub_data_2  = *sub2_p;
  //  curr_input.data.sub_data_3  = *sub3_p;
  //  
  //  curr_input.keep = 1;
  //  curr_input.strb = 1;
  //  curr_input.user = 1;
  //  curr_input.id   = 0;
  //  curr_input.dest = 0;
  //  
  //  // DEBUG 
  //  std::cout << "(input) curr_input[0]: " << curr_input.data.sub_data_0 << std::endl;
  //  std::cout << "(input) curr_input[1]: " << curr_input.data.sub_data_1 << std::endl;
  //  std::cout << "(input) curr_input[2]: " << curr_input.data.sub_data_2 << std::endl;
  //  std::cout << "(input) curr_input[3]: " << curr_input.data.sub_data_3 << std::endl;
  //
  //    inputStream << curr_input;
  //}

  short input_data_sub0 = (0) * 256;                                            
  short input_data_sub1 = (1) * 256;                                            
  short input_data_sub2 = (0) * 256;                                            
  short input_data_sub3 = (0) * 256;                                            
  fp_data_type *sub0_p = (fp_data_type *)&input_data_sub0;                          
  fp_data_type *sub1_p = (fp_data_type *)&input_data_sub1;                          
  fp_data_type *sub2_p = (fp_data_type *)&input_data_sub2;                          
  fp_data_type *sub3_p = (fp_data_type *)&input_data_sub3;                          
  quad_fp_side_channel curr_input;                                                  
  curr_input.data.sub_data_0  = *sub0_p;                                            
  curr_input.data.sub_data_1  = *sub1_p;                                            
  curr_input.data.sub_data_2  = *sub2_p;                                            
  curr_input.data.sub_data_3  = *sub3_p;                                           
  curr_input.keep = 1;                                                              
  curr_input.strb = 1;                                                              
  curr_input.user = 1;                                                              
  curr_input.id   = 0;                                                              
  curr_input.dest = 0;                                                               
  inputStream << curr_input;                                                      

  input_data_sub0 = (2) * 256;                                            
  input_data_sub1 = (3) * 256;                                            
  input_data_sub2 = (0) * 256;                                            
  input_data_sub3 = (0) * 256;                                            
  sub0_p = (fp_data_type *)&input_data_sub0;                          
  sub1_p = (fp_data_type *)&input_data_sub1;                          
  sub2_p = (fp_data_type *)&input_data_sub2;                          
  sub3_p = (fp_data_type *)&input_data_sub3;                          
  curr_input.data.sub_data_0  = *sub0_p;                                            
  curr_input.data.sub_data_1  = *sub1_p;                                            
  curr_input.data.sub_data_2  = *sub2_p;                                            
  curr_input.data.sub_data_3  = *sub3_p;                                           
  curr_input.keep = 1;                                                              
  curr_input.strb = 1;                                                              
  curr_input.user = 1;                                                              
  curr_input.id   = 0;                                                              
  curr_input.dest = 0;                                                               
  inputStream << curr_input;

  input_data_sub0 = (4) * 256;                                            
  input_data_sub1 = (5) * 256;                                            
  input_data_sub2 = (0) * 256;                                            
  input_data_sub3 = (0) * 256;                                            
  sub0_p = (fp_data_type *)&input_data_sub0;                          
  sub1_p = (fp_data_type *)&input_data_sub1;                          
  sub2_p = (fp_data_type *)&input_data_sub2;                          
  sub3_p = (fp_data_type *)&input_data_sub3;                          
  curr_input.data.sub_data_0  = *sub0_p;                                            
  curr_input.data.sub_data_1  = *sub1_p;                                            
  curr_input.data.sub_data_2  = *sub2_p;                                            
  curr_input.data.sub_data_3  = *sub3_p;                                           
  curr_input.keep = 1;                                                              
  curr_input.strb = 1;                                                              
  curr_input.user = 1;                                                              
  curr_input.id   = 0;                                                              
  curr_input.dest = 0;                                                               
  inputStream << curr_input;

  input_data_sub0 = (6) * 256;                                            
  input_data_sub1 = (7) * 256;                                            
  input_data_sub2 = (0) * 256;                                            
  input_data_sub3 = (0) * 256;                                            
  sub0_p = (fp_data_type *)&input_data_sub0;                          
  sub1_p = (fp_data_type *)&input_data_sub1;                          
  sub2_p = (fp_data_type *)&input_data_sub2;                          
  sub3_p = (fp_data_type *)&input_data_sub3;                          
  curr_input.data.sub_data_0  = *sub0_p;                                            
  curr_input.data.sub_data_1  = *sub1_p;                                            
  curr_input.data.sub_data_2  = *sub2_p;                                            
  curr_input.data.sub_data_3  = *sub3_p;                                           
  curr_input.keep = 1;                                                              
  curr_input.strb = 1;                                                              
  curr_input.user = 1;                                                              
  curr_input.id   = 0;                                                              
  curr_input.dest = 0;                                                               
  inputStream << curr_input;

  

  
  
   
  for(int i=0;i<IMG_WIDTH*IMG_HEIGHT*INPUT_CHANNEL/4;i++){
    short input_data_sub0 = (i*4  ) * 256;
    short input_data_sub1 = (i*4+1) * 256;
    short input_data_sub2 = (i*4+2) * 256;
    short input_data_sub3 = (i*4+3) * 256;

    fp_data_type *sub0_p = (fp_data_type *)&input_data_sub0;
    fp_data_type *sub1_p = (fp_data_type *)&input_data_sub1;
    fp_data_type *sub2_p = (fp_data_type *)&input_data_sub2;
    fp_data_type *sub3_p = (fp_data_type *)&input_data_sub3;

    quad_fp_side_channel curr_input;
    curr_input.data.sub_data_0  = *sub0_p;
    curr_input.data.sub_data_1  = *sub1_p;
    curr_input.data.sub_data_2  = *sub2_p;
    curr_input.data.sub_data_3  = *sub3_p;
    
    curr_input.keep = 1;
    curr_input.strb = 1;
    curr_input.user = 1;
    curr_input.id   = 0;
    curr_input.dest = 0;
    
    // DEBUG 
    //std::cout << "(input) curr_input[0]: " << curr_input.data.sub_data_0 << std::endl;
    //std::cout << "(input) curr_input[1]: " << curr_input.data.sub_data_1 << std::endl;
    //std::cout << "(input) curr_input[2]: " << curr_input.data.sub_data_2 << std::endl;
    //std::cout << "(input) curr_input[3]: " << curr_input.data.sub_data_3 << std::endl;

    inputStream << curr_input;
  }
  
  
  yolo_conv_pw_top(inputStream, outputStream, OUTPUT_CHANNEL, INPUT_CHANNEL,
		   (OUTPUT_CHANNEL+3)/4, (INPUT_CHANNEL+3)/4,
		   IMG_HEIGHT, IMG_WIDTH);

  
  static fp_data_type output_data[IMG_WIDTH*IMG_HEIGHT*OUTPUT_CHANNEL];
  
  for(int pix_idx=0;pix_idx<(IMG_WIDTH*IMG_HEIGHT);pix_idx++)
  { for(int ch_idx=0;ch_idx<(OUTPUT_CHANNEL)/4;ch_idx++)
    {
      quad_fp_side_channel curr_output;
      outputStream.read(curr_output);

      output_data[4*(pix_idx*(OUTPUT_CHANNEL)/4+ch_idx)] = curr_output.data.sub_data_0;
      output_data[4*(pix_idx*(OUTPUT_CHANNEL)/4+ch_idx)+1] = curr_output.data.sub_data_1;
      output_data[4*(pix_idx*(OUTPUT_CHANNEL)/4+ch_idx)+2] = curr_output.data.sub_data_2;
      output_data[4*(pix_idx*(OUTPUT_CHANNEL)/4+ch_idx)+3] = curr_output.data.sub_data_3;

      // DEBUG 
      std::cout << "(input) output_data[0]: " << output_data[4*(pix_idx*(OUTPUT_CHANNEL)/4+ch_idx)] << std::endl;
      std::cout << "(input) output_data[0]: " << output_data[4*(pix_idx*(OUTPUT_CHANNEL)/4+ch_idx)+1] << std::endl;
      std::cout << "(input) output_data[0]: " << output_data[4*(pix_idx*(OUTPUT_CHANNEL)/4+ch_idx)+2] << std::endl;
      std::cout << "(input) output_data[0]: " << output_data[4*(pix_idx*(OUTPUT_CHANNEL)/4+ch_idx)+3] << std::endl;

      if(curr_output.last==1)
        printf("%d\n",pix_idx*(OUTPUT_CHANNEL)/4+ch_idx);
    }
  }
  
  
  return 0;
}
*/

#define INPUT_CHANNEL 4
#define OUTPUT_CHANNEL 8

#define IN_IMG_IN_CHANNEL 1

#define IMG_WIDTH 16
#define IMG_HEIGHT 16

#define ERROR 5

int main()
{
  //入力, 出力 ストリーム
  yolo_quad_stream inputStream("in_stream"), outputStream("out_stream");

  int k = 0;

  for(int i=0;i<OUTPUT_CHANNEL*INPUT_CHANNEL/4;i++){
    short input_data_sub0 = (short)(tb_weights[k++]*256);
    short input_data_sub1 = (short)(tb_weights[k++]*256);
    short input_data_sub2 = (short)(tb_weights[k++]*256);
    short input_data_sub3 = (short)(tb_weights[k++]*256);

    fp_data_type *sub0_p = (fp_data_type *)&input_data_sub0;
    fp_data_type *sub1_p = (fp_data_type *)&input_data_sub1;
    fp_data_type *sub2_p = (fp_data_type *)&input_data_sub2;
    fp_data_type *sub3_p = (fp_data_type *)&input_data_sub3;

    quad_fp_side_channel curr_input;
    curr_input.data.sub_data_0  = *sub0_p;
    curr_input.data.sub_data_1  = *sub1_p;
    curr_input.data.sub_data_2  = *sub2_p;
    curr_input.data.sub_data_3  = *sub3_p;
    
    curr_input.keep = 1;
    curr_input.strb = 1;
    curr_input.user = 1;
    curr_input.id   = 0;
    curr_input.dest = 0;
    
    // DEBUG 
    //std::cout << "(input) curr_input[0]: " << curr_input.data.sub_data_0 << std::endl;
    //std::cout << "(input) curr_input[1]: " << curr_input.data.sub_data_1 << std::endl;
    //std::cout << "(input) curr_input[2]: " << curr_input.data.sub_data_2 << std::endl;
    //std::cout << "(input) curr_input[3]: " << curr_input.data.sub_data_3 << std::endl;

    inputStream << curr_input;
  }
  k = 0;
  
  for(int i=0;i<IN_IMG_IN_CHANNEL*INPUT_CHANNEL*IMG_HEIGHT*IMG_WIDTH/4;i++){
    short input_data_sub0 = (short)(tb_input[k++]*256);
    short input_data_sub1 = (short)(tb_input[k++]*256);
    short input_data_sub2 = (short)(tb_input[k++]*256);
    short input_data_sub3 = (short)(tb_input[k++]*256);

    quad_fp_side_channel curr_input;

    fp_data_type *sub0_p = (fp_data_type *)&input_data_sub0;
    fp_data_type *sub1_p = (fp_data_type *)&input_data_sub1;
    fp_data_type *sub2_p = (fp_data_type *)&input_data_sub2;
    fp_data_type *sub3_p = (fp_data_type *)&input_data_sub3;

    curr_input.data.sub_data_0  = *sub0_p;
    curr_input.data.sub_data_1  = *sub1_p;
    curr_input.data.sub_data_2  = *sub2_p;
    curr_input.data.sub_data_3  = *sub3_p;
    
    curr_input.keep = 1;
    curr_input.strb = 1;
    curr_input.user = 1;
    curr_input.id   = 0;
    curr_input.dest = 0;
    
    // DEBUG 
    //std::cout << "(input) curr_input[0]: " << curr_input.data.sub_data_0 << std::endl;

    inputStream << curr_input;
  }
  
  //IPを呼び出す
  yolo_conv_pw_top(inputStream, outputStream, OUTPUT_CHANNEL, INPUT_CHANNEL,
      (OUTPUT_CHANNEL+3)/4, (INPUT_CHANNEL+3)/4,
      IMG_HEIGHT, IMG_WIDTH);


  //出力ストリーム 
  static fp_data_type output_data[IMG_WIDTH*IMG_HEIGHT*OUTPUT_CHANNEL];
  int error_flag = 0;
  for(int pix_idx=0;pix_idx<(IMG_WIDTH*IMG_HEIGHT);pix_idx++)
  {
    for(int ch_idx=0;ch_idx<(OUTPUT_CHANNEL)/4;ch_idx++)
    {
      quad_fp_side_channel curr_output;
      outputStream.read(curr_output);

      output_data[4*(pix_idx*(OUTPUT_CHANNEL)/4+ch_idx)] = curr_output.data.sub_data_0;
      output_data[4*(pix_idx*(OUTPUT_CHANNEL)/4+ch_idx)+1] = curr_output.data.sub_data_1;
      output_data[4*(pix_idx*(OUTPUT_CHANNEL)/4+ch_idx)+2] = curr_output.data.sub_data_2;
      output_data[4*(pix_idx*(OUTPUT_CHANNEL)/4+ch_idx)+3] = curr_output.data.sub_data_3;

      // DEBUG 
      //std::cout << "(input) output_data[0]: " << output_data[4*(pix_idx*(OUTPUT_CHANNEL)/4+ch_idx)] << std::endl;

      if(curr_output.last==1)
        printf("%d\n",pix_idx*(OUTPUT_CHANNEL)/4+ch_idx);
    }
  }

  short *ptr = (short *)&output_data[0];
  //テストベンチと結果を比較
  for(int i=0;i<OUTPUT_CHANNEL*IMG_WIDTH*IMG_HEIGHT; i++){
    if(abs((short)(tb_output[i] * 256) - (ptr[i])) > 5){
      printf("tb_output[%d]: %d, output_data[%d]: %d\n", i, (short)(tb_output[i] * 256), i, ptr[i]);
      error_flag = 1;
    }
    printf("[%d] test-bench:%d, hls-output:%d\n",i ,(short)(tb_output[i] * 256), ptr[i]);
  }
  if(error_flag)
    return 1;
  else
    return 0;
}
