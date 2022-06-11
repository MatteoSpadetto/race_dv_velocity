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
    threshold(frame_bw_a, img_th_a, 128, 255, THRESH_BINARY);
    inRange(img_th_a, Scalar(0, 0, 50), Scalar(255, 255, 255), img_th_a);
    imshow("a", img_th_a);

    Mat img_th_b;
    Mat frame_bw_b;
    cv::cvtColor(frame_b, frame_bw_b, COLOR_RGB2HSV);
    threshold(frame_bw_b, img_th_b, 128, 255, THRESH_BINARY);
    inRange(img_th_b, Scalar(0, 0, 50), Scalar(255, 255, 255), img_th_b);

    /// Erode and dilate ///
    size_t elem_x_erd = 1;
    size_t elem_y_erd = 1;
    size_t elem_x_dil = 1;
    size_t elem_y_dil = 1;
    Mat element_erd = getStructuringElement(MORPH_ELLIPSE, Size(2 * elem_x_erd + 1, 2 * elem_y_erd + 1), Point(elem_x_erd, elem_y_erd)); // Setting dilation
    Mat element_dil = getStructuringElement(MORPH_ELLIPSE, Size(2 * elem_x_dil + 1, 2 * elem_y_dil + 1), Point(elem_x_dil, elem_y_dil)); // Setting dilation

    erode(img_th_a, img_th_a, element_erd);  // Erode
    dilate(img_th_a, img_th_a, element_dil); // Dilate

    erode(img_th_b, img_th_b, element_erd);  // Erode
    dilate(img_th_b, img_th_b, element_dil); // Dilate

    Mat tmp_img = img_th_b.clone();
    cvtColor(tmp_img, tmp_img, COLOR_GRAY2RGBA);

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
        if (contourArea(contours_b[i]) >= C_AREA)
        {
            cv::Moments M = cv::moments(contours_b[i]);
            cv::Point center_b(M.m10 / M.m00, M.m01 / M.m00);
            centers_b.push_back(center_b);
        }
    }
    for (int i = 0; i < contours_a.size(); i++)
    {
        circle(tmp_img, centers_a[i], 1, Scalar(0, 0, 255), 2);
    }
    for (int i = 0; i < contours_a.size(); i++)
    {
        circle(tmp_img, centers_b[i], 1, Scalar(0, 255, 0), 2);
    }

    /// Check centers correlations ///
    vector<vector<pt_dist_t>> feat_vect;
    for (int m = 0; m < centers_a.size(); m++)
    {
        vector<pt_dist_t> tmp_feat_vect;
        for (int j = 0; j < centers_b.size(); j++)
        {
            float tmp_y = centers_b[j].y - centers_a[m].y;
            float tmp_x = centers_b[j].x - centers_a[m].x;
            float tmp_theta = atan2(tmp_y, tmp_x) * 180 / CV_PI;
            if ((tmp_x * tmp_x) + (tmp_y * tmp_y) < CHECK_R) // Check just the RoI (area below because moving forward)
            {
                line(tmp_img, centers_a[m], centers_b[j], Scalar(255, 0, 0), 1);
                double len = norm(centers_a[m] - centers_b[j]); // Store all conncetion lengths between centers
                pt_dist_t tmp_pt_dist;
                tmp_pt_dist.theta = tmp_theta;
                tmp_pt_dist.dist = len;
                tmp_feat_vect.push_back(tmp_pt_dist);
            }
        }
        feat_vect.push_back(tmp_feat_vect);
    }


    imshow("a", tmp_img);
    waitKey(0);

    // ofstream file_t;
    // file_t.open("../gvs/data_csv/data_csv_45_t.csv");

    vector<float> thetas;
    for (int i = 0; i < feat_vect.size(); i++)
    {
        for (int j = 0; j < feat_vect[i].size(); j++)
        {
            thetas.push_back(feat_vect[i][j].theta);
            // file_t << feat_vect[i][j].theta << endl;
            // cout << feat_vect[i][j].theta << endl;
        }
    }
    // file_t.close();

    float thetas_mode = get_mode_float(thetas, 0.5); // Get the mode of remaining thetas
    float result = thetas_mode;
    // cout << "RES: " << result << endl;

    /*vector<float> dists;
    float dd = 0;
    int n = 0;
    for (int i = 0; i < feat_vect.size(); i++)
    {
        for (int m = 0; m < feat_vect[i].size(); m++)
        {
            dists.push_back(feat_vect[i][m].dist);
            dd += feat_vect[i][m].dist;
            n++;
        }
    }
    float dists_mode = dd / n;
    dists_mode = get_mode_float(dists, 1); // Get the mode of the vector (aka choose the most common length)
    cout << "dists" << dists_mode << endl;
    /// Rearrange the angles in a vetor ///
    vector<float> thetas;
    float sum = 0;
    int k = 0;
    for (int i = 0; i < feat_vect.size(); i++)
    {
        for (int m = 0; m < feat_vect[i].size(); m++)
        {
            thetas.push_back(feat_vect[i][m].theta); // Take just the vector with amplitude around the mode
            int UP_LIM = 5;
            int DOWN_LIM = 5;
            if ((feat_vect[i][m].dist > dists_mode - DOWN_LIM) && (feat_vect[i][m].dist < dists_mode + UP_LIM))
            {
                sum += feat_vect[i][m].theta;
                cout << "cc" << feat_vect[i][m].theta << endl;
                k++;
            }
        }
    }
    sum /= k;
    cout << "Theta new: " << sum << endl;*/

    return result;
}
