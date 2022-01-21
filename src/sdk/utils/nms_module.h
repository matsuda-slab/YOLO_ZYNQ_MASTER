#ifndef _NMS_MODULE
#define _NMS_MODULE
//参考
//"https://blog.csdn.net/random_repick/article/details/106495272"

#include <vector>
#include <list>

using namespace std;

typedef struct bbox
{
  int c;         // 物体classID
  float p;       // 物体存在確率
  // 左上座標
  int x;
  int y;
  // 右下座標
  int xx;
  int yy;
} box;

class Cmpare  
{  
 public:  
  bool operator()(const box a,const box b) const {
    return a.p > b.p;  
  }
};  

float IOU(box &a, box &b) {
  int x1 = a.x, y1 = a.y, xx1 = a.xx, yy1 = a.yy;
  int x2 = b.x, y2 = b.y, xx2 = b.xx, yy2 = b.yy;
  if(b.x > a.x) {
    x1 = b.x;
    x2 = a.x;
  }
  if(b.y > a.y) {
    y1 = b.y;
    y2 = a.y;
  }
  if(b.xx < a.xx) {
    xx1 = b.xx;
    xx2 = a.xx;
  }
  if(b.yy < a.yy) {
    yy1 = b.yy;
    yy2 = a.yy;
  }
  if(x1 >= xx1 || y1 >= yy1) return 0;
  int area1 = (xx1 - x1) * (yy1 - y1);
  int area2 = (xx2 - x2) * (yy2 - y2);
  return 1.0*area1/area2;
}

void NMS(list<box> &bb, float nms_threshold) {
    bb.sort(Cmpare());
    list<box>::iterator ib = bb.begin();
    while(ib != bb.end()) {
      list<box>::iterator it = ib;
      it++;
      while(it != bb.end()) {
	if(IOU(*ib, *it) > nms_threshold) {
	  bb.erase(it++);
	} else {
	  it++;
	}
      }
      ib++;
    } 
}



void nms_process(vector<box> &bb, int cls_num = 80, float obj_thresholod = 0.1, float nms_threshold = 0.5) {
  vector<list<box> > cls(cls_num);
  for(int i = 0; i < bb.size(); ++i) {
    if(bb[i].p > obj_thresholod)
      cls[bb[i].c].push_back(bb[i]);
  }
  bb.clear();
  for(int i = 0; i < cls_num; ++i) {
    if(cls[i].size() != 0) {
      NMS(cls[i],nms_threshold);
      list<box>::iterator ib = cls[i].begin();
      while(ib != cls[i].end()) {
        bb.push_back(*ib);
        ib++;
      }
    }
  }
}
#endif
