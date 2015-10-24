#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

void update_lmp(cv::Mat m, int& i);
void update_lmp_level(cv::Mat m, int i, int& j);

Vec3b WHITE (255,255,255);
// blue border
Vec3b BORDER1 (227,218,191);
// green border
Vec3b BORDER2 (184,255,176);
// pink border
Vec3b BORDER3 (128,112,255);
// red border
Vec3b BORDER4 (56,68,230);
// yellow border
Vec3b BORDER5(128,240,240);

int main (int argc, char** argv) 
{
    if (argc != 2)
    {
        cout << "image name not provided" << endl;
        return -1;
    }
    
    Mat rgb_atlas = imread(argv[1]);
	if (rgb_atlas.empty())
	{
	    cout << "image is not loaded. check if image name is correct." << endl;
		return -1;
	}
    // lmp initialized as index[max].
    int lmp = rgb_atlas.cols - 1;
    update_lmp(rgb_atlas, lmp);
    int offset = rgb_atlas.cols / 2 -lmp;
    ofstream output("offset.txt", ios::out);
    output << offset;
    output.close();
    return 0;
}

// Pass by reference: update left most pixel inside function update_lmp_level
void update_lmp_level (Mat m, int i, int& lmp)
{
    int j = m.cols / 2 - 100;
    Vec3b rgb = m.at<Vec3b>(i, j);
    int lmp_current;
    while (rgb == WHITE || rgb == BORDER1 || rgb == BORDER2 || rgb == BORDER3 || rgb == BORDER4 || rgb == BORDER5)
    {   
        j++;
        rgb = m.at<Vec3b>(i, j);
		if (j == m.cols / 2 + 100)
		    return;
    }
    
    lmp_current = j;
    if (lmp_current < lmp)	
		lmp = lmp_current;
    return;
}

void update_lmp (Mat image, int& lmp)
{
    for (int i = 0; i < image.rows; i++)
        update_lmp_level(image, i, lmp);
    return;
}



