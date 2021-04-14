#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>       /* exp */
#define IM_TYPE	CV_8UC3
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
    output2 = gaussianfilter(input_gray, 51,1,1, "zero-paddle"); //Boundary process: zero-paddle, mirroring, adjustkernel
    finish = clock();
    namedWindow("Gaussian Filter", WINDOW_AUTOSIZE);
    imshow("Gaussian Filter", output2);
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    std::cout << duration << "초" << std::endl;
    
    namedWindow("Grayscale", WINDOW_AUTOSIZE);
    imshow("Grayscale", input_gray);

    output1 = gaussianfilter(input_gray, 3,1,1, "adjustkernel"); //Boundary process: zero-paddle, mirroring, adjustkernel
    namedWindow("gaussianfilter-adjustkernel", WINDOW_AUTOSIZE);
    imshow("gaussianfilter-adjustkernel", output1);

    output2 = gaussianfilter(input_gray, 3,1,1, "mirroring"); //Boundary process: zero-paddle, mirroring, adjustkernel
    namedWindow("gaussianfilter-mirror", WINDOW_AUTOSIZE);
    imshow("gaussianfilter-mirror", output2);



    waitKey(0);

    return 0;
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

            float sum1=0.0;

			if (!strcmp(opt, "zero-paddle")) {
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {

                        if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
                            kernelvalue=kernel.at<float>(a+n, b+n);
                            sum1 += kernelvalue*(float)(input.at<G>(i + a, j + b));
                        }
					}
				}
                output.at<G>(i, j) = (G)sum1;

			}

			else if (!strcmp(opt, "mirroring")) {

				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {

                        kernelvalue=kernel.at<float>(a+n, b+n);

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

                        sum1 += kernelvalue*(float)(input.at<G>(tempa, tempb));

					}
				}
                output.at<G>(i, j) = (G)sum1;

			}


			else if (!strcmp(opt, "adjustkernel")) {
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {

                        float kernelvalue=kernel.at<float>(a+n, b+n);

                        float sum1 = 0.0;
                        float sum2 = 0.0;
                        if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
                            sum1 += kernelvalue*(float)(input.at<G>(i + a, j + b));
                            sum2 += kernelvalue;
                        }
                        else{
                            kernelvalue = 0;
                        }
                        output.at<G>(i, j) = (G)(sum1/sum2);
					}
				}
			}
		}
	}
	return output;
}
