all:
	g++ -o yolo_conv_tb yolo_conv_tb.cpp ../src/yolo_conv.cpp -I/home/cad/xilinx-vivado-2019.1/Vivado/2019.1/include -g

gdb:
	gdb ./yolo_conv_tb

clean:
	rm -f error.log yolo_conv_tb
