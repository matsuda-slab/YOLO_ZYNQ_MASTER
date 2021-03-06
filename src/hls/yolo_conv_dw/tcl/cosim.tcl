# Include variables
source tcl/names.tcl

# Project setting
open_project $PROJECT_NAME
set_top $TOP_MODULE

# File setting
add_files $SRC_FILES
add_files -tb $TB_FILES

# Other setting
open_solution $SOLUTION
set_part {xc7z020clg400-1} -tool vivado
create_clock -period $CLOCK_PIRIOD -name default
config_export -format ip_catalog -rtl verilog

# Cosimulation & Export IP
cosim_design
#cosim_design -setup -trace_level all -compiled_library_dir /home/cad/xilinx-vivado-2018.3/libs/Xcelium_18.09.001 -tool xcelium
export_design -rtl verilog -format ip_catalog
exit
