//Gray = 0.299R + 0.587G + 0.114B
//THis is the formula used for conversion 

//This expects a single dimension array of the form 
//RGBRGBRGB..

#ifndef CONVERT_GRAY_H 
#define CONVERT_GRAY_H
#ifdef __cplusplus
extern "C" {
#endif

int pcv_convert_gray(unsigned char* input,int width,int height,unsigned char* output); 

#ifdef __cplusplus
}
#endif

#include<stdlib.h>
#ifdef PCV_CONVERT_GRAY

int pcv_convert_gray(unsigned char* input,int width,int height,unsigned char* output){
    int output_index=0; 
    for(int y=0;y<(width*height*3);y=y+3){
        float t=(0.299*(float)input[y])+(0.587*(float)input[y+1])+(0.114*(float)input[y+2]);
        output[output_index]=(int)t;
        output_index++;  
    }
    return 0; 
}
#endif
#endif 
