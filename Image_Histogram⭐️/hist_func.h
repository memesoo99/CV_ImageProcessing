#pragma once

#include <opencv2/opencv.hpp>
#include <stdio.h>

#define L 256		// # of intensity levels
#define IM_TYPE	CV_8UC3

using namespace cv;

// Image Type
// "G" for GrayScale Image, "C" for Color Image
#if (IM_TYPE == CV_8UC3)
typedef uchar G;
typedef cv::Vec3b C;
#elif (IM_TYPE == CV_16SC3)
typedef short G;
typedef Vec3s C;
#elif (IM_TYPE == CV_32SC3)
typedef int G;
typedef Vec3i C;
#elif (IM_TYPE == CV_32FC3)
typedef float G;
typedef Vec3f C;
#elif (IM_TYPE == CV_64FC3)
typedef double G;
typedef Vec3d C;
#endif

// generate PDF for single channel image
float *cal_PDF(Mat &input) {

	int count[L] = { 0 }; // 0-255 intensity count할 배열 초기화
	float *PDF = (float*)calloc(L, sizeof(float)); //float 크기만큼 L개 메모리 동적할당

	// Count
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			count[input.at<G>(i, j)]++; //(y,x)
    //printf("input.at<G>(i, j) : %d ", input.at<G>(0, 1));
	// Compute PDF

    for (int i = 0; i < L; i++){
		PDF[i] = (float)count[i] / (float)(input.rows * input.cols);
    }
	return PDF;
}

// generate PDF for color image
float **cal_PDF_RGB(Mat &input) {

	int count[L][3] = { 0 };
	float **PDF = (float**)malloc(sizeof(float*) * L);

	for (int i = 0; i < L; i++)
		PDF[i] = (float*)calloc(3, sizeof(float));
    
	// Count
    for (int i = 0; i < input.rows; i++){
        for (int j = 0; j < input.cols; j++){
            C temp = input.at<C>(i,j);
            count[temp[0]][0]++; //(y,x) B
            count[temp[1]][1]++; //(y,x) G
            count[temp[2]][2]++; //(y,x) R
        }
    }
	// Compute PDF
    for (int i = 0; i < L; i++){
        PDF[i][0] = (float)count[i][0] / (float)(input.rows * input.cols);
        PDF[i][1] = (float)count[i][1] / (float)(input.rows * input.cols);
        PDF[i][2] = (float)count[i][2] / (float)(input.rows * input.cols);

    }
    
	return PDF;
}

// generate CDF for single channel image
float *cal_CDF(Mat &input) {

	int count[L] = { 0 };
	float *CDF = (float*)calloc(L, sizeof(float));

	// Count
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			count[input.at<G>(i, j)]++;

	// Compute CDF
	for (int i = 0; i < L; i++) {
		CDF[i] = (float)count[i] / (float)(input.rows * input.cols);

		if (i != 0)
			CDF[i] += CDF[i - 1];
	}

	return CDF;
}

// generate CDF for color image
float **cal_CDF_RGB(Mat &input) {

	int count[L][3] = { 0 };
	float **CDF = (float**)malloc(sizeof(float*) * L);

	for (int i = 0; i < L; i++)
		CDF[i] = (float*)calloc(3, sizeof(float));

	// Count
    for (int i = 0; i < input.rows; i++){
        for (int j = 0; j < input.cols; j++){
            C temp = input.at<C>(i,j); //BGR로 받음
            count[temp[0]][0]++; //(y,x) B
            count[temp[1]][1]++; //(y,x) G
            count[temp[2]][2]++; //(y,x) R
        }
    }

	// Compute CDF
    for (int i = 0; i < L; i++) {
        CDF[i][0] = (float)count[i][0] / (float)(input.rows * input.cols); //B
        CDF[i][1] = (float)count[i][1] / (float)(input.rows * input.cols); //G
        CDF[i][2] = (float)count[i][2] / (float)(input.rows * input.cols); //R

        if (i != 0){
            CDF[i][0]+= CDF[i - 1][0];
            CDF[i][1]+= CDF[i - 1][1];
            CDF[i][2]+= CDF[i - 1][2];}
    }

	return CDF;
}

Mat plot_CDF(float* CDF){
    Mat histImage(512, 512, CV_8U);
    Mat hist;
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, CV_32F);
    histImage = Scalar(255);
    int binW = cvRound((double)histImage.cols / L);
    int x1, y1, x2, y2;
   // int binW = 3;

    for (int i = 0; i < L; i++)
        {
            x1 = i*binW;
            y1 = histImage.rows - 400*(CDF[i]);
            x2 = (i + 1)*binW;
            y2 = histImage.rows;

            rectangle(histImage, Point(x1, y1), Point(x2, y2), Scalar(0), -1);
        }
        return histImage;
}


Mat plot_PDF(float* PDF){
    Mat histImage(512, 512, CV_8U);
    Mat hist;
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, CV_32F);
    histImage = Scalar(255);
    int binW = cvRound((double)histImage.cols / L);
    int x1, y1, x2, y2;
   // int binW = 3;

    for (int i = 0; i < L; i++)
        {
            x1 = i*binW;
            y1 = histImage.rows - 10240*(PDF[i]);
            x2 = (i + 1)*binW;
            y2 = histImage.rows;

            rectangle(histImage, Point(x1, y1), Point(x2, y2), Scalar(0), -1);
        }
    return histImage;
}
