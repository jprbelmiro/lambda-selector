#include<stdio.h>
#include<iostream>
#include <opencv2/core.hpp> 
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "coordinates.cpp"

using namespace std;

#define RADIUS 185
#define CENTER_X 328
#define CENTER_Y 273

void GetIrisRegion(cv::Mat *mask, int radius){
	*mask = cv::Mat::zeros(radius, 2*radius, CV_8UC1);
    cv::Size axes(radius, radius);
    cv::Point center(radius, 0);
    double angle=0;
    double startAngle=0;
    double endAngle=180;


    ellipse(*mask, center, axes, angle, startAngle, endAngle, cv::Scalar(255), -1, 8);
}

void GetInputIrisRegion(cv::Mat input, cv::Mat *output, cv::Mat mask){
	output->create(mask.size(), mask.type());

	cv::Point offset(CENTER_X-RADIUS, CENTER_Y);
	for(int j = 0; j < output->rows; j++){
		for(int i = 0; i < output->cols; i++){
			if(mask.at<uchar>(j,i) == 255)
				output->at<uchar>(j,i) = input.at<uchar>(offset.y + j, offset.x + i);
			else 
				output->at<uchar>(j,i) = 0;
		} 
	}
	//cv::threshold(*output, *output, 127, 255, cv::THRESH_BINARY_INV);
}

void getLineVec(cv::Mat Iris, std::vector<float> *line, cv::Point center, int radius, double theta){
	Polar pol(radius, theta);
	Cartesian cart = pol.toCartesian();
	cv::Point endLine(center.x - cart.x, center.y - cart.y);

	cv::LineIterator it(Iris, center, endLine, 8);
	for(int i = 0; i < it.count; i++, ++it){
    	line->push_back((float) **it);
	}
}

void GetIrisFreq(cv::Mat Iris){
	//in Get Iris Region we set the height size equals to radius
	int radius = Iris.size().height;
	cv::Point center(radius, 0);
	std::vector<float> line;
	double min, max;
	cv::Point min_loc, max_loc;
	cv::Mat complexI(line.size(), line.size(), CV_32F);
	float alpha = 1.0f / radius, freq;
	int optLen, samples = 0; 

	for(double theta = CV_PI; theta < 2*CV_PI; theta += 0.1){
		getLineVec(Iris, &line, center, radius, theta);
		optLen = cv::getOptimalDFTSize(line.size());
		cv::dft(line, complexI, 0);

		//tntar com /complexI.size()
		//cv::Core.divide(1.0/radius, complexI, complexI);
		complexI.at<float>(0) = 0.0f;
		complexI = abs(complexI) * alpha;

		cv::minMaxLoc(complexI, &min, &max, &min_loc, &max_loc);
		cout <<  max_loc.x << endl;
		freq += line.size()/(max_loc.x);
		cout << "freq (" << samples + 1 << ") = " << radius/(max_loc.x)<< endl;

		samples++;
		line.clear();
	}

	cout << "Estimate frequency = " << freq/samples << endl;

	//changes here
	/*

	cv::Mat padded;                            //expand input image to optimal size
	int m = cv::getOptimalDFTSize( Iris.rows );
	int n = cv::getOptimalDFTSize( Iris.cols ); // on the border add zero values
	//cv::copyMakeBorder(output, padded, 0, m - output.rows, 0, n - output.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

	cv::Mat planes[] = { cv::Mat_<float>(Iris), cv::Mat::zeros(Iris.size(), CV_32F)};

	cv::Mat complexI;
  	cv::merge(planes, 2, complexI);
	cv::dft(complexI, complexI);//, cv::DFT_COMPLEX_OUTPUT
	cv::split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	cv::magnitude(planes[0], planes[1], planes[0]);
    
	cv::Mat magI = planes[0];
	//phase += cv::Scalar::all(1); 

	cout << magI.rows << " " << magI.cols << endl;

	double min, max;
	cv::Point min_loc, max_loc;


	std::vector<int> magcol;
	for(int i = 0; i < magI.cols; i++){
		magcol.push_back(magI.at<double>(i,170));
		cout << magI.at<float>(i,170)<< "---";
	}

	cv::minMaxLoc(magcol, &min, &max, &min_loc, &max_loc);
	cout <<  180 / max << endl;
	cout << max_loc << endl;
	*/
}

string type2str(int type) {
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch ( depth ) {
		case CV_8U:  r = "8U"; break;
		case CV_8S:  r = "8S"; break;
		case CV_16U: r = "16U"; break;
		case CV_16S: r = "16S"; break;
		case CV_32S: r = "32S"; break;
		case CV_32F: r = "32F"; break;
		case CV_64F: r = "64F"; break;
		default:     r = "User"; break;
	}

	r += "C";
	r += (chans+'0');

	return r;
}


int main(int argc, char **argv){
	cv::Mat input;
	cv::Mat iris;
	cv::Mat mask;
	
	//need to resize center
	cv::Point center(CENTER_X, CENTER_Y);

	//Input image and input type for debugging
	if(argc == 2){
		input = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
		cout << type2str(input.type()) << '\n';
	} else {
		printf("Error, usage: %s imageName\n", argv[0]);
		exit(-1);
	}

	GetIrisRegion(&mask, RADIUS);	
	imshow("interpolation.png", mask);
	cv::waitKey(0);
	GetInputIrisRegion(input, &iris, mask);
	imshow("interpolation.png", iris);
	cv::waitKey(0);
	GetIrisFreq(iris);

	return 0;
}

