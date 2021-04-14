Gaussian Filter RGB

1. 코드의 목적

이 코드는 gaussian filter를 RGB 3채널의 컬러 이미지에 mirroring, adjust_kernel, zero-padding 세개지 방식을 적용해 필터링하는 코드입니다.

2. How to run this code
-OpenCV version :4.3.1
-33번째줄에 input 변수에 변환을 원하는 이미지의 경로를 삽입해주세요

3. Parameter 관리

- Mat gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt);
<입력>
const Mat input : 변환할 이미지
int n : 2n+1*2n+1 kernel생성할때 사이즈 변수
float sigmaT : w(s,t)에서 t에적용할 시그마
float sigmaS : w(s,t)에서 s에적용할 시그마
const char* opt : 캐릭터형을 변환방법을 입력
<출력>
변환된 이미지가 Mat 8UC3으로 반환됨.