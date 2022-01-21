#include <stdio.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdio.h>

typedef struct {
  int w;
  int h;
  int c;
  float *data;
} image;

image make_empty_image(int w, int h, int c)
{
  image out;
  out.data = 0;
  out.h = h;
  out.w = w;
  out.c = c;
  return out;
}

image make_image(int w, int h, int c)
{
  image out = make_empty_image(w,h,c);
  out.data = (float*)calloc(h*w*c, sizeof(float));
  return out;
}

image load_image_stb(char *filename, int channels)
{
  int w, h, c;
  //FILE *fp;
  //fp = fopen("input_image.h","w");

  //fprintf(fp,"#ifndef INPUT_IMAGE_H\n");
  // fprintf(fp,"#define INPUT_IMAGE_H\n\n");




  unsigned char *data = stbi_load(filename, &w, &h, &c, channels);
  if (!data) {
    fprintf(stderr, "Cannot load image \"%s\"\nSTB Reason: %s\n", filename, stbi_failure_reason());
    exit(0);
  }
  if(channels) c = channels;
  int i,j,k;
  image im = make_image(w, h, c);
  //fprintf(fp,"int InputImage_w=%d;\n",w);
  //fprintf(fp,"int InputImage_h=%d;\n",h);  
  //fprintf(fp,"int InputImage_c=%d;\n",c);     
  //fprintf(fp,"float InputImage[]={");
  for(k = 0; k < c; ++k){
    for(j = 0; j < h; ++j){
      for(i = 0; i < w; ++i){
        int dst_index = i + w*j + w*h*k;
        int src_index = k + c*i + c*w*j;
        im.data[dst_index] = (float)data[src_index]/255.;
        //if(dst_index==w-1 + w*(h-1) + w*h*(c-1))
        //    fprintf(fp,"%f};\n",im.data[dst_index]);
        //else
        //    fprintf(fp,"%f,\n", im.data[dst_index]);

      }
    }
  }
  free(data);
  //fprintf(fp,"#endif\n");
  //fclose(fp);
  return im;
}


static float get_pixel(image m, int x, int y, int c)
{
  //assert(x < m.w && y < m.h && c < m.c);
  return m.data[c*m.h*m.w + y*m.w + x];
}
static void set_pixel(image m, int x, int y, int c, float val)
{
  if (x < 0 || y < 0 || c < 0 || x >= m.w || y >= m.h || c >= m.c) return;
  //assert(x < m.w && y < m.h && c < m.c);
  m.data[c*m.h*m.w + y*m.w + x] = val;
}
static void add_pixel(image m, int x, int y, int c, float val)
{
  //assert(x < m.w && y < m.h && c < m.c);
  m.data[c*m.h*m.w + y*m.w + x] += val;
}

void fill_image(image m, float s)
{
  int i;
  for(i = 0; i < m.h*m.w*m.c; ++i) m.data[i] = s;
}

void free_image(image m)
{
  if(m.data){
    free(m.data);
  }
}

image resize_image(image im, int w, int h)
{
  image resized = make_image(w, h, im.c);   
  image part = make_image(w, im.h, im.c);
  int r, c, k;
  float w_scale = (float)(im.w - 1) / (w - 1);
  float h_scale = (float)(im.h - 1) / (h - 1);
  for(k = 0; k < im.c; ++k){
    for(r = 0; r < im.h; ++r){
      for(c = 0; c < w; ++c){
        float val = 0;
        if(c == w-1 || im.w == 1){
          val = get_pixel(im, im.w-1, r, k);
        } else {
          float sx = c*w_scale;
          int ix = (int) sx;
          float dx = sx - ix;
          val = (1 - dx) * get_pixel(im, ix, r, k) + dx * get_pixel(im, ix+1, r, k);
        }
        set_pixel(part, c, r, k, val);
      }
    }
  }
  for(k = 0; k < im.c; ++k){
    for(r = 0; r < h; ++r){
      float sy = r*h_scale;
      int iy = (int) sy;
      float dy = sy - iy;
      for(c = 0; c < w; ++c){
        float val = (1-dy) * get_pixel(part, c, iy, k);
        set_pixel(resized, c, r, k, val);
      }
      if(r == h-1 || im.h == 1) continue;
      for(c = 0; c < w; ++c){
        float val = dy * get_pixel(part, c, iy+1, k);
        add_pixel(resized, c, r, k, val);
      }
    }
  }

  free_image(part);
  return resized;
}

