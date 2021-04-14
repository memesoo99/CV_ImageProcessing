#include <iostream>
#include <opencv2/opencv.hpp>

#define IM_TYPE    CV_8UC3

using namespace cv;

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

Mat kmeans_gray_position(int clusterCount, int attempts,int sigma, Mat input);
Mat kmeans_gray(int clusterCount, int attempts, Mat input);

// Note that this code is for the case when an input data is a color value.
int main() {
    //input image
    Mat input = imread("writing.jpg", IMREAD_COLOR);
    Mat input_gray;
    cvtColor(input, input_gray, COLOR_RGB2GRAY);

    if (!input.data)
    {
        std::cout << "Could not open" << std::endl;
        return -1;
    }
    int attempts = 5;
    int clusterCount = 10;
    float sigma = 100.0;
    namedWindow("Original_RGB", WINDOW_AUTOSIZE);
    imshow("Original_RGB", input);

    namedWindow("Original_GRAY", WINDOW_AUTOSIZE);
    imshow("Original_GRAY", input_gray);

    Mat gray_pos =kmeans_gray_position(clusterCount, attempts,sigma,input_gray);
    Mat gray_ =kmeans_gray(clusterCount, attempts,input_gray);

    imshow("kmeans_gray_position", gray_pos);
    imshow("kmeans_gray", gray_);
    waitKey(0);

    return 0;
}

Mat kmeans_gray_position(int clusterCount, int attempts,int sigma, Mat input){
    Mat Normalize;
    input.convertTo(Normalize, CV_32F, 1.0/255);  //normalize
    Mat samples(Normalize.rows * Normalize.cols, 3, CV_32F);

    for (int y = 0; y < input.rows; y++) {
        for (int x = 0; x < input.cols; x++) {
            samples.at<float>(y + x * input.rows, 0) = (float)Normalize.at<float>(y,x);
            samples.at<float>(y + x * input.rows, 1) = (float)y /(float)Normalize.rows/sigma;
            samples.at<float>(y + x * input.rows, 2) = (float)x /(float)Normalize.cols/sigma;
        }
    }
    Mat labels;
    Mat centers;
    kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);

    Mat new_image = Mat::zeros(input.size(), input.type());
    for (int y = 0; y < input.rows; y++)
        for (int x = 0; x < input.cols; x++)
        {
            int cluster_idx = labels.at<int>(y + x*Normalize.rows, 0);
            new_image.at<G>(y, x) = (G)(centers.at<float>(cluster_idx, 0)*255);
        }
    return new_image;
}

Mat kmeans_gray(int clusterCount, int attempts, Mat input){
    Mat samples(input.rows * input.cols, 1, CV_32F);

    for (int y = 0; y < input.rows; y++)
        for (int x = 0; x < input.cols; x++)
            samples.at<float>(y + x*input.rows) =(float) input.at<G>(y, x);

    Mat labels;
    Mat centers;
    kmeans(samples, clusterCount, labels, TermCriteria(TermCriteria::MAX_ITER|TermCriteria::EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);

    Mat new_image(input.size(), input.type());
    for (int y = 0; y < input.rows; y++)
        for (int x = 0; x < input.cols; x++)
        {
            int cluster_idx = labels.at<int>(y + x*input.rows, 0);
            new_image.at<G>(y, x) = (G)(centers.at<float>(cluster_idx, 0));
        }
    return new_image;
}
