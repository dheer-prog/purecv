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
 
int pcv_general_norm(void* input,
int height,
int width, 
int bottom, 
int upp,
int input_type){
    if(!width || !height || !bottom || !upp || upp<bottom){return 1;}
    if(input_type==1){
        float (*out_f)[width]=(float (*)[width])input; 
        float mini=FLOAT_MAX; 
        float maxer=0; 
        for(int y=0;y<height;y++){
            for(int x=0;x<height;x++){
                mini=MIN(mini,out_f[y][x]); 
                maxer=MAX(maxer,out_f[y][x]); 
            }
        }
        float denum=1.0f/(maxer-mini); 
        float num=(float)upp-(float)bottom; 
        for(int y=0;y<height;y++){
            for(int x=0;x<width;x++){ 
                out_f[y][x]=(((out_f[y][x]-mini)*denum)*num)+bottom; 
                 
            }
        }
        return 0; 
    }
    else{
        unsigned char (*out_u8)[width]=(unsigned char (*)[width])input;
         
        unsigned char  mini=255; 
        unsigned char  maxer=0; 
        for(int y=0;y<height;y++){
            for(int x=0;x<height;x++){
                mini=MIN(mini,out_u8[y][x]); 
                maxer=MAX(maxer,out_u8[y][x]); 
            }
        }
        float denum=1.0f/(float)(maxer-mini); 
        float num=upp-bottom; 
        for(int y=0;y<height;y++){
            for(int x=0;x<width;x++){ 
                out_u8[y][x]=(((out_u8[y][x]-mini)*denum)*num)+bottom; 
                 
            }
        }
        return 0;  
    }
    
     

}

#endif
#endif
