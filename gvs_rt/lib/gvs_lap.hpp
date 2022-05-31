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

#ifndef GVS_LAP
#define GVS_LAP

#define LAP_X_CROP 350
#define LAP_Y_CROP 350

float gvs_lap(cv::Mat frame, float angle);

#endif