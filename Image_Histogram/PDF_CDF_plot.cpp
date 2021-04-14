#include "hist_func.h"
#include <stdio.h>
#include <iostream>

int main() {

	Mat input = imread("input.jpg", IMREAD_COLOR);
	Mat input_gray;

    cvtColor(input, input_gray, COLOR_RGB2GRAY);	// convert RGB to Grayscale *input_gray : 회색이미지*

	// PDF, CDF txt files
    FILE *f_PDF = fopen("PDF.txt", "w+");
    FILE *f_CDF = fopen("CDF.txt", "w+");


	// each histogram
	float *PDF = cal_PDF(input_gray);		// PDF of Input image(Grayscale) : [L] - hist_func
	float *CDF = cal_CDF(input_gray);		// CDF of Input image(Grayscale) : [L] - hist_func

	for (int i = 0; i < L; i++) {
		// write PDF, CDF
		fprintf(f_PDF, "%d\t%f\n", i, PDF[i]);
		fprintf(f_CDF, "%d\t%f\n", i, CDF[i]);
	}

	// memory release
	fclose(f_PDF);
	fclose(f_CDF);

	////////////////////// Show each image ///////////////////////

	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	//////////////////////////////////////////////////////////////

    namedWindow("PLOT_PDF", WINDOW_AUTOSIZE);
    imshow("PLOT_PDF", plot_PDF(PDF));
    
    namedWindow("PLOT_CDF", WINDOW_AUTOSIZE);
    imshow("PLOT_CDF", plot_CDF(CDF));
    
    waitKey(0);
    free(PDF);
    free(CDF);
	return 0;
}


