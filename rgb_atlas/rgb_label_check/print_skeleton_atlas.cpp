#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

Vec3b BLACK (0, 0, 0);
Scalar WHITE (255, 255, 255);

int main(int argc, char** argv)
{
    if (argc < 2)
	{
	    cout << "image name not provided" << endl;
		return -1;
	}
	
	// read image
	Mat atlas = imread(argv[1]);
	if (atlas.empty())
	{
	    cout << "image is not loaded. check if image name is correct." << endl;
		return -1;
	}
	
	// traverse whole image 
	Mat contour (atlas.rows, atlas.cols, CV_8UC3, WHITE);
	Vec3b current, next;
	bool dif = false;
	int rgb_index, box_id;
	for (int i = 0; i < atlas.rows; ++i) 
	    for (int j = 0; j < atlas.cols; ++j)
	    {    
	        current = atlas.at<Vec3b>(i, j);
			next = atlas.at<Vec3b>(i, std::min(j + 1, atlas.cols - 1));
			dif = (current != next);
			
			// if current pixel different from next pixel, paint 3 black pixels on contour at previous, current, and next positions.
			if (dif)
			{
				// paint 3 black pixels on contour at previous, current, and next positions.   
			    contour.at<Vec3b>(i, j) = BLACK;
			    if (j > 0)
			        contour.at<Vec3b>(i, j - 1) = BLACK;
			    if (j < atlas.cols - 1)
			        contour.at<Vec3b>(i, j + 1) = BLACK;
			}
	    }
	
	// write 3 black pixels to midline.
	for (int i = 0; i < atlas.rows; ++i)
	{
	    contour.at<Vec3b>(i, atlas.cols / 2) = BLACK;
		contour.at<Vec3b>(i, atlas.cols / 2 + 1) = BLACK;
		contour.at<Vec3b>(i, atlas.cols / 2 + 2) = BLACK;
	}

    imwrite(strcat(argv[1], "_skeleton.tif"), contour);
    return 0;
}