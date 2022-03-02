open_project -reset yolo_acc_prj
set_top yolo_acc_top
add_files src/yolo_fp.h
add_files src/yolo_acc.cpp -cflags "-DROW_TRIP=416 -DCOL_TRIP=416 -DFOLD=1"
add_files src/yolo_acc.h
add_files src/yolo_stream.h
add_files -tb tb/layer_input.dat
add_files -tb tb/yolo_acc_tb.cpp
add_files -tb tb/layer_output_sdk.dat
add_files -tb tb/weight_file.h
open_solution "solution1"
set_part {xc7z020-clg400-1} -tool vivado
create_clock -period 10 -name default
config_export -format ip_catalog -rtl verilog
csim_design
exit
