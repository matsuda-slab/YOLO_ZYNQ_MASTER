# Include variables
source tcl/names.tcl

# Project setting
open_project -reset $PROJECT_NAME
set_top $TOP_MODULE

# File setting
add_files $SRC_FILES
add_files -tb $TB_FILES

# Other setting
open_solution $SOLUTION
set_part {xc7z020clg400-1} -tool vivado
create_clock -period $CLOCK_PIRIOD -name default
config_export -format ip_catalog -rtl verilog

#open_solution "solution1"
#set_part {xc7z020-clg400-1} -tool vivado
#create_clock -period 7.5 -name default
#config_export -format ip_catalog -rtl verilog

csim_design
csynth_design
cosim_design -setup -trace_level all -compiled_library_dir "/home/cad/xilinx-vivado-2018.3/libs/Xcelium_18.09.001" -tool xcelium
export_design -rtl verilog -format ip_catalog
exit
