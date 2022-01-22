#include "yolo_conv_dw.h"
//#include "weight_file.h"

void yolo_conv_dw_top(yolo_quad_stream &inStream, yolo_quad_stream &outStream,
    ap_uint<MAX_CH_BIT> output_ch, ap_uint<MAX_CH_BIT> input_ch, ap_uint<MAX_FOLD_CH_BIT> fold_output_ch, ap_uint<MAX_FOLD_CH_BIT> fold_input_ch, //ap_uint<3> kernel_dim,
    ap_uint<9> input_h, ap_uint<9> input_w, ap_uint<9> real_input_h,
    ap_uint<3> fold_win_area)
{
#pragma HLS INTERFACE s_axilite port=fold_input_ch bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=fold_output_ch bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=input_ch bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=output_ch bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=fold_win_area bundle=CTRL_BUS
  //#pragma HLS INTERFACE s_axilite port=kernel_dim bundle=CTRL_BUS
  //#pragma HLS INTERFACE s_axilite port=leaky bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=real_input_h bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=input_w bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=input_h bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS
#pragma HLS INTERFACE axis register both port=outStream
#pragma HLS INTERFACE axis register both port=inStream

  //yolo_inter_stream out_stream_group[MAX_KERNEL_NUM];   // 16bit x 32個
  //yolo_inter_stream out_stream_group[4];   // 16bit x 4個
//#pragma HLS ARRAY_PARTITION variable=out_stream_group complete dim=1  // 16bit x 32 が分解されて 32並列
//#pragma HLS STREAM variable=out_stream_group depth=2 dim=1

  /*
     4並列で処理するためにラインバッファを4つに分けていると思われる
     MAX_KERNEL_NUM は, たぶん一度に処理できるチャネル数の最大値
     ラインバッファには, hls::LineBuffer を使用
     */
  line_buff_type line_buff_group_0[MAX_KERNEL_NUM/4];   // (416+2)x3 のラインバッファが 8チャネル分
  line_buff_type line_buff_group_1[MAX_KERNEL_NUM/4];
  line_buff_type line_buff_group_2[MAX_KERNEL_NUM/4];
  line_buff_type line_buff_group_3[MAX_KERNEL_NUM/4];

  fp_mid_type val_output[MAX_KERNEL_NUM];               // 32bit x 32個
#pragma HLS ARRAY_PARTITION variable=val_output complete dim=1

  quad_fp_side_channel curr_input;      // 入力ストリームを入れる

  /*
     local_weight_type = floatが3*3要素の1次元配列
     入力チャネル x 出力チャネル の最大数分だけ, 3*3の重み用のメモリを用意
dim1:出力チャネル次元, dim2:入力チャネル次元, dim3:3*3の次元
*/ 
  //local_weight_type local_mem_group[MAX_KERNEL_NUM][MAX_INPUT_CH];  // 3x3 が 32x32個
  local_weight_type local_mem_group[MAX_KERNEL_NUM];  // depthwise は in_channel 分がいらない
  // SCRIPT_START P_mem DO NOT EDIT OR DELETE THIS LINE
  //#pragma HLS ARRAY_PARTITION variable=local_mem_group block factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=local_mem_group block factor=4 dim=1
  // SCRIPT_END P_mem DO NOT EDIT OR DELETE THIS LINE
#pragma HLS ARRAY_PARTITION variable=local_mem_group complete dim=2         // 3x3の配列を, 9個バラバラにする (9並列?)

  //	fp_weight_type kernel_bias_fp[MAX_KERNEL_NUM];
  //#pragma HLS ARRAY_PARTITION variable=kernel_bias_fp block factor=4 dim=1



  /* 入力ストリーム重みデータをメモリに入れる部分 */
  /* 1クロックで4個のデータが local_mem_groupに格納されていき, 
     それが このレイヤの 出力チャネル x 入力チャネル x 3 回繰り返される
     「3回」は, 4個データ入れる が2回, 1個データを入れる が1回 */
  for(int k = 0; k < output_ch; k++)
  {
    //#pragma HLS LOOP_TRIPCOUNT min=16 max=16
#pragma HLS LOOP_TRIPCOUNT min=32 max=32
    for(int j = 0; j < fold_win_area; j++)
    {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=3 max=3
      curr_input = inStream.read();

      /* 1回のreadで, 4個のデータが取り込まれる
         4個ずつ処理 を2回と, 1個だけ処理 を1回する
         下のifは, 4個ずつ処理(1,2週目)なのか 1個だけ処理(3周目)なのかの判断
         */
      local_mem_group[k].data[4*j] = curr_input.data.sub_data_0;
      /* MAX_KERNEL_DIM : 3 */
      if(j!=(MAX_KERNEL_DIM*MAX_KERNEL_DIM+3)/4-1)
      {
        local_mem_group[k].data[4*j+1] = curr_input.data.sub_data_1;
        local_mem_group[k].data[4*j+2] = curr_input.data.sub_data_2;
        local_mem_group[k].data[4*j+3] = curr_input.data.sub_data_3;
      }

    }

  }

  /* 入力データを受け取って, すでに保持してある重みデータと積和演算を行う部分 */
  // row_idx, col_idx は, パディング領域も含めた座標
  for(int row_idx = 0; row_idx < input_h+1; row_idx++)
    //extra one row to send rest data
  {
    //#pragma HLS LOOP_TRIPCOUNT min=419 max=419
#pragma HLS LOOP_TRIPCOUNT min=16 max=211
    for(int col_idx = 0; col_idx < input_w; col_idx++)
    {
      //#pragma HLS LOOP_TRIPCOUNT min=418 max=418
#pragma HLS LOOP_TRIPCOUNT min=15 max=210
      for(int input_ch_idx = 0; input_ch_idx < fold_input_ch; input_ch_idx++)
      {
#pragma HLS PIPELINE
        //#pragma HLS LOOP_TRIPCOUNT min=1 max=1
#pragma HLS LOOP_TRIPCOUNT min=4 max=8
        // なぜTRIPCOUNT 1?  fold_input_ch は, 4とか8にもなるのに

        quad_fp_side_channel curr_output;
        ap_fixed<16, 8, AP_RND_CONV, AP_SAT> output_rec_0;
        ap_fixed<16, 8, AP_RND_CONV, AP_SAT> output_rec_1;
        ap_fixed<16, 8, AP_RND_CONV, AP_SAT> output_rec_2;
        ap_fixed<16, 8, AP_RND_CONV, AP_SAT> output_rec_3;

        int conv_row_count = 0, conv_col_count = 0;


        if((row_idx > MAX_KERNEL_DIM-2) && (col_idx > MAX_KERNEL_DIM-2))
          /* if ((row_idx > 1) && (col_idx > 1)) と同じ
             つまり, (2,2)にあたる入力が来たときからconvの計算が始まる
             (2,2)のデータのときは, conv_row_count = 0, conv_col_count = 0
             (2,3)のデータのときは, conv_row_count = 0, conv_col_count = 1
             つまり, これらcountは, 3*3個のconv演算の中央の画素の座標を表す */
        {
          conv_row_count = row_idx - (MAX_KERNEL_DIM-1);
          conv_col_count = col_idx - (MAX_KERNEL_DIM-1);
        }
        else
        {
          conv_row_count = 0;
          conv_col_count = 0;
        }


        // このifの意味がわからん (input_hは416とかになる...)
        if(row_idx != input_h)
        {
          if(((row_idx == 0)||
                (row_idx == real_input_h-1)||
                (col_idx == 0)||
                (col_idx == input_w-1)))
          {
            //padding
            // パディングに当たる座標のときは, 入力データは0埋め
            curr_input.data.sub_data_0 = 0;
            curr_input.data.sub_data_1 = 0;
            curr_input.data.sub_data_2 = 0;
            curr_input.data.sub_data_3 = 0;
          }
          else
          {
            //stream input
            curr_input = inStream.read();
          }


          /* ラインバッファのシフトと, 新しいデータを入れる処理 */
          yolo_line_buffer(curr_input.data.sub_data_0, &line_buff_group_0[input_ch_idx], col_idx);
          yolo_line_buffer(curr_input.data.sub_data_1, &line_buff_group_1[input_ch_idx], col_idx);
          yolo_line_buffer(curr_input.data.sub_data_2, &line_buff_group_2[input_ch_idx], col_idx);
          yolo_line_buffer(curr_input.data.sub_data_3, &line_buff_group_3[input_ch_idx], col_idx);

          //wait for line biffer to fill first conv op
          if((row_idx > MAX_KERNEL_DIM-2) && (col_idx > MAX_KERNEL_DIM-2))
            // (2,2)のデータが来たらconv演算スタート. それまではスキップ
          {
            // window_type は, 3x3のウィンドウ (hls::Window)
            window_type kernel_window_0, kernel_window_1, kernel_window_2, kernel_window_3;

            // ここで3x3の演算用ウィンドウに計算対象の値が入る
            kernel_window_0 = slide_window(conv_col_count, &line_buff_group_0[input_ch_idx]);
            kernel_window_1 = slide_window(conv_col_count, &line_buff_group_1[input_ch_idx]);
            kernel_window_2 = slide_window(conv_col_count, &line_buff_group_2[input_ch_idx]);
            kernel_window_3 = slide_window(conv_col_count, &line_buff_group_3[input_ch_idx]);

            fp_mid_type sub0_val_output;    // 32bit. 16bit同士のかけ算の結果を入れるので, 16x2=32bit必要になる
            fp_mid_type sub1_val_output;
            fp_mid_type sub2_val_output;
            fp_mid_type sub3_val_output;

            //core of conv, macc
            /* kernel_window : 入力データ, local_mem_group : 重みデータ */
            sub0_val_output = window_macc(kernel_window_0, local_mem_group[4*input_ch_idx]);
            sub1_val_output = window_macc(kernel_window_1, local_mem_group[4*input_ch_idx+1]);
            sub2_val_output = window_macc(kernel_window_2, local_mem_group[4*input_ch_idx+2]);
            sub3_val_output = window_macc(kernel_window_3, local_mem_group[4*input_ch_idx+3]);

            //accumulate for number of input channels
            output_rec_0 = sub0_val_output;    // 32bit -> 16bit
            output_rec_1 = sub1_val_output;    // 32bit -> 16bit
            output_rec_2 = sub2_val_output;    // 32bit -> 16bit
            output_rec_3 = sub3_val_output;    // 32bit -> 16bit

            // not write to internal FIFO as input_ch == output_ch
            // write current calculation result to output stream
            curr_output.data.sub_data_0 = output_rec_0;
            curr_output.data.sub_data_1 = output_rec_1;
            curr_output.data.sub_data_2 = output_rec_2;
            curr_output.data.sub_data_3 = output_rec_3;
            
            curr_output.keep = curr_input.keep;
            curr_output.strb = curr_input.strb;
            curr_output.user = curr_input.user;
            if((row_idx == input_h) && (input_ch_idx == fold_input_ch-1)) {
              curr_output.last = 1;
            }
            else {
              curr_output.last = 0;
            }
            curr_output.id = curr_input.id;
            curr_output.dest = curr_input.dest;
            outStream.write(curr_output);
          }
        }

      }
    }
  }
}

