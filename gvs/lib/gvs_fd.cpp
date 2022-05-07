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