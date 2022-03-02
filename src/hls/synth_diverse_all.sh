#!/bin/bash

cd yolo_conv/
vivado_hls -f tcl/synth_diverse.tcl

cd ../yolo_acc/
vivado_hls -f tcl/synth_diverse.tcl

cd ../yolo_max_pool/
vivado_hls -f tcl/synth_diverse.tcl

cd yolo_conv_dw
vivado_hls -f tcl/synth_diverse.tcl

cd yolo_conv_pw
vivado_hls -f tcl/synth_diverse.tcl
