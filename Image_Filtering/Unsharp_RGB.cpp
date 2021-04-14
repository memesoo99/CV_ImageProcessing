#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>       /* exp */
#define IM_TYPE    CV_8UC3
#include <ctime>
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

Mat gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt);
Mat UnsharpMasking(const Mat input, int n, float sigmaT, float sigmaS, const char* opt, float k);

int main() {
    clock_t start, finish;
    double duration;

    Mat input = imread("lena.jpg", IMREAD_COLOR);
    Mat output;



    if (!input.data)
    {
        std::cout << "Could not open" << std::endl;
        return -1;
    }

    namedWindow("Grayscale", WINDOW_AUTOSIZE);
    imshow("Grayscale", input);

    start = clock();
    output = UnsharpMasking(input, 1,1,1, "zero-paddle",0.8); //Boundary process: zero-paddle, mirroring, adjustkernel
    finish = clock();
    namedWindow("Unsharp Filter", WINDOW_AUTOSIZE);
    imshow("Unsharp Filter", output);


    waitKey(0);

    return 0;
}


Mat UnsharpMasking(const Mat input, int n, float sigmaT, float sigmaS, const char* opt, float k) {
    Mat L;
    //std::cout<< n<<sigmaT<<sigmaS<<opt<<k;
    L = gaussianfilter(input, n,sigmaT,sigmaS, opt); //Boundary process: zero-paddle, mirroring, adjustkernel

    Mat intermediate = k*L;
    Mat output = input - intermediate;
    output = output/(1-k);
    return output;
}

Mat gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt) {

    Mat kernel;

    int row = input.rows;
    int col = input.cols;
    int kernel_size = (2 * n + 1);
    int tempa;
    int tempb;
    float denom;
    float kernelvalue;

 // Initialiazing Kernel Matrix
    kernel = Mat::zeros(kernel_size, kernel_size, CV_32F);


    denom = 0.0;
    for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
        for (int b = -n; b <= n; b++) {
            float value1 = exp(-(pow(a, 2) / (2 * pow(sigmaS, 2))) - (pow(b, 2) / (2 * pow(sigmaT, 2))));
            kernel.at<float>(a+n, b+n) = value1;
            denom += value1;
        }
    }

    for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
        for (int b = -n; b <= n; b++) {
            kernel.at<float>(a+n, b+n) /= denom;
        }
    }

    Mat output = Mat::zeros(row, col, input.type());


    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {

            float sum1_r = 0.0;
            float sum2_r = 0.0;
            float sum1_g = 0.0;
            float sum2_g = 0.0;
            float sum1_b = 0.0;
            float sum2_b = 0.0;

            if (!strcmp(opt, "zero-paddle")) {
                for (int a = -n; a <= n; a++) {
                    for (int b = -n; b <= n; b++) {

                        if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
                            kernelvalue=kernel.at<float>(a+n, b+n);
                            sum1_r += kernelvalue*(float)(input.at<C>(i + a, j + b)[2]);
                            sum1_g += kernelvalue*(float)(input.at<C>(i + a, j + b)[1]);
                            sum1_b += kernelvalue*(float)(input.at<C>(i + a, j + b)[0]);
                        }
                    }
                }
                output.at<C>(i, j)[0] = (G)sum1_b;
                output.at<C>(i, j)[1] = (G)sum1_g;
                output.at<C>(i, j)[2] = (G)sum1_r;
            }

            else if (!strcmp(opt, "mirroring")) {
                for (int a = -n; a <= n; a++) {
                    for (int b = -n; b <= n; b++) {
                        kernelvalue=kernel.at<float>(a+n, b+n);

                        if (i + a > row - 1) {  //mirroring for the border pixels row-1 =511 오른쪽으로 벗어남
                            tempa = i - a;
                        }
                        else if (i + a < 0) { //범위를 왼쪽으로 벗어남 i,j = (0,0)
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

                        sum1_r += kernelvalue*(float)(input.at<C>(tempa, tempb)[2]);
                        sum1_g += kernelvalue*(float)(input.at<C>(tempa, tempb)[1]);
                        sum1_b += kernelvalue*(float)(input.at<C>(tempa, tempb)[0]);

                    }
                }
                output.at<C>(i, j)[0] = (G)sum1_b;
                output.at<C>(i, j)[1] = (G)sum1_g;
                output.at<C>(i, j)[2] = (G)sum1_r;
            }


            else if (!strcmp(opt, "adjustkernel")) {
                for (int a = -n; a <= n; a++) {
                    for (int b = -n; b <= n; b++) {

                        float kernelvalue=kernel.at<float>(a+n, b+n);

                        for (int a = -n; a <= n; a++) { // for each kernel window
                            for (int b = -n; b <= n; b++) {
                                if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
                                    sum1_r += kernelvalue*(float)(input.at<C>(i + a, j + b)[2]);
                                    sum2_r += kernelvalue;
                                    sum1_g += kernelvalue*(float)(input.at<C>(i + a, j + b)[1]);
                                    sum2_g += kernelvalue;
                                    sum1_b += kernelvalue*(float)(input.at<C>(i + a, j + b)[0]);
                                    sum2_b += kernelvalue;
                                }
                            }
                        }
                        //output.at<G>(i, j) = (G)(sum1/sum2);
                        output.at<C>(i, j)[0] = (G)(sum1_b/sum2_b);
                        output.at<C>(i, j)[1] = (G)(sum1_g/sum2_g);
                        output.at<C>(i, j)[2] = (G)(sum1_r/sum2_r);
                    }
                }
            }
        }
    }
    return output;
}
