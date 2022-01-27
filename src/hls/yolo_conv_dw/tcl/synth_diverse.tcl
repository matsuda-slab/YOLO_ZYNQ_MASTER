set MACROS {"-DOC_TRIP=3 -DROW_TRIP=419 -DCOL_TRIP=418 -DFOLD=1"
            "-DOC_TRIP=16 -DROW_TRIP=211 -DCOL_TRIP=210 -DFOLD=4"
            "-DOC_TRIP=32 -DROW_TRIP=107 -DCOL_TRIP=106 -DFOLD=8"
            "-DOC_TRIP=32 -DROW_TRIP=55 -DCOL_TRIP=54 -DFOLD=8"
            "-DOC_TRIP=32 -DROW_TRIP=29 -DCOL_TRIP=28 -DFOLD=8"
            "-DOC_TRIP=32 -DROW_TRIP=16 -DCOL_TRIP=15 -DFOLD=8"}

set SOLUTIONS {"group0" "group1" "group2" "group3" "group4_12-13" "group6-7_9"}

# Include variables
source tcl/names.tcl

# Project setting
open_project $PROJECT_NAME
set_top $TOP_MODULE

# File setting
add_files src/yolo_conv_dw.h
add_files src/yolo_fp.h
add_files src/yolo_stream.h
add_files -tb $TB_FILES

foreach macro $MACROS sol $SOLUTIONS {
  add_files src/yolo_conv_dw.cpp -cflags $macro
  open_solution $sol
  set_part {xc7z020-clg400-1} -tool vivado
  create_clock -period 7.5 -name default
  config_export -format ip_catalog -rtl verilog
  csynth_design
}
exit
