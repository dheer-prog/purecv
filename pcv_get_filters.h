#ifndef IP_FILTERS_H
#define IP_FILTERS_H

#ifdef __cplusplus
extern "C" {
#endif

int get_filters(
    int n,
    unsigned char *output,
    char* filter
);

#ifdef __cplusplus
}
#endif

#ifdef PCV_GET_FILTERS

void get_canny(unsigned char (*out)[n])

int get_filters(
    int n,
    unsigned char *output, 
    unsigned char* filter
)
unsigned char (*out)[n]=(unsigned char(*)[n])output;

if(strncmp(filter,"CANNY",5)==0){

}
else if(strncmp(filter,"SOBEL_X",7)==0){

}
else if(strncmp(filter,"SOBEL_Y")==0){

}
else if(strncmp(filter,"GAUSSIAN_BLUR",13)==0){

}
else{return -1;}
#endif
#endif
