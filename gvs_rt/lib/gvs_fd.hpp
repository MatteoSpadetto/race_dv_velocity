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
#include "gvs_utils.hpp"

#ifndef GVS_FD
#define GVS_FD

#define THETA_MIN_FD 0
#define THETA_MAX_FD 180
#define C_AREA 0
#define THRESH 100
#define FD_X_CROP 450
#define FD_Y_CROP 450

using namespace cv;
using namespace std;

float get_mode_float(vector<float> input, float lim);                          // Get vector mode
float mean(vector<float> in_vec, int start_frame, int end_frame);              // Get vector mean
float stdev(vector<float> in_vec, float mean, int start_frame, int end_frame); // Get vector standard deviation
float gvs_fd(cv::Mat frame_a, cv::Mat frame_b);                                // Get angle from GVS_FD

#endif