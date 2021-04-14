#include "hist_func.h"

void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF);
void hist_match_Color(G* T_trans,G* G_trans, G* Final_trans);
int main() {

    /////////////////// 1. 이미지 준비
    Mat input = imread("input.jpg", IMREAD_COLOR);
    Mat reference = imread("img02.jpg", IMREAD_COLOR);

    Mat input_YUV;
    Mat reference_YUV;
    
    /////////////////// 2. RGB -> YUV
    cvtColor(input, input_YUV, COLOR_RGB2YUV);
    cvtColor(reference, reference_YUV, COLOR_RGB2YUV);

    Mat result = input.clone();
    Mat intermediate_inp = input_YUV.clone();
    Mat intermediate_ref = reference_YUV.clone();
    // PDF or transfer function txt files
    
    /////////////////// 3. Channel Y 추출
    Mat input_channels[3];
    split(input_YUV, input_channels);
    Mat input_Y = input_channels[0];

    Mat reference_channels[3];
    split(reference_YUV, reference_channels);
    Mat reference_Y = reference_channels[0];

    float **PDF_RGB_reference = cal_PDF_RGB(reference);  // PDF of reference image(RGB) : [L][3]
    float *CDF_YUV_reference = cal_CDF(reference_Y);    // CDF of Ref Y channel image

    float **PDF_RGB_input = cal_PDF_RGB(input);       // PDF of Input image(RGB) : [L][3]
    float *CDF_YUV_input = cal_CDF(input_Y);        // CDF of Inp Y channel image

    FILE *f_PDF = fopen("Histogram_Match_Origin_PDF.txt", "w+"); //origin RGB
    FILE *f_matched_PDF = fopen("Histogram_Match_Output_PDF.txt", "w+"); //output RGB
    FILE *final_trans_func_match= fopen("Histogram_Match_function.txt", "w+"); //transfer functionfor Y channel
//    FILE *f_matched_CDF = fopen("Histogram_Match_Output_CDF.txt", "w+");

    float *inp_PDF = cal_PDF(input_YUV);    // PDF of Input image(Grayscale) : [L]
    float *inp_CDF = cal_CDF(input_YUV);    // CDF of Input image(Grayscale) : [L]


    /////////////////// 4. Y->HM 변환
    G T_trans_func_eq[L] = { 0 };            // transfer function for T(r)
    G G_trans_func_eq[L] = { 0 };            // transfer function for G(z)
    G Final_trans_func_eq[L] = { 0 };

    /////////////////// 5. Apply HM for Y channel
    hist_eq(input_Y, intermediate_inp, T_trans_func_eq, CDF_YUV_input);           // input 이미지의 Y 채널에서 T 추출
    hist_eq(reference_Y, intermediate_ref, G_trans_func_eq, CDF_YUV_reference);   // reference 이미지의 Y 채널에서 G추출
    hist_match_Color(T_trans_func_eq, G_trans_func_eq, Final_trans_func_eq); // Final transferfunction cncnf

    Mat output_Y = input_Y.clone();
    
    /////////////////// 6. 최종적으로 구해진 G 역함수로 원본이미지 변형시키기
    ///여기서 모든이미지는 아직 YUV 형식이다.
    for (int i = 0; i < input.rows; i++)
        for (int j = 0; j < input.cols; j++)
            output_Y.at<G>(i, j) = Final_trans_func_eq[input_Y.at<G>(i, j)];

    
    /////////////////// 7. YUV -> RGB
    input_channels[0] = output_Y;
    merge(input_channels, 3, result);
    cvtColor(result, result , COLOR_YUV2RGB );
    float **PDF_RGB_result = cal_PDF_RGB(result);       // PDF of Input image(RGB) : [L][3]


    for (int i = 0; i < L; i++) {
        // write PDF
        fprintf(f_PDF, "%d\t%f\t%f\t%f\n", i, PDF_RGB_input[i][2],PDF_RGB_input[i][1],PDF_RGB_input[i][0]); //input 함수의 채널으니 BGR 형식으로 되어있기 때문에 RGB형식으로 입력하기 위해서는 순서를 바꿔서 넣어줘여한다.
        fprintf(f_matched_PDF, "%d\t%f\t%f\t%f\n", i, PDF_RGB_result[i][0],PDF_RGB_result[i][1],PDF_RGB_result[i][2]); // ㅏ반대로 PDF_RGB_result는 YUV를 RGB로 변환한 함수이므로 0,1,2순서로 넣어야 한다.

        // write transfer functions
        fprintf(final_trans_func_match, "%d\t%d\n", i, Final_trans_func_eq[i]);

    }

    // memory release
    free(inp_PDF);
    free(inp_CDF);
    fclose(f_PDF);
    fclose(f_matched_PDF);
    fclose(final_trans_func_match);

    //fclose(f_matched_CDF);

    ////////////////////// Show each image ///////////////////////

    namedWindow("INPUT", WINDOW_AUTOSIZE);
    imshow("INPUT", input);

    namedWindow("REFERENCE", WINDOW_AUTOSIZE);
    imshow("REFERENCE", reference);

    namedWindow("RESULT", WINDOW_AUTOSIZE);
    imshow("RESULT", result);
//
//
//    namedWindow("Intermediate_inp", WINDOW_AUTOSIZE);
//    imshow("Intermediate_inp", intermediate_inp);
//
//
//    namedWindow("Intermediate_REF", WINDOW_AUTOSIZE);
//    imshow("Intermediate_REF", intermediate_ref);

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

int return_mapping_down(int a, G* G_trans){
    if(a<0){
        return -10;
    }
    for(int j = 0; j<L;j++){
        if(G_trans[j] == a){
            return j;
        }
    }
    return return_mapping_down(a-1,G_trans);

}

int return_mapping_up(int a, G* G_trans){
    if(a>L-1){
        return -20;
    }
    for(int j = 0; j<L;j++){
        if(G_trans[j]==a){
            return j;
        }
    }
    return return_mapping_up(a+1,G_trans);

}

void hist_match_Color(G* T_trans,G* G_trans, G* Final_trans){
    for (int i = 0; i < L; i++){
        int a = T_trans[i];
        int temp = return_mapping_down(a,G_trans);
        if (temp == -10) temp = return_mapping_up(a, G_trans);
        else {Final_trans[i] = temp;}
    }

}
