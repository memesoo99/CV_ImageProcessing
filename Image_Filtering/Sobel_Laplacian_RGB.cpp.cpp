#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>       /* exp */
#define IM_TYPE    CV_8UC3

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

Mat sobelfilter(const Mat input);
Mat laplacianfilter(const Mat input) ;

int main() {

    Mat input = imread("lena.jpg", IMREAD_COLOR);
    Mat output;

    if (!input.data)
    {
        std::cout << "Could not open" << std::endl;
        return -1;
    }

    namedWindow("Grayscale", WINDOW_AUTOSIZE);
    imshow("Grayscale", input);

    output = sobelfilter(input); //Boundary process: zero-paddle, mirroring, adjustkernel

    namedWindow("Sobel Filter", WINDOW_AUTOSIZE);
    imshow("Sobel Filter", output);

    output = laplacianfilter(input); //Boundary process: zero-paddle, mirroring, adjustkernel

    namedWindow("Laplacian Filter", WINDOW_AUTOSIZE);
    imshow("Laplacian Filter", output);

    waitKey(0);

    return 0;
}


Mat sobelfilter(const Mat input) {

    int row = input.rows;
    int col = input.cols;
    int n = 1; // Sobel Filter Kernel N
    int tempa;
    int tempb;
    int Sxvalue;
    int Syvalue;

    // Initialiazing 2 Kernel Matrix with 3x3 size for Sx and Sy
    Mat_<int> Sx_(3,3);
    Mat_<int> Sy_(3,3);

    Sx_ << -1,0,1,-2,0,2,-1,0,1;
    Sy_ << -1,-2,-1,0,0,0,1,2,1;

    Mat output = Mat::zeros(row, col, input.type());

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            float sum1_r = 0.0;
            float sum1_g = 0.0;
            float sum1_b = 0.0;
            float sum2_r = 0.0;
            float sum2_g = 0.0;
            float sum2_b = 0.0;

            for (int a = -n; a <= n; a++) {
                for (int b = -n; b <= n; b++) {
                    // Use mirroring boundary process
                    // Find output M(x,y) = sqrt( input.at<G>(x, y)*Sx + input.at<G>(x, y)*Sy )
                    Sxvalue=Sx_.at<int>(a+n, b+n);
                    Syvalue=Sy_.at<int>(a+n, b+n);

                    if (i + a > row - 1) {  //mirroring for the border pixels row-1 =511 오른쪽으로 벗어남
                        tempa = i - a;
                    }
                    else if (i + a < 0) { //범위를 왼쪽으로 벗어남 i,j = (0,0), n=1 ?????????
                        tempa = -(i + a);
                    }
                    else { //정상
                        tempa = i + a;
                    }

                    if (j + b > col - 1) {//범위를 아래로 벗어남
                        tempb = j - b;
                    }
                    else if (j + b < 0) { //범위를 위로 벗어남
                        tempb = -(j + b);
                    }
                    else {
                        tempb = j + b;
                    }
                    sum1_r += Sxvalue*(float)(input.at<C>(tempa, tempb)[2]);
                    sum1_g += Sxvalue*(float)(input.at<C>(tempa, tempb)[1]);
                    sum1_b += Sxvalue*(float)(input.at<C>(tempa, tempb)[0]);

                    sum2_r += Syvalue*(float)(input.at<C>(tempa, tempb)[2]);
                    sum2_g += Syvalue*(float)(input.at<C>(tempa, tempb)[1]);
                    sum2_b += Syvalue*(float)(input.at<C>(tempa, tempb)[0]);

                }

            }
            output.at<C>(i, j)[2] = (G)sqrt(sum1_r*sum1_r + sum2_r*sum2_r);
            output.at<C>(i, j)[1] = (G)sqrt(pow(sum1_g,2)+pow(sum2_g,2));
            output.at<C>(i, j)[0] = (G)sqrt(pow(sum1_b,2)+pow(sum2_b,2));

        }
    }
    return output;
}

Mat laplacianfilter(const Mat input) {

    int row = input.rows;
    int col = input.cols;
    int n = 1; // Sobel Filter Kernel N
    int tempa;
    int tempb;
    int kernelvalue;


    // Initialiazing 2 Kernel Matrix with 3x3 size for Sx and Sy
    //Fill code to initialize Sobel filter kernel matrix for Sx and Sy (Given in the lecture notes)

    Mat_<int> kernel(3,3);
    kernel << -1,0,1,-2,0,2,-1,0,1;
    Mat output = Mat::zeros(row, col, input.type());

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            float sum1_r = 0.0;
            float sum1_g = 0.0;
            float sum1_b = 0.0;

            for (int a = -n; a <= n; a++) {
                for (int b = -n; b <= n; b++) {

                    kernelvalue=kernel.at<int>(a+n, b+n);

                    if (i + a > row - 1) {  //mirroring for the border pixels row-1 =511 오른쪽으로 벗어남
                        tempa = i - a;
                    }
                    else if (i + a < 0) { //범위를 왼쪽으로 벗어남 i,j = (0,0), n=1 ?????????
                        tempa = -(i + a);
                    }
                    else { //정상
                        tempa = i + a;
                    }

                    if (j + b > col - 1) {//범위를 아래로 벗어남
                        tempb = j - b;
                    }
                    else if (j + b < 0) { //범위를 위로 벗어남
                        tempb = -(j + b);
                    }
                    else {
                        tempb = j + b;
                    }
                    sum1_b += kernelvalue*(float)(input.at<C>(tempa, tempb)[0]);
                    sum1_g += kernelvalue*(float)(input.at<C>(tempa, tempb)[1]);
                    sum1_r += kernelvalue*(float)(input.at<C>(tempa, tempb)[2]);

                }

            }
            output.at<C>(i, j)[0] = (G)sum1_b;
            output.at<C>(i, j)[1] = (G)sum1_g;
            output.at<C>(i, j)[2] = (G)sum1_r;
        }
    }
    return output;
}
