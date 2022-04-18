#include "gvs_utils.hpp"

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

std::string merge_path(int n)
{
    String frame_path_var = var_path(n);
    return (FRAME_PATH_0 + frame_path_var + FRAME_PATH_1);
}

void show_img(Mat img, String title)
{
    // resize(img, img, Size(img.cols * 0.5, img.rows * 0.5), INTER_LINEAR);
    imshow(title, img);
    return;
}
