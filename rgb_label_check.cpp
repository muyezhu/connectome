#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

const int ROI_NUMBER = 820;
int index_list [ROI_NUMBER];
string roi_list [ROI_NUMBER];
Vec3b BLACK (0, 0, 0);
Scalar WHITE (255, 255, 255);
// Define all border color indices. When index is valid, no need to print texts for borders.
int border_index [7];

// index: rgb index. nswe, boundaries of smallest box that the (r,g,b) colored area fits in.
typedef struct roi_box
{ 
    int index, n, s, w, e;
    roi_box() {};
	roi_box(int i) : index(i), n(-1), s(-1), w(-1), e(-1) {};
	~roi_box() {};
} roi_box;

class known_rois
{
    // box_list can also contain both valid and invalid indices.
	// Our atlas image has small number of (rgb) values. If the images contain large number of rgb values, the vector implementation of rgb index
	// collection forces linear search at each pixel, making the run time very long. ajust to a linked list data structure, with each new index
	// inserted at the positions where the resulting new list remain sorted. binary search the list for queries.
	vector<roi_box> box_list;
	void update_box_(roi_box& b, int i, int j);
    public:
	    known_rois() {};
		// given rgb_index i, return corresponding box's position in vector, or -1 if not found.
		int query(int i);
		// given rgb_index i, add roi_box(i) to roi_list
		void add(int i);
		// update box boundaries.
		void update_box(int id, int i, int j);
		int list_size();
		vector<roi_box>::iterator first();
		vector<roi_box>::iterator end();
		// given position of box box_list, return a box.
		roi_box get_box(int id);
};

int known_rois::query(int i)
{
	if (box_list.size() > 0)
	{
	    int j = 0;
        vector<roi_box>::iterator iter;
		for (iter = box_list.begin(); iter < box_list.end(); ++iter, ++j)
		    if ((*iter).index == i)
                return j;
	}
	return -1;
}

void known_rois::add(int i)
{
    roi_box new_box (i);
	box_list.push_back(new_box);	
}

void known_rois::update_box(int rgb_index, int i, int j)
{
	int id = query(rgb_index); 
	assert (id >= 0);
	update_box_(box_list[id], i, j);
}

void known_rois::update_box_(roi_box& b, int i, int j)
{
    if (b.n < 0)
	    b.n = i;	
    else
	    b.n = std::min(b.n, i);
	if (b.s < 0)
	    b.s = i;
    else
	    b.s = std::max(b.s, i);
	if (b.w < 0)
	    b.w = j;
	else
	    b.w = std::min(b.w, j);
	if (b.e < 0)
	    b.e = j;
	else
	    b.e = std::max(b.e, j);
}

int known_rois::list_size()
{
    return box_list.size();
}

vector<roi_box>::iterator known_rois::first()
{
    return box_list.begin();
}

vector<roi_box>::iterator known_rois::end()
{
    return box_list.end();
}

