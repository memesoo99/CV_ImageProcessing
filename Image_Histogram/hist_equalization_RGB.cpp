#include "hist_func.h"
void hist_eq_Color(Mat &input, Mat &equalized, G(*trans_func)[3], float **CDF);

int main() {

	Mat input = imread("input.jpg", IMREAD_COLOR);
	Mat equalized_RGB = input.clone();

	// PDF or transfer function txt files
    FILE *f_PDF_RGB = fopen("PDF_RGB.txt", "w+");
    FILE *f_equalized_PDF_RGB = fopen("equalized_PDF_RGB.txt", "w+");
    FILE *f_trans_func_eq_RGB = fopen("trans_func_eq_RGB.txt", "w+");

	float **PDF_GBR = cal_PDF_RGB(input);	// PDF of Input image(RGB) : [L][3] opencv는 channel을 반환할떄 RGB가 아닌 GBR 로 반환합니다.
	float **CDF_GBR = cal_CDF_RGB(input);	// CDF of Input image(RGB) : [L][3]

	G trans_func_eq_RGB[L][3] = { 0 };		// transfer function

	// histogram equalization on RGB image
    hist_eq_Color(input, equalized_RGB, trans_func_eq_RGB, CDF_GBR);

	// equalized PDF (RGB)
    float **equalized_PDF_RGB = cal_PDF_RGB(equalized_RGB);
	for (int i = 0; i < L; i++) {
		// write PDF
        fprintf(f_PDF_RGB, "%d\t%f\t%f\t%f\n", i, PDF_GBR[i][2],PDF_GBR[i][1],PDF_GBR[i][0]); //RGB순으로 저장
        fprintf(f_equalized_PDF_RGB, "%d\t%f\t%f\t%f\n", i, equalized_PDF_RGB[i][2],equalized_PDF_RGB[i][1],equalized_PDF_RGB[i][0]);

		// write transfer functions
        fprintf(f_trans_func_eq_RGB, "%d\t%d\t%d\t%d\n", i, trans_func_eq_RGB[i][2], trans_func_eq_RGB[i][1], trans_func_eq_RGB[i][0]);
	}

	// memory release
	free(PDF_GBR);
	free(CDF_GBR);
	fclose(f_PDF_RGB);
	fclose(f_equalized_PDF_RGB);
	fclose(f_trans_func_eq_RGB);

	////////////////////// Show each image ///////////////////////

	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Equalized_RGB", WINDOW_AUTOSIZE);
	imshow("Equalized_RGB", equalized_RGB);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
}

// histogram equalization on 3 channel image
void hist_eq_Color(Mat &input, Mat &equalized, G(*trans_func)[3], float **CDF) {

    for (int i = 0; i < L; i++){
        trans_func[i][0] = (G)((L - 1) * CDF[i][0]); //각각의 채널별로 trans_func 생성해야됨
        trans_func[i][1] = (G)((L - 1) * CDF[i][1]);
        trans_func[i][2] = (G)((L - 1) * CDF[i][2]); //BGR로 저장
    }

    for (int i = 0; i < input.rows; i++){
        for (int j = 0; j < input.cols; j++){
            C temp_equal = (0,0,0);
            C temp_input = input.at<C>(i,j);
            temp_equal[0] = trans_func[temp_input[0]][0];
            temp_equal[1] = trans_func[temp_input[1]][1];
            temp_equal[2] = trans_func[temp_input[2]][2];//BGR 형식
            equalized.at<C>(i,j) = temp_equal;
        }

    }

}
