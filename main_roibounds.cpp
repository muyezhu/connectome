#include "RoiBounds.h"

// Input supplied are image name list, and values of RGB channels. argv[0] is name of the program.
int main(int argc, char** argv) 
{
    if (argc < 2)
    {
        std::cout << "Err: atlas_img_list.txt not provided." << std::endl;
        return -1;
    }
    
    if (argc >= 2 && argc < 5 )
    {
        std::cout << "Err: rgb values are not appropriately assigned." << std::endl;
        return -1;
    }
    
   
    uchar red = std::stoi(argv[2]);
    uchar green = std::stoi(argv[3]);
    uchar blue = std::stoi(argv[4]);
    RoiBounds roi_bounds(red, green, blue); 
   
    // First line of atlas_img_list.txt is read to extract roi name, to serve as file name "roi_bounds.csv". 
    std::ifstream img_list(argv[1]);
    std::string roi = "";
    std::string csv_file_name = "";    // output csv file name.
    std::string line = "";
    std::fstream csv_file;
    if (img_list.good())
    {
        std::getline(img_list, line);
        std::istringstream ls(line);
        char ch;
        while (ls >> ch)
        {
            if (ch == '-')
                break;
            roi += ch;
        } 
    }
    csv_file_name += (roi + "_bounds.csv");
    // Create and open a blank csv file to hold struct level_bounds values.    
    csv_file.open(csv_file_name, std::fstream::out | std::fstream::trunc);
    
    while (img_list.good())
    {
      	RoiBounds::level_bounds current_level;
        std::cout << line << std::endl;
        cv::Mat img = cv::imread(line, CV_LOAD_IMAGE_COLOR);
        // load image.
        if (!img.data)
        {
            std::cout << "Err: " << line << " can not be loaded." << std::endl;
            return -1;
        }
        std::cout << "Success: " << line << " loaded."<< std::endl;
        
        // Extract ARA level.
        std::stringstream ss(line);
	char temp;
        while (ss >> temp)
	  if (temp == '-')
	    break;
        if (ss >> current_level.level)
            std::cout << "Success: ARA level extracted." << std::endl;
        else
            std::cout << "Err: ARA level cannot be extracted." << std::endl;
 
        // Obtain bounds. Current code works for right hemisphere only.
        roi_bounds.get_bounds(img, current_level);

        if (current_level.xm < 0 || current_level.xl < 0 || current_level.yd < 0 || current_level.yv < 0)
        {
            std::cout << "Err: Bounds are not properly extracted." << std::endl;
            return -1;
        }
        std::cout << "Success: Bounds extracted." << std::endl;
        roi_bounds.push_back_bounds(current_level); 
        
        if (csv_file.good())
        {
	  csv_file << current_level.level << "," << current_level.xm << ","
                    << current_level.xl << "," << current_level.yd << ","
                    << current_level.yv << "\n";
            std::cout << "Success: Printed current level info to .csv file." << std::endl;
        }
        else 
        {
            std::cout << "Err: cannot write to csv file." << std::endl;
            RoiBounds::print_level_bounds(current_level);
        }
        
        std::getline(img_list, line);
        
    }        
    return 0;
}
