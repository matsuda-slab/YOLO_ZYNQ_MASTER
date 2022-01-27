open_project yolo_yolo_prj
set_top yolo_yolo_top
add_files src/yolo_stream.h
add_files src/yolo_yolo.cpp
add_files src/yolo_fp.h
add_files src/yolo_yolo.h
add_files -tb tb/yolo_yolo_tb.cpp
add_files -tb tb/layer_input.dat
add_files -tb tb/layer_output_sdk.dat
open_solution "solution1"
set_part {xc7z020-clg400-1} -tool vivado
create_clock -period 10 -name default
config_export -format ip_catalog -rtl verilog
csynth_design
exit

