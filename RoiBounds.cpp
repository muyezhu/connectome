#include "RoiBounds.h"

// Code below considers only the right hemisphere. 

RoiBounds::RoiBounds(uchar red, uchar green, uchar blue): roi_color{blue, green, red} {};
RoiBounds::RoiBounds(): roi_color{0, 0, 0}
{
    std::cout << "Warning: no RGB values provided. Default values are 0, 0, 0." << std::endl;
};

RoiBounds::level_bounds::level_bounds(): level(0), xm(-1), xl(-1), yd(-1),yv(-1) {};

// Obtain bounds. Current code is for right hemisphere.
void RoiBounds::get_bounds(const cv::Mat& img, level_bounds& current_level)
{
    cv::Vec3b color;
    for (int y = 0; y < img.rows; y ++)
        {
            for (int x = img.cols / 2; x < img.cols; x ++)
            {
                color = img.at<cv::Vec3b>(y, x);
                if (color == RoiBounds::roi_color)
                {
                    if (current_level.xm < 0)
                        current_level.xm = x;
                    current_level.xm = std::min(current_level.xm,x);
                    // current_level.yd = y where the first pixel of indicated color is encountered.
                    if (current_level.yd < 0)
                        current_level.yd = y;
                    current_level.yv = std::max(std::max(0,y), current_level.yd);
                    break;
                }
            }
            for (int x = img.cols - 1; x >= img.cols / 2; x--)
            {
                color = img.at<cv::Vec3b>(y, x);
                if (color == RoiBounds::roi_color)
                {
                    current_level.xl = std::max(std::max(0, x), current_level.xl);
                    if (current_level.yd < 0)
                        current_level.yd = y;
                    current_level.yv = std::max(std::max(0,y), current_level.yd);
                    break;
                }
            }
        }
}

void RoiBounds::push_back_bounds(RoiBounds::level_bounds current_level)
{
    bounds.push_back(current_level);
}

void RoiBounds::print_level_bounds(const level_bounds& current_level)
{
    std::string output;
    std::string level = std::to_string(current_level.level);
    std::string xm = std::to_string(current_level.xm);
    std::string xl = std::to_string(current_level.xl);
    std::string yd = std::to_string(current_level.yd);
    std::string yv = std::to_string(current_level.yv);
    output = level + "\t" + xm + "\t" + xl + "\t" + yd + "\t" + yv + "\n";
    std::cout << output << std::endl;
}



