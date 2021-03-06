# Include variables
source tcl/names.tcl

# Project setting
open_project $PROJECT_NAME
set_top $TOP_MODULE

# File setting
add_files $SRC_FILES
add_files src/yolo_conv_dw.cpp -cflags "-DOC_TRIP=32 -DROW_TRIP=107 -DCOL_TRIP=106 -DFOLD=8"
add_files -tb $TB_FILES

# Other setting
open_solution $SOLUTION
set_part {xc7z020clg400-1} -tool vivado
create_clock -period $CLOCK_PIRIOD -name default
config_export -format ip_catalog -rtl verilog

# Synthesis
csynth_design
exit