void yolo_line_buffer(fp_data_type curr_data, line_buff_type *line_buff, int col_idx)
{

  line_buff->shift_up(col_idx);
  line_buff->insert_top(curr_data, col_idx);

}

/* getval で ラインバッファの指定したインデックスから値を取り出し,
   windowの insert で, 指定したインデックスに値を入れる */
window_type slide_window(int conv_count, line_buff_type *line_buff)
{
  window_type kernel_window;

  for(int win_row = 0; win_row < 3; win_row++)
  {
    for(int win_col = 0; win_col < 3; win_col++)
    {
      fp_data_type val = (fp_data_type)line_buff->getval(win_row, win_col+conv_count);
      kernel_window.insert(val, win_row, win_col);
    }
  }

  return kernel_window;
}

/* 畳み込み演算本体 */
fp_mid_type window_macc(window_type window, local_weight_type weight)
{

  fp_mid_type sum = 0;
  for(int win_row = 0; win_row < 3; win_row++)
  {
    for(int win_col = 0; win_col < 3; win_col++)
    {
      fp_data_type val_in = window.getval(win_row, win_col);
      sum += val_in * weight.data[win_row*3+win_col];
    }
  }
  return sum;
}

/*
void write_output(fp_data_type val_output,  yolo_inter_stream &out_stream)
{
  out_stream.write(val_output);
}

void out_stream_merge(yolo_inter_stream out_stream_group[4], yolo_quad_stream &outStream, int input_ch_idx, quad_fp_side_channel curr_input, ap_uint<1> last, int output_ch, int fold_output_ch)
{
  // 入力4チャネルに対応する4チャネルの畳み込みをFIFOから出力する
  // kind of rotation transmission
  // for every INPUT_CHANNEL inputs, get KERNEL_NUM outputs
  // the transmission is distributed evenly for efficient pipeline
  if(!(out_stream_group[0].empty()))
  {
    quad_fp_side_channel curr_output;

    curr_output.data.sub_data_0 = out_stream_group[0].read();
    curr_output.data.sub_data_1 = out_stream_group[1].read();
    curr_output.data.sub_data_2 = out_stream_group[2].read();

    if(!(out_stream_group[3].empty()))
    {
      curr_output.data.sub_data_3 = out_stream_group[3].read();
    }
    else
    {
      curr_output.data.sub_data_3 = 0;
    }

    // inputをそのままoutputにしていいのか? 遅延があるのでは?
    curr_output.keep = curr_input.keep;
    curr_output.strb = curr_input.strb;
    curr_output.user = curr_input.user;

    curr_output.last = last;

    curr_output.id   = curr_input.id;
    curr_output.dest = curr_input.dest;

    outStream.write(curr_output);

  }

}
*/
