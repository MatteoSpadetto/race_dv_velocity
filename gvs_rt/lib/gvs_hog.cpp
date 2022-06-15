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

float gvs_hog(cv::Mat frame)
{

    // frame.convertTo(frame, CV_32F);              // Convert to multi-channels
    cvtColor(frame, frame, COLOR_RGB2HSV);
    GaussianBlur(frame, frame, Size(5, 5), 0);

    /*
    Mat frame_b;
    threshold(frame, frame_b, 0, 255, THRESH_OTSU);

    imshow("f", frame_b);
    waitKey(0);*/

    /*float kdata[] = {0, 1, 0, 1, 5, 1, 0, 1, 0};
    Mat kernel(3, 3, CV_32F, kdata);
    Mat filt_c;
    filter2D(frame, filt_c, CV_32F, kernel);*/

    vector<Mat> channels;
    split(frame, channels);

    // equalizeHist(channels[2], channels[2]);

    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(4);
    clahe->apply(channels[2], channels[2]);

    Mat gx, gy, mag_mat, ang_mat;
    Sobel(channels[2], gx, CV_32F, 1, 0);     // Gradient
    Sobel(channels[2], gy, CV_32F, 0, 1);     // Gradient
    cartToPolar(gx, gy, mag_mat, ang_mat, 1); // Get HOG


    /// Testing point ///
    Point test_p = Point((ang_mat.cols / 2), (ang_mat.rows / 2));

    /// Build Histogram Of Gradients (HOG) ///
    vector<int> bins;
    vector<float> grd_bins;

    float ang_step = 0;

    while (ang_step < 180)
    {
        bins.push_back(0);
        grd_bins.push_back(ang_step);
        for (int i = 0; i < ang_mat.cols; i++)
        {
            for (int j = 0; j < ang_mat.rows; j++)
            {
                if (((i - test_p.x) * (i - test_p.x)) + ((j - test_p.y) * (j - test_p.y)) <= (RADIUS) * (RADIUS))
                {
                    if (ang_mat.at<float>(j, i) == ang_step)
                    {
                        bins[bins.size() - 1] += mag_mat.at<float>(j, i);
                    }
                    else if ((ang_mat.at<float>(j, i) > ang_step) && (ang_mat.at<float>(j, i) < ang_step + 0.5))
                    {
                        bins[bins.size() - 1] += mag_mat.at<float>(j, i);
                    }
                }
            }
        }
        ang_step += 0.5; // More accurate means slower (a lot)
    }

    Scalar mean, std_dev;
    meanStdDev(bins, mean, std_dev);

    ofstream file_c;
    file_c.open("../gvs/data_csv/data_c.csv");
    float tt = 0;
    float index = 0;
    for (int i = 0; i < grd_bins.size(); i++)
    {
        if (bins[i] < (mean[0] + 3 * std_dev[0]))
        {
            file_c << grd_bins[i] << "," << bins[i] << endl;
            if (bins[i] > tt)
            {
                index = grd_bins[i];
                tt = bins[i];
            }
        }
    }
    file_c.close();

    /// Sort HOG ///
    for (int i = 0; i < bins.size(); i++)
    {
        for (int j = 0; j < bins.size(); j++)
        {
            if (bins[i] > bins[j])
            {
                float tmp_bins = bins[i];
                float tmp_grd_bins = grd_bins[i];
                bins[i] = bins[j];
                grd_bins[i] = grd_bins[j];
                bins[j] = tmp_bins;
                grd_bins[j] = tmp_grd_bins;
            }
        }
    }

    float result = 180 - index;
    if (result < 270 && result > 90)
    {
        result -= 180;
    }
    return result;
}