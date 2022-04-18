#include "lib/gvs_utils.hpp"
#include "lib/gvs_fd.hpp"
#include "lib/gvs_ae.hpp"
#include "lib/gvs_dft.hpp"
#include "math.h"

using namespace cv;
using namespace std;

int flag_mode = GVS_FD_MODE;

int main(int argc, char const *argv[])
{
    uint16_t frame_id = START_FRAME;

    switch (flag_mode)
    {
    case GVS_FD_MODE:
        while (frame_id < END_FRAME)
        {
            /// Import 2 frames ///
            Mat img_a = imread(merge_path(frame_id - STEP), IMREAD_COLOR);
            Mat img_b = imread(merge_path(frame_id), IMREAD_COLOR);

            /// Crop 2 frames around the center ///
            int width = img_a.cols - CROP_W;
            int heigth = img_a.rows - CROP_H;
            cv::Rect crop_region((img_a.cols / 2) - (width / 2), (img_a.rows / 2) - (heigth / 2), width, heigth);
            img_a = img_a(crop_region);
            img_b = img_b(crop_region);

            /// Check for not corrupted data ///
            if (!img_a.data || !img_b.data)
            {
                cout << "Incorrect data frame\n";
                return -1;
            }

            /// Go to grayscale ///
            Mat img_th_a;
            Mat img_bw_a;
            cv::cvtColor(img_a, img_bw_a, COLOR_RGBA2GRAY);
            equalizeHist(img_bw_a, img_bw_a);
            threshold(img_bw_a, img_th_a, 0, 255, THRESH_BINARY_INV);

            Mat img_th_b;
            Mat img_bw_b;
            cv::cvtColor(img_b, img_bw_b, COLOR_RGBA2GRAY);
            equalizeHist(img_bw_b, img_bw_b);
            threshold(img_bw_b, img_th_b, 0, 255, THRESH_BINARY_INV);

            /// Erode and dilate ///
            size_t elem_x_erd = 3;
            size_t elem_y_erd = 3;
            size_t elem_x_dil = 4;
            size_t elem_y_dil = 4;
            Mat element_erd = getStructuringElement(MORPH_ELLIPSE, Size(2 * elem_x_erd + 1, 2 * elem_y_erd + 1), Point(elem_x_erd, elem_y_erd)); // Setting dilation
            Mat element_dil = getStructuringElement(MORPH_ELLIPSE, Size(2 * elem_x_dil + 1, 2 * elem_y_dil + 1), Point(elem_x_dil, elem_y_dil)); // Setting dilation

            erode(img_th_a, img_th_a, element_erd);  // Erode
            dilate(img_th_a, img_th_a, element_dil); // Dilate

            erode(img_th_b, img_th_b, element_erd);  // Erode
            dilate(img_th_b, img_th_b, element_dil); // Dilate

            /// Find contours ///
            Mat canny_output_a;
            int thresh = 100;
            Canny(img_th_a, canny_output_a, THRESH, THRESH * 2);
            vector<vector<Point>> contours_a;
            findContours(canny_output_a, contours_a, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            for (size_t i = 0; i < contours_a.size(); i++)
            {
                Scalar color = Scalar(255, 255, 255);
                drawContours(img_b, contours_a, (int)i, color, 2);
            }

            Mat canny_output_b;
            Canny(img_th_b, canny_output_b, THRESH, THRESH * 2);
            vector<vector<Point>> contours_b;
            findContours(canny_output_b, contours_b, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            for (size_t i = 0; i < contours_b.size(); i++)
            {
                Scalar color = Scalar(0, 0, 255);
                drawContours(img_b, contours_b, (int)i, color, 2);
            }

            /// Find contour centers ///
            std::vector<cv::Point> centers_a;
            for (int i = 0; i < contours_a.size(); i++)
            {
                cv::Moments M = cv::moments(contours_a[i]);
                cv::Point center_a(M.m10 / M.m00, M.m01 / M.m00);
                centers_a.push_back(center_a);
            }

            std::vector<cv::Point> centers_b;
            for (int i = 0; i < contours_b.size(); i++)
            {
                cv::Moments M = cv::moments(contours_b[i]);
                cv::Point center_b(M.m10 / M.m00, M.m01 / M.m00);
                centers_b.push_back(center_b);
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
                        // line(img_b, centers_a[m], centers_b[j], Scalar(255, 255, 255), 1);
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
            ofstream file_feat;
            file_feat.open("data_csv/data_feat.csv");
            for (int i = 0; i < feat_vect.size(); i++)
            {
                for (int m = 0; m < feat_vect[i].size(); m++)
                {
                    dists.push_back(feat_vect[i][m].dist);
                    file_feat << feat_vect[i][m].dist << "," << feat_vect[i][m].theta << "\n";
                }
            }
            file_feat.close();
            float dists_mode = get_mode_float(dists); // Get the mode of the vector (aka choose the most common length)

            /// Rearrange the angles in a vetor ///
            ofstream file_dist_theta;
            file_dist_theta.open("data_csv/data_dt.csv");

            vector<float> thetas;
            for (int i = 0; i < feat_vect.size(); i++)
            {
                for (int m = 0; m < feat_vect[i].size(); m++)
                {
                    if ((feat_vect[i][m].dist > dists_mode - 10) && (feat_vect[i][m].dist < dists_mode + 10))
                    {
                        thetas.push_back(feat_vect[i][m].theta); // Take just the vector with amplitude around the mode
                        file_dist_theta << feat_vect[i][m].dist << "," << feat_vect[i][m].theta << '\n';
                    }
                }
            }
            file_dist_theta.close();
            float thetas_mode = get_mode_float(thetas); // Get the mode of remaining thetas
            cout << "thetas_mode: " << thetas_mode << endl;

            /// Debug arrow on image ///
            arrowedLine(img_b,
                        Point((img_b.cols / 2) + (dists_mode * cos(thetas_mode * CV_PI / 180) / 2), (img_b.rows / 2) + (dists_mode * sin(thetas_mode * CV_PI / 180) / 2)),
                        Point((img_b.cols / 2) - (dists_mode * cos(thetas_mode * CV_PI / 180) / 2), (img_b.rows / 2) - (dists_mode * sin(thetas_mode * CV_PI / 180) / 2)),
                        Scalar(0, 0, 255), 5);
            resize(img_b, img_b, Size(img_b.cols * 0.5, img_b.rows * 0.5), INTER_LINEAR);
            imshow("Frame Diff GVS", img_b);
            waitKey(0);
            frame_id++;
        }
        break;
    case GVS_AE_MODE:
        for (int l = 0; l < 1; l++)
        {
            /// Import motion blurred frame ///
            Mat img_tot = imread("./test_1/mb_2.png", IMREAD_COLOR);

            /// Testing rotation ///
            /*Point2f center((img_tot.cols - 1) / 2.0, (img_tot.rows - 1) / 2.0);
            Mat rotation_matix = getRotationMatrix2D(center, 45, 1.0);
            warpAffine(img_tot, img_tot, rotation_matix, img_tot.size());
            int width = img_tot.cols - 500;
            int heigth = img_tot.rows - 400;
            cv::Rect crop_region((img_tot.cols / 2) - (width / 2), (img_tot.rows / 2) - (heigth / 2), width, heigth);
            img_tot = img_tot(crop_region);*/

            /// Go to gray scale ///
            cv::cvtColor(img_tot, img_tot, COLOR_RGBA2GRAY);

            /// Sobel filter ///
            cv::Sobel(img_tot, img_tot, CV_8UC1, 1, 0);

            /// Find magnitude Mat ///
            Mat padded_img;
            int m = getOptimalDFTSize(img_tot.rows);
            int n = getOptimalDFTSize(img_tot.cols);
            copyMakeBorder(img_tot, padded_img, 0, m - img_tot.rows, 0, n - img_tot.cols, BORDER_CONSTANT, Scalar::all(0));
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
            magI = ideal_low_pass_filter(magI, CUT_FREQ);

            /// Find maximum on of each col of magnitude Mat ///
            vector<Point> store; // Store the magnitude max point values
            int bound = int(magI.cols / 2);
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
            for (int i = 0; i < store.size(); i++)
            {
                circle(magI, Point(int(magI.cols / 2) + i, store[i].y), 1, Scalar(255, 255, 255), 1);
            }

            ofstream file_freq;
            file_freq.open("data_csv/data_freq_2.csv");
            vector<float> store_data;
            for (int i = 0; i < magI.rows; i++)
            {
                file_freq << magI.at<float>(i, magI.rows / 2) << "," << i << "\n";
                store_data.push_back(magI.at<float>(i, magI.rows / 2));
            }
            file_freq.close();

            int xc = int(magI.cols / 2);
            int yc = int(magI.rows / 2);
            int maxp = yc;
            int store_id = 0;

            int flag_app = MEAN_ANGLE;
            float mean_ang = 0.0f;
            float len = 0.0f;

            /// Linear interpoilation between points of store ///
            if (flag_app == LINEAR)
            {
                for (int i = 0; i < store.size(); i++)
                {
                    if (store[i].y >= maxp)
                    {
                        maxp = store[i].y;
                        store_id = i;
                    }
                }
                cv::cvtColor(magI, magI, COLOR_GRAY2RGBA);
                circle(magI, Point(store[store_id].x, store[store_id].y), 2, Scalar(0, 0, 255), 2);
                circle(magI, Point(xc + (xc - store[store_id].x), yc + (yc - store[store_id].y)), 2, Scalar(0, 0, 255), 2);
                line(magI, Point(store[store_id].x, store[store_id].y), Point(xc + (xc - store[store_id].x), yc + (yc - store[store_id].y)), Scalar(0, 0, 255), 1);
            }
            /// Mean of angles between center and each store point ///
            else if (flag_app == MEAN_ANGLE)
            {
                for (int k = 0; k < store.size(); k++)
                {
                    float angle = (atan2(xc - store[k].x, yc - store[k].y)) * (180 / CV_PI);
                    mean_ang += angle;
                }
                mean_ang /= store.size();
                mean_ang += 90;
                double s = sin(mean_ang * CV_PI / 180);
                double c = cos(mean_ang * CV_PI / 180);
                Point p0(xc + c * 250, yc - s * 250);
                Point p1(xc - c * 250, yc + s * 250);

                cv::cvtColor(magI, magI, COLOR_GRAY2RGBA);
                circle(magI, p0, 2, Scalar(0, 0, 255), 2);
                circle(magI, p1, 2, Scalar(0, 0, 255), 2);
                line(magI, Point(xc, yc), p0, Scalar(0, 0, 255), 1);
                line(magI, Point(xc, yc), p1, Scalar(0, 0, 255), 1);
            }

            int id_r = int(store_data.size() / 2) + 5;
            int min_id_r = 0;
            while (store_data[id_r] >= store_data[id_r + 1])
            {
                min_id_r = id_r;
                id_r++;
            }
            int id_l = int(store_data.size() / 2) - 5;
            int min_id_l = 0;
            float ff = 0.0f;
            while (store_data[id_l] >= store_data[id_r - 1])
            {
                min_id_l = id_l;
                id_l--;
                ff = store_data[id_l - 1];
            }
            float magnitude = min_id_r - min_id_l;
            float s1 = sin((mean_ang + 90) * CV_PI / 180);
            float c1 = cos((mean_ang + 90) * CV_PI / 180);
            Point pc = Point(magI.cols / 2, (magI.rows / 2));
            Point p4(pc.x + c1 * magnitude, pc.y - s1 * magnitude);

            circle(magI, Point(magI.cols / 2, (magI.rows / 2)), 2, Scalar(255, 0, 0), 2);
            arrowedLine(magI, pc, p4, Scalar(255, 0, 0), 2);

            cout << "Mean_angle: " << mean_ang << endl;
            cout << "Magnitude: " << magnitude << endl;

            show_img(img_tot, "Space domain");
            show_img(magI, "Frequency domain");
            waitKey(0);
        }
        break;
    default:
        cout << "Set mode" << endl;
        break;
    }
    return 0;
}