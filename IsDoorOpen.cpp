#include <opencv2/opencv.hpp>
#include "motors.h"
#include <stdio.h>
#include <algorithm> 

//For sleeping
#include <unistd.h>

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

int main(int argc, char** argv){

  if( argc < 3){
    printf("usage: IsDoorOpen <serial port> <serial baud>\n");
    return 1;
  }


  fpos_t pos;
  fgetpos(stdout, &pos);  // save the position in the file stream
  int fd = dup(fileno(stdout));
  freopen("/dev/null", "w", stdout);

  int i, frame;
  Motors motors;
  Mat  depth;
  Point p[4];
  char ch;
  bool door_open;

  //                 v --- num points per frame sampled
  unsigned char vals[4];
  unsigned char max;

  //                     v --- num frames sampled
  unsigned char avg_vals[6];

  motors.init_serial(argc, argv);
  sleep(2);
  motors.init_motors(20, 30);

  VideoCapture cam(CAP_OPENNI2_ASUS);


  //Set up the points in the door to measure
  p[0] = Point(220, 270);
  p[1] = Point(170, 270);
  p[2] = Point(170, 430);
  p[3] = Point(130, 430);


  for(frame=0; frame<6; frame++){
    cam.grab();
    cam.retrieve( depth, CV_CAP_OPENNI_DEPTH_MAP );


    double min, max;
    minMaxIdx(depth, &min, &max);
    depth.convertTo(depth, CV_8UC1, 255./(max-min), -min);

    measure_points(depth, vals, p, 4);

    avg_vals[frame] = avg_val(vals, 4);
    usleep(40000);
   }

  max = max_val(avg_vals, 6);

  char txt[50];
  sprintf(txt, "Sampled 4 points over 5 imgs avg(max(points)):%d", max);
  putText(depth, txt, Point(20, 20), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(0,0,0), 1, CV_AA );


  if(max>175){
    door_open = true;
    sprintf(txt, "Door is open");
    putText(depth, txt, Point(20, 40), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(0,0,0), 1, CV_AA );
  }
  else{
    door_open = false;
    sprintf(txt, "Door is closed");
    putText(depth, txt, Point(20, 40), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(0,0,0), 1, CV_AA );
  }



  if(argc > 3 && strcmp(argv[3], "--show") == 0){
    imshow("Door", depth);
    waitKey(0);
  }

  fflush(stdout);   
  dup2(fd, fileno(stdout));  // restore the stdout
  close(fd);
  clearerr(stdout);  

  fsetpos(stdout, &pos); // move to the correct position

  printf("%d\n", door_open);
}
