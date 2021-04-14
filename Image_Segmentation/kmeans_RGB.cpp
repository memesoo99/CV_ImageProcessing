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

Mat kmeans_RGB_position(int clusterCount, int attempts,int sigma, Mat input);
Mat kmeans_RGB(int clusterCount, int attempts, Mat input);

// Note that this code is for the case when an input data is a color value.
int main() {
    //input image
    Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
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
  
    
    Mat RGB_pos =kmeans_RGB_position(clusterCount, attempts,sigma,input);
    Mat RGB_ =kmeans_RGB(clusterCount, attempts,input);

    imshow("kmeans_RGB_position", RGB_pos);
    imshow("kmeans_RGB", RGB_);
    waitKey(0);

    return 0;
}

Mat kmeans_RGB_position(int clusterCount, int attempts,int sigma, Mat input){
    Mat Normalize;
    input.convertTo(Normalize, CV_32F, 1.0/255);  //normalize
    
    //RGB + x,y 5개의 컬럼 필요
    Mat samples(Normalize.rows * Normalize.cols, 5, CV_32F);

    for (int y = 0; y < input.rows; y++) {
        for (int x = 0; x < input.cols; x++) {
            for (int z = 0; z < 3; z++)
            samples.at<float>(y + x*input.rows, z) = input.at<C>(y, x)[z];
            samples.at<float>(y + x * input.rows, 3) = (float)y / (float)input.rows / sigma;
            samples.at<float>(y + x * input.rows, 4) = (float)x / (float)input.cols / sigma;
        }
    }
    Mat labels;
    Mat centers;
    kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);
    Mat new_image = Mat::zeros(input.size(), input.type());
    for (int y = 0; y < input.rows; y++)
        for (int x = 0; x < input.cols; x++)
        {
            int cluster_idx = labels.at<int>(y + x*input.rows, 0);
            new_image.at<C>(y, x)[0] = (G)centers.at<float>(cluster_idx, 0);
            new_image.at<C>(y, x)[1] = (G)centers.at<float>(cluster_idx, 1);
            new_image.at<C>(y, x)[2] = (G)centers.at<float>(cluster_idx, 2);
        }
    return new_image;
}

Mat kmeans_RGB(int clusterCount, int attempts, Mat input){
    Mat samples(input.rows * input.cols, 3, CV_32F);

    for (int y = 0; y < input.rows; y++)
        for (int x = 0; x < input.cols; x++){
            for (int z = 0; z < 3; z++)
                samples.at<float>(y + x*input.rows, z) = input.at<C>(y, x)[z];
    }

    Mat labels;
    Mat centers;
    kmeans(samples, clusterCount, labels, TermCriteria(TermCriteria::MAX_ITER|TermCriteria::EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);

    Mat new_image(input.size(), input.type());
    for (int y = 0; y < input.rows; y++)
        for (int x = 0; x < input.cols; x++)
        {
            int cluster_idx = labels.at<int>(y + x*input.rows, 0);
            new_image.at<C>(y, x)[0] = (G)centers.at<float>(cluster_idx, 0);
            new_image.at<C>(y, x)[1] = (G)centers.at<float>(cluster_idx, 1);
            new_image.at<C>(y, x)[2] = (G)centers.at<float>(cluster_idx, 2);
        }
    return new_image;
}
