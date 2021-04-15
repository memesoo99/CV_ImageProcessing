# Image Histogram

## Histogram Matching

1. Explanation of the Code

    Histogram matching converts the original image's histogram distribution as simliar as possible to the histogram distribution of the reference image.

    R : 입력 이미지

    Z : 레퍼런스 이미지

[image]

입력이미지를 Equalizing하는 함수 T를 구하고, 레퍼런스 함수를 Equalizing 하는 함수 G를 구해서 두 변환함수를 이용해 최종변환함수를 구하고 그 매핑함수를 다시 원본이미지에 적용해 결과를 도출하는 식으로 작성했다.

[원본이미지에 적용할 매핑함수 구하는 코드]

```jsx
void hist_match_Color(G* T_trans,G* G_trans, G* Final_trans){
    for (int i = 0; i < L; i++){
        int a = T_trans[i];
        int temp = return_mapping_down(a,G_trans);
        if (temp == -10) temp = return_mapping_up(a, G_trans);
        else {Final_trans[i] = temp;}
    }
}
```

입력은 위에 수식에서 나온 T함수와, G함수, 그리고 새로생성한 변환함수를 저장할 Fianl_trans 이렇게 세가지 입니다.

1. 우선, 원본이미지에서 도출한 T함수에 접근해 각 intensity가 어떤 값으로 매핑 되어있는지를 변수 a에 저장합니다.
2. return_mapping_down함수에 매핑된 값(a)과, G함수를 인자로 전달합니다. Return_mapping_down함수의 의의는 a를매핑된 값으로 갖고있는 화소값을 찾는것이고, 만약 a가 어떤 화소값에도 매칭되어있지 않는 경우엔 최대한 가까이 있는 화소값을 구하는 구조입니다.

```jsx
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
```

Return_mapping_down함수는 재귀함수로 구현되어있습니다. 일단 a가 0보다 작은지 체크하는 if문을 건너뛰고, 그 다음 for문을 보면, for문을 통해 G매핑에서 가장 낮은 intensity부터 매핑 결과가 a인 intensity를 찾습니다. 이떄 a를 매핑결과로 갖는 intensity를 찾으면 해당 intensity값을 리턴합니다. 만약 찾지 못한다면,  a를 하나씩 감소해나가며 a가 0이 될때까지 계속 재귀적으로 호출합니다. 만약 0~a까지의 화소값을 매핑값으로 아무도 갖지 않는다면 if문에 걸려 -10을 리턴하고 return_mapping_down함수를 빠져나와

```jsx
void hist_match_Color(G* T_trans,G* G_trans, G* Final_trans){
    for (int i = 0; i < L; i++){
        int a = T_trans[i];
        int temp = return_mapping_down(a,G_trans);
        if (temp == -10) temp = return_mapping_up(a, G_trans);
        else {Final_trans[i] = temp;}
    }
}
```

Hist_match_color 함수의 Temp에 -10을 저장합니다. Temp가 -10의 값을 갖는것이 확인되면 a언더에는 매칭이 되는 값이 없다고 판단하여 a-255를 탐색하는 return_mapping_up함수를 호출합니다.

```jsx
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
```

Return_mapping_up함수도 return_mapping_down함수와 똑같이 동작합니다. 다만 아래로탐색, 위로 탐색을 모두했음에도 단 한개의 일치하는 값을 찾지 못할 시 임의로 0을 반환하도록 설정되었습니다.

위 과정을 모든 픽셀에 적용해서 Final_trans라는 매핑함수를 생성한뒤, 원본이미지에 적용해서 이미지를 변환하면 결과값을 얻을 수 있습니다.

2. Result

[img]

맨 우측을 reference로 해서 히스토그램 매칭을 실시했을때, 이미지가 비슷하게 바뀌는것을 확인 할 수 있었습니다.

[oimg]

좌측부터 차례대로 입력 히스토그램, 레퍼런스 히스토그램, 결과물 히스토그램입니다.

입력과 결과물을 비교해 봤을때 레퍼런스와 비슷한 분포로 변형됐음을 확인 할 수 있습니다.
