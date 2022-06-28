#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <string>

#ifndef GVS_UTILS
#define GVS_UTILS

using namespace cv;
using namespace std;

#define GVS_HOG_MODE 0
#define GVS_FFT_MODE 1
#define LINEAR 0
#define MEAN_ANGLE 1
#define START_FRAME 2
#define END_FRAME 970
#define CROP_W 350
#define CROP_H 250
#define STEP 1
#define FRAME_PATH_0 "../../test_speed/" //"../../test_fd/20220407T144859/gss_camera/"
#define FRAME_PATH_1 ".png"

typedef struct
{
    double dist; // Frame id
    double theta;
} pt_dist_t; // Frames with their informations

std::string var_path(long n);                                                  // Create string piece path to img
std::string merge_path(int n);                                                 //  Build complete path to img
void line_ang_p(float angle, cv::Point center, cv::Mat &img_in);               // Draw a line with center and angle
void rot_img(cv::Mat &img_in, float angle, bool crop, int crop_x, int crop_y); // Rotate image

#endif