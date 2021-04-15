// opencv_test.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;


Mat get_Gaussian_Kernel(int n, double sigma_t, double sigma_s, bool normalize);
Mat get_Laplacian_Kernel();
Mat Gaussianfilter(const Mat input, int n, double sigma_t, double sigma_s);
Mat Laplacianfilter(const Mat input);
Mat Mirroring(const Mat input, int n);


int main() {
    Mat input = imread("lena.jpg", IMREAD_COLOR);
    Mat input_gray;

    // check for validation
    if (!input.data) {
        printf("Could not open\n");
        return -1;
    }

    //Gaussian smoothing parameters
    int window_radius = 2;
    double sigma_t = 2.0;
    double sigma_s = 2.0;

    cvtColor(input, input_gray, COLOR_RGB2GRAY);    // convert RGB to Grayscale
    input_gray.convertTo(input_gray, CV_64F, 1.0 / 255);    // 8-bit unsigned char -> 64-bit floating point

    Mat h_f = Gaussianfilter(input_gray, window_radius, sigma_t, sigma_s);    // h(x,y) * f(x,y)

    Mat Laplacian = Laplacianfilter(h_f);

    normalize(Laplacian, Laplacian, 0, 1, NORM_MINMAX);

    namedWindow("Grayscale", WINDOW_AUTOSIZE);
    imshow("Grayscale", input_gray);

    namedWindow("Gaussian blur", WINDOW_AUTOSIZE);
    imshow("Gaussian blur", h_f);

    namedWindow("Laplacian filter", WINDOW_AUTOSIZE);
    imshow("Laplacian filter", Laplacian);

    waitKey(0);

    return 0;
}



Mat Gaussianfilter(const Mat input, int n, double sigma_t, double sigma_s) {

    int row = input.rows;
    int col = input.cols;

    // generate gaussian kernel
    Mat kernel = get_Gaussian_Kernel(n, sigma_t, sigma_s, true);
    Mat output = Mat::zeros(row, col, input.type());

    //Intermediate data generation for mirroring
    Mat input_mirror = Mirroring(input, n);

    for (int i = n; i < row+n; i++) {
        for (int j = n; j < col+n; j++) {
            double sum=0.0;
            double kernelvalue;
            for (int a = -n; a <= n; a++) {
                for (int b = -n; b <= n; b++) {
                    kernelvalue = kernel.at<double>(a + n, b + n);
                    sum += kernelvalue * input_mirror.at<double>(i + a, j + b);
                }
            }
            output.at<double>(i-n,j-n) = sum;
        }
    }

    return output;
}

Mat Laplacianfilter(const Mat input) {

    int row = input.rows;
    int col = input.cols;

    Mat kernel = get_Laplacian_Kernel();
    Mat output = Mat::zeros(row, col, input.type());

    int n = 1;
    Mat input_mirror = Mirroring(input, n);

    for (int i = n; i < row+n; i++) {
            for (int j = n; j < col+n; j++) { // About all image pixels
                //Fill the code
                double sum = 0.0;
                double kernelvalue;
                for (int a = -n; a <= n; a++) {
                    for (int b = -n; b <= n; b++) { // About all kernel pixels
                        kernelvalue = kernel.at<double>(a + n, b + n);
                        sum += kernelvalue * input_mirror.at<double>(i + a, j + b);
                    }
                }
                output.at<double>(i - n, j - n) = abs(sum); //absolute value for visualization
            }
        }

    return output;
}

Mat Mirroring(const Mat input, int n) //mirroring으로 input을 padding만큼 확장
{
    int row = input.rows;
    int col = input.cols;

    Mat input2 = Mat::zeros(row + 2 * n, col + 2 * n, input.type());
    int row2 = input2.rows;
    int col2 = input2.cols;

    for (int i = n; i < row + n; i++) {
        for (int j = n; j < col + n; j++) {
            input2.at<double>(i, j) = input.at<double>(i - n, j - n);
        }
    }
    for (int i = n; i < row + n; i++) {
        for (int j = 0; j < n; j++) {
            input2.at<double>(i, j) = input2.at<double>(i, 2 * n - j);
        }
        for (int j = col + n; j < col2; j++) {
            input2.at<double>(i, j) = input2.at<double>(i, 2 * col - 2 + 2 * n - j);
        }
    }
    for (int j = 0; j < col2; j++) {
        for (int i = 0; i < n; i++) {
            input2.at<double>(i, j) = input2.at<double>(2 * n - i, j);
        }
        for (int i = row + n; i < row2; i++) {
            input2.at<double>(i, j) = input2.at<double>(2 * row - 2 + 2 * n - i, j);
        }
    }

    return input2;
}


Mat get_Gaussian_Kernel(int n, double sigma_t, double sigma_s, bool normalize) {

    int kernel_size = (2 * n + 1);
    Mat kernel = Mat::zeros(kernel_size, kernel_size, CV_64F);
    double kernel_sum = 0.0;

    for (int i = -n; i <= n; i++) {
        for (int j = -n; j <= n; j++) {
            kernel.at<double>(i + n, j + n) = exp(-((i * i) / (2.0*sigma_t * sigma_t) + (j * j) / (2.0*sigma_s * sigma_s)));
            kernel_sum += kernel.at<double>(i + n, j + n);
        }
    }

    if (normalize) {
        for (int i = 0; i < kernel_size; i++)
            for (int j = 0; j < kernel_size; j++)
                kernel.at<double>(i, j) /= kernel_sum;        // normalize
    }

    return kernel;
}

Mat get_Laplacian_Kernel() {

    Mat kernel = Mat::zeros(3, 3, CV_64F);

    kernel.at<double>(0, 1) = 1.0;
    kernel.at<double>(2, 1) = 1.0;
    kernel.at<double>(1, 0) = 1.0;
    kernel.at<double>(1, 2) = 1.0;
    kernel.at<double>(1, 1) = -4.0;

    return kernel;
}
