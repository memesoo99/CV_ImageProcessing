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
Mat gaussianseperable(const Mat input, int n, float sigmaT, float sigmaS, const char* opt);

int main() {
    clock_t start, finish;
    double duration;

    Mat input = imread("lena.jpg", IMREAD_COLOR);
    Mat input_gray;
    Mat output1,output2, output3;


    cvtColor(input, input_gray, COLOR_RGB2GRAY);



    if (!input.data)
    {
        std::cout << "Could not open" << std::endl;
        return -1;
    }
    //std::cout<<input_gray<<std::endl;
    namedWindow("Grayscale", WINDOW_AUTOSIZE);
    imshow("Grayscale", input_gray);

    start = clock();
    output2 = gaussianseperable(input_gray, 1,1,1, "zero-paddle"); //Boundary process: zero-paddle, mirroring, adjustkernel
    finish = clock();
    namedWindow("Gaussian Filter", WINDOW_AUTOSIZE);
    imshow("Gaussian Filter", output2);
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    std::cout << duration << "초" << std::endl;
    
    namedWindow("Grayscale", WINDOW_AUTOSIZE);
    imshow("Grayscale", input_gray);

    output1 = gaussianseperable(input_gray, 3,1,1, "adjustkernel"); //Boundary process: zero-paddle, mirroring, adjustkernel
    namedWindow("gaussianfilter-adjustkernel", WINDOW_AUTOSIZE);
    imshow("gaussianfilter-adjustkernel", output1);

    output2 = gaussianseperable(input_gray, 3,1,1, "mirroring"); //Boundary process: zero-paddle, mirroring, adjustkernel
    namedWindow("gaussianfilter-mirror", WINDOW_AUTOSIZE);
    imshow("gaussianfilter-mirror", output2);



    waitKey(0);

    return 0;
}

Mat gaussianseperable(const Mat input, int n, float sigmaT, float sigmaS, const char* opt) {
    //clock_t start=0, finish = 0;
    Mat kernel;
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
            float res = 0.0;
            
            if (!strcmp(opt, "zero-paddle")) {
                for (int a = -n; a <= n; a++) {
                    float sum1 = 0.0;
                    for (int b = -n; b <= n; b++) {
                        if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
                            sum1 += float(kernelt.at<float>(b + n))*(float)(input.at<G>(i + a, j + b));
                        }
                    }
                    res += float(kernels.at<float>(a + n))*sum1;
                }
                output.at<G>(i, j) = (G)res;
            }

            else if (!strcmp(opt, "mirroring")) {
                float sum1 = 0.0;
                for (int a = -n; a <= n; a++) {
                    float sum1 = 0.0;
                    for (int b = -n; b <= n; b++) {
                        if (i + a > row - 1) {  //mirroring for the border pixels
                            tempa = i - a;
                        }
                        else if (i + a < 0) {
                            tempa = -(i + a);
                        }
                        else {
                            tempa = i + a;
                        }
                        if (j + b > col - 1) {
                            tempb = j - b;
                        }
                        else if (j + b < 0) {
                            tempb = -(j + b);
                        }
                        else {
                            tempb = j + b;
                        }
                        sum1 += float(kernelt.at<float>(b + n))*(float)(input.at<G>(tempa, tempb));
                    }
                    res += float(kernels.at<float>(a + n))*sum1;
                }
                output.at<G>(i, j) = (G)res;
            }
            else if (!strcmp(opt, "adjustkernel")) {
                float ress = 0.0;
                float rest = 0.0;
                
                float sum1 = 0.0;
                float sum2 = 0.0;
                for (int a = -n; a <= n; a++) { // for each kernel window w(a,b)
                    for (int b = -n; b <= n; b++) {
                        //w(t)
                        if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
                            sum1 += float(kernelt.at<float>(b + n)) * (float)(input.at<G>(i + a, j + b));
                            sum2 += float(kernelt.at<float>(b + n));
                        }
                    }
                    ress += kernels.at<float>(a + n)*sum1;
                    rest += kernels.at<float>(a + n)*sum2;
                }
                output.at<G>(i, j) = (G)(ress/rest);
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
