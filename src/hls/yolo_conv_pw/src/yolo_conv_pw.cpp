#include <iostream>
#include "yolo_conv_pw.h"
//#include "weight_file.h"

#define PRAGMA_SUB(x) _Pragma (#x)
#define DO_PRAGMA(x) PRAGMA_SUB(x)

void yolo_conv_pw_top(yolo_quad_stream &inStream, yolo_quad_stream &outStream,
    ap_uint<MAX_CH_BIT> output_ch, ap_uint<MAX_CH_BIT> input_ch,
    ap_uint<MAX_FOLD_CH_BIT> fold_output_ch, 
    ap_uint<MAX_FOLD_CH_BIT> fold_input_ch,
    ap_uint<9> input_h, ap_uint<9> input_w
    )
{
#pragma HLS INTERFACE s_axilite port=fold_input_ch bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=fold_output_ch bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=input_ch bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=output_ch bundle=CTRL_BUS
  //#pragma HLS INTERFACE s_axilite port=kernel_dim bundle=CTRL_BUS
  //#pragma HLS INTERFACE s_axilite port=leaky bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=input_w bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=input_h bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS
#pragma HLS INTERFACE axis register both port=outStream
#pragma HLS INTERFACE axis register both port=inStream

  yolo_inter_stream out_stream_group[MAX_KERNEL_NUM];
#pragma HLS ARRAY_PARTITION variable=out_stream_group complete dim=1
#pragma HLS STREAM variable=out_stream_group depth=2 dim=1

  fp_mid_type val_output[MAX_KERNEL_NUM];
#pragma HLS ARRAY_PARTITION variable=val_output complete dim=1

  fp_mid_type val_output_mid[MAX_KERNEL_NUM];
#pragma HLS ARRAY_PARTITION variable=val_output_mid complete dim=1

  quad_fp_side_channel curr_input;

  local_weight_type local_mem_group[MAX_KERNEL_NUM][MAX_INPUT_CH];  // 1x1 が 32x32個
  // SCRIPT_START P_mem DO NOT EDIT OR DELETE THIS LINE
//#pragma HLS ARRAY_PARTITION variable=local_mem_group block factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=local_mem_group complete dim=1
#pragma HLS ARRAY_PARTITION variable=local_mem_group block factor=4 dim=2
  // SCRIPT_END P_mem DO NOT EDIT OR DELETE THIS LINE
  //#pragma HLS ARRAY_PARTITION variable=local_mem_group complete dim=3

  //	fp_weight_type kernel_bias_fp[MAX_KERNEL_NUM];
  //#pragma HLS ARRAY_PARTITION variable=kernel_bias_fp block factor=4 dim=1


WEIGHT_LOOP_OC:
  for(int k = 0; k < output_ch; k++)
  {
DO_PRAGMA(HLS LOOP_TRIPCOUNT min=OC_TRIP max=OC_TRIP)
//#pragma HLS LOOP_TRIPCOUNT min=16 max=16
WEIGHT_LOOP_FOLD:
    for(int i = 0; i < fold_input_ch; i++)
    {
DO_PRAGMA(HLS LOOP_TRIPCOUNT min=FOLD max=FOLD)
//#pragma HLS LOOP_TRIPCOUNT min=3 max=3
#pragma HLS PIPELINE
      curr_input = inStream.read();
      local_mem_group[k][4*i].data   = curr_input.data.sub_data_0;
      local_mem_group[k][4*i+1].data = curr_input.data.sub_data_1;
      local_mem_group[k][4*i+2].data = curr_input.data.sub_data_2;
      local_mem_group[k][4*i+3].data = curr_input.data.sub_data_3;

    }
  }

ROW_LOOP:
  for(int row_idx = 0; row_idx < input_h; row_idx++)
    //extra one row to send rest data
  {
DO_PRAGMA(HLS LOOP_TRIPCOUNT min=ROW_TRIP max=ROW_TRIP)
//#pragma HLS LOOP_TRIPCOUNT min=417 max=417
COL_LOOP:
    for(int col_idx = 0; col_idx < input_w; col_idx++)
    {
DO_PRAGMA(HLS LOOP_TRIPCOUNT min=COL_TRIP max=COL_TRIP)
//#pragma HLS LOOP_TRIPCOUNT min=416 max=416
FOLD_LOOP:
      //for(int input_ch_idx = 0; input_ch_idx < fold_input_ch; input_ch_idx++)
      for(int input_ch_idx = 0; input_ch_idx < fold_output_ch; input_ch_idx++)
      {
#pragma HLS PIPELINE
DO_PRAGMA(HLS LOOP_TRIPCOUNT min=FOLD max=FOLD)
//#pragma HLS LOOP_TRIPCOUNT min=1 max=1


        if(row_idx != input_h)
        {
          //stream input
          if(input_ch_idx < fold_input_ch)
            curr_input = inStream.read();

          //copy data to allow parallelism

          // 32
          for(int kernel_idx = 0; kernel_idx < MAX_KERNEL_NUM; kernel_idx++)
          {

            fp_mid_type sub0_val_output;
            fp_mid_type sub1_val_output;
            fp_mid_type sub2_val_output;
            fp_mid_type sub3_val_output;

            //std::cout << "data[0] :" << curr_input.data.sub_data_0 << std::endl;
            //core of conv, macc
            if(input_ch_idx < fold_input_ch) {
              sub0_val_output = curr_input.data.sub_data_0 * local_mem_group[kernel_idx][4*input_ch_idx].data;
              sub1_val_output = curr_input.data.sub_data_1 * local_mem_group[kernel_idx][4*input_ch_idx+1].data;
              sub2_val_output = curr_input.data.sub_data_2 * local_mem_group[kernel_idx][4*input_ch_idx+2].data;
              if(input_ch == 3) {
                sub3_val_output = 0;
              }
              else {
                sub3_val_output = curr_input.data.sub_data_3 * local_mem_group[kernel_idx][4*input_ch_idx+3].data;
              }
            }
            else {
              sub0_val_output = 0;
              sub1_val_output = 0;
              sub2_val_output = 0;
              sub3_val_output = 0;
            }

            /* DEBUG */
            //std::cout << "curr_input[0]: " << curr_input.data.sub_data_0;
            //std::cout << ", weight[0]: " << local_mem_group[kernel_idx][4*input_ch_idx].data << std::endl;
            val_output[kernel_idx] = post_process(sub0_val_output, sub1_val_output, sub2_val_output, sub3_val_output,
                input_ch_idx, val_output[kernel_idx]);

            //accumulate for number of input channels
            //if(input_ch_idx == fold_input_ch-1)
            if(input_ch_idx == fold_input_ch-1)
            {
              if(kernel_idx < output_ch)
              {
                ap_fixed<16, 8, AP_RND_CONV, AP_SAT> output_rec = val_output[kernel_idx];    // 32bit -> 16bit
                /* DEBUG */
                //std::cout << "val_output(32bit)[" << kernel_idx << "]: " << val_output[kernel_idx];
                //std::cout << ",  output_rec(16bit): " << output_rec << std::endl;
                if(!(out_stream_group[kernel_idx].full()))
                  //write data to internal FIFO
                  write_output(output_rec, out_stream_group[kernel_idx]);
              }
            }
          }

        }

        //read data from the internal FIFO, and write to the output
        //printf("%d,%d\n",row_idx,col_idx);
        ap_uint<1> last;
        if((row_idx == input_h-1) && (col_idx == input_h-1) && (input_ch_idx == fold_input_ch-1))
          last = 1;
        else
          last = 0;
        out_stream_merge(out_stream_group, outStream, input_ch_idx, curr_input, last, output_ch);
        //out_stream_merge(out_stream_group, outStream, input_ch_idx, curr_input, last, output_ch, fold_output_ch);

      }
    }

  }

}

