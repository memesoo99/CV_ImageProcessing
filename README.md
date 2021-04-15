# CV_ImageProcessing
### 🎯For specific result of each process, checkout the readme.txt of each folder

> Fundamental Image processing techniques for Computer Vision
> 

---

## 💻 Development Environment

- Language : C++
- IDE : XCode
- OpenCV : 4.5.2

---

## 💻 Project Info

1. Image_Display
    - rotate.cpp : Image rotation using different Interpolation techniques
    - stitching.cpp : Image Stit hing using Affine Transformation

2. Image_Histogram
    - hist_equalization_RGB/YUV/Gray.cpp : histogram equalization for different channels
    - hist_func.h : header file for calculating PDF and CDF
    - PDF_CDF_plot.cpp : Plotting histogram of PDF/CDF
    - hist_stretching.cpp : Stretching Image Histogram in order to increase image contrast
    - hist_matching_gray/RGB.cpp : Unifying the contrast level of a group of images to the contrast level of reference image

3. Image_Filtering : using zero-padding, mirroring, adjustive filtering
    - Gaussian_Filter_Gray / Gaussian_Filter_RGB : Gaussian filter
    - GaussianSeperableGray / GaussianSeperableRGB : Gaussian Filter usung seperable filter multiplication
    - MeanFilterGray / MeanFilterRGB
    - Sobel_Laplacian_RGB / Sobel_Laplacian_Gray : Sobel filtering + laplacian filtering
    - Unsharp : Image Unsharpening

4. Image_Restoration
    - Gaussian_Noise_Removal.cpp : Gaussian Noise Add and removal using Gaussian Filter + Bilateral Filter
    - salt_and_pepper.cpp : Adding salt_and_pepper noise and noise filterting

5. Image_Segmantation
    - Adaptive_threshold_segmentation.cpp : Image segmenting using pixelwise adaptive threshold
    - kmeans_gray/RGB : kmeans clustering
    
6. Edge_Corner_Detection
    - Canny_Edge_Detection : Using opencv::Canny
    - LoG : Laplacian of Gaussian
    - Harris_corner : Harris corner Detector using cv::cornerHarris. Added non-maximum suppression and subpixel accuracy
