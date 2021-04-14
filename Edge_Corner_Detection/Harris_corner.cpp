// opencv_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include <iostream>
#include <string>
#include <stdlib.h>

using namespace cv;
using namespace std;

vector<Point2f> MatToVec(const Mat input);
Mat NonMaximum_Suppression(const Mat input, Mat corner_mat, int radius);
Mat Mirroring(const Mat input, int n);
void type2str(int type);


int main() {

	//Use the following three images.
//	Mat input = imread("checkerboard.png", IMREAD_COLOR);
//	Mat input = imread("checkerboard2.jpg", IMREAD_COLOR);
    Mat input = imread("lena.jpg", IMREAD_COLOR);

	// check for validation
	if (!input.data) {
		printf("Could not open\n");
		return -1;
	}

	int row = input.rows;
	int col = input.cols;

	Mat input_gray, input_visual;
	Mat output, output_norm, corner_mat;
	vector<Point2f> points;

	corner_mat = Mat::zeros(row, col, CV_8U);

	//Option for the non-maximum suppression
	//Compare the result when 'true' or 'false'
	bool NonMaxSupp = false;

	//Option for subpixel refinement in corner detection,
	//Compare the result when 'true' or 'false'
	bool Subpixel = true;


	cvtColor(input, input_gray, COLOR_RGB2GRAY);	// convert RGB to Grayscale

	//Harris corner detection using 'cornerHarris'
	//Note that 'src' of 'cornerHarris' can be either 1) input single-channel 8-bit or 2) floating-point image.

    cornerHarris(input_gray, output, 3, 3, 0.01, BORDER_DEFAULT);

	//Scale the Harris response map 'output' from 0 to 1.
	//This is for display purpose only.
	normalize(output, output_norm, 0, 1.0, NORM_MINMAX);
	namedWindow("Harris Response", WINDOW_AUTOSIZE);
	imshow("Harris Response", output_norm);


	//Threshold the Harris corner response.
	//corner_mat = 1 for corner, 0 otherwise.
	input_visual = input.clone();
	double minVal, maxVal;		Point minLoc, maxLoc;
	minMaxLoc(output, &minVal, &maxVal, &minLoc, &maxLoc);

	int corner_num = 0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (output.at<float>(i, j) > 0.01*maxVal)
			{
				//input_visual.at<Vec3b>(i, j)[0] = 0;		input_visual.at<Vec3b>(i, j)[1] = 0;	input_visual.at<Vec3b>(i, j)[2] = 255;
				circle(input_visual, Point(j, i), 2, Scalar(0, 0, 255), 1, 8, 0);	//You can also use this function of drawing a circle. For details, search 'circle' in OpenCV.

				corner_mat.at<uchar>(i, j) = 1;
				corner_num++;
			}

			else
				output.at<float>(i, j) = 0.0;
		}
	}
	printf("After cornerHarris, corner number = %d\n\n", corner_num);
	namedWindow("Harris Corner", WINDOW_AUTOSIZE);
	imshow("Harris Corner", input_visual);

	//Non-maximum suppression
	if (NonMaxSupp)
	{
		NonMaximum_Suppression(output, corner_mat, 2);

		corner_num = 0;
		input_visual = input.clone();
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				if (corner_mat.at<uchar>(i, j) == 1) {
					//input_visual.at<Vec3b>(i, j)[0] = 0;		input_visual.at<Vec3b>(i, j)[1] = 0;	input_visual.at<Vec3b>(i, j)[2] = 255;
					circle(input_visual, Point(j, i), 2, Scalar(0, 0, 255), 1, 8, 0);	//You can also use this function of drawing a circle. For details, search 'circle' in OpenCV.
					corner_num++;
				}
			}
		}

		printf("After non-maximum suppression, corner number = %d\n\n", corner_num);
		namedWindow("Harris Corner (Non-max)", WINDOW_AUTOSIZE);
		imshow("Harris Corner (Non-max)", input_visual);
	}

	//Sub-pixel refinement for detected corners
	if (Subpixel)
	{
		Size subPixWinSize(3, 3);
		TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 20, 0.03);

		points = MatToVec(corner_mat);

		//Fill the code///////////////////////////////////////////////////////////

        cornerSubPix(output, points, subPixWinSize, Size(-1, -1), termcrit);

		//Display the set of corners
		input_visual = input.clone();
		for (int k = 0; k < points.size(); k++) {

			int x = points[k].x;
			int y = points[k].y;

			if (x<0 || x>col - 1 || y<0 || y>row - 1)
			{
				points.pop_back();
				continue;
			}

			//input_visual.at<Vec3b>(i, j)[0] = 0;		input_visual.at<Vec3b>(i, j)[1] = 0;	input_visual.at<Vec3b>(i, j)[2] = 255;
			circle(input_visual, Point(x, y), 2, Scalar(0, 0, 255), 1, 8, 0);	//You can also use this function of drawing a circle. For details, search 'circle' in OpenCV.
		}

		printf("After subpixel-refinement, corner number = %d\n\n", points.size());
		namedWindow("Harris Corner (subpixel)", WINDOW_AUTOSIZE);
		imshow("Harris Corner (subpixel)", input_visual);
	}

	waitKey(0);

	return 0;
}

