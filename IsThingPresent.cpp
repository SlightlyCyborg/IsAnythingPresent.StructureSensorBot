#include <opencv2/opencv.hpp>
#include "motors.h"
#include <stdio.h>
#include <algorithm> 
#include <boost/program_options.hpp>
//For sleeping
#include <unistd.h>
#include <string>

//My math functions
#include "cv_math.h"

using namespace cv;
namespace po = boost::program_options;


int main(int argc, char** argv){

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "produce help message")

    ("threshold,t", po::value<int>(), "set threshold 0-255 of existence")

    ("serial_port,s", po::value<std::string>()->default_value("/dev/ttyACM0"), 
     "port arduino is on")

    ("baud,b", po::value<int>()->default_value(9600),
     "baud rate arduino is communicating on")

    ("xpos,x", po::value<int>(), "x position to move to")

    ("ypos,y", po::value<int>(), "y position to move to")

    ("points,p", po::value<int>(), "points to measure")
    ;

  po::variables_map vm;
  po::store(parse_command_line(argc, argv, desc), vm);


  if( argc < 3 || vm.count("help")){
    printf("usage: IsThingPresent <serial port> <serial baud>\n");
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
  bool thing_present;

  //                 v --- num points per frame sampled
  unsigned char vals[4];
  unsigned char max;

  //                     v --- num frames sampled
  unsigned char avg_vals[6];

  motors.init_serial(vm["serial_port"].as<std::string>(),
		     vm["baud"].as<int>());

  sleep(2);
  motors.init_motors(vm["xpos"].as<int>(), vm["ypos"].as<int>());

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


  if(max<vm["threshold"].as<int>()){
    thing_present = true;
    sprintf(txt, "Thing is present");
    putText(depth, txt, Point(20, 40), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(0,0,0), 1, CV_AA );
  }
  else{
    thing_present = false;
    sprintf(txt, "Thing is absent");
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

  printf("%d\n", thing_present);
}
