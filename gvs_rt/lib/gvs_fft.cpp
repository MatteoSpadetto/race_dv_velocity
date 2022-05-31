#include "gvs_fft.hpp"

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

float gvs_fft(cv::Mat &frame)
{
    /// Go to gray scale ///
    cv::cvtColor(frame, frame, COLOR_RGBA2GRAY);

    /// Find magnitude Mat ///
    Mat padded_img;
    int m = getOptimalDFTSize(frame.rows);
    int n = getOptimalDFTSize(frame.cols);

    copyMakeBorder(frame, padded_img, 0, m - frame.rows, 0, 0, BORDER_CONSTANT, Scalar::all(0));
    Mat planes[] = {Mat_<float>(padded_img), Mat::zeros(padded_img.size(), CV_32F)};
    Mat complexI;
    merge(planes, 2, complexI);
    dft(complexI, complexI);
    split(complexI, planes);
    magnitude(planes[0], planes[1], planes[0]); // planes[0] = magnitude
    Mat magI = planes[0];
    magI += Scalar::all(1); // switch to logarithmic scale
    log(magI, magI);
    // crop the spectrum, if it has an odd frame_idber of rows or columns
    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
    int cx = magI.cols / 2;
    int cy = magI.rows / 2;
    Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right
    Mat tmp;                            // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp); // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);
    normalize(magI, magI, 0, 1, NORM_MINMAX);

    /// LP filter on dft ///
    magI = ideal_high_pass_filter(magI, 0.5);
    magI = ideal_low_pass_filter(magI, 15);

    /// Find maximum on of each col of magnitude Mat ///
    vector<Point> store; // Store the magnitude max point values
    int bound = 75;
    int llim = int(magI.cols / 2);
    int ulim = int(magI.cols / 2) + bound;
    int err = ERROR * magI.rows / 2;
    for (int k = llim; k < ulim; k++)
    {
        float max_col = 0.0f;
        int max_col_id = 0;
        for (int i = 0; i < magI.rows; i++)
        {
            if (magI.at<float>(i, k) > max_col)
            {
                max_col = magI.at<float>(i, k);
                max_col_id = i;
            }
        }
        if (store.size() > 1)
        {
            if (abs(max_col_id - store[store.size() - 1].y) <= err)
            {
                store.push_back(Point(k, max_col_id));
            }
        }
        else
        {
            store.push_back(Point(k, max_col_id));
        }
    }
    /*for (int i = 0; i < store.size(); i++)
    {
         circle(magI, Point(int(magI.cols / 2) + i, store[i].y), 1, Scalar(255, 255, 255), 1);
    }*/
    Point p0 = Point(int(magI.cols / 2), store[0].y);
    Point p1 = Point(int(magI.cols / 2) + store.size(), store[store.size() - 1].y);
    // line(magI, p0, p1, Scalar(255, 255, 0), 2, 2);
    float tmp_y = p1.y - p0.y;
    float tmp_x = p1.x - p0.x;
    float angle_fft = -atan2(tmp_y, tmp_x) * 180 / CV_PI;
    return angle_fft;
}