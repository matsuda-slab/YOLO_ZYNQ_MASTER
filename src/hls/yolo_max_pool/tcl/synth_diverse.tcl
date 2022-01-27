set MACROS {"-DROW_TRIP=416 -DCOL_TRIP=416 -DFOLD=4 -DSTRIDE_TRIP=2"
            "-DROW_TRIP=208 -DCOL_TRIP=208 -DFOLD=8 -DSTRIDE_TRIP=2"
            "-DROW_TRIP=104 -DCOL_TRIP=104 -DFOLD=8 -DSTRIDE_TRIP=2"
            "-DROW_TRIP=52 -DCOL_TRIP=52 -DFOLD=8 -DSTRIDE_TRIP=2"
            "-DROW_TRIP=26 -DCOL_TRIP=26 -DFOLD=8 -DSTRIDE_TRIP=2"
            "-DROW_TRIP=14 -DCOL_TRIP=14 -DFOLD=8 -DSTRIDE_TRIP=1"}

set SOLUTIONS {"group0" "group1" "group2" "group3" "group5" "group6"}

open_project -reset yolo_max_pool_prj
set_top yolo_max_pool_top
add_files src/yolo_fp.h
add_files src/yolo_max_pool.h
add_files src/yolo_stream.h
add_files -tb tb/layer_input.dat
add_files -tb tb/yolo_max_pool_tb.cpp
add_files -tb tb/layer_output_sdk.dat

foreach macro $MACROS sol $SOLUTIONS {
  add_files src/yolo_max_pool.cpp -cflags $macro
  open_solution $sol
  set_part {xc7z020-clg400-1} -tool vivado
  create_clock -period 10 -name default
  config_export -format ip_catalog -rtl verilog
  csynth_design
}
exit