vector<Point2f> MatToVec(const Mat input)
{
	vector<Point2f> points;

	for (int i = 0; i < input.rows; i++) {
		for (int j = 0; j < input.cols; j++) {
			if (input.at<uchar>(i, j) == 1) {
				points.push_back(Point2f((float)j, (float)i));
			}
		}
	}

	return points;
}

//corner_mat = 1 for corner, 0 otherwise.
Mat NonMaximum_Suppression(const Mat input, Mat corner_mat, int radius)
{
	int row = input.rows;
	int col = input.cols;

	Mat input_mirror = Mirroring(input, radius);

    int corner;

	for (int i = radius; i < row+radius; i++) {
		for (int j = radius; j < col+radius; j++) {
            corner = 1;
            if (corner_mat.at<uchar>(i - radius, j - radius) != 0) { //corner_mat = 1 for corner, 0 otherwise.
                for (int a = -radius; a <= radius; a++) {
                    for (int b = -radius; b <= radius; b++) { //kernel
                        if (input_mirror.at<float>(i - radius, j - radius) < input_mirror.at<float>(i + a, j + b)) {
                            corner = 0;
                        }
                    }
                }
            }
            if (corner == 0) { // not a corner
                corner_mat.at<uchar>(i - radius, j - radius) = 0;
            }
            

		}
	}

	return input;
}

Mat Mirroring(const Mat input, int n)
{
	int row = input.rows;
	int col = input.cols;

	Mat input2 = Mat::zeros(row + 2 * n, col + 2 * n, input.type());
	int row2 = input2.rows;
	int col2 = input2.cols;

	for (int i = n; i < row + n; i++) {
		for (int j = n; j < col + n; j++) {
			input2.at<float>(i, j) = input.at<float>(i - n, j - n);
		}
	}
	for (int i = n; i < row + n; i++) {
		for (int j = 0; j < n; j++) {
			input2.at<float>(i, j) = input2.at<float>(i, 2 * n - j);
		}
		for (int j = col + n; j < col2; j++) {
			input2.at<float>(i, j) = input2.at<float>(i, 2 * col - 2 + 2 * n - j);
		}
	}
	for (int j = 0; j < col2; j++) {
		for (int i = 0; i < n; i++) {
			input2.at<float>(i, j) = input2.at<float>(2 * n - i, j);
		}
		for (int i = row + n; i < row2; i++) {
			input2.at<float>(i, j) = input2.at<float>(2 * row - 2 + 2 * n - i, j);
		}
	}

	return input2;
}


//If you want to know the type of 'Mat', use the following function
//For instance, for 'Mat input'
//type2str(input.type());

void type2str(int type) {
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch (depth) {
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
	r += (chans + '0');

	printf("Matrix: %s \n", r.c_str());
}
