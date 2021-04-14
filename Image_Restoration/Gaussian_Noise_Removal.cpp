#include <opencv2/opencv.hpp>
#include <stdio.h>

#define IM_TYPE    CV_64FC3

using namespace cv;

//ADD
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

Mat Add_Gaussian_noise(const Mat input, double mean, double sigma);
Mat Gaussianfilter_Gray(const Mat input, int n, double sigma_t, double sigma_s, const char *opt);
Mat Gaussianfilter_RGB(const Mat input, int n, double sigma_t, double sigma_s, const char *opt);
Mat Bilateralfilter_Gray(const Mat input, int n, double sigma_t, double sigma_s, double sigma_r, const char *opt);
Mat Bilateralfilter_RGB(const Mat input, int n, double sigma_t, double sigma_s, double sigma_r, const char *opt);

int main() {

    Mat input = imread("lena.jpg", IMREAD_COLOR);
	Mat input_gray;

	// check for validation
	if (!input.data) {
		printf("Could not open\n");
		return -1;
	}

	cvtColor(input, input_gray, COLOR_RGB2GRAY);	// convert RGB to Grayscale

	// 8-bit unsigned char -> 64-bit floating point
	input.convertTo(input, CV_64FC3, 1.0 / 255);
	input_gray.convertTo(input_gray, CV_64F, 1.0 / 255);

	// Add noise to original image
	Mat noise_Gray = Add_Gaussian_noise(input_gray, 0, 0.1);
	Mat noise_RGB = Add_Gaussian_noise(input, 0, 0.1);

	// Denoise, using gaussian filter
	Mat Denoised_Gray = Gaussianfilter_Gray(noise_Gray, 3, 10, 10, "adjustkernel");
	Mat Denoised_RGB = Gaussianfilter_RGB(noise_RGB, 3, 10, 10, "zero-padding");
    
    Mat Bilateral_Denoised_Gray = Bilateralfilter_Gray(noise_Gray, 3, 10, 10, 3, "adjustkernel");
    Mat Bilateral_Denoised_RGB = Bilateralfilter_RGB(noise_RGB, 3, 10, 10,3, "adjustkernel");

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Gaussian Noise (Grayscale)", WINDOW_AUTOSIZE);
	imshow("Gaussian Noise (Grayscale)", noise_Gray);

	namedWindow("Gaussian Noise (RGB)", WINDOW_AUTOSIZE);
	imshow("Gaussian Noise (RGB)", noise_RGB);

	namedWindow("Denoised (Grayscale)", WINDOW_AUTOSIZE);
	imshow("Denoised (Grayscale)", Denoised_Gray);

	namedWindow("Denoised (RGB)", WINDOW_AUTOSIZE);
	imshow("Denoised (RGB)", Denoised_RGB);
    
    namedWindow("Bilateral Denoised (Grayscale)", WINDOW_AUTOSIZE);
    imshow("Bilateral Denoised (Grayscale)", Bilateral_Denoised_Gray);

    namedWindow("Bilateral Denoised (RGB)", WINDOW_AUTOSIZE);
    imshow("Bilateral Denoised (RGB)", Bilateral_Denoised_RGB);

	waitKey(0);

	return 0;
}

Mat Add_Gaussian_noise(const Mat input, double mean, double sigma) {

	Mat NoiseArr = Mat::zeros(input.rows, input.cols, input.type());
	RNG rng;
	rng.fill(NoiseArr, RNG::NORMAL, mean, sigma);

	add(input, NoiseArr, NoiseArr);

	return NoiseArr;
}

