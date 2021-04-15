#include "hist_func.h"

void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF);
void hist_match_Color(G* T_trans,G* G_trans, G* Final_trans);
int main() {
    //////////////1. 이미지 준비
    Mat input = imread("input.jpg", IMREAD_COLOR);  //변환하고싶은 이미지
    Mat reference = imread("img03.jpeg", IMREAD_COLOR);  //닯고 싶은 이미지

    Mat input_gray;
    Mat reference_gray;

    cvtColor(input, input_gray, COLOR_RGB2GRAY);    // convert RGB to Grayscale
    cvtColor(reference, reference_gray, COLOR_RGB2GRAY); // convert RGB to Grayscale

    Mat result = input_gray.clone();
    Mat intermediate_inp = input_gray.clone();
    Mat intermediate_ref = reference_gray.clone();
    
    //////////////2. Output 파일 준비
    // PDF or transfer function txt files

    FILE *f_PDF = fopen("Histogram_Match_Origin_PDF.txt", "w+");
    FILE *f_matched_PDF = fopen("Histogram_Match_Output_PDF.txt", "w+");
    FILE *final_trans_func_eq= fopen("Histogram_Match_function.txt", "w+");
//    FILE *f_matched_CDF = fopen("Histogram_Match_Output_CDF.txt", "w+");

    float *inp_PDF = cal_PDF(input_gray);    // PDF of Input image(Grayscale) : [L]
    float *inp_CDF = cal_CDF(input_gray);    // CDF of Input image(Grayscale) : [L]

    float *ref_PDF = cal_PDF(reference_gray);    // PDF of Input image(Grayscale) : [L]
    float *ref_CDF = cal_CDF(reference_gray);    // CDF of Input image(Grayscale) : [L]

    G T_trans_func_eq[L] = { 0 };            // transfer function for T(r)
    G G_trans_func_eq[L] = { 0 };            // transfer function for G(z)
    G Final_trans_func_eq[L] = { 0 };        // 최종으로 쓰일 transfer function

    hist_eq(input_gray, intermediate_inp, T_trans_func_eq, inp_CDF);           // histogram equalization on grayscale image
    hist_eq(reference_gray, intermediate_ref, G_trans_func_eq, ref_CDF);       // histogram equalization on grayscale image
    hist_match_Color(T_trans_func_eq, G_trans_func_eq, Final_trans_func_eq);   // 원본 이미지에 적용할 final_transfer_function : Final_trans_func_eq


    // 최종적으로 구해진 G 역함수로 원본이미지 변형시키기
    for (int i = 0; i < input.rows; i++)
        for (int j = 0; j < input.cols; j++)
            result.at<G>(i, j) = Final_trans_func_eq[input_gray.at<G>(i, j)];
    float *equalized_PDF = cal_PDF(result);
    //float *equalized_CDF = cal_CDF(result);


    for (int i = 0; i < L; i++) {
        // write PDF
        fprintf(f_PDF, "%d\t%f\n", i, inp_PDF[i]);
        fprintf(f_matched_PDF, "%d\t%f\n", i, equalized_PDF[i]);
//        fprintf(f_matched_CDF, "%d\t%f\n", i, equalized_CDF[i]);
        // write transfer functions
        fprintf(final_trans_func_eq, "%d\t%d\n", i, Final_trans_func_eq[i]);
    }
    //////////////////////// histogram 출력 //////////////////////////
    namedWindow("input",WINDOW_AUTOSIZE);
    imshow("input", plot_PDF(inp_PDF));
    
    namedWindow("reference",WINDOW_AUTOSIZE);
    imshow("reference", plot_PDF(ref_PDF));
    
    namedWindow("output",WINDOW_AUTOSIZE);
    imshow("output", plot_PDF(equalized_PDF));
    
    ///////////////////////////////////////////////////////////////////////////////////
    
    
    // memory release
    free(inp_PDF);
    free(inp_CDF);
    fclose(f_PDF);
    fclose(f_matched_PDF);
    fclose(final_trans_func_eq);
    //fclose(f_matched_CDF);

    ////////////////////// Show each image ///////////////////////

    namedWindow("Grayscale_inp", WINDOW_AUTOSIZE);
    imshow("Grayscale_inp", input_gray);

    namedWindow("Grayscale_ref", WINDOW_AUTOSIZE);
    imshow("Grayscale_ref", reference_gray);

    namedWindow("RESULT", WINDOW_AUTOSIZE);
    imshow("RESULT", result);
    


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
