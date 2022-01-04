#include "yolo_acc.h"


// サブチャネルに分けた分の処理を足していく
// 例えば, 入力チャネル数が128の層なら, yolo_acc_topの処理を4回行い,
// yolo_acc_topが呼ばれるたびに, 32チャネル分の出力, 64チャネル分の出力,
// 96チャネル分の出力, 128チャネル分の出力(その層の畳込みの出力) が得られる
void yolo_acc_top(yolo_quad_stream &inStream_a, yolo_quad_stream &inStream_b,
    yolo_quad_stream &outStream,
    ap_uint<9> input_h, ap_uint<9> input_w,
    ap_uint<MAX_FOLD_CH_BIT> fold_input_ch,
    ap_uint<1> leaky, ap_uint<1> bias_en)
{
#pragma HLS INTERFACE s_axilite port=bias_en bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=leaky bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=fold_input_ch bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=input_w bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=input_h bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS
#pragma HLS INTERFACE axis register both port=outStream
#pragma HLS INTERFACE axis register both port=inStream_a
#pragma HLS INTERFACE axis register both port=inStream_b

  fp_weight_type kernel_bias_fp[MAX_KERNEL_NUM];
  // factor=1で分割って意味ある?
  // SCRIPT_START P_acc DO NOT EDIT OR DELETE THIS LINE
#pragma HLS ARRAY_PARTITION variable=kernel_bias_fp cyclic factor=1 dim=1
  // SCRIPT_END P_acc DO NOT EDIT OR DELETE THIS LINE

  // bias値を受け取る部分
  for(ap_uint<MAX_FOLD_CH_BIT> i=0;i<fold_input_ch;i++)//division 2 is not safe here!!!
  {
#pragma HLS LOOP_TRIPCOUNT min=4 max=4
#pragma HLS PIPELINE
    if(bias_en==1)
    {
      quad_fp_side_channel curr_input;
      curr_input = inStream_b.read();
      kernel_bias_fp[4*i] = curr_input.data.sub_data_0;
      kernel_bias_fp[4*i+1] = curr_input.data.sub_data_1;
      kernel_bias_fp[4*i+2] = curr_input.data.sub_data_2;
      kernel_bias_fp[4*i+3] = curr_input.data.sub_data_3;
    }
  }



  for(int row_idx=0;row_idx<input_h;row_idx++)
  {
#pragma HLS LOOP_TRIPCOUNT min=416 max=416
    for(int col_idx=0;col_idx<input_w;col_idx++)
    {
#pragma HLS LOOP_TRIPCOUNT min=416 max=416
      for(int input_ch_idx=0;input_ch_idx<fold_input_ch;input_ch_idx++)
      {
#pragma HLS LOOP_TRIPCOUNT min=4 max=4
#pragma HLS PIPELINE
        quad_fp_side_channel curr_input_a, curr_input_b;
        quad_fp_side_channel curr_output;

        fp_data_type output_acc_0, output_acc_1, output_acc_2, output_acc_3;

        curr_input_a = inStream_a.read();     // たぶんいままでの積算
        curr_input_b = inStream_b.read();     // たぶん今回の32チャネル分の畳み込み層の結果

        // 処理済みのチャネル分の積算に, 今回の32チャネル分の結果を足す
        output_acc_0 = curr_input_a.data.sub_data_0 + curr_input_b.data.sub_data_0;
        output_acc_1 = curr_input_a.data.sub_data_1 + curr_input_b.data.sub_data_1;
        output_acc_2 = curr_input_a.data.sub_data_2 + curr_input_b.data.sub_data_2;
        output_acc_3 = curr_input_a.data.sub_data_3 + curr_input_b.data.sub_data_3;

        // bias加算 と LeakyReLUをかける処理
        // 32チャネルずつ毎回biasを足す? biasは, 入力チャネル数分完了したあとに1回だけ足すのでは?
        curr_output.data.sub_data_0 = post_process_unit(output_acc_0, kernel_bias_fp[4*input_ch_idx], bias_en, leaky);
        curr_output.data.sub_data_1 = post_process_unit(output_acc_1, kernel_bias_fp[4*input_ch_idx+1], bias_en, leaky);
        curr_output.data.sub_data_2 = post_process_unit(output_acc_2, kernel_bias_fp[4*input_ch_idx+2], bias_en, leaky);
        curr_output.data.sub_data_3 = post_process_unit(output_acc_3, kernel_bias_fp[4*input_ch_idx+3], bias_en, leaky);


        curr_output.keep = curr_input_a.keep;
        curr_output.strb = curr_input_a.strb;
        curr_output.user = curr_input_a.user;

        if((input_ch_idx == MAX_KERNEL_NUM/4-1)   // つまりinput_ch_idx が 7 になった かつ 最後の画素
            &&(col_idx == input_w-1)
            &&(row_idx == input_h-1))
          curr_output.last = 1;
        else
          curr_output.last = 0;

        curr_output.id = curr_input_a.id;
        curr_output.dest = curr_input_a.dest;

        outStream.write(curr_output);

      }
    }
  }

}

fp_data_type post_process_unit(fp_data_type data_in, fp_weight_type bias, ap_uint<1> bias_en, ap_uint<1> leaky)
{
  fp_data_type biased_output=0,activated_output=0;
  if(bias_en)
  {
    biased_output = data_in + bias;
    // LeakyReLUの処理(0.1をかける)
    if(leaky&&biased_output<0)
    {
      activated_output = biased_output * (fp_data_type).1;
    }
    else
    {
      activated_output = biased_output;
    }

    return activated_output;
  }
  else
  {
    return data_in;
  }
}
