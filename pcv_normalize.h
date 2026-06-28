/*
This assume IEEE 754 format 
*/

#ifndef FLOAT_MAX
#define FLOAT_MAX 3.402823466e+38f
#endif

#ifndef PCV_NORM_H 
#define PCV_NORM_H

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#ifdef PCV_NORMALIZE
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef INT_MAX
#define INT_MAX  
#endif
int pcv_general_norm(void* input,
int height,
int width, 
int bottom, 
int upp,
void* output){
    void (*in)[width]=(void *[width])input;
    float mini=

}

#endif
#endif
