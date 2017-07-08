#ifndef _CVMATH_H
#define _CVMATH_H


#include <opencv2/opencv.hpp>

using namespace cv;

void measure_points
(Mat &im, unsigned char* rv, Point* points, int num_points){

  int i;

  for(i=0; i<num_points; i++){
    rv[i] = im.at<uchar>(points[i]);
  }
}

int avg_val(unsigned char* vals, int num_vals){
  short i;
  int avg = 0;

  for(i=0; i<num_vals; i++){
    avg += vals[i];
  }
  avg/=num_vals;

  return avg;
}

unsigned char max_val(unsigned char* vals, int num_vals){
  int i;
  unsigned char rv;

  for(i=0; i<num_vals-1; i++){
    rv = max(vals[i], vals[i+1]);
  }

  return rv;
}

#endif
