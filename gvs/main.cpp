#include "lib/gvs_utils.hpp"
#include "lib/gvs_fd.hpp"
#include "lib/gvs_hog.hpp"
#include "lib/gvs_dft.hpp"
#include "math.h"
#include <iomanip>

using namespace cv;
using namespace std;

#define gvs_mode GVS_LAP_MODE

int main(int argc, char const *argv[])
{
    cout << std::setprecision(3) << std::fixed;
    ofstream file_hog_tot;
    file_hog_tot.open("./data_csv/data_hog_tot_90.csv");
    for (int p = 1; p < 2; p++)
    {
        if (gvs_mode == GVS_FD_MODE)
        {
            uint16_t frame_id = START_FRAME;
            ofstream file_fd;
            file_fd.open("data_csv/data_fd_45.csv");
            vector<float> thetas_tot;
            vector<float> dists_tot;
            while (frame_id < END_FRAME)
            {
                /// Import 2 frames ///
                Mat img_a = imread(merge_path(frame_id - STEP), IMREAD_COLOR);
                Mat img_b = imread(merge_path(frame_id), IMREAD_COLOR);

                /// Check for not corrupted data ///
                if (!img_a.data || !img_b.data)
                {
                    cout << "Incorrect data frame\n";
                    return -1;
                }
                /// Testing rotation ///
                rot_img(img_a, 45, false);
                rot_img(img_b, 45, false);

                /// Crop 2 frames around the center ///
                int width = 750;
                int heigth = 750;
                cv::Rect crop_region((img_a.cols / 2) - (width / 2), (img_a.rows / 2) - (heigth / 2), width, heigth);
                img_a = img_a(crop_region);
                img_b = img_b(crop_region);

                /// Go to grayscale ///
                Mat img_th_a;
                Mat img_bw_a;
                cv::cvtColor(img_a, img_bw_a, COLOR_RGB2HSV);
                vector<Mat> channels_a;
                split(img_bw_a, channels_a);
                // equalizeHist(img_bw_a, img_bw_a);
                threshold(channels_a[1], img_th_a, 0, 255, THRESH_BINARY);

                Mat img_th_b;
                Mat img_bw_b;
                cv::cvtColor(img_b, img_bw_b, COLOR_RGB2HSV);
                vector<Mat> channels_b;
                split(img_bw_b, channels_b);
                // equalizeHist(img_bw_b, img_bw_b);
                threshold(channels_b[1], img_th_b, 0, 255, THRESH_BINARY);

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

                imshow("dddd_a", img_th_a);

                imshow("dddd_b", img_th_b);
                /// Find contours ///
                Mat canny_output_a;
                Canny(img_th_a, canny_output_a, THRESH, THRESH * 3);
                vector<vector<Point>> contours_a;
                findContours(canny_output_a, contours_a, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
                for (size_t i = 0; i < contours_a.size(); i++)
                {
                    if (contourArea(contours_a[i]) >= C_AREA)
                    {
                        Scalar color = Scalar(255, 255, 255);
                        drawContours(img_b, contours_a, (int)i, color, 2);
                    }
                }

                Mat canny_output_b;
                Canny(img_th_b, canny_output_b, THRESH, THRESH * 3);
                vector<vector<Point>> contours_b;
                findContours(canny_output_b, contours_b, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
                for (size_t i = 0; i < contours_b.size(); i++)
                {
                    Scalar color = Scalar(0, 0, 255);
                    if (contourArea(contours_b[i]) >= C_AREA)
                    {
                        drawContours(img_b, contours_b, (int)i, color, 2);
                    }
                }

                /// Find contour centers ///
                std::vector<cv::Point> centers_a;
                for (int i = 0; i < contours_a.size(); i++)
                {
                    if (contourArea(contours_a[i]) >= C_AREA)
                    {
                        cv::Moments M = cv::moments(contours_a[i]);
                        cv::Point center_a(M.m10 / M.m00, M.m01 / M.m00);
                        centers_a.push_back(center_a);
                    }
                }

                std::vector<cv::Point> centers_b;
                for (int i = 0; i < contours_b.size(); i++)
                {
                    if (contourArea(contours_b[i]) >= C_AREA)
                    {
                        cv::Moments M = cv::moments(contours_b[i]);
                        cv::Point center_b(M.m10 / M.m00, M.m01 / M.m00);
                        centers_b.push_back(center_b);
                    }
                }

                /// Check centers correlations ///
                vector<vector<pt_dist_t>> feat_vect;
                Mat tmp_img = img_b.clone();
                for (int m = 0; m < centers_a.size(); m++)
                {
                    vector<pt_dist_t> tmp_feat_vect;
                    for (int j = 0; j < centers_b.size(); j++)
                    {
                        float tmp_y = centers_b[j].y - centers_a[m].y;
                        float tmp_x = centers_b[j].x - centers_a[m].x;
                        float tmp_theta = atan2(tmp_y, tmp_x) * 180 / CV_PI;
                        if (tmp_theta > THETA_MIN_FD && tmp_theta < THETA_MAX_FD && centers_a[m].y < centers_b[j].y) // Check just the RoI (area below because moving forward)
                        {
                            line(img_b, centers_a[m], centers_b[j], Scalar(255, 255, 255), 1);
                            double len = norm(centers_a[m] - centers_b[j]); // Store all conncetion lengths between centers
                            pt_dist_t tmp_pt_dist;
                            tmp_pt_dist.pt_a = centers_a[m];
                            tmp_pt_dist.pt_b = centers_b[j];
                            tmp_pt_dist.dist = len;
                            tmp_pt_dist.theta = tmp_theta;
                            tmp_feat_vect.push_back(tmp_pt_dist);
                        }
                    }
                    feat_vect.push_back(tmp_feat_vect);
                }

                /// Rearrange distances in a vector ///
                vector<float> dists;
                for (int i = 0; i < feat_vect.size(); i++)
                {
                    for (int m = 0; m < feat_vect[i].size(); m++)
                    {
                        dists.push_back(feat_vect[i][m].dist);
                    }
                }
                float dists_mode = get_mode_float(dists, 5); // Get the mode of the vector (aka choose the most common length)

                /// Rearrange the angles in a vetor ///
                vector<float> thetas;
                for (int i = 0; i < feat_vect.size(); i++)
                {
                    for (int m = 0; m < feat_vect[i].size(); m++)
                    {
                        if ((feat_vect[i][m].dist > dists_mode - 10) && (feat_vect[i][m].dist < dists_mode + 10))
                        {
                            thetas.push_back(feat_vect[i][m].theta); // Take just the vector with amplitude around the mode
                        }
                    }
                }
                float thetas_mode = get_mode_float(thetas, 1); // Get the mode of remaining thetas

                /// Debug arrow on image ///
                arrowedLine(img_b,
                            Point((img_b.cols / 2) + (dists_mode * cos(thetas_mode * CV_PI / 180) / 2), (img_b.rows / 2) + (dists_mode * sin(thetas_mode * CV_PI / 180) / 2)),
                            Point((img_b.cols / 2) - (dists_mode * cos(thetas_mode * CV_PI / 180) / 2), (img_b.rows / 2) - (dists_mode * sin(thetas_mode * CV_PI / 180) / 2)),
                            Scalar(0, 0, 255), 5);
                resize(img_b, img_b, Size(img_b.cols * 0.5, img_b.rows * 0.5), INTER_LINEAR);
                imshow("Frame Diff GVS", img_b);
                waitKey(0);
                if (thetas_mode != 0)
                {
                    if (frame_id - START_FRAME < WINDOW_SIZE)
                    {
                        dists_tot.push_back(dists_mode);
                        thetas_tot.push_back(thetas_mode);
                        cout << "dists_mode: " << dists_mode << "\t---\t"
                             << "thetas_mode: " << 90 - thetas_mode << endl;
                        file_fd << frame_id - START_FRAME << "," << dists_mode << "," << 90 - thetas_mode << '\n';
                        // TAKE DIST AND THETA FROM HERE for first 10 frames
                    }
                    else
                    {
                        float theta_mean = mean(thetas_tot, thetas_tot.size() - WINDOW_SIZE, thetas_tot.size());
                        float dist_mean = mean(dists_tot, dists_tot.size() - WINDOW_SIZE, dists_tot.size());

                        float theta_std = stdev(thetas_tot, theta_mean, thetas_tot.size() - WINDOW_SIZE, thetas_tot.size());
                        float dist_std = stdev(dists_tot, dist_mean, thetas_tot.size() - WINDOW_SIZE, dists_tot.size());

                        float theta_cut_off = theta_std * 3;
                        float theta_l_lim = theta_mean - theta_cut_off;
                        float theta_u_lim = theta_mean + theta_cut_off;

                        float dist_cut_off = dist_std * 3;
                        float dist_l_lim = dist_mean - dist_cut_off;
                        float dist_u_lim = dist_mean + dist_cut_off;

                        cout << "###########" << endl;
                        cout << "theta_mean: " << theta_mean << endl;
                        cout << "dist_mean: " << dist_mean << endl;
                        cout << "theta_std: " << theta_std << endl;
                        cout << "dist_std: " << dist_std << endl;

                        if (((thetas_mode > theta_l_lim) && (thetas_mode < theta_u_lim)) && ((dists_mode > dist_l_lim) && (dists_mode < dist_u_lim)))
                        {
                            dists_tot.push_back(dists_mode);
                            thetas_tot.push_back(thetas_mode);
                            cout << "dists_mode: " << dists_mode << "\t---\t"
                                 << "thetas_mode: " << 90 - thetas_mode << endl;
                            file_fd << frame_id - START_FRAME << "," << dists_mode << "," << 90 - thetas_mode << '\n';
                            // TAKE DIST AND THETA FROM HERE for first 10 frames
                        }
                    }
                }

                frame_id++;
            }
            file_fd.close();
        }
        else if (gvs_mode == GVS_HOG_MODE)
        {
            // Read image
            Mat img = imread("../../test_speed/mb_speed_100.png", IMREAD_COLOR);
            // String path = "../../test_speed/mb_speed_" + to_string(p) + ".png";
            // Mat img = imread(path, IMREAD_COLOR);
            /// Check for not corrupted data ///
            if (!img.data)
            {
                cout << "Incorrect data frame\n";
                return -1;
            }

            vector<float> store_ang;
            vector<float> store_speed;
            cv::cvtColor(img, img, COLOR_RGBA2GRAY);

            /// Testing rotation ///
            rot_img(img, 45, true);
            Mat img_out = img;

            img.convertTo(img, CV_32F);            // Convert to multi-channels
            GaussianBlur(img, img, Size(3, 3), 0); // Blur to avoid big outliers
            Mat gx, gy, mag, angle;
            Sobel(img, gx, CV_32F, 1, 0, 3);    // Gradient
            Sobel(img, gy, CV_32F, 0, 1, 3);    // Gradient
            cartToPolar(gx, gy, mag, angle, 1); // Get HOG

            vector<Point> test_p;
            vector<int> bins;
            vector<float> ang_bins;

#define hog_mode OPT_P
            if (hog_mode == OPT_P)
            {
                /// Find optimal point to analyse (aka highest magnitude area) ///
                Point p = find_opt_p(angle, mag);
                test_p.push_back(Point(p.y, p.x));
            }
            else if (hog_mode == FOUR_P)
            {
                test_p.push_back(Point((angle.cols / 2) + PD_X, (angle.rows / 2) + PD_Y));
                test_p.push_back(Point((angle.cols / 2) + PD_X, (angle.rows / 2) - PD_Y));
                test_p.push_back(Point((angle.cols / 2) - PD_X, (angle.rows / 2) + PD_Y));
                test_p.push_back(Point((angle.cols / 2) - PD_X, (angle.rows / 2) - PD_Y));
            }
            else if (hog_mode == FIVE_P)
            {
                test_p.push_back(Point((angle.cols / 2), (angle.rows / 2)));
                test_p.push_back(Point((angle.cols / 2) + PD_X, (angle.rows / 2) + PD_Y));
                test_p.push_back(Point((angle.cols / 2) + PD_X, (angle.rows / 2) - PD_Y));
                test_p.push_back(Point((angle.cols / 2) - PD_X, (angle.rows / 2) + PD_Y));
                test_p.push_back(Point((angle.cols / 2) - PD_X, (angle.rows / 2) - PD_Y));
            }

            for (int l = 0; l < test_p.size(); l++)
            {
                float ang_step = 0;
                circle(img_out, Point(test_p[l].x, test_p[l].y), RADIUS, Scalar(255, 0, 255), 1);

                while (ang_step < 180)
                {
                    bins.push_back(0);
                    ang_bins.push_back(ang_step);
                    for (int i = 0; i < angle.cols; i++)
                    {
                        for (int j = 0; j < angle.rows; j++)
                        {
                            if ((abs(i - test_p[l].x) * abs(i - test_p[l].x)) + (abs(j - test_p[l].y) * abs(j - test_p[l].y)) <= (RADIUS) * (RADIUS))
                            {
                                if (angle.at<float>(j, i) == ang_step)
                                {
                                    bins[bins.size() - 1] += mag.at<float>(j, i);
                                }
                                else if ((angle.at<float>(j, i) > ang_step - 1) && (angle.at<float>(j, i) < ang_step))
                                {
                                    bins[bins.size() - 1] += mag.at<float>(j, i);
                                }
                            }
                        }
                    }
                    ang_step += 1;
                }

                ofstream file_hog;
                file_hog.open("./data_csv/data_hog_45.csv");
                for (int i = 0; i < ang_bins.size(); i++)
                {
                    file_hog << ang_bins[i] << "," << bins[i] << endl;
                }
                file_hog.close();

                /// Sort in descendent order ///
                for (int i = 0; i < bins.size(); i++)
                {
                    for (int j = 0; j < bins.size(); j++)
                    {
                        if (bins[i] > bins[j])
                        {
                            float tmp_bins = bins[i];
                            float tmp_ang_bins = ang_bins[i];
                            bins[i] = bins[j];
                            ang_bins[i] = ang_bins[j];
                            bins[j] = tmp_bins;
                            ang_bins[j] = tmp_ang_bins;
                        }
                    }
                }

                /// Store bins hist in .csv ///

                float final_dir_hog = (180 - ang_bins[0]); // Delete 180 offset (add 90 for motion direction)
                line_ang_p(final_dir_hog, Point(test_p[l].x, test_p[l].y), img_out);
                store_ang.push_back(ang_bins[0]); // Store img gradient direction

                rot_img(img, -final_dir_hog, false);

                int width = 300;
                int heigth = 300;
                Rect crop_region = Rect((img.cols / 2) - (width / 2), (img.rows / 2) - (heigth / 2), width, heigth);
                img = img(crop_region);

                Scalar mean;
                Scalar std_dev;
                meanStdDev(bins, mean, std_dev, Mat());
                store_speed.push_back(std_dev[0]);
            }

            /// Mean value of store_ang ///
            float sum_ang = 0;
            float sum_speed = 0;
            for (int k = 0; k < store_ang.size(); k++)
            {
                sum_ang += store_ang[k];
                sum_speed += store_speed[k];
            }
            sum_ang /= store_ang.size();
            sum_speed /= store_speed.size();

            float final_dir_hog = (180 - sum_ang);
            line_ang_p(final_dir_hog, Point(img_out.cols / 2, img_out.rows / 2), img_out);

            // imshow("HOG direction", img_out);
            // waitKey(0);
            cout << "Final angle_hog: " << final_dir_hog << " --- Final speed_hog: " << sum_speed << endl;
            file_hog_tot << p << "," << final_dir_hog << "," << sum_speed << endl;
        }
        else if (gvs_mode == GVS_LAP_MODE)
        {
            // https://pyimagesearch.com/2015/09/07/blur-detection-with-opencv/
            // Read image
            vector<float> ker;
            vector<float> speed;
            for (int i = 1; i < 150; i++)
            {
                String path = "../../test_speed/mb_speed_" + to_string(i) + ".png";
                Mat img = imread(path, IMREAD_COLOR);
                int angl = 45;
                Mat img_out;
                rot_img(img, angl, true);
                imshow("Not Laplacian", img);
                /// Check for not corrupted data ///
                if (!img.data)
                {
                    cout << "Incorrect data frame\n";
                    return -1;
                }
                rot_img(img, -angl, false);
                int width = 300;
                int heigth = 300;
                Rect crop_region = Rect((img.cols / 2) - (width / 2), (img.rows / 2) - (heigth / 2), width, heigth);
                img = img(crop_region);

                Mat img_lap;
                cv::cvtColor(img, img, COLOR_RGB2GRAY);
                size_t elem_x_erd = 5;
                size_t elem_y_erd = 5;
                size_t elem_x_dil = 5;
                size_t elem_y_dil = 5;
                Mat element_erd = getStructuringElement(MORPH_CROSS, Size(2 * elem_x_erd + 1, 2 * elem_y_erd + 1), Point(elem_x_erd, elem_y_erd)); // Setting dilation
                Mat element_dil = getStructuringElement(MORPH_CROSS, Size(2 * elem_x_dil + 1, 2 * elem_y_dil + 1), Point(elem_x_dil, elem_y_dil)); // Setting dilation

                erode(img, img, element_erd);  // Erode
                dilate(img, img, element_dil); // Dilate
                Laplacian(img, img_lap, CV_64F, 3);

                /// PROBLEMS WITH SP NOISE ///
                /// CALCULATE MODEL ///

                Scalar mean;
                Scalar std_dev;
                meanStdDev(img_lap, mean, std_dev, Mat());
                cout << "Frame: " << i << " --- Std_dev: " << std_dev[0] << endl;
                ker.push_back(i * 1);
                speed.push_back(std_dev[0]);
                imshow("Laplacian", img_lap);
            }

            ofstream file_lap;
            file_lap.open("./data_csv/data_lap_45.csv");
            for (int i = 0; i < ker.size(); i++)
            {
                file_lap << ker[i] << "," << (speed[i]) << endl;
            }
            file_lap.close();
        }
    }
    file_hog_tot.close();
    return 0;
}