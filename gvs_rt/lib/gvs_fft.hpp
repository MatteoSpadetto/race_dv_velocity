#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <string>
#include "gvs_utils.hpp"

#ifndef GVS_FFT
#define GVS_FFT

#define CUT_FREQ 50
#define ERROR 0.05

// https://www.codedevlib.com/article/opencv-ideal-high-pass-low-pass-filters-c-36227
// https://vovkos.github.io/doxyrest-showcase/opencv/sphinx_rtd_theme/page_tutorial_discrete_fourier_transform.html

cv::Mat ideal_low_kernel(cv::Mat &scr, float sigma);
cv::Mat ideal_low_pass_filter(cv::Mat &src, float sigma);
cv::Mat ideal_high_kernel(cv::Mat &scr, float sigma);
cv::Mat ideal_high_pass_filter(cv::Mat &src, float sigma);
cv::Mat frequency_filter(cv::Mat &scr, cv::Mat &blur);
cv::Mat image_make_border(cv::Mat &src);
void fftshift(cv::Mat &plane0, cv::Mat &plane1);
float gvs_fft(cv::Mat &frame);

#endif