#include "hist_func.h"
#include <stdlib.h>
#include <stdio.h>

void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF);

int main() {

    Mat input = imread("input.jpg", IMREAD_COLOR);
    Mat equalized_YUV = input.clone();
    ////////1. Convert RGB channel to YUV
    cvtColor(input, equalized_YUV, COLOR_RGB2YUV);    // RGB -> YUV

    ///////2. Split each channel YUV - Y는 HE를 수행할 채널로 따로 변수 생성에서 저장함
    Mat channels[3];
    split(equalized_YUV, channels);
    Mat Y = channels[0];                        // U = channels[1], V = channels[2]

    // PDF or transfer function txt files
    FILE *f_equalized_PDF_YUV = fopen("Hist_Eq_YUV_EqualizedPDF.txt", "w+"); // RGB->YUV->Y'UV->R'GB에서 마지막 R'GB를 저장할 파일
    FILE *f_PDF_RGB = fopen("Hist_Eq_YUV_OriginalPDF.txt", "w+");
    FILE *f_trans_func_eq_YUV = fopen("trans_func_eq_YUV.txt", "w+");

    ///////3. calculate CDF of Y - Histogram Equalization을 하기 위해서는 Y의 CDF를 구해야 합니다.
    float **PDF_RGB = cal_PDF_RGB(input);        // PDF of Input image(RGB) : [L][3]
    float *CDF_YUV = cal_CDF(Y);                // CDF of Y channel image

    G trans_func_eq_YUV[L] = { 0 };            // transfer function

    ///////4. Apply histogram equalization of Y
    hist_eq(Y, channels[0], trans_func_eq_YUV, CDF_YUV); // Y의 변환된 값을 channels[0]에 덮어씌웁니다.
    
    ///////5. merge equalized Y' with other channels - temp = Y'UV
    Mat temp;
    merge(channels, 3, temp); // channel의 Y는 변환된 값이고, channel[1], channel[2]는 고유의 값을 유지합니다.

    ///////6. Convert YUV to RGB and get the equalized image
    cvtColor(temp, temp , COLOR_YUV2RGB ); // temp = R'GB

    // equalized PDF (YUV)
    float **equalized_PDF_YUV = cal_PDF_RGB(temp);

    for (int i = 0; i < L; i++) {
        // write PDF
        fprintf(f_equalized_PDF_YUV, "%d\t%f\t%f\t%f\n", i, equalized_PDF_YUV[i][2],equalized_PDF_YUV[i][1],equalized_PDF_YUV[i][0]); //RGB순으로 저장
        fprintf(f_PDF_RGB, "%d\t%f\t%f\t%f\n", i, PDF_RGB[i][2],PDF_RGB[i][1],PDF_RGB[i][0]); //RGB순으로 저장

        // write transfer functions
        fprintf(f_trans_func_eq_YUV, "%d\t%d\n", i, trans_func_eq_YUV[i]);
    }

    // memory release
    free(PDF_RGB);
    free(CDF_YUV);
    fclose(f_PDF_RGB);
    fclose(f_equalized_PDF_YUV);
    fclose(f_trans_func_eq_YUV);

    ////////////////////// Show each image ///////////////////////


    namedWindow("RGB", WINDOW_AUTOSIZE);
    imshow("RGB", input);

    namedWindow("Equalized_YUV", WINDOW_AUTOSIZE);
    imshow("Equalized_YUV", temp);

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
