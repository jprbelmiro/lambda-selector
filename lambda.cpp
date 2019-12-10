#include<iostream>
#include<cmath>
#include <opencv2/opencv.hpp>

using namespace std;

#define NUM_OF_MERIDIANS 32
#define INITIAL_RADIUS 10
#define INCREMENTAL_RADIUS 5

void drawSampleMeridians(cv::Mat img, cv::Point center, int radius){
    cv::Size axes(radius, radius);
    double angle=0;
    double startAngle=0;
    double endAngle=180;

    ellipse(img, center, axes, 0,0,180, cv::Scalar(255), 1, 8);
}
int main(){
	cv::Mat img(300, 400, CV_8UC1, cv::Scalar(0));
	cv::Point center(200,0);

	for(int i = 0; i < NUM_OF_MERIDIANS; i++){
		drawSampleMeridians(img, center, INITIAL_RADIUS + i*INCREMENTAL_RADIUS);
	}
	
	cv::namedWindow("Meridians", cv::WINDOW_AUTOSIZE );
    imshow("Meridians", img);
    
    cv::waitKey(0);

    return 0;
}

