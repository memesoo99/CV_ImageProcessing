#include <iostream>
#include <opencv2/opencv.hpp>

#define IM_TYPE	CV_8UC3

using namespace cv;
using namespace std;

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

Mat meanfilter(const Mat input, int n, const char* opt);

int main() {

	Mat input = imread("lena.jpg", IMREAD_COLOR);
    Mat input_gray = input.clone();
	Mat output1,output2,output3;

	cvtColor(input, input_gray, COLOR_RGB2GRAY); // Converting image to gray

	if (!input.data)
	{
		cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);
    
	output1 = meanfilter(input_gray, 3, "adjustkernel"); //Boundary process: zero-paddle, mirroring, adjustkernel
	namedWindow("Mean Filter-adjustkernel", WINDOW_AUTOSIZE);
	imshow("Mean Filter-adjustkernel", output1);
    
    output2 = meanfilter(input_gray, 3, "mirroring"); //Boundary process: zero-paddle, mirroring, adjustkernel
    namedWindow("Mean Filter-mirror", WINDOW_AUTOSIZE);
    imshow("Mean Filter-mirror", output2);
    
    output3 = meanfilter(input_gray, 3, "zero-paddle"); //Boundary process: zero-paddle, mirroring, adjustkernel
    namedWindow("Mean Filter-zero", WINDOW_AUTOSIZE);
    imshow("Mean Filter-zero", output3);


	waitKey(0);

	return 0;
}


Mat meanfilter(const Mat input, int n, const char* opt) {

	Mat kernel;

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;

 // Initialiazing Kernel Matrix
	kernel = Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size * kernel_size); // 7*7 kernel , normalize?
	float kernelvalue=kernel.at<float>(0, 0);  // To simplify, as the filter is uniform. All elements of the kernel value are same. 0.0204082
    //cout<<kernel;
	Mat output = Mat::zeros(row, col, input.type());


	for (int i = 0; i < row; i++) { //for each pixel in the output
		for (int j = 0; j < col; j++) {

			if (!strcmp(opt, "zero-paddle")) {
				float sum1 = 0.0;
				for (int a = -n; a <= n; a++) { // for each kernel window
					for (int b = -n; b <= n; b++) {

						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
							sum1 += kernelvalue*(float)(input.at<G>(i + a, j + b)); 
						}
					}
				}
				output.at<G>(i, j) = (G)sum1;
			}

			else if (!strcmp(opt, "mirroring")) {
				float sum1 = 0.0;
				for (int a = -n; a <= n; a++) { // for each kernel window a= kernel x좌표
					for (int b = -n; b <= n; b++) {

						if (i + a > row - 1) {  //mirroring for the border pixels row-1 =511 오른쪽으로 벗어남
							tempa = i - a;
						}
						else if (i + a < 0) { //범위를 왼쪽으로 벗어남
							tempa = -(i + a);
						}
						else { //정싱
							tempa = i + a;
						}

						if (j + b > col - 1) {//범위를 아래로 벗어남
							tempb = j - b;
						}
						else if (j + b < 0) {
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
				float sum1 = 0.0;
				float sum2 = 0.0;
				for (int a = -n; a <= n; a++) { // for each kernel window
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							sum1 += kernelvalue*(float)(input.at<G>(i + a, j + b));
							sum2 += kernelvalue;
						}
					}
				}
				output.at<G>(i, j) = (G)(sum1/sum2);
			}
		}
	}
	return output;
}
