#include <fstream>
#include <opencv2/core.hpp> 
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "coordinates.cpp"
#include <map>

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

	cv::LineIterator it(Iris, center, endLine, 4);
	for(int i = 0; i < it.count; i++, ++it){
    	line->push_back((float) **it);
	}
}


std::map<float, int>  createHistogram(std::vector<float> values, int nbins){
	double min, max;
	cv::Point min_loc, max_loc;

	cv::minMaxLoc(values, &min, &max, &min_loc, &max_loc);
	float width = (float) (max - min)/nbins;
	std::map<float, int> hist;
	//initializing histogram
	for(int i = 0; i < nbins; i++){
		//center of bin will be the value of each key
		hist[min + width*i + width/2] = 0;
	}
	for(int i = 0; i < values.size(); i++){
		for (std::map<float,int>::iterator it=hist.begin(); it!=hist.end(); ++it){
			if(abs(values[i] - it->first) <= width/2) (it->second)++;
		}
	}
	//debug histogram
	//for (std::map<float,int>::iterator it=hist.begin(); it!=hist.end(); ++it){
	//	cout << it->first << "=>" << it->second << endl;
	//}
	
	return hist;
}

void getXmaxHist(std::map<float, int> hist){
	int max = -1;
	float max_key = -1;

	for (std::map<float,int>::iterator it=hist.begin(); it!=hist.end(); ++it){
		if (it->second > max){
			max = it->second;
			max_key = it->first;
		}
	}

	cout << "Max height histogram frequency = " << max_key << endl;
}

//debbug
void printTestFFT(std::vector<float> line, cv::Mat mag, std::ofstream *line_input, std::ofstream *mag_result){
	for (std::vector<float>::iterator it=line.begin(); it!=line.end(); ++it){
		if(it != line.end()-1)
			*line_input << (float) *it << ",";
		else
			*line_input << (float) *it << endl;;
	}
	for(int i = 0; i < mag.rows; i++){
	    for(int j = 0; j < mag.cols; j++){
	    	if(i != mag.rows-1 || j != mag.cols - 1)
	    		*mag_result << (double) mag.at<double>(i,j) << ",";
	    	else
	    		*mag_result << (double) mag.at<double>(i,j) << endl;
		}
	}
}
void GetIrisFreq(cv::Mat Iris){
	//in Get Iris Region we set the height size equals to radius
	int radius = Iris.size().height;
	cv::Point center(radius, 0);
	std::vector<float> line, freq;
	double min, max;
	int min_loc, max_loc;
	cv::Mat complexI(line.size(), line.size(), CV_32F), mag;
	float alpha = 1.0f / radius;
	int samples = 0; 

	std::ofstream lineOut, magOut;
	lineOut.open("line.csv");
	magOut.open("mag_result.csv");

	for(double theta = CV_PI; theta < 2*CV_PI; theta += 0.1){
		getLineVec(Iris, &line, center, radius, theta);

	    cv::dft(line, complexI, cv::DFT_ROWS|cv::DFT_COMPLEX_OUTPUT);
	    //Make place for both the complex and the real values
	    cv::Mat planes[] = {cv::Mat::zeros(line.size(),1, CV_32F), cv::Mat::zeros(line.size(),1, CV_32F)};
	    cv::split(complexI, planes);                   // planes[0] = Re(DFT(I)), planes[1] = Im(DFT(I))

	    //debug fft
	    for(int i = 0; i < planes[0].rows; i++){
	    	for(int j = 0; j < planes[0].cols; j++){
	    		cout << (double) planes[0].at<double>(i,j) << " + " << 
	    			(double) planes[1].at<double>(i,j) << "i" << endl;
	    	}
	    }

	    cout << endl << endl << endl << endl;
	    cv::magnitude(planes[0], planes[1], planes[0]);
	    mag = planes[0];

	    max = -1;
	    max_loc = -1;
	    for(int i = 0; i < mag.rows; i++){
	    	for(int j = 0; j < mag.cols; j++){
	    		if(mag.at<double>(i,j) > max){
	    			max = mag.at<double>(i,j);
	    			max_loc = j;
	    		}
	    		//cout << "mag(" << i << ")(" << j << ") = " << (double) mag.at<double>(i,j) << endl;
	    	}
	    }
    	/*
		cv::dft(line, complexI, 0);

		//tntar com /complexI.size()
		//cv::Core.divide(1.0/radius, complexI, complexI);
		complexI.at<float>(0) = 0.0f;
		complexI = abs(complexI) * alpha;
		cv::minMaxLoc(mag, &min, &max, &min_loc, &max_loc);
		*/
		//debbug max values and freq value
		cout << (double) max << " " << (float) line.size()/(max_loc + 1) <<  endl;
		freq.push_back((float) mag.size().width/(max_loc + 1));
		
		samples++;

		//debbug FFT
		printTestFFT(line, mag, &lineOut, &magOut);
		line.clear();
	}

	lineOut.close();
	magOut.close();

	float sum_of_freq = 0;
	for(std::vector<float>::iterator it = freq.begin(); it != freq.end(); ++it)
    	sum_of_freq += *it;

    std::map<float, int> freq_hist;
    freq_hist = createHistogram(freq, 25);
    getXmaxHist(freq_hist);

	cout << "Sample mean frequency = " << (float)sum_of_freq/samples << endl;
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
	GetInputIrisRegion(input, &iris, mask);
	GetIrisFreq(iris);

	return 0;
}
