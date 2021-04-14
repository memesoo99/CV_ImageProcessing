#include "hist_func.h"

void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF);

int main() {

	Mat input = imread("input.jpg", IMREAD_COLOR);
	Mat input_gray;

    cvtColor(input, input_gray, COLOR_RGB2GRAY);	// convert RGB to Grayscale

	Mat equalized = input_gray.clone();  //결과를 덮어씌울 이미지 생성.

	// PDF or transfer function txt files
    FILE *f_PDF = fopen("hist_gray_PDF.txt", "w+");    //변환전 PDF
    FILE *f_equalized_PDF_gray = fopen("equalized_PDF_gray.txt", "w+");  // 변환후 PDF
    FILE *f_trans_func_eq = fopen("trans_func_eq_gray.txt", "w+"); //변환 function

	float *PDF = cal_PDF(input_gray);	// PDF of Input image(Grayscale) : [L]
	float *CDF = cal_CDF(input_gray);	// CDF of Input image(Grayscale) : [L]

	G trans_func_eq[L] = { 0 };			// transfer function
    // histogram equalization on grayscale image
	hist_eq(input_gray, equalized, trans_func_eq, CDF);		// equalized 에 변환된 이미지 저장
	float *equalized_PDF_gray = cal_PDF(equalized);			// equalized PDF (grayscale)

	for (int i = 0; i < L; i++) {
		// write PDF
		fprintf(f_PDF, "%d\t%f\n", i, PDF[i]);
		fprintf(f_equalized_PDF_gray, "%d\t%f\n", i, equalized_PDF_gray[i]);

		// write transfer functions
		fprintf(f_trans_func_eq, "%d\t%d\n", i, trans_func_eq[i]);
	}

    ///////////////////////histogram 출력///////////
//    namedWindow("original histogram", WINDOW_AUTOSIZE);
//    imshow("original histogram", plot_PDF(PDF));
//
//    namedWindow("Equalized histogram", WINDOW_AUTOSIZE);
//    imshow("Equalized histogram", plot_PDF(equalized_PDF_gray));
    //////////////////////////////////////////////////////////////////
    
	// memory release
	free(PDF);
	free(CDF);
	fclose(f_PDF);
	fclose(f_equalized_PDF_gray);
	fclose(f_trans_func_eq);

	////////////////////// Show each image ///////////////////////

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("Equalized", WINDOW_AUTOSIZE);
	imshow("Equalized", equalized);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
}

// histogram equalization
void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF) {

	// compute transfer function
	for (int i = 0; i < L; i++)
		trans_func[i] = (G)((L - 1) * CDF[i]);

	// perform the transfer function
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			equalized.at<G>(i, j) = trans_func[input.at<G>(i, j)];
}
