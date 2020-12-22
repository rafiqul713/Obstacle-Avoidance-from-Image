/*
    Author: Rafiqul Islam

*/

#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>
#include <cmath>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cmath>
#include <ctime>
using namespace cv;
using namespace std;

int num_grid = 3; // (3X3)
/*
    0  1  2 
    3  4  5 
    6  7  8  
*/

#define MIDDLE 4 //floor((num_grid*num_grid)/2)
#define RIGHT (MIDDLE + 1)
#define LEFT (MIDDLE - 1)
#define UP (MIDDLE - num_grid)
#define DOWN (MIDDLE + num_grid)

class Coordinates {
public:
    int x, y, w, h, pixel_counter;
    string direction;
    Coordinates(int x, int y, int w, int h, string direction, int pixel_counter)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        this->direction = direction;
        this->pixel_counter = pixel_counter;
    }

    bool operator<(const Coordinates& a) const
    {
        return pixel_counter < a.pixel_counter; // sort accessing order based on pixel counter
        // higher pixel counter means lesser probability of obstacle
        // lower pixel counter means higher probability of obstacle
    }

    void display()
    {
        cout << "-----------------------------------" << endl;
        cout << "Direction-> " << this->direction << endl;
        cout << "X-> " << this->x << endl;
        cout << "Y-> " << this->y << endl;
        cout << "Width-> " << this->w << endl;
        cout << "Height-> " << this->h << endl;
        cout << "Pixel counter-> " << this->pixel_counter << endl;
        cout << "-----------------------------------" << endl;
    }
};


class ObjectiveFunction {
    public:
        float obstacle_distance() {
            return 0;
        }
        float smooth_trajectory() {
            return 0;
        }

        float maintain_the_same_height() {
            return 0;
        }

        float towards_the_target_orientation() {
            return 0;
        }
};

class ObstacleAvoidanceApproach {
public:
    void approach1(Mat image)
    {
        int width = image.cols;
        int height = image.rows;
        int GRID_SIZE_WIDTH = width / num_grid;
        int GRID_SIZE_HEIGHT = height / num_grid;
        vector<Rect> grid;
        Mat grey;
        vector<Coordinates> coordinates; //store coordinates and direction of the grid
        for (int y = 0; y < height - GRID_SIZE_HEIGHT; y += GRID_SIZE_HEIGHT) {
            for (int x = 0; x < width - GRID_SIZE_WIDTH; x += GRID_SIZE_WIDTH) {
                Rect grid_rect(x, y, GRID_SIZE_WIDTH, GRID_SIZE_HEIGHT);
                cout << grid_rect << endl;
                grid.push_back(grid_rect); //store x1,y1, width, height of each grid
            }
        }
        int predefined_direction[5] = { MIDDLE, RIGHT, LEFT, UP, DOWN };
        string dir[5] = { "Middle", "Right", "Left", "Up", "Down" };
        Mat org_grey;
        cvtColor(image, org_grey, CV_BGR2GRAY); //Convert to greyscale
        //org_grey.convertTo(org_grey, CV_8U, 255.0);
        imshow("Input image ", image);
        float THRESHOLD = 100; //0=black/obstacle 255=white/no obstacle
        int counter; //number of pixel inside the grid that are greater than the threshold

        for (int direction = 0; direction < 5; direction++) {
            //at a time crop each directional part (middle, left, right, up, down) of the image
            Mat cropped = image(Rect(grid[predefined_direction[direction]].x, grid[predefined_direction[direction]].y, GRID_SIZE_WIDTH, GRID_SIZE_HEIGHT));
            counter = 0;
            cvtColor(cropped, grey, CV_BGR2GRAY, 255.0);
            grey.convertTo(grey, CV_8UC1); // convert pixel value between 0 and 255
            //imshow("Converted greyscale", grey);
            float max = -999999.0; //maximum pixel value inside the grid
            float min = 999999.0; // minimum pixel value inside the grid
            for (int i = 0; i < cropped.rows; i++) {
                for (int j = 0; j < cropped.cols; j++) {
                    if ((grey.at<uchar>(i, j)) > THRESHOLD) {
                        counter++; // count the number of pixel those are greater than theshold
                    }
                    if ((grey.at<uchar>(i, j)) > max) {
                        max = grey.at<uchar>(i, j);
                    }
                    if ((grey.at<uchar>(i, j)) < min) {
                        min = grey.at<uchar>(i, j);
                    }
                }
            }
            cout << dir[direction] << " # of pixels greater than threshold " << counter << " Max " << max << " Min " << min << endl;
            //store each of the directional part of the image
            coordinates.push_back(Coordinates(grid[predefined_direction[direction]].x, grid[predefined_direction[direction]].y, GRID_SIZE_WIDTH, GRID_SIZE_HEIGHT, dir[direction], counter));
            //coordinates[direction].display();
           
        }

        sort(coordinates.begin(), coordinates.end()); // sort accending order
        
        for(int i=0;i<coordinates.size();i++){
            coordinates[i].display();
        }
        
        //consider only one directional part those have brghter the most number of brighter pixel or obstacle free path
        Rect r = Rect(coordinates[coordinates.size() - 1].x, coordinates[coordinates.size() - 1].y, coordinates[coordinates.size() - 1].w, coordinates[coordinates.size() - 1].h);
        rectangle(image, r, Scalar(0, 0, 255), 2);
        imshow("Obstacle free grid", image);
        waitKey();
    }

