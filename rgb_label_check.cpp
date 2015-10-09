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
vector<int> index_list;
string roi_list [ROI_NUMBER];
Vec3b BLACK (0, 0, 0);
Scalar WHITE (255, 255, 255);
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
		// check if i is in either roi_list or invalid_list. return position if found, -1 if not found.
		int query(int i);
		// add roi_box(i) to roi_list
		void add(int i);
		// for i already in roi_list, update the nswe values.
		bool index_valid(int i);
		void update_box(int id, int i, int j);
		int list_size();
};

bool known_rois::index_valid(int i)
{
    if (binary_search(index_list.begin(), index_list.end(), i))
	    return true;
		
    return false;
}

int known_rois::query(int i)
{
	if (box_list.size() > 0)
	{
	    int j = 0;
        vector<roi_box>::iterator iter;
		for (iter = box_list.begin(); iter < box_list.end(); ++iter, ++j)
		{
		    if ((*iter).index == i)
                return j;
		}
	}
	return -1;
}

void known_rois::add(int i)
{
    roi_box new_box (i);
	box_list.push_back(new_box);	
}

void known_rois::update_box(int id, int i, int j)
{
    assert (id >= 0);
	roi_box box = box_list[id]; 
	update_box_(box, i, j);
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

void get_index(const string str, vector<int> index_list)
 {
     // str.c_str(), returns char*, because ifstream constructor takes char*, not string, as argument. 
     ifstream index_csv(str.c_str(), ifstream::in);
	 int i = 0;
	 int count = 0;
	 string s;
	 while (index_csv.good())
	 {
	    getline(index_csv, s, ',');
		istringstream(s) >> i;
		index_list.push_back(i);
		count++;
	 }
	 index_csv.close();
	 if (count != ROI_NUMBER)
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
 
 int calc_index(Vec3b c)
 {
     int r = ((int)c[2]) << 16;
	 int g = ((int)c[1]) << 8;
	 int b = c[0];
     int rgb_index = r + b + g;
 }
 
 void print_rois(const Mat& img, const know_rois& k)
 {
     vector<roi_box>::iterator iter;
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
	Mat contour (atlas.rows, atlas.cols, CV_8UC3, WHITE);
	imwrite("blank.tif", contour);
	if (atlas.empty())
	{
	    cout << "image is not loaded. check if image name is correct." << endl;
		return -1;
	}
	
    // obtain arrays of rgb_index and roi names 
    get_index("index_transpose.csv", index_list);
	get_roi("roi_transpose.csv", roi_list);
	
	// traverse whole image 
	known_rois i_know_u;
	Vec3b current, next;
	bool dif = false;
	int rgb_index, box_id;
	for (int i = 0; i < atlas.rows; i++)
	{   cout << i << endl;
	    for (int j = 0; j < atlas.cols; j++)
	    {    
	        current = atlas.at<Vec3b>(i, j);
			// if current pixel is different from previous (checked in last loop), query known_rois and/or update nswe.
			if (dif)
			{
			    rgb_index = calc_index(current);
				box_id = i_know_u.query(rgb_index);
				if (box_id < 0)
				{
				    i_know_u.add(rgb_index);
					box_id = i_know_u.list_size() - 1;
				}
			    i_know_u.update_box(box_id, i, j);
			}
			
			next = atlas.at<Vec3b>(i, std::min(j + 1, atlas.cols - 1));
			dif = (current != next);
			
			// if current pixel different from next pixel, query known_rois and/or update nswe.
			if (dif)
			{
			    rgb_index = calc_index(current);
			    box_id = i_know_u.query(rgb_index);
				if (box_id < 0)
				{
				    i_know_u.add(rgb_index);
					box_id = i_know_u.list_size() - 1;
				}
				i_know_u.update_box(box_id, i, j);
				// paint 3 black pixels on contour at previous, current, and next positions.   
			    atlas.at<Vec3b>(i, j) = BLACK;
			    if (j > 0)
			        contour.at<Vec3b>(i, j - 1) = BLACK;
			    if (j < atlas.cols - 1)
			        contour.at<Vec3b>(i, j + 1) = BLACK;
			}
	    }
	}
	// print rois on contour image.
	print_roi(contour,i_know_u);
	
	imwrite(strcat(argv[1], "_contour.tif"), contour);
	cout << "box_list size: " << i_know_u.list_size() << endl;
    return 0;
}