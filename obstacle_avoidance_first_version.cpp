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
using namespace cv;
using namespace std;

int num_grid = 3; // (3X3)
/*
    0  1  2 
    3  4  5 
    6  7  8  
*/

#define MIDDLE 4 //(num_grid/2)
#define RIGHT (MIDDLE + 1)
#define LEFT (MIDDLE - 1)
#define UP (MIDDLE - num_grid)
#define DOWN (MIDDLE + num_grid)


class Coordinates{
    public:
        int x,y,w,h, pixel_counter;
        string direction;
        Coordinates(int x,int y,int w,int h,string direction, int pixel_counter){
            this->x=x;
            this->y=y;
            this->w=w;
            this->h=h;
            this->direction=direction;
            this->pixel_counter=pixel_counter;
        }

        bool operator<(const Coordinates &a)const {
            return pixel_counter < a.pixel_counter; // sort accessing order based on pixel counter
                                                    // higher pixel counter means lesser probability of obstacle
                                                    // lower pixel counter means higher probability of obstacle
        }

        void display(){
            cout<<"-----------------------------------"<<endl;
            cout<<"Direction-> "<<this->direction<<endl;
            cout<<"X-> "<<this->x<<endl;
            cout<<"Y-> "<<this->y<<endl;
            cout<<"Width-> "<<this->w<<endl;
            cout<<"Height-> "<<this->h<<endl;
            cout<<"Pixel counter-> "<<this->pixel_counter<<endl;
            cout<<"-----------------------------------"<<endl;
 
        }
};


class ObstacleAvoidanceApproach{
    void approach1(){
        
    }
};

const char* keyword = "{image i |<none>| input image   }";

int main(int argc, char** argv)
{
    CommandLineParser parser(argc, argv, keyword);
    Mat image;
    Mat grey;
    String image_path;
    if (parser.has("image")) {
        image_path = parser.get<String>("image");
        cout << "Image path " << image_path << endl;
        image = imread(image_path, IMREAD_COLOR);
        if (image.empty()) {
            cout << "Could not read the image" << endl;
            return 0;
        }

        int width = image.cols;
        int height = image.rows;
        int GRID_SIZE_WIDTH = width / num_grid;
        int GRID_SIZE_HEIGHT = height / num_grid;
        vector<Rect> grid;
        vector<Coordinates>coordinates;

        for (int y = 0; y < height - GRID_SIZE_HEIGHT; y += GRID_SIZE_HEIGHT) {
            for (int x = 0; x < width - GRID_SIZE_WIDTH; x += GRID_SIZE_WIDTH) {
                //int k = x * y + x;
                Rect grid_rect(x, y, GRID_SIZE_WIDTH, GRID_SIZE_HEIGHT);
                cout << grid_rect << endl;
                grid.push_back(grid_rect);
                //rectangle(image, grid_rect,Scalar(0,255,0),1);
                //imshow("Input depth image", image);
                //imshow(format("grid%d",k), image(grid_rect));
                //waitKey();
                //cout<<"Grid rectange "<<grid_rect<<endl;
            }
        }

        int predefined_direction[5] = { MIDDLE, RIGHT, LEFT, UP, DOWN };
        string dir[5] = { "Middle", "Right", "Left", "Up", "Down" };
        Mat org_grey;
        cvtColor(image, org_grey, CV_BGR2GRAY); //Convert to greyscale
        //org_grey.convertTo(org_grey, CV_8U, 255.0);
        imshow("Input image ",image);
        float THRESHOLD = 100; //0=black/obstacle 255=white/no obstacle
        int counter; //number of pixel inside the grid that are greater than the threshold

        for (int direction = 0; direction < 5; direction++) {
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
                        counter++;
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
            coordinates.push_back(Coordinates(grid[predefined_direction[direction]].x, grid[predefined_direction[direction]].y, GRID_SIZE_WIDTH, GRID_SIZE_HEIGHT, dir[direction], counter));
            //coordinates[direction].display();
            //imshow("Grayscale", grey);

            //imshow(dir[direction],cropped);
            //waitKey();
        }

        //Test
        sort(coordinates.begin(),coordinates.end());
        /*for(int i=0;i<coordinates.size();i++){
            coordinates[i].display();
        }*/
        Rect r=Rect(coordinates[coordinates.size()-1].x,coordinates[coordinates.size()-1].y,coordinates[coordinates.size()-1].w,coordinates[coordinates.size()-1].h); 
        rectangle(image,r,Scalar(0,0,255),2);
        imshow("Obstacle free grid",image);
        waitKey();
        
    }
    else {
        cout << "Wrong command" << endl;
    }
}