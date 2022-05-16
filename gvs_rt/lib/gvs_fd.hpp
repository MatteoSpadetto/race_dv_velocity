#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <string>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <numeric>
#include <vector>

#ifndef GVS_FD
#define GVS_FD

#define WINDOW_SIZE 5
#define THETA_MIN_FD 0
#define THETA_MAX_FD 180
#define C_AREA 30
#define THRESH 100

using namespace cv;
using namespace std;

float get_mode_float(vector<float> input, float lim);                          // Get vector mode
float mean(vector<float> in_vec, int start_frame, int end_frame);              // Get vector mean
float stdev(vector<float> in_vec, float mean, int start_frame, int end_frame); // Get vector standard deviation

#endif