open_project -reset yolo_conv_prj
set_top yolo_conv_top
add_files src/yolo_fp.h
add_files src/yolo_conv.h
add_files src/yolo_stream.h
#add_files src/yolo_conv.cpp -cflags "-D OC_TRIP=32 -DIC_TRIP=32 -DROW_TRIP=107 -DCOL_TRIP=106 -DFOLD=8"
add_files src/yolo_conv.cpp
add_files -tb tb/layer_output_sdk.dat
add_files -tb tb/weight_file.h
add_files -tb tb/yolo_conv_tb.cpp
add_files -tb tb/yolo_conv_tb
add_files -tb tb/Makefile
add_files -tb tb/layer_input.dat
add_files -tb tb/layer_output_hls.dat
add_files -tb tb/error.log
open_solution "solution1"
set_part {xc7z020-clg400-1} -tool vivado
create_clock -period 7.5 -name default
config_export -format ip_catalog -rtl verilog
csynth_design
exit
