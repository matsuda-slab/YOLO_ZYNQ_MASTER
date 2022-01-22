#ifndef YOLO_CONV_H
#define YOLO_CONV_H

#include "yolo_stream.h"
#include "hls_video.h"

#define STREAM_TX_SIZE 6
#define MAX_CH_BIT (5+1)
#define MAX_FOLD_CH_BIT (5-2+1)

// SCRIPT_START N_max DO NOT EDIT OR DELETE THIS LINE
#define MAX_KERNEL_NUM 32
// SCRIPT_END N_max DO NOT EDIT OR DELETE THIS LINE

#define MAX_INPUT_CH 32
//#define MAX_LINE_BUFF_WIDTH (416+2)
#define MAX_LINE_BUFF_WIDTH (208+2)
#define MAX_KERNEL_DIM 3

typedef hls::Window<3, 3, fp_data_type> window_type;
typedef hls::LineBuffer<3, MAX_LINE_BUFF_WIDTH, fp_data_type> line_buff_type;
typedef struct local_weight_type
{
	fp_weight_type data[3*3];
} local_weight_type;

void yolo_conv_dw_top(yolo_quad_stream &inStream, yolo_quad_stream &outStream,
		           ap_uint<MAX_CH_BIT> output_ch, ap_uint<MAX_CH_BIT> input_ch, ap_uint<MAX_FOLD_CH_BIT> fold_output_ch, ap_uint<MAX_FOLD_CH_BIT> fold_input_ch, //ap_uint<3> kernel_dim,
		           ap_uint<9> input_h, ap_uint<9> input_w, ap_uint<9> real_input_h,
				   ap_uint<3> fold_win_area);

void yolo_line_buffer(fp_data_type curr_data, line_buff_type *line_buff, int col_idx);

window_type slide_window(int conv_count, line_buff_type *line_buff);

fp_mid_type window_macc(window_type window, local_weight_type weight);

//void write_output(fp_data_type val_output,  yolo_inter_stream &out_stream);

//void out_stream_merge(yolo_inter_stream out_stream_group[4], yolo_quad_stream &outStream, int input_ch_idx, quad_fp_side_channel curr_input, ap_uint<1> last, int output_ch, int fold_output_ch);

#endif
