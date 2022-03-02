# Implementation of YOLOv3-tiny on FPGA

## Procedure
### 1. Prepare Development Environment
This flow requires environment or tools below.
- Python3
  - torch
  - numpy
- Vivado 2019.1
- Vivado HLS 2019.1
So please switch the python environment that torch and numpy are available,
and set PATH to Vivado installed directory.

### 2. Synthesize and create IP
In this directory run `make`
Some minites later, core IPs for YOLO will be created automatically.
These IPs are in 'ip' directory.

### Reference
* https://github.com/Yu-Zhewen/Tiny_YOLO_v3_ZYNQ
