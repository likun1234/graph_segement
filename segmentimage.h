#ifndef SEGMENT_IMAGE  
#define SEGMENT_IMAGE  
typedef unsigned char uchar;  
#include "opencv2/opencv.hpp"  
#include "misc.h"  
#include<vector>
#include<iostream>
#include <cstdlib>
using namespace std;
using namespace cv;  
    //typedef struct {  
    //  uchar r, g, b;   
    //} rgb;  
      
rgb random_rgb();  
      
    //static inline float diff(std::vector<Mat> &rgbs int x1, int y1, int x2, int y2)  
    //{  
    //  return norm(rgbs)  
    //}  

// vector<Point2f> &vp2f
Mat segment_image(Mat &img, float sigma, float c, int min_size, int *num_ccs, vector<Point2f> &vp2f);
      
void picture_pro(Mat &mat,int k);

      
#endif  
