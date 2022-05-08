#include "gvs_utils.hpp"

/// Create string piece path to img ///
std::string var_path(long n)
{
    long tmp = n;
    string num_str;
    if (n < 0 || n > 9999)
    {
        cout << "File path name out of range " << endl;
    }
    else if (n == 0)
    {
        num_str = "0000";
    }
    else
    {
        int count = 0;
        while (tmp != 0)
        {
            tmp = tmp / 10;
            ++count;
        }
        string tmp_str = to_string(n);
        num_str = string((4 - count), '0') + tmp_str;
    }
    return num_str;
}

///  Build complete path to img ///
std::string merge_path(int n)
{
    String frame_path_var = var_path(n);
    return (FRAME_PATH_0 + frame_path_var + FRAME_PATH_1);
}

/// Draw a line with center and angle ///
void line_ang_p(float angle, cv::Point center, cv::Mat &img_in)
{
    float s = sin((angle)*CV_PI / 180);
    float c = cos((angle)*CV_PI / 180);
    Point p0 = Point(center.x + c * 70, center.y - s * 70);
    Point p1 = Point(center.x - c * 70, center.y + s * 70);
    // cv::cvtColor(img_in, img_in, COLOR_GRAY2RGBA);

    circle(img_in, Point(img_in.cols / 2, img_in.rows / 2), 3, Scalar(255, 255, 255), 2);
    line(img_in, p0, p1, Scalar(255, 255, 255), 1);
    return;
}

/// Rotate image ///
void rot_img(cv::Mat &img_in, float angle, bool crop)
{
    Point2f center = Point((img_in.cols) / 2, (img_in.rows) / 2);
    Mat rotation_matix = getRotationMatrix2D(center, angle, 1.0); // Use just positive angles
    warpAffine(img_in, img_in, rotation_matix, img_in.size());
    if (crop)
    {
        int width = 400;
        int heigth = 400;
        Rect crop_region = Rect((img_in.cols / 2) - (width / 2), (img_in.rows / 2) - (heigth / 2), width, heigth);
        img_in = img_in(crop_region);
    }
}