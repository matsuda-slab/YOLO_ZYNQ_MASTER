set MACROS {"-DOC_TRIP=16 -DIC_TRIP=3 -DROW_TRIP=419 -DCOL_TRIP=418 -DFOLD=1"
            "-DOC_TRIP=32 -DIC_TRIP=16 -DROW_TRIP=211 -DCOL_TRIP=210 -DFOLD=4"
            "-DOC_TRIP=32 -DIC_TRIP=32 -DROW_TRIP=107 -DCOL_TRIP=106 -DFOLD=8"
            "-DOC_TRIP=32 -DIC_TRIP=32 -DROW_TRIP=55 -DCOL_TRIP=54 -DFOLD=8"
            "-DOC_TRIP=32 -DIC_TRIP=32 -DROW_TRIP=29 -DCOL_TRIP=28 -DFOLD=8"
            "-DOC_TRIP=32 -DIC_TRIP=32 -DROW_TRIP=16 -DCOL_TRIP=15 -DFOLD=8"}

set SOLUTIONS {"group0" "group1" "group2" "group3" "group4_12-13" "group6-11"}

open_project -reset yolo_conv_prj
set_top yolo_conv_top
add_files src/yolo_fp.h
add_files src/yolo_conv.h
add_files src/yolo_stream.h
#add_files src/yolo_conv.cpp -cflags "-D OC_TRIP=32 -DIC_TRIP=32 -DROW_TRIP=211 -DCOL_TRIP=210 -DFOLD=4"
add_files -tb tb/layer_output_sdk.dat
add_files -tb tb/weight_file.h
add_files -tb tb/yolo_conv_tb.cpp
add_files -tb tb/yolo_conv_tb
add_files -tb tb/Makefile
add_files -tb tb/layer_input.dat
add_files -tb tb/layer_output_hls.dat
add_files -tb tb/error.log
#open_solution "solution1"
#set_part {xc7z020-clg400-1} -tool vivado
#create_clock -period 7.5 -name default
#config_export -format ip_catalog -rtl verilog
#csynth_design

foreach macro $MACROS sol $SOLUTIONS {
  add_files src/yolo_conv.cpp -cflags $macro
  open_solution $sol
  set_part {xc7z020-clg400-1} -tool vivado
  create_clock -period 7.5 -name default
  config_export -format ip_catalog -rtl verilog
  csynth_design
}
exit