fp_mid_type post_process(fp_mid_type sub0_val_output, fp_mid_type sub1_val_output, fp_mid_type sub2_val_output, fp_mid_type sub3_val_output,
    int input_ch_idx, fp_mid_type val_output)
{
  fp_mid_type sub_add0, sub_add1, sub_add2;

  //fp_data_type biased_output=0,activated_output=0;
  if(input_ch_idx == 0)
  {
    val_output = 0;
  }

  sub_add0 = sub0_val_output + sub1_val_output;
  sub_add1 = sub2_val_output + sub3_val_output;
  sub_add2 = sub_add0 + sub_add1;
  val_output += sub_add2;

  //val_output += sub0_val_output;
  //val_output += sub1_val_output;
  //val_output += sub2_val_output;
  //val_output += sub3_val_output;


  return val_output;
  //	}
  }


void write_output(fp_data_type val_output,  yolo_inter_stream &out_stream)
{
  out_stream.write(val_output);
}

void out_stream_merge(yolo_inter_stream out_stream_group[MAX_KERNEL_NUM], yolo_quad_stream &outStream, int input_ch_idx, quad_fp_side_channel curr_input, ap_uint<1> last, int output_ch)
//void out_stream_merge(yolo_inter_stream out_stream_group[MAX_KERNEL_NUM], yolo_quad_stream &outStream, int input_ch_idx, quad_fp_side_channel curr_input, ap_uint<1> last, int output_ch, int fold_output_ch )
{

  // kind of rotation transmission
  // for every INPUT_CHANNEL inputs, get KERNEL_NUM outputs
  // the transmission is distributed evenly for efficient pipeline
  if(!(out_stream_group[4*input_ch_idx].empty()))
  {
    //std::cout << "input_ch_idx : " << input_ch_idx << std::endl;
    quad_fp_side_channel curr_output;

    curr_output.data.sub_data_0 = out_stream_group[4*input_ch_idx].read();
    curr_output.data.sub_data_1 = out_stream_group[4*input_ch_idx+1].read();
    curr_output.data.sub_data_2 = out_stream_group[4*input_ch_idx+2].read();

    if(!(out_stream_group[4*input_ch_idx+3].empty()))
    {
      curr_output.data.sub_data_3 = out_stream_group[4*input_ch_idx+3].read();
    }
    else
    {
      curr_output.data.sub_data_3 = 0;
    }

    curr_output.keep = curr_input.keep;
    curr_output.strb = curr_input.strb;
    curr_output.user = curr_input.user;

    curr_output.last = last;

    curr_output.id = curr_input.id;
    curr_output.dest = curr_input.dest;
    outStream.write(curr_output);

  }

  /*
     if(!(out_stream_group[4*kernel_idx].empty())) {
     quad_fp_side_channel curr_output[8];
     for(int i = 0; i < MAX_KERNEL_NUM/4; i++) {
     if(output_ch < i) {
     curr_output[i].data.sub_data_0 = 0;
     curr_output[i].data.sub_data_1 = 0;
     curr_output[i].data.sub_data_2 = 0;
     curr_output[i].data.sub_data_3 = 0;
     }
     else {
     curr_output[i].data.sub_data_0 = out_stream_group[4*i].read();
     curr_output[i].data.sub_data_1 = out_stream_group[4*i+1].read();
     curr_output[i].data.sub_data_2 = out_stream_group[4*i+2].read();
     if(out_stream_group[4*i+3].empty()) {
     curr_output[i].data.sub_data_3 = 0;
     }
     else {
     curr_output[i].data.sub_data_3 = out_stream_group[4*i+3].read();
     }
     }
     curr_output[i].keep = curr_input.keep;
     curr_output[i].strb = curr_input.strb;
     curr_output[i].user = curr_input.user;
     curr_output[i].id   = curr_input.id;
     curr_output[i].dest = curr_input.dest;

     curr_output[i].last = last;
     outStream[i].write(curr_output[i]);
     }
     }
     */

}
