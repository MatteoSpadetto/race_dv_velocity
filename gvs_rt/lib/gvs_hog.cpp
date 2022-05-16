#include "gvs_hog.hpp"

/// Find point with highest mag sum ///
cv::Point find_opt_p(cv::Mat mat_ang, cv::Mat mat_mag)
{
    float sum = 0.0f;
    int x = mat_ang.rows / 2;
    int y = mat_ang.cols / 2;
    for (int i = mat_ang.rows / 2 - 100; i < mat_ang.rows / 2 + 100; i++)
    {
        for (int j = mat_ang.cols / 2 - 100; j < mat_ang.cols / 2 + 100; j++)
        {
            float sum_tmp = 0;
            for (int k = -20; k < 20; k++)
            {
                for (int m = -20; m < 20; m++)
                {
                    sum_tmp += mat_mag.at<float>(i + k, j + m);
                }
            }
            if (sum_tmp >= sum)
            {
                sum = sum_tmp;
                int x = i;
                int y = j;
            }
        }
    }
    return cv::Point(x, y);
}