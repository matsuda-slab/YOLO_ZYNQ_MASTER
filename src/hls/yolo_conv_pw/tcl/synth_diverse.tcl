set MACROS {"-DOC_TRIP=16 -DIC_TRIP=3 -DROW_TRIP=416 -DCOL_TRIP=416 -DFOLD=1"
            "-DOC_TRIP=32 -DIC_TRIP=16 -DROW_TRIP=208 -DCOL_TRIP=208 -DFOLD=4"
            "-DOC_TRIP=32 -DIC_TRIP=32 -DROW_TRIP=104 -DCOL_TRIP=104 -DFOLD=8"
            "-DOC_TRIP=32 -DIC_TRIP=32 -DROW_TRIP=52 -DCOL_TRIP=52 -DFOLD=8"
            "-DOC_TRIP=32 -DIC_TRIP=32 -DROW_TRIP=26 -DCOL_TRIP=26 -DFOLD=8"
            "-DOC_TRIP=32 -DIC_TRIP=32 -DROW_TRIP=13 -DCOL_TRIP=13 -DFOLD=8"}

set SOLUTIONS {"group0" "group1" "group2" "group3" "group4_12-13" "group6-11"}

# Include variables
source tcl/names.tcl

# Project setting
open_project $PROJECT_NAME
set_top $TOP_MODULE

# File setting
add_files src/yolo_conv_pw.h
add_files src/yolo_fp.h
add_files src/yolo_stream.h
add_files -tb $TB_FILES

foreach macro $MACROS sol $SOLUTIONS {
  add_files src/yolo_conv_pw.cpp -cflags $macro
  open_solution $sol
  set_part {xc7z020-clg400-1} -tool vivado
  create_clock -period 7.5 -name default
  config_export -format ip_catalog -rtl verilog
  csynth_design
}
exit
