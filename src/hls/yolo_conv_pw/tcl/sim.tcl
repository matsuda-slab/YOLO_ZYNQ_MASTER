open_project -reset yolo_conv_pw_prj
set_top yolo_conv_pw_top

add_files src/yolo_conv_pw.cpp
add_files src/yolo_fp.h
add_files src/yolo_stream.h
add_files src/yolo_conv_pw.h

add_files -tb tb/include/conv_tb.h
add_files -tb tb/yolo_conv_pw_tb.cpp
open_solution "solution1"
set_part {xc7z020clg400-1} -tool vivado
create_clock -period 10 -name default
config_export -format ip_catalog -rtl verilog
csim_design
##csynth_design
##cosim_design
##export_design -rtl verilog -format ip_catalog
##
exit
