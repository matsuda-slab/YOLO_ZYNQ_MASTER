set PROJECT_NAME yolo_conv_pw_prj
set TOP_MODULE yolo_conv_pw_top
set SOLUTION "solution1"
set PART_NAME xc7z020clg400-1
set CLOCK_PIRIOD 7.5

set SRC_FILES {
  src/yolo_fp.h
  src/yolo_stream.h
  src/yolo_conv_pw.h
}

set TB_FILES {
  tb/include/conv_tb.h
  tb/yolo_conv_pw_tb.cpp
}
