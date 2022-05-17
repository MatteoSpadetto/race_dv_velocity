#include "gvs_fd.hpp"

/// Get vector mode ///
float get_mode_float(vector<float> input, float lim)
{
    float result;
    float mode_val[input.size()][2];
    for (int i = 0; i < input.size(); i++)
    {
        float val = input[i];
        float val_min = val - lim;
        float val_max = val + lim;
        mode_val[i][0] = val;
        float count = 0;
        for (int j = 0; j < input.size(); j++)
        {
            if ((input[j] > val_min) && (input[j] < val_max))
            {
                count++;
            }
        }
        mode_val[i][1] = count;
    }
    int id = 0;
    int max_count = 0;
    for (int i = 0; i < input.size(); i++)
    {
        if (mode_val[i][1] >= max_count)
        {
            id = i;
            max_count = mode_val[i][1];
        }
    }
    return mode_val[id][0];
}

/// Get vector mean ///
float mean(vector<float> in_vec, int start_frame, int end_frame)
{
    float sum = 0;
    for (int i = start_frame; i < end_frame; i++)
        sum += in_vec[i];
    return sum / (end_frame - start_frame);
}

/// Get vector standard deviation ///
float stdev(vector<float> in_vec, float mean, int start_frame, int end_frame)
{
    if (in_vec.size() <= 1u)
        return std::numeric_limits<double>::quiet_NaN();

    auto const add_square = [mean](float sum, int i)
    {
        auto d = i - mean;
        return sum + d * d;
    };
    float total = std::accumulate(in_vec.begin(), in_vec.end(), 0.0, add_square);
    return sqrt(total / (in_vec.size() - 1));
}

float gvs_fd(cv::Mat frame_a, cv::Mat frame_b)
{
    /// Go to HSV and use S ///
    Mat img_th_a;
    Mat frame_bw_a;
    cv::cvtColor(frame_a, frame_bw_a, COLOR_RGB2HSV);
    vector<Mat> channels_a;
    split(frame_bw_a, channels_a);
    threshold(channels_a[0], img_th_a, 0, 255, THRESH_BINARY);

    Mat img_th_b;
    Mat frame_bw_b;
    cv::cvtColor(frame_b, frame_bw_b, COLOR_RGB2HSV);
    vector<Mat> channels_b;
    split(frame_bw_b, channels_b);
    threshold(channels_b[0], img_th_b, 0, 255, THRESH_BINARY);

    Mat gx_a, gy_a, mag_mat_a, ang_mat_a;
    Sobel(img_th_a, gx_a, CV_32F, 1, 0, 3);           // Gradient
    Sobel(img_th_a, gy_a, CV_32F, 0, 1, 3);           // Gradient
    cartToPolar(gx_a, gy_a, mag_mat_a, ang_mat_a, 1); // Get HO
    mag_mat_a.convertTo(img_th_a, CV_8U);

    Mat gx_b, gy_b, mag_mat_b, ang_mat_b;
    Sobel(img_th_b, gx_b, CV_32F, 1, 0, 3);           // Gradient
    Sobel(img_th_b, gy_b, CV_32F, 0, 1, 3);           // Gradient
    cartToPolar(gx_b, gy_b, mag_mat_b, ang_mat_b, 1); // Get HO

    mag_mat_b.convertTo(img_th_b, CV_8U);


    /// Erode and dilate ///
    size_t elem_x_erd = 1;
    size_t elem_y_erd = 1;
    size_t elem_x_dil = 1;
    size_t elem_y_dil = 1;
    Mat element_erd = getStructuringElement(MORPH_ELLIPSE, Size(2 * elem_x_erd + 1, 2 * elem_y_erd + 1), Point(elem_x_erd, elem_y_erd)); // Setting dilation
    Mat element_dil = getStructuringElement(MORPH_ELLIPSE, Size(2 * elem_x_dil + 1, 2 * elem_y_dil + 1), Point(elem_x_dil, elem_y_dil)); // Setting dilation

    dilate(img_th_a, img_th_a, element_dil); // Dilate
    erode(img_th_a, img_th_a, element_erd);  // Erode

    dilate(img_th_b, img_th_b, element_dil); // Dilate
    erode(img_th_b, img_th_b, element_erd);  // Erode

    /// Find contours ///
    Mat canny_output_a;
    std::vector<cv::Point> centers_a;
    Canny(img_th_a, canny_output_a, THRESH, THRESH * 3);
    vector<vector<Point>> contours_a;
    findContours(canny_output_a, contours_a, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (size_t i = 0; i < contours_a.size(); i++)
    {
        if (contourArea(contours_a[i]) >= C_AREA)
        {
            Scalar color = Scalar(255, 255, 255);
            drawContours(frame_b, contours_a, (int)i, color, 2);
            cv::Moments M = cv::moments(contours_a[i]);
            cv::Point center_a(M.m10 / M.m00, M.m01 / M.m00);
            centers_a.push_back(center_a);
        }
    }

    Mat canny_output_b;
    std::vector<cv::Point> centers_b;
    Canny(img_th_b, canny_output_b, THRESH, THRESH * 3);
    vector<vector<Point>> contours_b;
    findContours(canny_output_b, contours_b, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (size_t i = 0; i < contours_b.size(); i++)
    {
        Scalar color = Scalar(0, 0, 255);
        if (contourArea(contours_b[i]) >= C_AREA)
        {
            drawContours(frame_b, contours_b, (int)i, color, 2);
            cv::Moments M = cv::moments(contours_b[i]);
            cv::Point center_b(M.m10 / M.m00, M.m01 / M.m00);
            centers_b.push_back(center_b);
        }
    }

    /// Check centers correlations ///
    vector<vector<pt_dist_t>> feat_vect;
    Mat tmp_img = frame_b.clone();
    for (int m = 0; m < centers_a.size(); m++)
    {
        vector<pt_dist_t> tmp_feat_vect;
        for (int j = 0; j < centers_b.size(); j++)
        {
            float tmp_y = centers_b[j].y - centers_a[m].y;
            float tmp_x = centers_b[j].x - centers_a[m].x;
            float tmp_theta = atan2(tmp_y, tmp_x) * 180 / CV_PI;
            if ((tmp_x * tmp_x) + (tmp_y * tmp_y) < 3000) // Check just the RoI (area below because moving forward)
            {
                line(frame_b, centers_a[m], centers_b[j], Scalar(255, 255, 255), 1);
                double len = norm(centers_a[m] - centers_b[j]); // Store all conncetion lengths between centers
                pt_dist_t tmp_pt_dist;
                tmp_pt_dist.pt_a = centers_a[m];
                tmp_pt_dist.pt_b = centers_b[j];
                tmp_pt_dist.theta = tmp_theta;
                tmp_feat_vect.push_back(tmp_pt_dist);
            }
        }
        feat_vect.push_back(tmp_feat_vect);
    }

    imshow("f", frame_b);
    waitKey(0);

    /// Rearrange the angles in a vetor ///
    vector<float> thetas;
    for (int i = 0; i < feat_vect.size(); i++)
    {
        for (int m = 0; m < feat_vect[i].size(); m++)
        {
            thetas.push_back(feat_vect[i][m].theta); // Take just the vector with amplitude around the mode
        }
    }
    float thetas_mode = get_mode_float(thetas, 1); // Get the mode of remaining thetas
    float result = 90 - thetas_mode;
    return result;
}
