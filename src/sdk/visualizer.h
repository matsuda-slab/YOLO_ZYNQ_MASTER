#ifndef _VIS
#define _VIS
#include <fstream>
#include <iostream>
#include <string>

#include <vector> //nms
#include <list> //nms

#include <cstdlib>      // srand,rand

#include <xtime_l.h>

#include "utils/vis_module.h" 
#include "utils/nms_module.h"

#define IMG_SIZE       416
#define GRID_13        13
#define GRID_26        26
#define CH_256         256
#define CH_255         255
#define GRID_WIDTH_13  416/13
#define GRID_WIDTH_26  416/26
//#define CLASS_NUM      80
#define CLASS_NUM      1
#define ANCHOR_BOX_NUM 3

#define CAR_NO         2

typedef struct point {
  uint32_t x1;
  uint32_t y1;
  uint32_t x2;
  uint32_t y2;
} point_t;

//float fix_to_float(short input);

std::vector<point_t> postprocess(short* input_image, short* yolo_out_0, short* yolo_out_1){

  //short >> float
  float grid13_array[GRID_13 * GRID_13 * CH_256];
  //float grid13_array[GRID_13 * GRID_13 * 18];
  float grid26_array[GRID_26 * GRID_26 * CH_256];
  //float grid26_array[GRID_26 * GRID_26 * 18];
  for(int i = 0; i < GRID_13 * GRID_13 * CH_256/8; i++){
    grid13_array[i] = fix_to_float(yolo_out_0[i]);
  }
  for(int i = 0; i < GRID_26 * GRID_26 * CH_256/8; i++){
    grid26_array[i] = fix_to_float(yolo_out_1[i]);
  }
  //for(int i = 0; i < GRID_13 * GRID_13; i++){
  //  for(int j = 0; j < 3; i++){
  //    grid13_array[i*18 + j*6]     = fix_to_float(yolo_out_0[i*256 + j*85]);     // x
  //    grid13_array[i*18 + j*6 + 1] = fix_to_float(yolo_out_0[i*256 + j*85 + 1]); // y
  //    grid13_array[i*18 + j*6 + 2] = fix_to_float(yolo_out_0[i*256 + j*85 + 2]); // w
  //    grid13_array[i*18 + j*6 + 3] = fix_to_float(yolo_out_0[i*256 + j*85 + 3]); // h
  //    grid13_array[i*18 + j*6 + 4] = fix_to_float(yolo_out_0[i*256 + j*85 + 4]); // conf
  //    grid13_array[i*18 + j*6 + 5] = fix_to_float(yolo_out_0[i*256 + j*85 + 5 + CAR_NO]); // class (car)
  //  }
  //}
  //for(int i = 0; i < GRID_26 * GRID_26; i++){
  //  for(int j = 0; j < 3; i++){
  //    grid26_array[i*18 + j*6]     = fix_to_float(yolo_out_1[i*256 + j*85]);     // x
  //    grid26_array[i*18 + j*6 + 1] = fix_to_float(yolo_out_1[i*256 + j*85 + 1]); // y
  //    grid26_array[i*18 + j*6 + 2] = fix_to_float(yolo_out_1[i*256 + j*85 + 2]); // w
  //    grid26_array[i*18 + j*6 + 3] = fix_to_float(yolo_out_1[i*256 + j*85 + 3]); // h
  //    grid26_array[i*18 + j*6 + 4] = fix_to_float(yolo_out_1[i*256 + j*85 + 4]); // conf
  //    grid26_array[i*18 + j*6 + 5] = fix_to_float(yolo_out_1[i*256 + j*85 + 5 + CAR_NO]); // class (car)
  //  }
  //}


  //channel reorder
  //8*13*13*32 >> 13*13*256
  //8*26*26*32 >> 13*13*256
  float grid13_array_reorder[GRID_13 * GRID_13 * CH_256];
  float grid26_array_reorder[GRID_26 * GRID_26 * CH_256];
  int cnt = 0;
  for(int i = 0; i < GRID_13 * GRID_13; i++){
    for(int j = 0; j < 8;j++){
      for(int k = 0;k < 32;k++){
        grid13_array_reorder[cnt] = grid13_array[(GRID_13 * GRID_13 * 32)*j + 32*i + k];
        cnt++;
      }
    }
  } 
  cnt = 0;
  for(int i = 0;i < GRID_26 * GRID_26; i++){
    for(int j = 0; j < 8; j++){
      for(int k = 0; k < 32; k++){
        grid26_array_reorder[cnt] = grid26_array[(GRID_26 * GRID_26 * 32)*j + 32*i + k];
        cnt++;
      }
    }
  } 


  //channel reshape 256ch >> 255ch
  //13*13*256 >> 13*13*255
  //26*26*256 >> 26*26*255
  float grid13_array_reshape[GRID_13*GRID_13*18];
  float grid26_array_reshape[GRID_26*GRID_26*18];
  cnt = 0;
  for(int i = 0; i < GRID_13*GRID_13*18; i = i + 18){
    grid13_array_reshape[i]      = grid13_array_reorder[(i/18)*256];
    grid13_array_reshape[i + 1]  = grid13_array_reorder[(i/18)*256 + 1];
    grid13_array_reshape[i + 2]  = grid13_array_reorder[(i/18)*256 + 2];
    grid13_array_reshape[i + 3]  = grid13_array_reorder[(i/18)*256 + 3];
    grid13_array_reshape[i + 4]  = grid13_array_reorder[(i/18)*256 + 4];
    grid13_array_reshape[i + 5]  = grid13_array_reorder[(i/18)*256 + 7];     // cls (car)
    grid13_array_reshape[i + 6]  = grid13_array_reorder[(i/18)*256 + 85];
    grid13_array_reshape[i + 7]  = grid13_array_reorder[(i/18)*256 + 86];
    grid13_array_reshape[i + 8]  = grid13_array_reorder[(i/18)*256 + 87];
    grid13_array_reshape[i + 9]  = grid13_array_reorder[(i/18)*256 + 88];
    grid13_array_reshape[i + 10] = grid13_array_reorder[(i/18)*256 + 89];
    grid13_array_reshape[i + 11] = grid13_array_reorder[(i/18)*256 + 92];    // cls (car)
    grid13_array_reshape[i + 12] = grid13_array_reorder[(i/18)*256 + 170];
    grid13_array_reshape[i + 13] = grid13_array_reorder[(i/18)*256 + 171];
    grid13_array_reshape[i + 14] = grid13_array_reorder[(i/18)*256 + 172];
    grid13_array_reshape[i + 15] = grid13_array_reorder[(i/18)*256 + 173];
    grid13_array_reshape[i + 16] = grid13_array_reorder[(i/18)*256 + 174];
    grid13_array_reshape[i + 17] = grid13_array_reorder[(i/18)*256 + 177];   // cls (car)
    cnt++;
  }
  cnt = 0;
  for(int i = 0; i < GRID_26*GRID_26*18; i = i + 18){
    grid26_array_reshape[i]      = grid26_array_reorder[(i/18)*256];
    grid26_array_reshape[i + 1]  = grid26_array_reorder[(i/18)*256 + 1];
    grid26_array_reshape[i + 2]  = grid26_array_reorder[(i/18)*256 + 2];
    grid26_array_reshape[i + 3]  = grid26_array_reorder[(i/18)*256 + 3];
    grid26_array_reshape[i + 4]  = grid26_array_reorder[(i/18)*256 + 4];
    grid26_array_reshape[i + 5]  = grid26_array_reorder[(i/18)*256 + 7];
    grid26_array_reshape[i + 6]  = grid26_array_reorder[(i/18)*256 + 85];
    grid26_array_reshape[i + 7]  = grid26_array_reorder[(i/18)*256 + 86];
    grid26_array_reshape[i + 8]  = grid26_array_reorder[(i/18)*256 + 87];
    grid26_array_reshape[i + 9]  = grid26_array_reorder[(i/18)*256 + 88];
    grid26_array_reshape[i + 10] = grid26_array_reorder[(i/18)*256 + 89];
    grid26_array_reshape[i + 11] = grid26_array_reorder[(i/18)*256 + 92];
    grid26_array_reshape[i + 12] = grid26_array_reorder[(i/18)*256 + 170];
    grid26_array_reshape[i + 13] = grid26_array_reorder[(i/18)*256 + 171];
    grid26_array_reshape[i + 14] = grid26_array_reorder[(i/18)*256 + 172];
    grid26_array_reshape[i + 15] = grid26_array_reorder[(i/18)*256 + 173];
    grid26_array_reshape[i + 16] = grid26_array_reorder[(i/18)*256 + 174];
    grid26_array_reshape[i + 17] = grid26_array_reorder[(i/18)*256 + 177];
    cnt++;
  }


  //(座標x,y) (大きさw,h) (物体確率) (class確率80)
  //2+2+1+80 = 85
  //85 * 3(anchorBOXの数) = 255
  //13*13*255, 26*26*255
  //座標と大きさを計算,確率はそのまま
  int anchor_box_13[3][2] = {{81,82},{135,169},{344,319}};
  int anchor_box_26[3][2] = {{10,14},{23,27},{37,58}};
  int w_cnt = 0;
  int h_cnt = 0;
  for(int i = 0; i < GRID_13*GRID_13*18; i = i + 18){
    grid13_array_reshape[i]      = GRID_WIDTH_13 * w_cnt + GRID_WIDTH_13 * grid13_array_reshape[i];
    grid13_array_reshape[i + 1]  = GRID_WIDTH_13 * h_cnt + GRID_WIDTH_13 * grid13_array_reshape[i + 1];
    grid13_array_reshape[i + 2]  = anchor_box_13[0][0] * std::exp(grid13_array_reshape[i + 2]);
    grid13_array_reshape[i + 3]  = anchor_box_13[0][1] * std::exp(grid13_array_reshape[i + 3]);

    grid13_array_reshape[i + 6]  = GRID_WIDTH_13 * w_cnt + GRID_WIDTH_13 * grid13_array_reshape[i + 6];
    grid13_array_reshape[i + 7]  = GRID_WIDTH_13 * h_cnt + GRID_WIDTH_13 * grid13_array_reshape[i + 7];
    grid13_array_reshape[i + 8]  = anchor_box_13[1][0] * std::exp(grid13_array_reshape[i + 8]);
    grid13_array_reshape[i + 9]  = anchor_box_13[1][1] * std::exp(grid13_array_reshape[i + 9]);

    grid13_array_reshape[i + 12] = GRID_WIDTH_13 * w_cnt + GRID_WIDTH_13 * grid13_array_reshape[i + 12];
    grid13_array_reshape[i + 13] = GRID_WIDTH_13 * h_cnt + GRID_WIDTH_13 * grid13_array_reshape[i + 13];
    grid13_array_reshape[i + 14] = anchor_box_13[2][0] * std::exp(grid13_array_reshape[i + 14]);
    grid13_array_reshape[i + 15] = anchor_box_13[2][1] * std::exp(grid13_array_reshape[i + 15]);

    w_cnt++;
    if(w_cnt == GRID_13){
      w_cnt = 0;
      h_cnt++;
    }
  }
  w_cnt = 0;
  h_cnt = 0;
  for(int i = 0; i < GRID_26*GRID_26*18; i = i + 18){
    grid26_array_reshape[i]      = GRID_WIDTH_26 * w_cnt + GRID_WIDTH_26 * grid26_array_reshape[i];
    grid26_array_reshape[i + 1]  = GRID_WIDTH_26 * h_cnt + GRID_WIDTH_26 * grid26_array_reshape[i + 1];
    grid26_array_reshape[i + 2]  = anchor_box_26[0][0] * std::exp(grid26_array_reshape[i + 2]);
    grid26_array_reshape[i + 3]  = anchor_box_26[0][1] * std::exp(grid26_array_reshape[i + 3]);

    grid26_array_reshape[i + 6]  = GRID_WIDTH_26 * w_cnt + GRID_WIDTH_26 * grid26_array_reshape[i + 6];
    grid26_array_reshape[i + 7]  = GRID_WIDTH_26 * h_cnt + GRID_WIDTH_26 * grid26_array_reshape[i + 7];
    grid26_array_reshape[i + 8]  = anchor_box_26[1][0] * std::exp(grid26_array_reshape[i + 8]);
    grid26_array_reshape[i + 9]  = anchor_box_26[1][1] * std::exp(grid26_array_reshape[i + 9]);

    grid26_array_reshape[i + 12] = GRID_WIDTH_26 * w_cnt + GRID_WIDTH_26 * grid26_array_reshape[i + 12];
    grid26_array_reshape[i + 13] = GRID_WIDTH_26 * h_cnt + GRID_WIDTH_26 * grid26_array_reshape[i + 13];
    grid26_array_reshape[i + 14] = anchor_box_26[2][0] * std::exp(grid26_array_reshape[i + 14]);
    grid26_array_reshape[i + 15] = anchor_box_26[2][1] * std::exp(grid26_array_reshape[i + 15]);

    w_cnt++;
    if(w_cnt == GRID_26){
      w_cnt = 0;
      h_cnt++;
    }
  }

  //combine 13*13detections & 26*26detections
  //13*13*255, 26*26*255 >> (13*13+26*26)*255
  float grid13and26_array[(GRID_13*GRID_13+GRID_26*GRID_26)*18];
  cnt = 0;
  for(int i = 0; i < GRID_13*GRID_13*18; i++){
    grid13and26_array[cnt] = grid13_array_reshape[i];
    cnt++;
  }
  for(int i = 0; i < GRID_26*GRID_26*18; i++){
    grid13and26_array[cnt] = grid26_array_reshape[i];
    cnt++;
  }


  //temp
  /*
     if(1){
     std::ofstream ofs("../zynq_yolo_output.txt");
     for(int i=0;i<(GRID_13*GRID_13+GRID_26*GRID_26)*CH_255;i++){
     ofs << grid13and26_array[i] << std::endl;
     }
     }
     */

  //class確率が最大のものを抽出
  //bbox:中心座標、面積 >> bbox:左上座標、右下座標
  //NMS
  vector<box> nms_boxes;
  for(int i = 0; i < (GRID_13*GRID_13 + GRID_26*GRID_26) * ANCHOR_BOX_NUM; i++){
    box nms_box;
    float cx          = grid13and26_array[i * (18 / ANCHOR_BOX_NUM) + 0];
    float cy          = grid13and26_array[i * (18 / ANCHOR_BOX_NUM) + 1];
    float cw          = grid13and26_array[i * (18 / ANCHOR_BOX_NUM) + 2];
    float ch          = grid13and26_array[i * (18 / ANCHOR_BOX_NUM) + 3];
    float obj_score   = grid13and26_array[i * (18 / ANCHOR_BOX_NUM) + 4];
    float class_score = grid13and26_array[i * (18 / ANCHOR_BOX_NUM) + 5];
    int class_id = 0;
    nms_box.c  = class_id;
    nms_box.p  = obj_score;
    nms_box.x  = (int)(cx - cw / 2);
    nms_box.y  = (int)(cy - ch / 2);
    nms_box.xx = (int)(cx + cw / 2);
    nms_box.yy = (int)(cy + ch / 2);
    nms_boxes.push_back(nms_box);
  }
  //引数1:box型vector、引数2:class数、 引数3:物体確率閾値、引数4:NMS閾値

  XTime tEnd, tStart;
  XTime_GetTime(&tStart);
  nms_process(nms_boxes, CLASS_NUM, 0.3, 0.3);//NMS
  XTime_GetTime(&tEnd);
  int time_used_hw = 1.0*((tEnd-tStart)*1000000)/(COUNTS_PER_SECOND);
  xil_printf("NMS : %d(us)***\n\r", time_used_hw);

  //class読込 BBox色作成
  //std::ifstream cls_read_file("coco.names");
  //std::string cls_read_file_str;
  //std::string cls_name[CLASS_NUM];
  //int cls_color[CLASS_NUM][3];

  //if(cls_read_file.fail()) {
  //  std::cerr << "Failed to open file." << std::endl;
  //  return -1;
  //}
  cnt = 0;
  //while(getline(cls_read_file,cls_read_file_str)) {
  //  cls_color[cnt][0] = make_color(cnt);
  //  cls_color[cnt][1] = make_color(cnt+1);
  //  cls_color[cnt][2] = make_color(cnt+2);
  //  cls_name[cnt] = cls_read_file_str;
  //  cnt++;
  //}  
  //画像生成
  //void rectangle(Mat& img, Point pt1, Point pt2, const Scalar& color, int thickness=1, int lineType=8, int shift=0)
  //void putText(Mat& img, const string& text, Point org, int fontFace, double fontScale, Scalar color, int thickness=1, int lineType=8, bool bottomLeftOrigin=false)
  //std::vector<point_t*> box_list;
  std::vector<point_t> box_list;
  if(1){
    for(int i = 0; i < nms_boxes.size(); i++){
      int cls_id = nms_boxes[i].c;
      //xil_printf("cls_id : %d\r\n", cls_id);
      //if (cls_id == 2) {         // car
      //cv::Point pt1,pt2;
      point_t pt;
      //point_t* pt;
      //xil_printf("pt address : %x\r\n", pt);
      pt.x1 = nms_boxes[i].x;
      pt.y1 = nms_boxes[i].y;
      pt.x2 = nms_boxes[i].xx;
      pt.y2 = nms_boxes[i].yy;
      //cv::rectangle(in_img, pt1, pt2, cv::Scalar(cls_color[cls_id][0], cls_color[cls_id][1], cls_color[cls_id][2]),2);
      //cv::putText(in_img, cls_name[cls_id], pt1, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(cls_color[cls_id][0], cls_color[cls_id][1], cls_color[cls_id][2]), 2);
      box_list.push_back(pt);
      //xil_printf("pt (postprocess) : (%d, %d), (%d, %d)\r\n", pt.x1, pt.y1, pt.x2, pt.y2);
      //} // endif
    }
  }
  //表示
  //if(1){
  //  cv::namedWindow("in_img", cv::WINDOW_AUTOSIZE);
  //  cv::imshow("in_img", in_img);
  //  cv::waitKey(0);
  //  cv::destroyAllWindows();
  //}
  //in_img.release();

  return box_list;
}
#endif