roi_box known_rois::get_box(int id)
{
    return box_list[id];
}
void get_index(const string str, int index_list [ROI_NUMBER])
 {
     // str.c_str(), returns char*, because ifstream constructor takes char*, not string, as argument. 
     ifstream index_csv(str.c_str(), ifstream::in);
	 int i = 0;
	 string s;
	 while (index_csv.good())
	 {
	    getline(index_csv, s, ',');
		istringstream(s) >> index_list[i++];
	 }
	 index_csv.close();
	 if (i != ROI_NUMBER)
	     cout << "indices are not appropriately read" << endl;
 }
 // Note: make sure there's no "," in any rois!!
 void get_roi(const string str, string roi [ROI_NUMBER])
 {
     ifstream roi_csv(str.c_str(), ifstream::in);
	 int i = 0;
	 string s;
	 while (roi_csv.good())
	 {
	     getline(roi_csv, s, ',');
		 roi[i++] = s;
	 }
	 roi_csv.close();
	 if (i != ROI_NUMBER)
	     cout << "rois are not appropriately read" << endl;
 }
 
 int index_valid(int i)
{
    for (int j = 0; j < ROI_NUMBER; ++j)
	    if (index_list[j] == i)
		    return j;
		
    return -1;
}

 int calc_index(Vec3b c)
 {
     int r = ((int)c[2]) << 16;
	 int g = ((int)c[1]) << 8;
	 int b = c[0];
     int rgb_index = r + b + g;
	 return rgb_index;
 }
 
 void get_border_index()
 {
	 Vec3b BORDER0 (227,218,191);
     Vec3b BORDER1 (184,255,176);
     Vec3b BORDER2 (128,112,255);
     Vec3b BORDER3 (56,68,230);
     Vec3b BORDER4 (128,240,240);
	 Vec3b BORDER5 (255, 100, 255);
	 Vec3b BORDER6 (255, 255, 255);
	 
     border_index[0] = calc_index(BORDER0);
	 border_index[1] = calc_index(BORDER1);
	 border_index[2] = calc_index(BORDER2);
	 border_index[3] = calc_index(BORDER3);
	 border_index[4] = calc_index(BORDER4);
	 border_index[5] = calc_index(BORDER5);
	 border_index[6] = calc_index(BORDER6);
 }
 
 int find_y(const Mat& img, int x, int y_low, int y_high, int ref_index)
 {
     int y1 = y_low;
	 int y2 = y_high;
	 int y, rgb_index;
	 Vec3b rgb;
	 // outsider: indicate if the target pixel is of different rgb_index than ref_index
	 bool outsider = true;
	 // scattered: indicate if the roi is scattered, and at given x there's no ref_index pixel existent in roi_box.
	 bool scattered = true;
	 
	 for (int i = y_low; i <= y_high; i++)
	 {
	    rgb_index = calc_index(img.at<Vec3b>(i, x)); 
		if (rgb_index == ref_index)
		{
		    scattered = false;
			break;
		}
	 }
	 if (scattered)
	     return (y1 + y2) / 2;
		 
	 while (outsider)
	 {
	     while (y1 <= y_high)
	     {
	        rgb = img.at<Vec3b>(y1, x);
            rgb_index = calc_index(rgb);
            if (rgb_index == ref_index)
                break;
            ++y1;			
	     }
	     while (y2 >= y_low)
	     {
	        rgb = img.at<Vec3b>(y2, x);
            rgb_index = calc_index(rgb);
            if (rgb_index == ref_index)
                break;
            --y2;			
	     }
		 y = (y1 + y2) / 2;
		 rgb_index = calc_index(img.at<Vec3b>(y, x));
		 if (rgb_index == ref_index)
		     outsider = false;
	     else
		     y2 = y;
     }
	 return y;
 }
 
 bool is_border(int rgb_index)
 {
    if (rgb_index == border_index[0] || rgb_index == border_index[1] || rgb_index == border_index[2] || rgb_index == border_index[3] || rgb_index == border_index[4]) 
	    return true;
	return false;
 }
 void print_rois(Mat& img, Mat& img_orig, known_rois& k, char* argv)
 {
     vector<roi_box>::iterator iter;
	 get_border_index();
	 string filename = strcat(argv, "invalid_log.txt");
	 ofstream invalid_log (filename.c_str(), ios::app);
	 
	 for (iter = k.first(); iter < k.end(); ++iter)
	 {
		 int id = k.query((*iter).index);
		 //assert ((*iter).index == k.get_box(id).index);
	     int x = (k.get_box(id).w + k.get_box(id).e) / 2;
         int y = find_y(img_orig, x, k.get_box(id).n, k.get_box(id).s, k.get_box(id).index);
	     // print roi name if index is valid, except border colors
		 int pos = index_valid((*iter).index);
         bool scattered = (calc_index(img_orig.at<Vec3b>(y, x)) != (*iter).index);
	     if (pos > 0 && (!is_border(k.get_box(id).index)))
		 {
		     // find roi's x axis mid point. at this x location, find y value.
			 string text = "X " + roi_list[pos];
			 if (scattered)
			     text += " (S)";
		     // -5 in x coordinate is to adjust for textbox placement offset.
			 putText(img, text, Point(x - 5, y), FONT_HERSHEY_SIMPLEX, 2, Scalar(0,0,255), 3);
		 }
		 if (pos < 0)
		 {
		     string text = "X invalid";
			 if (scattered)
			     text += " (S)";
			 putText(img, text, Point(x - 5, y), FONT_HERSHEY_SIMPLEX, 2, Scalar(0,0,255), 3);	 
			 invalid_log << "invalid rgb at (" << x << ", " << y << ")\n";		 
		 }
	 }
 }
 
 
/*  (r, g, b) are calculated into unique indices with index = r << 16 + g << 8 + b.
 *   The (index, roi_name) pairs are sorted in excel and stored in 2 separate csv files.
 */
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

    // obtain arrays of rgb_index and roi names 
    get_index("index_transpose.csv", index_list);
	get_roi("roi_transpose.csv", roi_list);
	
	// traverse whole image 
	Mat contour (atlas.rows, atlas.cols, CV_8UC3, WHITE);
	known_rois i_know_u;
	Vec3b current, next;
	bool dif = false;
	int rgb_index, box_id;
	for (int i = 0; i < atlas.rows; ++i)
	{   
	    for (int j = atlas.cols / 2; j < atlas.cols; ++j)
	    {    
	        current = atlas.at<Vec3b>(i, j);
			// if current pixel is different from previous (checked in last loop), query known_rois and/or update nswe.
			if (dif)
			{
			    rgb_index = calc_index(current);
				box_id = i_know_u.query(rgb_index);
				if (box_id < 0)
        			i_know_u.add(rgb_index);
			    i_know_u.update_box(rgb_index, i, j);
			}
			
			next = atlas.at<Vec3b>(i, std::min(j + 1, atlas.cols - 1));
			dif = (current != next);
			
			// if current pixel different from next pixel, query known_rois and/or update nswe.
			if (dif)
			{
			    rgb_index = calc_index(current);
			    box_id = i_know_u.query(rgb_index);
				if (box_id < 0)
			        i_know_u.add(rgb_index);
				i_know_u.update_box(rgb_index, i, j);
				// paint 3 black pixels on contour at previous, current, and next positions.   
			    contour.at<Vec3b>(i, j) = BLACK;
			    if (j > 0)
			        contour.at<Vec3b>(i, j - 1) = BLACK;
			    if (j < atlas.cols - 1)
			        contour.at<Vec3b>(i, j + 1) = BLACK;
			}
	    }
	}
	
	// write 3 black pixels to midline.
	for (int i = 0; i < atlas.rows; ++i)
	{
	    contour.at<Vec3b>(i, atlas.cols / 2) = BLACK;
		contour.at<Vec3b>(i, atlas.cols / 2 + 1) = BLACK;
		contour.at<Vec3b>(i, atlas.cols / 2 + 2) = BLACK;
	}

    // print roi names.
	char case_name [50];
	strcpy(case_name, argv[1]);
	print_rois(contour, atlas, i_know_u, case_name);
	// copy left side atlas to contour. More efficient matrix copy methods??
	for (int i = 0; i < atlas.rows; ++i)
	    for (int j = 0; j < atlas.cols / 2; ++j)
		    contour.at<Vec3b>(i, j) = atlas.at<Vec3b>(i, j);
			
	imwrite(strcat(argv[1], "_text.tif"), contour);
    return 0;
}