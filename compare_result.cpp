#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>
#include <string>
#include <sstream>
#include <cmath>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cmath>
#include <ctime>
using namespace cv;
using namespace std;

int main() {
    string folder_1,folder_2;
    vector<cv::String>images_1,images_2;
    folder_1="/home/rafiqul/Documents/Thesis/Code/GitHub/masters_thesis/Obstacle-Avoidance-from-Image/exp1";
    folder_2="/home/rafiqul/Documents/Thesis/Code/GitHub/masters_thesis/Obstacle-Avoidance-from-Image/exp2";
    cv::glob(folder_1, images_1, false);
    cv::glob(folder_2, images_2, false);
    Mat img1,img2;
    for(int i=0;i<images_1.size();i++) {
        img1=imread(images_1[i]);
        img2=imread(images_2[i]);
        hconcat(img1,img2,img1);
        imshow("Compare  ",img1);
        waitKey();
    }

}