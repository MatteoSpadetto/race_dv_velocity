#include "lib/gvs_utils.hpp"
#include "lib/gvs_fd.hpp"
#include "lib/gvs_hog.hpp"
#include "lib/gvs_lap.hpp"
#include "math.h"
#include <iomanip>
#include <chrono>

#define FAKE_ANGLE 30
#define SPEED_TH 40

using namespace cv;
using namespace std;

int main(int argc, char const *argv[])
{
    cout << std::setprecision(3) << std::fixed;
    vector<float> thetas_fd;
    vector<int> time;
    vector<float> thetas_hog;
    vector<float> speeds_lap;
    float fake_speed = 200.0f;
    float theta_main;

    for (int frame_id = 20; frame_id < END_FRAME; frame_id++)
    {
        /// FIND DIRECTION ///

        /// Case low speed use GVS_FD ///
        if (frame_id < 35)
        {
            /// Import 2 frames ///
            String path_a = "../../test_speed_v1/mb_speed_" + to_string(frame_id) + ".png";
            String path_b = "../../test_speed_v1/mb_speed_" + to_string(frame_id + 1) + ".png";

            Mat frame_a = imread(path_a, IMREAD_COLOR);
            Mat frame_b = imread(path_b, IMREAD_COLOR);

            /// Check for not corrupted data ///
            if (!frame_a.data || !frame_b.data)
            {
                cout << "Incorrect data frame" << endl;
                return -1;
            }

            /// Testing rotation ///
            rot_img(frame_a, FAKE_ANGLE, true, FD_X_CROP, FD_Y_CROP);
            rot_img(frame_b, FAKE_ANGLE, true, FD_X_CROP, FD_Y_CROP);

            /// Get ang_mat ///
            float theta_fd = gvs_fd(frame_a, frame_b);

            thetas_fd.push_back(theta_fd); // TAKE DIST AND THETA FROM HERE for first 10 frames
            theta_main = theta_fd;
        }
        /// Case high speed use GVS_HOG ///
        else
        {
            /// Import frame ///
            String path = "../../test_speed_v1/mb_speed_" + to_string(frame_id) + ".png";
            Mat frame = imread(path, IMREAD_COLOR);

            /// Check for not corrupted data ///
            if (!frame.data)
            {
                cout << "Incorrect data frame" << endl;
                return -1;
            }

            /// Testing rotation ///
            rot_img(frame, FAKE_ANGLE, true, HOG_X_CROP, HOG_Y_CROP);

            float theta_hog = gvs_hog(frame);
            thetas_hog.push_back(theta_hog); // Store img gradient direction
            theta_main = theta_hog;
            cout << "hog" << endl;
        }
        cout << "Frame: " << frame_id << " --- Theta: " << theta_main << endl;

        /// FIND SPEED with GVS_LAP ///

        if (true)
        {
            String path = "../../test_speed_v1/mb_speed_" + to_string(frame_id) + ".png";
            Mat img = imread(path, IMREAD_COLOR);

            rot_img(img, FAKE_ANGLE, true, 400, 400);

            /// Check for not corrupted data ///
            if (!img.data)
            {
                cout << "Incorrect data frame\n";
                return -1;
            }

            fake_speed = gvs_lap(img, theta_main);
            speeds_lap.push_back(fake_speed);
            cout << "Frame: " << frame_id  << " --- Std_dev: " << fake_speed << endl;
        }
    }
    return 0;
}

// std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
// std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
// time.push_back((std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000);