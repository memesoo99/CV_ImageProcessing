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
void denom_operation(int n, float sigma, const Mat kernel);
Mat gaussianseperable_RGB(const Mat input, int n, float sigmaT, float sigmaS, const char* opt);

int main() {
    clock_t start, finish;
    Mat input = imread("lena.jpg", IMREAD_COLOR);
    Mat output1, output2, output3;

    if (!input.data)
    {
        std::cout << "Could not open" << std::endl;
        return -1;
    }
    double duration = 0.0;
    namedWindow("Grayscale", WINDOW_AUTOSIZE);
    imshow("Grayscale", input);

    start = clock();
    output2 = gaussianseperable_RGB(input, 50,1,1, "zero-paddle"); //Boundary process: zero-paddle, mirroring, adjustkernel
    finish = clock();
    namedWindow("Gaussian Filter-zero", WINDOW_AUTOSIZE);
    imshow("Gaussian Filter-zero", output2);
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    std::cout << duration << "초" << std::endl;
   
    output1 = gaussianseperable_RGB(input, 3,1,1, "adjustkernel"); //Boundary process: zero-paddle, mirroring, adjustkernel
    namedWindow("gaussianfilter-adjustkernel", WINDOW_AUTOSIZE);
    imshow("gaussianfilter-adjustkernel", output1);

    output2 = gaussianseperable_RGB(input, 3,1,1, "mirroring"); //Boundary process: zero-paddle, mirroring, adjustkernel
    namedWindow("gaussianfilter-mirror", WINDOW_AUTOSIZE);
    imshow("gaussianfilter-mirror", output2);


    waitKey(0);

    return 0;
}


Mat gaussianseperable_RGB(const Mat input, int n, float sigmaT, float sigmaS, const char* opt) {
    //clock_t start=0, finish = 0;
    Mat kernelt, kernels;
    Mat adjust_kernel;

    int row = input.rows;
    int col = input.cols;
    int kernel_size = (2 * n + 1);
    int tempa;
    int tempb;

 // Initialiazing Kernel

    kernelt = Mat::zeros(1, kernel_size, CV_32F);   //W(t) 생성
    kernels = Mat::zeros(kernel_size, 1, CV_32F);   //W(s) 생성
    adjust_kernel = Mat::zeros(kernel_size, kernel_size, CV_32F);

    denom_operation(n, sigmaT, kernelt);    //W(t) 계산
    denom_operation(n, sigmaS, kernels);    //W(s) 계산

    Mat output = Mat::zeros(row, col, input.type());


    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            float res_R = 0.0;
            float res_G = 0.0;
            float res_B = 0.0;

            if (!strcmp(opt, "zero-paddle")) {

                for (int a = -n; a <= n; a++) {

                    float sum_r = 0.0;
                    float sum_g = 0.0;
                    float sum_b = 0.0;

                    for (int b = -n; b <= n; b++) {
                        if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
                            //w(t)먼저 계싼
                            sum_r += kernelt.at<float>(b + n)*(float)(input.at<C>(i + a, j + b)[2]);
                            sum_g += kernelt.at<float>(b + n)*(float)(input.at<C>(i + a, j + b)[1]);
                            sum_b += kernelt.at<float>(b + n)*(float)(input.at<C>(i + a, j + b)[0]);
                        }
                    }
                    //w(s) intermediate 에 적용
                    res_R += kernels.at<float>(a + n)*sum_r;
                    res_G += kernels.at<float>(a + n)*sum_g;
                    res_B += kernels.at<float>(a + n)*sum_b;
                }
                output.at<C>(i, j)[0] = (G)res_B;
                output.at<C>(i, j)[1] = (G)res_G;
                output.at<C>(i, j)[2] = (G)res_R;
            }

            else if (!strcmp(opt, "mirroring")) {

                for (int a = -n; a <= n; a++) {
                    float sum_r = 0.0;
                    float sum_g = 0.0;
                    float sum_b = 0.0;
                    for (int b = -n; b <= n; b++) {

                        if (i + a > row - 1) {  //mirroring for the border pixels row-1 =511 오른쪽으로 벗어남
                            tempa = i - a;
                        }
                        else if (i + a < 0) { //범위를 왼쪽으로 벗어
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
                        sum_r += kernelt.at<float>(b + n)*(float)(input.at<C>(tempa, tempb)[2]);
                        sum_g += kernelt.at<float>(b + n)*(float)(input.at<C>(tempa, tempb)[1]);
                        sum_b += kernelt.at<float>(b + n)*(float)(input.at<C>(tempa, tempb)[0]);
                    }
                    res_R += kernels.at<float>(a + n)*sum_r;
                    res_G += kernels.at<float>(a + n)*sum_g;
                    res_B += kernels.at<float>(a + n)*sum_b;
                }
                output.at<C>(i, j)[0] = (G)res_B;
                output.at<C>(i, j)[1] = (G)res_G;
                output.at<C>(i, j)[2] = (G)res_R;
            }

            else if (!strcmp(opt, "adjustkernel")) {

                // for adjust-kernel
                float res1_R = 0.0;
                float res1_G = 0.0;
                float res1_B = 0.0;
                float res2 = 0.0;

                for (int a = -n; a <= n; a++) {// for each kernel window w(a,b)

                    float sum1_R = 0.0;
                    float sum1_G = 0.0;
                    float sum1_B = 0.0;
                    float fin = 0.0;
                    for (int b = -n; b <= n; b++) {
                        if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
                            sum1_R += float(kernelt.at<float>(b + n))*(float)(input.at<C>(i + a, j + b)[2]);
                            sum1_G += float(kernelt.at<float>(b + n))*(float)(input.at<C>(i + a, j + b)[1]);

                            sum1_B += float(kernelt.at<float>(b + n))*(float)(input.at<C>(i + a, j + b)[0]);
                            fin += float(kernelt.at<float>(b + n));
                        }
                    }
                    res1_R += float(kernels.at<float>(a + n))*sum1_R;
                    res1_G += float(kernels.at<float>(a + n))*sum1_G;
                    res1_B += float(kernels.at<float>(a + n))*sum1_B;
                    res2 += float(kernels.at<float>(a + n))*fin;
                }
                output.at<C>(i, j)[2] = (G)(res1_R / res2);
                output.at<C>(i, j)[1] = (G)(res1_G / res2);
                output.at<C>(i, j)[0] = (G)(res1_B / res2);
            }


        }
    }
    return output;
}

void denom_operation(int n, float sigma, Mat kernel){
    float denom = 0.0;
    for (int i = -n; i <= n; i++) {
        float value1 = exp(-(pow(i, 2) / (2 * pow(sigma, 2))));
        kernel.at<float>(i + n) = value1;
        denom += value1;
    }
    for (int i = -n; i <= n; i++) {
        kernel.at<float>(i + n) /= denom;
    }

}
