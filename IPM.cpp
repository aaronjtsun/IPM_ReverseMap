#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

double cot(double i)
{
	return 1 / tan(i);
}

int main( int argc, char* argv[])
{
	if (argc != 2 )
	{
		printf("usage: ./ipm <Image_Path>\n");
		return -1;
	}
	Mat src, dst;
	Mat map_u, map_v;
	
	//load input image into source map
	src = imread( argv[1], 1 );

	if ( !src.data )
	{
		printf("No image data \n");
		return -1;
	}

	//dimensions of image, 4000 x 3000 pixels
	int m = 4000, n = 3000;

	//predetermined cx cy cz camera distance from origin, measured in field test
	double cx = 0/12.0, cy = -95/12.0, cz = 61.5/12.0;

	//theta and gamma values for camera pitch and yaw respectively, values calculated in IPM_Calibration code
	double theta = 3.88398 * M_PI / 180, gamma = -0.333147 * M_PI/180;

	//horizontal and vertical FOV divided by 2, if given by manufacturer
	double alpha_h = (122.5/2)*M_PI/180;
	double alpha_v = (94.6/2)*M_PI/180;

	//if only given diagonal FOV, use this instead to approximate horizontal and vertical FOV
	/*
	double den = sqrt(pow(m-1, 2) + pow(n-1, 2));
	double alpha = 111.0 / 2.0 * M_PI / 180.0;
	double alpha_h = atan((m-1)/den * tan(alpha));
	double alpha_v = atan((n-1)/den * tan(alpha));
	*/

	//desired pixel size of output
	int tv_width = 400;
	int tv_height = 800;

	//scaling factor of output
	double Sx = 5.0;
	double Sy = 5.0;

	//create 2D arrays for u,v using output pixel size
	double** U = new double*[tv_height];
	double** V = new double*[tv_height];
	for (int i = 0; i < tv_height; ++i) {
		U[i] = new double[tv_width];
		V[i] = new double[tv_width];
	}

	//reverse map each u,v to an x,y coordinate using derived equations
	for (int r = 0; r < tv_height; ++r) {
		double yp = (tv_height - r)/Sy;
		for (int c = 0; c < tv_width; ++c) {
			double xp = (c-(tv_width-1)/2.0)/Sx;
			double atanValueU = atan((xp-cx)/(yp-cy));
			double atanValueV = atan(cz/(sqrt(pow((xp-cx),2)+pow((yp-cy),2))));
			U[r][c] = (atanValueU-gamma+alpha_h) / ((2*alpha_h)/(m-1));		
			V[r][c] = (atanValueV-theta+alpha_v) / ((2*alpha_v)/(n-1));
		}
	}

	//create maps for u and v to insert into OpenCV remap function
	map_u.create( tv_height, tv_width, CV_32FC1 );
	map_v.create( tv_height, tv_width, CV_32FC1 );
	
	//create empty map for destination image
	dst.create(tv_height, tv_width, CV_32FC1);

	//insert U and V into map_u and map_v if calculated pixel values are within range of the input image
	for (int j = 0; j < tv_height; ++j) {
		for (int i = 0; i < tv_width; ++i) {	
			float u = U[j][i];
			float v = V[j][i];
			if (u >= 0.0 && u <= (double)m && v >= 0.0 && v <= (double)n) {
				map_u.at<float>(j,i) = u;
				map_v.at<float>(j,i) = v;
			}

		}
	}

	//call remap function using src, dst, map_u, and map_v and set option for interpolation to bilinear
	remap( src, dst, map_u, map_v, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0,0));
	
	//write resulting dst image into jpg file
	std::string filename = argv[1];
	filename = filename.substr(0, filename.length()-4);
	std::ostringstream name;
	name << filename << "_" << ((int)Sx) << "x" << ((int)Sy) << "scale_" << tv_width << "x" << tv_height << ".jpg";	
	imwrite( name.str(), dst );	

	//display resulting image in GUI
	namedWindow("Display Image", WINDOW_AUTOSIZE);
	imshow("Display Image", dst);
	waitKey(0);

	return 0;
}