void embed_image(image source, image dest, int dx, int dy)
{
  int x,y,k;
  for(k = 0; k < source.c; ++k){
    for(y = 0; y < source.h; ++y){
      for(x = 0; x < source.w; ++x){
        float val = get_pixel(source, x,y,k);
        set_pixel(dest, dx+x, dy+y, k, val);
      }
    }
  }
}

image letterbox_image(image im, int w, int h)
{
  int new_w = im.w;
  int new_h = im.h;
  if (((float)w/im.w) < ((float)h/im.h)) {
    new_w = w;
    new_h = (im.h * w)/im.w;
  } else {
    new_h = h;
    new_w = (im.w * h)/im.h;
  }
  image resized = resize_image(im, new_w, new_h);
  image boxed = make_image(w, h, im.c);
  fill_image(boxed, .5);
  //int i;
  //for(i = 0; i < boxed.w*boxed.h*boxed.c; ++i) boxed.data[i] = 0;
  embed_image(resized, boxed, (w-new_w)/2, (h-new_h)/2); 
  free_image(resized);
  return boxed;
}


void img_convert(short* res_img, bgr_t* orig_img, int orig_w, int orig_h){
  // output size
  const int tgt_size = 416;
  // ---
  XTime end, start;
  XTime_GetTime(&start);
  // calculate scale
  int   i, j, tgt_w, tgt_h, tgt_i, tgt_n;
  int   pad_w, pad_h;
  float scale;
  if(orig_w > orig_h){
    scale = (float)orig_w / tgt_size;
    tgt_w = tgt_size;
    tgt_h = orig_h / scale;
  }
  else{
    scale = (float)orig_h / tgt_size;
    tgt_w = orig_w / scale;
    tgt_h = tgt_size;
  }
  pad_w = (tgt_size - tgt_w) / 2;  
  pad_h = (tgt_size - tgt_h) / 2;
  // | (memo, where orig_w > orig_h)
  // | tgt_w = tgt_size, pad_w = 0
  // draws top black bar
  for(i = 0; i < pad_h * tgt_size; i++){
    *(res_img  ) = 0;
    *(res_img+1) = 0;
    *(res_img+2) = 0;
    res_img += 4;
  }
  // nearest neighbor resampling
  for(i = 0; i < tgt_h; i++){
    tgt_i = (int)(i * scale) * orig_w;
    for(j = 0; j < pad_w; j++){
      *(res_img  ) = 0;
      *(res_img+1) = 0;
      *(res_img+2) = 0;
      res_img += 4;
    }
    for(j = 0; j < tgt_w; j++){
      tgt_n = tgt_i + (int)(j * scale);
      *(res_img  ) = orig_img[tgt_n].g;
      *(res_img+1) = orig_img[tgt_n].b;
      *(res_img+2) = orig_img[tgt_n].r;
      res_img += 4;
    }
    for(j = 0; j < tgt_size - tgt_w - pad_w; j++){
      *(res_img  ) = 0;
      *(res_img+1) = 0;
      *(res_img+2) = 0;
      res_img += 4;
    }
  }
  // draws bottom black bar
  for(i = 0; i < (tgt_size - tgt_h - pad_h) * tgt_size; i++){
    *(res_img  ) = 0;
    *(res_img+1) = 0;
    *(res_img+2) = 0;
    res_img += 4;
  }
  // ---
  XTime_GetTime(&end);
  int time_used = 1.0*((end-start)*1000000)/(COUNTS_PER_SECOND);
  xil_printf("img_convert : %d(us)\r\n", time_used);
}


