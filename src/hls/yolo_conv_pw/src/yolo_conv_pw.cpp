#include <iostream>
#include "yolo_conv_pw.h"
//#include "weight_file.h"

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

  yolo_inter_stream out_stream_group[MAX_KERNEL_NUM];   // 16bit x 32個
#pragma HLS ARRAY_PARTITION variable=out_stream_group complete dim=1  // 16bit x 32 が分解されて 32並列
#pragma HLS STREAM variable=out_stream_group depth=2 dim=1

  fp_mid_type val_output[MAX_KERNEL_NUM];               // 32bit x 32個
#pragma HLS ARRAY_PARTITION variable=val_output complete dim=1

  quad_fp_side_channel curr_input;      // 入力ストリームを入れる

  /*
     local_weight_type = float
dim1:出力チャネル次元, dim2:入力チャネル次元
*/ 
  local_weight_type local_mem_group[MAX_KERNEL_NUM][MAX_INPUT_CH];  // 1x1 が 32x32個
  // SCRIPT_START P_mem DO NOT EDIT OR DELETE THIS LINE
#pragma HLS ARRAY_PARTITION variable=local_mem_group block factor=8 dim=1
  // SCRIPT_END P_mem DO NOT EDIT OR DELETE THIS LINE
  //#pragma HLS ARRAY_PARTITION variable=local_mem_group complete dim=3         // 3x3の配列を, 9個バラバラにする (9並列?)

  //	fp_weight_type kernel_bias_fp[MAX_KERNEL_NUM];
  //#pragma HLS ARRAY_PARTITION variable=kernel_bias_fp block factor=4 dim=1



  /* 入力ストリーム重みデータをメモリに入れる部分 */
  /* 1回のストリームで 4カーネル分の重みが入ってくる */
WEIGHT_LOOP_OC:
  for(int k = 0; k < output_ch; k++)
  {
#pragma HLS LOOP_TRIPCOUNT min=32 max=32
WEIGHT_LOOP_IC:
    for(int i = 0; i < fold_input_ch; i++)
    {
#pragma HLS LOOP_TRIPCOUNT min=8 max=8
#pragma HLS PIPELINE
      curr_input = inStream.read();
      /* 1回のreadで, 4個のデータが取り込まれる */
      local_mem_group[k][4*i].data   = curr_input.data.sub_data_0;
      local_mem_group[k][4*i+1].data = curr_input.data.sub_data_1;
      local_mem_group[k][4*i+2].data = curr_input.data.sub_data_2;
      local_mem_group[k][4*i+3].data = curr_input.data.sub_data_3;

    }
  }

  /* 入力データを受け取って, すでに保持してある重みデータと積和演算を行う部分 */
  // 1x1畳み込みではパディングはないので, row_idx, col_idx は, input_h, input_w
  // をそのまま使う
  for(int row_idx = 0; row_idx < input_h; row_idx++)
    //extra one row to send rest data
  {
#pragma HLS LOOP_TRIPCOUNT min=417 max=417
    for(int col_idx = 0; col_idx < input_w; col_idx++)
    {
#pragma HLS LOOP_TRIPCOUNT min=416 max=416
      for(int input_ch_idx = 0; input_ch_idx < fold_input_ch; input_ch_idx++)
      {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=4 max=8
        // なぜTRIPCOUNT 1?  fold_input_ch は, 4とか8にもなるのに


        /* 3x3 のときと違って, 最初のストリーム(0,0)の時から畳み込みが始まる */

        if(row_idx != input_h)
        {
          //stream input
          curr_input = inStream.read();

          //copy data to allow parallelism

          // 32
          for(int kernel_idx = 0; kernel_idx < MAX_KERNEL_NUM; kernel_idx++)
            // 親のループがPIPELINE指定されているので, このループは32並列にunrollされるはず
            // 入力4チャネル分に対して, 重みは32チャネル分並列に畳み込みを行う?
          {

            fp_mid_type sub0_val_output;    // 32bit. 16bit同士のかけ算の結果を入れるので, 16x2=32bit必要になる
            fp_mid_type sub1_val_output;
            fp_mid_type sub2_val_output;
            fp_mid_type sub3_val_output;

            //std::cout << "data[0] :" << curr_input.data.sub_data_0 << std::endl;
            //core of conv, macc
            /* kernel_window : 入力データ, local_mem_group : 重みデータ */
            sub0_val_output = curr_input.data.sub_data_0 * local_mem_group[kernel_idx][4*input_ch_idx].data;
            sub1_val_output = curr_input.data.sub_data_1 * local_mem_group[kernel_idx][4*input_ch_idx+1].data;
            sub2_val_output = curr_input.data.sub_data_2 * local_mem_group[kernel_idx][4*input_ch_idx+2].data;
            sub3_val_output = curr_input.data.sub_data_3 * local_mem_group[kernel_idx][4*input_ch_idx+3].data;
            std::cout << "curr_input[0]: " << curr_input.data.sub_data_0;
            std::cout << ", weight[0]: " << local_mem_group[kernel_idx][4*input_ch_idx].data << std::endl;
            // 4並列でかけ算した値の足し合せ
            // それぞれの出力チャネルごとにval_outputに入れる
            val_output[kernel_idx] = post_process(sub0_val_output, sub1_val_output, sub2_val_output, sub3_val_output,
                input_ch_idx, val_output[kernel_idx]);

            //accumulate for number of input channels
            if(input_ch_idx == fold_input_ch-1)
              // 入力チャネルを4チャネルずつ処理していったときの, 最後の4チャネルのときにここに入る
            {
              if(kernel_idx < output_ch)
              {
                ap_fixed<16, 8, AP_RND_CONV, AP_SAT> output_rec = val_output[kernel_idx];    // 32bit -> 16bit
                if(!(out_stream_group[kernel_idx].full()))
                  //write data to internal FIFO
                  // FIFOにつめるだけ
                  // out_stream_groupには, それぞれのカーネル(idx)ごとに, 入力32チャネル分の積和結果が入っている
                  write_output(output_rec, out_stream_group[kernel_idx]);
              }
            }
          }


        }


        //read data from the internal FIFO, and write to the output
        //printf("%d,%d\n",row_idx,col_idx);
        ap_uint<1> last;
        // 入力画素が最後の座標かつ, 最後の4チャネルの入力のとき last=1
        if((row_idx==input_h) && (input_ch_idx==fold_input_ch-1))
          last = 1;
        else
          last = 0;
        out_stream_merge(out_stream_group, outStream, input_ch_idx, curr_input, last, output_ch, fold_output_ch);

      }
    }

  }

}

