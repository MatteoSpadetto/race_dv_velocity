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

#define GVS_FD_MODE 0
#define GVS_AE_MODE 1
#define LINEAR 0
#define MEAN_ANGLE 1
#define START_FRAME 900
#define END_FRAME 990
#define CROP_W 310
#define CROP_H 100
#define STEP 1
#define THRESH 100
#define FRAME_PATH_0 "./test_0/20220407T144859/gss_camera/"
#define FRAME_PATH_1 ".png"

typedef struct
{
    Point pt_a; // Frame image
    Point pt_b;
    double dist; // Frame id
    double theta;
} pt_dist_t; // Frames with their informations

std::string var_path(long n);
std::string merge_path(int n);
void show_img(Mat img, String title);

#endif