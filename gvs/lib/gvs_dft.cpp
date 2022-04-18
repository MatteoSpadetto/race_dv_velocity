#include "gvs_dft.hpp"

// ideal low-pass filter
cv::Mat ideal_low_pass_filter(cv::Mat &src, float sigma)
{
    cv::Mat padded = image_make_border(src);
    cv::Mat ideal_kernel = ideal_low_kernel(padded, sigma);
    cv::Mat result = frequency_filter(padded, ideal_kernel);
    return result;
}

// ideal low-pass filter kernel function
cv::Mat ideal_low_kernel(cv::Mat &scr, float sigma)
{
    cv::Mat ideal_low_pass(scr.size(), CV_32FC1); //, CV_32FC1
    float d0 = sigma;                             // the smaller the radius D0, the larger the blur; the larger the radius D0, the smaller the blur
    for (int i = 0; i < scr.rows; i++)
    {
        for (int j = 0; j < scr.cols; j++)
        {
            float d = sqrt(pow(float(i - scr.rows / 2), 2) + pow(float(j - scr.cols / 2), 2)); // numerator, compute pow must be float type
            if (d <= d0)
            {
                ideal_low_pass.at<float>(i, j) = 1;
            }
            else
            {
                ideal_low_pass.at<float>(i, j) = 0;
            }
        }
    }
    return ideal_low_pass;
}

// Ideal high pass filter kernel function
cv::Mat ideal_high_kernel(cv::Mat &scr, float sigma)
{
    cv::Mat ideal_high_pass(scr.size(), CV_32FC1); //, CV_32FC1
    float d0 = sigma;                              // the smaller the radius D0, the larger the blur; the larger the radius D0, the smaller the blur
    for (int i = 0; i < scr.rows; i++)
    {
        for (int j = 0; j < scr.cols; j++)
        {
            float d = sqrt(pow(float(i - scr.rows / 2), 2) + pow(float(j - scr.cols / 2), 2)); // numerator, compute pow must be float type
            if (d <= d0)
            {
                ideal_high_pass.at<float>(i, j) = 0;
            }
            else
            {
                ideal_high_pass.at<float>(i, j) = 1;
            }
        }
    }
    return ideal_high_pass;
}

// ideal high pass filter
cv::Mat ideal_high_pass_filter(cv::Mat &src, float sigma)
{
    cv::Mat padded = image_make_border(src);
    cv::Mat ideal_kernel = ideal_high_kernel(padded, sigma);
    cv::Mat result = frequency_filter(padded, ideal_kernel);
    return result;
}

// Frequency filtering
cv::Mat frequency_filter(cv::Mat &scr, cv::Mat &blur)
{
    cv::Mat mask = scr == scr;
    scr.setTo(0.0f, ~mask);

    // create channel, store real and imaginary parts after dft (CV_32F, must be a single channel number)
    cv::Mat plane[] = {scr.clone(), cv::Mat::zeros(scr.size(), CV_32FC1)};

    cv::Mat complexIm;
    cv::merge(plane, 2, complexIm); // merge channels (merge two matrices into a 2-channel Mat-like container)
    cv::dft(complexIm, complexIm);  // perform Fourier transform and save the result in itself

    // separate channels (array separation)
    cv::split(complexIm, plane);

    // The following operation is frequency domain migration
    fftshift(plane[0], plane[1]);

    // product of ***************** filter function and DFT result ****************
    cv::Mat blur_r, blur_i, BLUR;
    cv::multiply(plane[0], blur, blur_r); // filter (real part multiplied by the corresponding element of the filter template)
    cv::multiply(plane[1], blur, blur_i); // filter (the imaginary part is multiplied by the corresponding element of the filter template)
    cv::Mat plane1[] = {blur_r, blur_i};

    // move back again for inversion
    fftshift(plane1[0], plane1[1]);
    cv::merge(plane1, 2, BLUR); // merge the real part with the imaginary part

    cv::idft(BLUR, BLUR); // idft also results in a complex number
    BLUR = BLUR / BLUR.rows / BLUR.cols;

    cv::split(BLUR, plane); // separate channels, mainly to get channels

    return plane[0];
}

// Image boundary processing
cv::Mat image_make_border(cv::Mat &src)
{
    int w = cv::getOptimalDFTSize(src.cols); // Get the optimal width of the DFT transform
    int h = cv::getOptimalDFTSize(src.rows); // get the optimal height of the DFT transformation

    cv::Mat padded;
    // constant method to expand image boundaries, constant = 0
    cv::copyMakeBorder(src, padded, 0, h - src.rows, 0, w - src.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    padded.convertTo(padded, CV_32FC1);

    return padded;
}

// fft transform followed by spectrum shift
void fftshift(cv::Mat &plane0, cv::Mat &plane1)
{
    // The following operation moves the image (zero frequency shift to the center)
    int cx = plane0.cols / 2;
    int cy = plane0.rows / 2;
    cv::Mat part1_r(plane0, cv::Rect(0, 0, cx, cy)); // element coordinates are represented as (cx, cy)
    cv::Mat part2_r(plane0, cv::Rect(cx, 0, cx, cy));
    cv::Mat part3_r(plane0, cv::Rect(0, cy, cx, cy));
    cv::Mat part4_r(plane0, cv::Rect(cx, cy, cx, cy));

    cv::Mat temp;
    part1_r.copyTo(temp); // swap top-left and bottom-right positions (real part)
    part4_r.copyTo(part1_r);
    temp.copyTo(part4_r);

    part2_r.copyTo(temp); // exchange position between top-right and bottom-left (real part)
    part3_r.copyTo(part2_r);
    temp.copyTo(part3_r);

    cv::Mat part1_i(plane1, cv::Rect(0, 0, cx, cy)); // element coordinates (cx,cy)
    cv::Mat part2_i(plane1, cv::Rect(cx, 0, cx, cy));
    cv::Mat part3_i(plane1, cv::Rect(0, cy, cx, cy));
    cv::Mat part4_i(plane1, cv::Rect(cx, cy, cx, cy));

    part1_i.copyTo(temp); // swap top-left and bottom-right positions (imaginary part)
    part4_i.copyTo(part1_i);
    temp.copyTo(part4_i);

    part2_i.copyTo(temp); // exchange position between upper right and lower left (imaginary part)
    part3_i.copyTo(part2_i);
    temp.copyTo(part3_i);
}