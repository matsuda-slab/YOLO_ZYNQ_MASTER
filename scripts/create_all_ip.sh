#!/bin/bash

cd "$(dirname "$0")"

ROOT=..
HLS_SRC=$ROOT/src/hls
IP_PATH=$ROOT/ip

TARGET_IP=yolo_conv_dw
TARGET_DIR=$HLS_SRC/$TARGET_IP
if [ ! -e $TARGET_DIR/tb/include/tb_sample.h ]; then
  python $TARGET_DIR/tb/python_sim/sample_gen.py
  cp $TARGET_DIR/tb/python_sim/tb_sample.h $TARGET_DIR/tb/include
fi
make -C $TARGET_DIR
mkdir -p $IP_PATH/xilinx_com_hls_${TARGET_IP}_top_1_0
cp $TARGET_DIR/${TARGET_IP}_prj/solution1/impl/ip/xilinx_com_hls_${TARGET_IP}_top_1_0.zip $IP_PATH

TARGET_IP=yolo_conv_pw
TARGET_DIR=$HLS_SRC/$TARGET_IP
if [ ! -e $TARGET_DIR/tb/include/conv_tb.h ]; then
  python $TARGET_DIR/tb/python_sim/conv.py
  cp $TARGET_DIR/tb/python_sim/head/conv_tb.h $TARGET_DIR/tb/include
fi
make -C $TARGET_DIR
mkdir -p $IP_PATH/xilinx_com_hls_${TARGET_IP}_top_1_0
cp $TARGET_DIR/${TARGET_IP}_prj/solution1/impl/ip/xilinx_com_hls_${TARGET_IP}_top_1_0.zip $IP_PATH

TARGET_IP=yolo_acc
TARGET_DIR=$HLS_SRC/$TARGET_IP
if [ ! -e $TARGET_DIR/tb/include/tb_sample.h ]; then
  python $TARGET_DIR/tb/python_sim/sample_gen.py
  cp $TARGET_DIR/tb/python_sim/tb_sample.h $TARGET_DIR/tb
fi
make -C $TARGET_DIR
mkdir -p $IP_PATH/xilinx_com_hls_${TARGET_IP}_top_1_0
cp $TARGET_DIR/${TARGET_IP}_prj/solution1/impl/ip/xilinx_com_hls_${TARGET_IP}_top_1_0.zip $IP_PATH

for tgt in yolo_max_pool yolo_upsamp yolo_yolo
do
  TARGET_DIR=$HLS_SRC/$tgt
  make -C $TARGET_DIR
  cp $TARGET_DIR/${tgt}_prj/solution1/impl/ip/xilinx_com_hls_${tgt}_top_1_0.zip $IP_PATH
done

