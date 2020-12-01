#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>
#include<cmath>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
using namespace std;


int num_grid=3; // (3X3)
/*
	0  1  2 
	3  4  5 
	6  7  8  
*/

#define MIDDLE 4 //(num_grid/2)
#define RIGHT  (MIDDLE+1)
#define LEFT   (MIDDLE-1) 
#define UP	   (MIDDLE-num_grid)
#define DOWN   (MIDDLE+num_grid)


const char* keyword = "{image i |<none>| input image   }";


int main(int argc, char** argv){
	CommandLineParser parser(argc, argv, keyword);
	Mat image;
	Mat grey;
	String image_path;
	if (parser.has("image")){
    	image_path = parser.get<String>("image");
    	cout<<"Image path "<<image_path<<endl;
    	image = imread(image_path, IMREAD_COLOR);
    	if(image.empty()){
    		cout<<"Could not read the image"<<endl;
    		return 0;
    	}

  
    	int width = image.cols;
		int height = image.rows;
		int GRID_SIZE_WIDTH = width/num_grid;
		int GRID_SIZE_HEIGHT = height/num_grid;
		vector<Rect> grid;

		for (int y=0;y<height-GRID_SIZE_HEIGHT;y+=GRID_SIZE_HEIGHT){
    		for(int x=0;x<width-GRID_SIZE_WIDTH;x+=GRID_SIZE_WIDTH){
        		int k = x*y + x;
        		Rect grid_rect(x, y,GRID_SIZE_WIDTH,GRID_SIZE_HEIGHT);
        		cout << grid_rect<< endl;
        		grid.push_back(grid_rect);
        		//rectangle(image, grid_rect,Scalar(0,255,0),1);
        		//imshow("Input depth image", image);
        		//imshow(format("grid%d",k), image(grid_rect));
        		//waitKey();
        		//cout<<"Grid rectange "<<grid_rect<<endl;
    		}
    	
    		
		}

		int predefined_direction[5]= {MIDDLE,RIGHT,LEFT,UP,DOWN};
		string dir[5]={"Middle","Right","Left","Up","Down"};
		Mat org_grey;
		cvtColor(image,org_grey, CV_BGR2GRAY);
		//org_grey.convertTo(org_grey, CV_8U, 255.0);
        imshow("Original image ",org_grey);
		float THRESHOLD=100;
		int counter;
		
		for(int direction=0;direction<5;direction++){
			Mat cropped=image(Rect(grid[predefined_direction[direction]].x,grid[predefined_direction[direction]].y,GRID_SIZE_WIDTH,GRID_SIZE_HEIGHT));
    		counter=0;
    		Mat grey2;//=Mat::zeros(cropped.cols,cropped.rows, CV_8U CV_64F);
    		cvtColor(cropped,grey, CV_BGR2GRAY,255.0);
            grey.convertTo(grey2,CV_8UC1);
            imshow("Here",grey2);
            float max=-999999.0;
    		float min=999999.0;
    		for(int i=0;i<cropped.rows;i++){
    			for(int j=0;j<cropped.cols;j++){
    				//cout<<cropped.at<float>(i,j)<<endl;
    				if((grey2.at<uchar>(i,j))>THRESHOLD){
    					cout<<"Pixel "<<grey2.at<float>(i,j)<<" = "<<grey.at<float>(i,j)<<endl;
                        counter++;
    				}
    				if((grey2.at<uchar>(i,j))>max){
    					max=grey2.at<uchar>(i,j);
    				}
    				if((grey2.at<uchar>(i,j))<min){
    					min=grey2.at<uchar>(i,j);
    				}
    				
    			}
    		}
    		cout<<dir[direction]<<" # of pixels greater than threshold "<<counter<<" Max "<<max<<" Min "<<min<<endl;
    		imshow("Grayscale",grey);
    		
		
    		//imshow(dir[direction],cropped);
    		waitKey();
		
		}
	
    }
    else{
    	cout<<"Wrong command"<<endl;
    }
    

}