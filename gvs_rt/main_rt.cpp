#include "lib/gvs_utils.hpp"
#include "lib/gvs_fd.hpp"
#include "lib/gvs_hog.hpp"
#include "lib/gvs_lap.hpp"
#include "math.h"
#include <iomanip>
#include <chrono>

using namespace cv;
using namespace std;

int main(int argc, char const *argv[])
{
    cout << std::setprecision(3) << std::fixed;
    vector<float> thetas_fd;
    vector<int> time;
    vector<float> thetas_hog;
    vector<float> speeds_lap;

    for (int frame = START_FRAME; frame < END_FRAME; frame++)
    {
        /// FIND DIRECTION ///
        int gvs_mode = GVS_HOG_MODE;
        /// Case low speed use GVS_FD ///
        if (gvs_mode == GVS_FD_MODE)
        {
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            /// Import 2 frames ///
            Mat frame_a = imread(merge_path(frame - STEP), IMREAD_COLOR);
            Mat frame_b = imread(merge_path(frame), IMREAD_COLOR);

            /// Check for not corrupted data ///
            if (!frame_a.data || !frame_b.data)
            {
                cout << "Incorrect data frame" << endl;
                return -1;
            }

            /// Testing rotation ///
            rot_img(frame_a, 45, true, FD_X_CROP, FD_Y_CROP);
            rot_img(frame_b, 45, true, FD_X_CROP, FD_Y_CROP);

            /// Get ang_mat ///
            float theta_fd = gvs_fd(frame_a, frame_b);

            if (frame - START_FRAME < WINDOW_SIZE)
            {
                // cout << "Frame: " << frame << " --- Theta: " << theta_fd << endl;
                thetas_fd.push_back(theta_fd); // TAKE DIST AND THETA FROM HERE for first 10 frames
            }
            else
            {
                Scalar thetas_mean, thetas_std;
                meanStdDev(thetas_fd, thetas_mean, thetas_std, Mat());

                float theta_cut_off = thetas_std[0] * 3;
                float theta_l_lim = thetas_mean[0] - theta_cut_off;
                float theta_u_lim = thetas_mean[0] + theta_cut_off;

                if (((theta_fd > theta_l_lim) && (theta_fd < theta_u_lim)))
                {
                    // cout << "Frame: " << frame << " --- Theta: " << theta_fd << endl;
                    thetas_fd.push_back(theta_fd); // TAKE DIST AND THETA FROM HERE for first 10 frames
                }
            }

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            time.push_back((std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000);
        }
        /// Case high speed use GVS_HOG ///
        else if (gvs_mode == GVS_HOG_MODE)
        {
            /// Import frame ///
            String path = "../../test_speed/mb_speed_" + to_string(frame - START_FRAME + 30) + ".png";
            Mat frame = imread(path, IMREAD_COLOR);

            /// Check for not corrupted data ///
            if (!frame.data)
            {
                cout << "Incorrect data frame" << endl;
                return -1;
            }

            /// Testing rotation ///
            rot_img(frame, 30, true, HOG_X_CROP, HOG_Y_CROP);
            Mat frame_lap;
            frame.copyTo(frame_lap);

            /// Processing gradient ///
            cv::cvtColor(frame, frame, COLOR_RGBA2GRAY); // Gray scale
            frame.convertTo(frame, CV_32F);              // Convert to multi-channels
            GaussianBlur(frame, frame, Size(3, 3), 0);   // Blur to avoid big outliers
            Mat gx, gy, mag_mat, ang_mat;
            Sobel(frame, gx, CV_32F, 1, 0, 3);        // Gradient
            Sobel(frame, gy, CV_32F, 0, 1, 3);        // Gradient
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
                            else if ((ang_mat.at<float>(j, i) > ang_step) && (ang_mat.at<float>(j, i) < ang_step + 1))
                            {
                                bins[bins.size() - 1] += mag_mat.at<float>(j, i);
                            }
                        }
                    }
                }
                ang_step += 1; // More accurate means slower (a lot)
            }

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

            float theta_hog = 180 - grd_bins[0];
            thetas_hog.push_back(grd_bins[0]); // Store img gradient direction

            float speed_lap = gvs_lap(frame_lap, theta_hog);
            speeds_lap.push_back(speed_lap);

            cout << "Theta_hog: " << theta_hog << " --- Speed_lap: " << speed_lap << endl;
        }
    }
    /// FIND SPEED with GVS_LAP ///

    /*float sum = 0;
    for (int i = 0; i < time.size(); i++)
    {
        sum += time[i];
    }
    sum /= time.size();
    cout << sum << endl;*/
    return 0;
}