/* 4並列に処理していた分を足し合わせる */
// val_outputには, 4チャネル分ずつ値が積算され, 最後の入力チャネルの時に, 最大32チャネル分の積和になる
fp_mid_type post_process(fp_mid_type sub0_val_output,fp_mid_type sub1_val_output,fp_mid_type sub2_val_output,fp_mid_type sub3_val_output,
    int input_ch_idx, fp_mid_type val_output)
{
  //fp_data_type biased_output=0,activated_output=0;
  // 最初のチャネルのときに val_outputを初期化
  if(input_ch_idx == 0)
  {
    val_output = 0;
  }

  val_output += sub0_val_output;
  val_output += sub1_val_output;
  val_output += sub2_val_output;
  val_output += sub3_val_output;


  return val_output;
  //	}
  }


void write_output(fp_data_type val_output,  yolo_inter_stream &out_stream)
{
  out_stream.write(val_output);
}

void out_stream_merge(yolo_inter_stream out_stream_group[MAX_KERNEL_NUM], yolo_quad_stream &outStream, int input_ch_idx, quad_fp_side_channel curr_input, ap_uint<1> last, int output_ch, int fold_output_ch )
{

  // kind of rotation transmission
  // for every INPUT_CHANNEL inputs, get KERNEL_NUM outputs
  // the transmission is distributed evenly for efficient pipeline
  for(int i = 0; i < STREAM_TX_SIZE; i++)     // STREAM_TX_SIZE : 6
  {
    int kernel_idx = i + input_ch_idx*STREAM_TX_SIZE;
    if(4*kernel_idx < MAX_KERNEL_NUM) {
      if(!(out_stream_group[4*kernel_idx].empty()))
      {
        quad_fp_side_channel curr_output;

        curr_output.data.sub_data_0 = out_stream_group[4*kernel_idx].read();
        curr_output.data.sub_data_1 = out_stream_group[4*kernel_idx+1].read();
        curr_output.data.sub_data_2 = out_stream_group[4*kernel_idx+2].read();


        if(!(out_stream_group[4*kernel_idx+3].empty()))
        {
          curr_output.data.sub_data_3 = out_stream_group[4*kernel_idx+3].read();
        }
        else
        {
          curr_output.data.sub_data_3 = 0;
        }

        // inputをそのままoutputにしていいのか? 遅延があるのでは?
        curr_output.keep = curr_input.keep;
        curr_output.strb = curr_input.strb;
        curr_output.user = curr_input.user;

        // 最後の出力チャネルグループのとき
        if(kernel_idx == fold_output_ch-1)
          curr_output.last = last;
        else
          curr_output.last = 0;

        curr_output.id = curr_input.id;
        curr_output.dest = curr_input.dest;
        outStream.write(curr_output);
        // outStream には, 入力32チャネル分の積和結果が, 出力チャネル4チャネル分ずつ入っていく
        // 32チャネル分 x 4

      }
    }
  }

}
