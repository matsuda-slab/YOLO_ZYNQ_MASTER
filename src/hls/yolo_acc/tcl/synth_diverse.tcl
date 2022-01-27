set MACROS {"-DROW_TRIP=416 -DCOL_TRIP=416 -DFOLD=4"
            "-DROW_TRIP=208 -DCOL_TRIP=208 -DFOLD=8"
            "-DROW_TRIP=104 -DCOL_TRIP=104 -DFOLD=8"
            "-DROW_TRIP=52 -DCOL_TRIP=52 -DFOLD=8"
            "-DROW_TRIP=26 -DCOL_TRIP=26 -DFOLD=8"
            "-DROW_TRIP=13 -DCOL_TRIP=13 -DFOLD=8"}

set SOLUTIONS {"group0" "group1" "group2" "group3" "group4_12-13" "group6-11"}

open_project -reset yolo_acc_prj
set_top yolo_acc_top
add_files src/yolo_fp.h
add_files src/yolo_acc.h
add_files src/yolo_stream.h
add_files -tb tb/layer_output_sdk.dat
add_files -tb tb/weight_file.h
add_files -tb tb/yolo_acc_tb.cpp
#add_files -tb tb/yolo_acc_tb
#add_files -tb tb/Makefile
add_files -tb tb/layer_input.dat
add_files -tb tb/layer_output_hls.dat
add_files -tb tb/error.log

foreach macro $MACROS sol $SOLUTIONS {
  add_files src/yolo_acc.cpp -cflags $macro
  open_solution $sol
  set_part {xc7z020-clg400-1} -tool vivado
  create_clock -period 10 -name default
  config_export -format ip_catalog -rtl verilog
  csynth_design
}
exit

