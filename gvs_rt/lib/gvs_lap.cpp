#include "gvs_lap.hpp"
//  https://pyimagesearch.com/2015/09/07/blur-detection-with-opencv/

float gvs_lap(cv::Mat frame, float angle)
{
    /// Rotate image to vertical ///
    rot_img(frame, -angle, true, LAP_X_CROP, LAP_Y_CROP);

    /// Morphology to mitigate noise effect ///
    Mat frame_lap;
    size_t elem_x_erd = 5;
    size_t elem_y_erd = 5;
    size_t elem_x_dil = 5;
    size_t elem_y_dil = 5;
    Mat element_erd = getStructuringElement(MORPH_CROSS, Size(2 * elem_x_erd + 1, 2 * elem_y_erd + 1), Point(elem_x_erd, elem_y_erd)); // Setting dilation
    Mat element_dil = getStructuringElement(MORPH_CROSS, Size(2 * elem_x_dil + 1, 2 * elem_y_dil + 1), Point(elem_x_dil, elem_y_dil)); // Setting dilation

    erode(frame, frame, element_erd);  // Erode
    dilate(frame, frame, element_dil); // Dilate
    Laplacian(frame, frame_lap, CV_64F, 3);

    /// Find std_dev ///
    Scalar lap_mean;
    Scalar lap_std_dev;
    meanStdDev(frame_lap, lap_mean, lap_std_dev, Mat());
    float result = lap_std_dev[0];
    return result;
}