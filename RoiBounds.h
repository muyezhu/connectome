#ifndef ROIBOUNDS_H
#define	ROIBOUNDS_H

#include <stdlib.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class RoiBounds
{
    public:
        // Data type that holds: ARA level, M-L and D-V boundaries.
        typedef struct level_bounds
        {
	    int level, xm, xl, yd, yv;
	    level_bounds();
        } level_bounds;
        cv::Vec3b roi_color;
        void get_bounds (const cv::Mat& img, level_bounds& current_level);
        void push_back_bounds (level_bounds current_level);
        static void print_level_bounds (const level_bounds& current_level);
        RoiBounds(uchar red, uchar green, uchar blue);
        RoiBounds();
     private:
        // Data type that hold level_bounds from all levels of ROI in a vector.
        std::vector<level_bounds> bounds;
                
};


#endif	/* ROIBOUNDS_H */

