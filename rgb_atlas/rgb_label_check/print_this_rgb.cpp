#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

void print_this_rgb(const Mat& atlas, const string& name, const string& R, const string& G, const string& B)
{
    Mat canvas (atlas.rows, atlas.cols, CV_8UC3, Scalar(255,255,255));
    int r, g, b;
    istringstream(R) >> r;
    istringstream(G) >> g;
    istringstream(B) >> b;
    cout << r << g << b;
    Vec3b ref (b, g, r);
    for (int i = 0; i < atlas.rows; ++i)
        for (int j = 0; j < atlas.cols; ++j)
        {
            Vec3b intensity = atlas.at<Vec3b>(i, j);
            if (intensity == ref)
                canvas.at<Vec3b>(i, j) = ref;
        }
    stringstream ss;
    ss << name << "_" << r << "_" << g << "_" << b << ".tif";
    string s = ss.str();
    imwrite(s, canvas);
}
int main(int argc, const char** argv)
{
    if(argc != 5)
    {
        cout << "arguments should be image name, r, g, b" << endl;
        return -1;
    }
    
    Mat atlas = imread(argv[1], IMREAD_COLOR);
    string name = argv[1];
    print_this_rgb(atlas, name, argv[2], argv[3], argv[4]);
    
    
}