void img_convert_orig(short* input_img_fix, bgr_t* orig_img, int orig_w, int orig_h)
{
  XTime end, start;
  XTime_GetTime(&start);
  int k, j, i;
  //現在:jpg画像->配列
  //zybo移植:temp_input配列にpcamからの数値を入れる
  //R0,...,Rn,G0,...,Gn,B0,...,Bn:左上の画素から順に
  //image im = load_image_stb(argv[1],3);
  /* im.data -> pcamの入力画像 (rgb_t orig_img) */
  //int temp_input[orig_h * orig_w * 3];
  //for(k = 0; k < orig_h * orig_w * 3; k = k + 3){
  //  temp_input[k]     = (int)(orig_img[k/3].r * 255);//here pcam input
  //  temp_input[k + 1] = (int)(orig_img[k/3].g * 255);//here pcam input
  //  temp_input[k + 2] = (int)(orig_img[k/3].b * 255);//here pcam input
  //}
  //zybo pcam resize
  image im_pcam = make_image(orig_w, orig_h, 3);  //w,h,c //pcamの場合の引数 1280,720,3
  //xil_printf("end make_image\r\n");
  for(k = 0; k < orig_h * orig_w; ++k){
    im_pcam.data[k]                   = (float)(orig_img[k].g / 255.0);//1に正規化
    im_pcam.data[k + orig_h*orig_w]   = (float)(orig_img[k].b / 255.0);//1に正規化
    im_pcam.data[k + orig_h*orig_w*2] = (float)(orig_img[k].r / 255.0);//1に正規化
  }
  image sized = letterbox_image(im_pcam, 416, 416);//画像をリサイズ
  //xil_printf("end letterbox_image\r\n");
  //入力画像をインタリーブ //入力は64bit幅なので1チャネル増やす
  //3channel:R0,...,R415,G0,...,G415,B0,...,B415
  //4channel:R0,G0,B0,0,...,R415,G415,B415,0
  short input_img[3][416][416];
  for(k = 0; k < 3; ++k){
    for(j = 0; j < 416; ++j){
      for(i = 0; i < 416; ++i){
        int dst_index = i + 416*j + 416*416*k;
        input_img[k][j][i] = (short)(sized.data[dst_index] * 256);//float to short
      }
    }
  }

  if (sized.data) {
    free_image(sized);
  }
  if (im_pcam.data) {
    free_image(im_pcam);
  }

  //short* input_img_fix = (short*)calloc(416 * 416 * 4, sizeof(short));
  //for(k = 0; k < 4; ++k){
  //  for(j = 0; j < 416; ++j){
  //    for(i = 0; i < 416; ++i){
  //      input_img_fix[k*416*416 + j*416 + i] = 0;//zero initial
  //    }
  //  }
  //}
  for(j = 0; j < 416; j++) {
    for(i = 0; i < 416 * 4; i = i + 4) {
      input_img_fix[j*416*4 + i]     = input_img[0][j][i>>2];
      input_img_fix[j*416*4 + i + 1] = input_img[1][j][i>>2];
      input_img_fix[j*416*4 + i + 2] = input_img[2][j][i>>2];
    }
  }
  //for(k = 0; k < 3; ++k){
  //  for(j = 0; j < 416; ++j){
  //    for(i = 0; i < 416; ++i){
  //      if(k == 0){
  //        input_img_fix[j*416+i+0] = input_img[k][j][i];
  //      }
  //      if(k == 1){
  //        input_img_fix[j*416+i+1] = input_img[k][j][i];
  //      }
  //      if(k == 2){
  //        input_img_fix[j*416+i+2] = input_img[k][j][i];
  //      }
  //    }
  //  }
  //}
  //結果をヘッダファイルに出力
  //FILE *fp_pcam;
  //fp_pcam = fopen("input_image_pcam.h","w");
  //fprintf(fp_pcam,"#ifndef LAYER_INPUT_H\n");
  //fprintf(fp_pcam,"#define LAYER_INPUT_H\n\n");
  //fprintf(fp_pcam,"short layer_input[]={");
  //for(j = 0; j < 416; ++j){
  //  for(i = 0; i < 416; ++i){
  //    for(k = 0; k < 4; ++k){
  //      if(k==3&&j==416-1&&i==416-1){
  //        fprintf(fp_pcam,"%d};\n",input_img_fix[j][i][k]);
  //      }
  //      else{
  //        fprintf(fp_pcam,"%d,\n",input_img_fix[j][i][k]);
  //      }
  //    }}}
  //fprintf(fp_pcam,"#endif\n");
  //fclose(fp_pcam);
  free(input_img);
  XTime_GetTime(&end);
  int time_used = 1.0*((end-start)*1000000)/(COUNTS_PER_SECOND);
  xil_printf("img_convert : %d(us)\r\n", time_used);
  //xil_printf("img_convert : %lf(us)\r\n", 1.0*(end-start));
  //return input_img_fix;
}
