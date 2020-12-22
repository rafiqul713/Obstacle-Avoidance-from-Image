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





class CandidateAttribute {
public:
    int x, y, w, h;
    float total_cost;
    CandidateAttribute(int x, int y, int w, int h, float total_cost)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        this->total_cost = total_cost;
    }

    bool operator<(const CandidateAttribute& a) const {
        return total_cost < a.total_cost; 
    }

    
};




class ObjectiveFunction {
    public:
        int W1,W2,W3,W4;
        ObjectiveFunction() {
            // initialize weight factor
            W1=1.0;
            W2=1.0;
            W3=1.0;
            W4=1.0;
        }
        float obstacle_distance_cost(Mat cropped_img) {
            int width,height;
            width=cropped_img.cols;
            height=cropped_img.rows;
            int obs_dist_cost=0;
            for (int y=0;y<height;y++){
                for(int x= 0;x< width;x++){
                    int pixel_value=cropped_img.at<uchar>(y,x);
                    if(pixel_value>=0 && pixel_value<50) {
                        obs_dist_cost+=4;
                    }
                    else if(pixel_value>=50 && pixel_value<100) {
                        obs_dist_cost+=3;
                    }
                    else if(pixel_value>=100 && pixel_value<150) {
                        obs_dist_cost+=2;
                    }
                    else if(pixel_value>=150 && pixel_value<200) {
                        obs_dist_cost+=1;
                    }
                    else if(pixel_value>=200 && pixel_value<=255) {
                        obs_dist_cost+=0;
                    }
                }
            }
            return W1*(float(obs_dist_cost/float(cropped_img.cols*cropped_img.rows)));
        }
        float smooth_trajectory_cost(int crnt_x1,int crnt_y1,int crnt_w, int crnt_h,
            int prev_x1,int prev_y1,int prev_w, int prev_h) {
            float smth_traj_cost=0.0;

            int crnt_x2,crnt_y2,prev_x2,prev_y2;
            crnt_x2=crnt_x1+crnt_w;
            crnt_y2=crnt_y1+crnt_h;
            prev_x2=prev_x1+prev_w;
            prev_y2=prev_y1+prev_h;

            // X1 coordinates
            if(abs(crnt_x1-prev_x1)<=5) {
                smth_traj_cost+=0;
            }
            else if(abs(crnt_x1-prev_x1)>5 && 10<=abs(crnt_x1-prev_x1)) {
                smth_traj_cost+=2;
            }

            else if(abs(crnt_x1-prev_x1)>10 && 15<=abs(crnt_x1-prev_x1)) {
                smth_traj_cost+=3;
            }
            else {
                smth_traj_cost+=4;
            }

            // Y1 coordinates
            if(abs(crnt_y1-prev_y1)<=5) {
                smth_traj_cost+=0;
            }

            else if(abs(crnt_y1-prev_y1)>5 && 10<=abs(crnt_y1-prev_y1)) {
                smth_traj_cost+=2;
            }

            else if(abs(crnt_y1-prev_y1)>10 && 15<=abs(crnt_y1-prev_y1)) {
                smth_traj_cost+=3;
            }

            else {
                smth_traj_cost+=4;
            }

            // X2 coordinates
            if(abs(crnt_x2-prev_x2)<=5) {
                smth_traj_cost+=0;
            }

            else if(abs(crnt_x2-prev_x2)>5 && 10<=abs(crnt_x2-prev_x2)) {
                smth_traj_cost+=2;
            }

            else if(abs(crnt_x2-prev_x2)>10 && 15<=abs(crnt_x2-prev_x2)) {
                smth_traj_cost+=3;
            }

            else {
                smth_traj_cost+=4;
            }


            // Y2 coordinates
            if(abs(crnt_y2-prev_y2)<=5) {
                smth_traj_cost+=0;
            }
            
            else if(abs(crnt_y2-prev_y2)>5 && 10<=abs(crnt_y2-prev_y2)) {
                smth_traj_cost+=2;
            }

            else if(abs(crnt_y2-prev_y2)>10 && 15<=abs(crnt_y2-prev_y2)) {
                smth_traj_cost+=3;
            }

            else  {
                smth_traj_cost+=4;
            }


            return W2*(smth_traj_cost/4.0);
        }

        float maintain_the_same_height_cost() {
            return 0;
        }

        float towards_the_target_orientation_cost() {
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
        int num_of_grid=10;
        int width= image.cols;
        int height= image.rows;
        bool optimization=false;
        ObjectiveFunction obj_f;

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
        vector<CandidateAttribute>candidates;
        for(int y=horizontal_line;y<horizontal_line+(GRID_SIZE_HEIGHT*3);y+=GRID_SIZE_HEIGHT) {
            for(int x=0;x<width-GRID_SIZE_WIDTH*2;x+=GRID_SIZE_WIDTH) {
                Rect kernel(x, y,GRID_SIZE_WIDTH*3,GRID_SIZE_HEIGHT*3);
                //rectangle(image, kernel, Scalar(0, 255, 0), 1);
                Mat cropped = image(Rect(x, y,GRID_SIZE_WIDTH*3,GRID_SIZE_HEIGHT*3));            
                float obs_dist_cost=obj_f.obstacle_distance_cost(cropped);
                
                // demo coordinates for smoothness trajectory
                // issue need to solve: test for multiple frames and store the previous candidate's coordinates

                int prev_x1,prev_y1,prev_w,prev_h;
                prev_x1=x+10;//x;
                prev_y1=y+10;//y;
                prev_w=GRID_SIZE_WIDTH*3;
                prev_h=GRID_SIZE_HEIGHT*3;
                float smth_traj_cost=obj_f.smooth_trajectory_cost(x,y,GRID_SIZE_WIDTH*3,GRID_SIZE_HEIGHT*3,prev_x1,prev_y1,
                prev_w,prev_h);
                cout<<"Obtacle distance cost "<<obs_dist_cost<<endl;
                cout<<"Smooth trajectory cost "<<smth_traj_cost<<endl;
                candidates.push_back(CandidateAttribute(x,y,GRID_SIZE_WIDTH*3,GRID_SIZE_HEIGHT*3,obs_dist_cost+smth_traj_cost));
                candidate++;
                //waitKey(500);
            }
            sort(candidates.begin(),candidates.end());
        }
        Rect r = Rect(candidates[0].x, candidates[0].y,candidates[0].w, candidates[0].h);
        cout<<"Least cost "<<candidates[0].total_cost<<" Most cost "<<candidates[candidates.size()-1].total_cost<<endl;
        rectangle(image, r, Scalar(0, 0, 255), 2);
        imshow("Obstacle free grid", image);
        waitKey();
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