/*
This assume IEEE 754 format 
*/

#ifndef FLOAT_MAX
#define FLOAT_MAX 3.402823466e+38f
#endif

#ifndef PCV_ROTATE_H
#define PCV_ROTATE_H

#ifdef __cplusplus
extern "C" {
#endif

int pcv_rotate(
    unsigned char* input,
    int width, 
    int height, 
    unsigned char* output,
    float theta 
);


#ifdef __cplusplus
}
#endif

#ifdef PCV_ROTATE
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif
#include <stdio.h>
#include <math.h>

float pcv_portable_trig(float degrees,int type) {
    // 1. Range reduction to the nearest 90-degree boundary
    // This finds which quadrant (k) the angle belongs to
    int k = (int)(degrees * 0.011111111111111112f + (degrees >= 0.0f ? 0.5f : -0.5f));
    
    // Exact subtraction because 90 is an integer representing a clean float value
    float x_reduced = degrees - (float)k * 90.0f; 
    
    // Convert only the tiny reduced angle [-45, 45] to radians for the polynomial
    float rad = x_reduced * 0.017453292519943295f; 
    float rad2 = rad * rad;

    // 2. Evaluate Minimax Polynomials for the small [-45, 45] window
    // Sine Polynomial coefficients (equivalent to an optimized x - x^3/3! + x^5/5!...)
    const float s1 = -0.1666666505f; 
    const float s2 =  0.0083332155f;
    const float s3 = -0.0001983355f;
    float poly_sin = rad + rad * rad2 * (s1 + rad2 * (s2 + rad2 * s3));

    // Cosine Polynomial coefficients (equivalent to an optimized 1 - x^2/2! + x^4/4!...)
    const float c1 = -0.5000000000f;
    const float c2 =  0.0416666418f;
    const float c3 = -0.0013888397f;
    float poly_cos = 1.0f + rad2 * (c1 + rad2 * (c2 + rad2 * c3));

    // 3. Check the quadrant index (k mod 4) to output the correct identity
    int quadrant = k & 3; 
    if (type==1)//type sin
    {
        if (quadrant == 0) return poly_sin;  // Nearest to 0 or 360 degrees
        if (quadrant == 1) return poly_cos;  // Nearest to 90 degrees (sin becomes cos)
        if (quadrant == 2) return -poly_sin; // Nearest to 180 degrees
        return -poly_cos;                    // Nearest to 270 degrees (sin becomes -cos)
    }
    else if(type==2){//type cos
        if (quadrant == 0) return poly_cos;  // Nearest to 0 or 360 degrees (cos stays cos)
        if (quadrant == 1) return -poly_sin; // Nearest to 90 degrees (cos becomes -sin)
        if (quadrant == 2) return -poly_cos; // Nearest to 180 degrees (cos becomes -cos)
        return poly_sin;  
    }
    return -2;
}
 
int pcv_rotate(
    unsigned char* input,
    int width, 
    int height, 
    unsigned char* output,
    float theta 
){ 
    unsigned char(*in)[width]=(unsigned char (*)[width])input;
    unsigned char(*out)[width]=(unsigned char (*)[width])output; 
    int x_cen=(width/2); 
    int y_cen=(height/2);
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            int x1=(x-x_cen)*(pcv_portable_trig(theta,2))+(y-y_cen)*(pcv_portable_trig(theta,1))+x_cen; 
            int y1=(x_cen-x)*(pcv_portable_trig(theta,1))+(y-y_cen)*(pcv_portable_trig(theta,2))+y_cen;
            if(x1<0 || y1<0 || x1>=width || y1>=height){out[y][x]=0;}
            else{out[y][x]=in[y1][x1];}
             
        }
    } 
    return 0;
}   
#endif 
#endif