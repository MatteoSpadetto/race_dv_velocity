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

#ifndef GVS_HOG
#define GVS_HOG

#define OPT_P 0
#define FOUR_P 1
#define FIVE_P 2
#define PD_X 200  // Test point x distance from center
#define PD_Y 60   // Test point y distance from center
#define RADIUS 50 // Radius of analysed circle area

cv::Point find_opt_p(cv::Mat mat_ang, cv::Mat mat_mag); // Find point with highest mag sum

#endif