    void approach2(Mat image)
    {   
        int num_of_grid=15;
        int width= image.cols;
        int height= image.rows;
        bool optimization=false;

        //divide the image with even number of grid 
        if(!optimization){
            if(width%num_of_grid!=0) {
                for(int i=1;i<width;i++){
                    if(width%(num_of_grid+i)==0){
                        num_of_grid+=i;
                        break;
                    }
                }
            }
        }
        
        vector<Rect>grid;
        int GRID_SIZE_WIDTH = floor(width / num_of_grid); 
        int GRID_SIZE_HEIGHT = floor(height / num_of_grid);
        int candidate=0;
        //int horizontal_line=(floor(num_of_grid/2)-1)*GRID_SIZE_HEIGHT;
        int horizontal_line=((height/2)-GRID_SIZE_HEIGHT*2);
        for(int y=horizontal_line;y<horizontal_line+(GRID_SIZE_HEIGHT*3);y+=GRID_SIZE_HEIGHT) {
            for(int x=0;x<width-GRID_SIZE_WIDTH*2;x+=GRID_SIZE_WIDTH) {
                Rect kernel(x, y,GRID_SIZE_WIDTH*3,GRID_SIZE_HEIGHT*3);
                rectangle(image, kernel, Scalar(0, 255, 0), 1);
                imshow("Image", image);
                candidate++;
                waitKey(500);
            }
        }
        
        cout<<"Number of candidates "<<candidate<<endl;





    }
};

const char* keyword = "{image i |<none>| input image} {approach a}";

int main(int argc, char** argv)
{
    CommandLineParser parser(argc, argv, keyword);
    ObstacleAvoidanceApproach approach;
    Mat image;
    String image_path;
    timespec ts_beg, ts_end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_beg);
    if (parser.has("image")) {
        image_path = parser.get<String>("image");
        cout << "Image path " << image_path << endl;
        image = imread(image_path, IMREAD_COLOR);
        if (image.empty()) {
            cout << "Could not read the image" << endl;
            return 0;
        }

        if (parser.has("approach")){
            string apprch= parser.get<String>("approach");
            if(apprch=="1"){
                approach.approach1(image);    
            }
            else if(apprch=="2"){
                approach.approach2(image);    
            }
            else{
                approach.approach1(image); //default
            }
        }
        else {
            cout<<"Provide approach number. For example: --approach=1"<<endl;
            
        }
    }

    else {
        cout << "Wrong command" << endl;
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_end);
    cout << (ts_end.tv_sec - ts_beg.tv_sec) + (ts_end.tv_nsec - ts_beg.tv_nsec) / 1e9 << " sec"<<endl;
    return 0;
}