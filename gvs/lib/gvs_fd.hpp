#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <string>

#ifndef GVS_FD
#define GVS_FD

#define THETA_MIN_FD 0
#define THETA_MAX_FD 180

using namespace cv;
using namespace std;

float get_mode_float(vector<float> input);

#endif