Mat Gaussianfilter_Gray(const Mat input, int n, double sigma_t, double sigma_s, const char *opt) {

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
    int tempa;
    int tempb;
    float denom;
    float kernelvalue;
    

	// Initialiazing Gaussian Kernel Matrix

    Mat kernel = Mat::zeros(kernel_size, kernel_size, CV_32F);
	Mat output = Mat::zeros(row, col, input.type());
    denom = 0.0;
    
    
    for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
        for (int b = -n; b <= n; b++) {
            float value1 = exp(-(pow(a, 2) / (2 * pow(sigma_s, 2))) - (pow(b, 2) / (2 * pow(sigma_t, 2))));
            kernel.at<float>(a+n, b+n) = value1;
            denom += value1;
        }
    }

    for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
        for (int b = -n; b <= n; b++) {
            kernel.at<float>(a+n, b+n) /= denom;
        }
    }
    
	// convolution
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
            float sum1=0.0;
            
			if (!strcmp(opt, "mirroring")) {

				for (int a = -n; a <= n; a++) { // for each kernel window
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

                        sum1 += kernelvalue*(float)(input.at<G>(tempa, tempb));

                    }
                }
                output.at<G>(i, j) = (G)sum1;

			}

			else if (!strcmp(opt, "zero-padding")) {

				for (int a = -n; a <= n; a++) { // for each kernel window
					for (int b = -n; b <= n; b++) {
                        if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
                            kernelvalue=kernel.at<float>(a+n, b+n);
                            sum1 += kernelvalue*(float)(input.at<G>(i + a, j + b));
                        }
					}
				}
                output.at<G>(i, j) = (G)sum1;

			}

			else if (!strcmp(opt, "adjustkernel")) {
                float sum1 = 0.0;
                float sum2 = 0.0;

				for (int a = -n; a <= n; a++) { // for each kernel window
					for (int b = -n; b <= n; b++) {

                        float kernelvalue=kernel.at<float>(a+n, b+n);

                        
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

Mat Gaussianfilter_RGB(const Mat input, int n, double sigma_t, double sigma_s, const char *opt) {

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
            float value1 = exp(-(pow(a, 2) / (2 * pow(sigma_s, 2))) - (pow(b, 2) / (2 * pow(sigma_t, 2))));
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

            if (!strcmp(opt, "zero-padding")) {
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

Mat Bilateralfilter_Gray(const Mat input, int n, double sigma_t, double sigma_s, double sigma_r, const char *opt){
    int row = input.rows;
    int col = input.cols;
    int kernel_size = (2 * n + 1);
    int tempa;
    int tempb;
    float denom;
    
    Mat kernel = Mat::zeros(kernel_size, kernel_size, CV_32F);
    Mat kernel_W = Mat::zeros(row, col, CV_32F);  //W(i,j)
    Mat output = Mat::zeros(row, col, input.type());  //W(i,j)


    
    for(int i = 0; i<row; i++){
        for(int j = 0; j<col; j++){
            denom = 0.0;
            for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
                for (int b = -n; b <= n; b++) {
                    float value1 = exp(-(pow(a, 2) / (2 * pow(sigma_s, 2))) - (pow(b, 2) / (2 * pow(sigma_t, 2))));
                    float value2 = exp(-pow(input.at<G>(i,j)-input.at<G>(i+a,j+b),2) / 2*pow(sigma_r,2));
                    kernel.at<float>(a + n, b + n) = value1;
                    denom = value1*value2;

                }
            }
            kernel_W.at<float>(i, j) = denom;
        }
    }
    
    for(int i = 0; i<row; i++){
        for(int j = 0; j<col; j++){
            if (!strcmp(opt, "zero-padding")) {
                float sum1 = 0.0;
                for (int x = -n; x <= n; x++) { // for each kernel window
                        for (int y = -n; y <= n; y++) {
                            if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) { //if the pixel is not a border pixel
                                        
                                float ex1 = kernel.at<float>(x + n, y + n);
                                float ex2 = exp(-(pow((input.at<G>(i, j) - input.at<G>(i + x, j + y)), 2)) / 2 * pow(sigma_r, 2));
                                float s = (1 / kernel_W.at<float>(i, j))*ex1*ex2; //w(s,t)
                                sum1 += s *(float)(input.at<G>(i + x, j + y));  //normalize 된 값들 다 더함.
                            }
                        }
                    }
                    output.at<G>(i, j) = (G)sum1;
                }
            else if (!strcmp(opt, "mirroring")){
                float sum1 = 0.0;
                for (int x = -n; x <= n; x++) { // for each kernel window
                    for (int y = -n; y <= n; y++) {
                    

                        if (i + x > row - 1) {  //mirroring for the border pixels row-1 =511 오른쪽으로 벗어남
                            tempa = i - x;
                        }
                        else if (i + x < 0) { //범위를 왼쪽으로 벗어남 i,j = (0,0)
                            tempa = -(i + x);
                        }
                        else { //정상
                            tempa = i + x;
                        }

                        if (j + y > col - 1) {//범위를 아래로 벗어남
                            tempb = j - y;
                        }
                        else if (j + y < 0) { //범위를 위로 벗어남
                            tempb = -(j + y);
                        }
                        else {
                            tempb = j + y;
                        }
                        float ex1 = kernel.at<float>(x+n, y+n);
                        float ex2 = exp(-(pow((input.at<G>(i, j) - input.at<G>(i + x, j + y)), 2)) / 2 * pow(sigma_r, 2));
                        float s = (1 / kernel_W.at<float>(i, j))*ex1*ex2; //w(s,t)
                        sum1 += s *(float)(input.at<G>(tempa, tempb));  //normalize 된 값들 다 더함.
            

                    }
                }
                output.at<G>(i, j) = (G)sum1;
                
            }
            else if (!strcmp(opt, "adjustkernel")){
                float sum1 = 0.0;
                float sum2 = 0.0;
                
                for (int x = -n; x <= n; x++) { // for each kernel window
                    for (int y = -n; y <= n; y++) {
                        if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
                            float ex1 = kernel.at<float>(x + n, y + n);
                            float ex2 = exp(-(pow((input.at<G>(i, j) - input.at<G>(i + x, j + y)), 2)) / 2 * pow(sigma_r, 2));
                            float s = (1 / kernel_W.at<float>(i, j))*ex1*ex2;

                            sum1 += s*(float)(input.at<G>(i + x, j + y));
                            sum2 += s; // 모든 weight값 합쳐서 정규화수 구함
                        }
                    }
                }
                output.at<G>(i, j) = (G)(sum1 / sum2);
            }
            
        }
    }
    return output;
}
Mat Bilateralfilter_RGB(const Mat input, int n, double sigma_t, double sigma_s, double sigma_r, const char *opt){
    int row = input.rows;
    int col = input.cols;
    int kernel_size = (2 * n + 1);
    int tempa;
    int tempb;
    float denom;
    
    Mat kernel = Mat::zeros(kernel_size, kernel_size, CV_32F);
    Mat kernel_W = Mat::zeros(row, col, CV_32F);  //W(i,j)
    Mat output = Mat::zeros(row, col, input.type());  //W(i,j)

    
    
    for(int i = 0; i<row; i++){
        for(int j = 0; j<col; j++){
            denom = 0.0;
            for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
                for (int b = -n; b <= n; b++) {
                    float value1 = exp(-(pow(a, 2) / (2 * pow(sigma_s, 2))) - (pow(b, 2) / (2 * pow(sigma_t, 2))));
                    float value2 = exp(-pow(input.at<G>(i,j)-input.at<G>(i+a,j+b),2) / 2*pow(sigma_r,2));
                    kernel.at<float>(a + n, b + n) = value1;
                    denom = value1*value2;

                }
            }
            kernel_W.at<float>(i, j) = denom;
        }
    }
    
    
    for(int i = 0; i<row; i++){
        for(int j = 0; j<col; j++){
            if (!strcmp(opt, "zero-padding")) {
                float sum_r = 0.0;
                float sum_g = 0.0;
                float sum_b = 0.0;
                for (int x = -n; x <= n; x++) { // for each kernel window
                        for (int y = -n; y <= n; y++) {
                            if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) { //if the pixel is not a border pixel
                                        
                                float ex1 = kernel.at<float>(x + n, y + n);
                                float ex2 = exp(-(pow((input.at<G>(i, j) - input.at<G>(i + x, j + y)), 2)) / 2 * pow(sigma_r, 2));
                                float s = (1 / kernel_W.at<float>(i, j))*ex1*ex2; //w(s,t)
                                
                                sum_r += s *(float)(input.at<C>(i + x, j + y)[2]);  //normalize 된 값들 다 더함.
                                sum_g += s *(float)(input.at<C>(i + x, j + y)[1]);  //normalize 된 값들 다 더함.
                                sum_b += s *(float)(input.at<C>(i + x, j + y)[0]);  //normalize 된 값들 다 더함.
                            }
                        }
                    }
                output.at<C>(i, j)[2] = (G)sum_r;
                output.at<C>(i, j)[1] = (G)sum_g;
                output.at<C>(i, j)[0] = (G)sum_b;
                
                }
            
            else if (!strcmp(opt, "mirroring")){
                float sum_r = 0.0;
                float sum_g = 0.0;
                float sum_b = 0.0;
                for (int x = -n; x <= n; x++) { // for each kernel window
                    for (int y = -n; y <= n; y++) {

                        if (i + x > row - 1) {  //mirroring for the border pixels row-1 =511 오른쪽으로 벗어남
                            tempa = i - x;
                        }
                        else if (i + x < 0) { //범위를 왼쪽으로 벗어남 i,j = (0,0), 
                            tempa = -(i + x);
                        }
                        else { //정상
                            tempa = i + x;
                        }

                        if (j + y > col - 1) {//범위를 아래로 벗어남
                            tempb = j - y;
                        }
                        else if (j + y < 0) { //범위를 위로 벗어남
                            tempb = -(j + y);
                        }
                        else {
                            tempb = j + y;
                        }
                        float ex1 = kernel.at<float>(x+n, y+n);
                        float ex2 = exp(-(pow((input.at<G>(i, j) - input.at<G>(i + x, j + y)), 2)) / 2 * pow(sigma_r, 2));
                        float s = (1 / kernel_W.at<float>(i, j))*ex1*ex2; //w(s,t)
                        
                        sum_r += s *(float)(input.at<C>(tempa,tempb)[2]);  //normalize 된 값들 다 더함.
                        sum_g += s *(float)(input.at<C>(tempa,tempb)[1]);  //normalize 된 값들 다 더함.
                        sum_b += s *(float)(input.at<C>(tempa,tempb)[0]);  //normalize 된 값들 다 더함.
                    }
                }
                output.at<C>(i, j)[2] = (G)sum_r;
                output.at<C>(i, j)[1] = (G)sum_g;
                output.at<C>(i, j)[0] = (G)sum_b;
                
            }
            else if (!strcmp(opt, "adjustkernel")){
                float sum_r = 0.0;
                float sum_g = 0.0;
                float sum_b = 0.0;
                float sum2 = 0.0;
                
                for (int x = -n; x <= n; x++) { // for each kernel window
                    for (int y = -n; y <= n; y++) {
                        if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
                            float ex1 = kernel.at<float>(x + n, y + n);
                            float ex2 = exp(-(pow((input.at<G>(i, j) - input.at<G>(i + x, j + y)), 2)) / 2 * pow(sigma_r, 2));
                            float s = (1 / kernel_W.at<float>(i, j))*ex1*ex2;

                            sum_r += s*(float)(input.at<C>(i + x, j + y)[2]);
                            sum_g += s*(float)(input.at<C>(i + x, j + y)[1]);
                            sum_b += s*(float)(input.at<C>(i + x, j + y)[0]);
                            sum2 += s; // 모든 weight값 합쳐서 정규화수 구함
                        }
                    }
                }
                output.at<C>(i, j)[2] = (G)(sum_r / sum2);
                output.at<C>(i, j)[1] = (G)(sum_g / sum2);
                output.at<C>(i, j)[0] = (G)(sum_b / sum2);
            }
            
        }
    }
    return output;
}


