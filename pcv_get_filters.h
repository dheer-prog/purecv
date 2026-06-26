#ifndef IP_FILTERS_H
#define IP_FILTERS_H

#ifdef __cplusplus
extern "C" {
#endif

int get_filters(
    int n,
    float *output,
    char* filter
);

#ifdef __cplusplus
}
#endif

#ifdef PCV_GET_FILTERS

void get_pascal_row(float* smooth,int n){
    float num=1.0f; 
    float denum=1.0f;
    smooth[0]=1; 
    for(int i=1;i<n;i++){
        num=num*((float)(n-i));
        denum=denum*i; 
        smooth[i]=num/denum;
    }
}

void get_derv(float* derv,int n){
    int i;
    float smoother[n - 2];

    get_pascal_row(smoother, n - 2);
    for (i = 0; i < n; ++i) {
        float left = 0.0f;
        float right = 0.0f;
        if (i >= 2) {
            left = smoother[i - 2];
        }
        if (i < (n - 2)) {
            right = smoother[i];
        }
        derv[i] = left - right;
    }
}

void get_sobel_x(float (*out)[n],int n){
    float dev[n]; 
    float smooth[n];
    if(n==3){
        dev[0]=-1; 
        dev[1]=0 ;
        dev[2]=-1; 
        smooth[0]=1; 
        smooth[1]=2; 
        smooth[2]=1; 
    }
    else{
        get_pascal_row(smooth,n); 
        get_derv(dev,n); 
    }
    for(int y=0;y<n;y++){
        for(int x=0;x<n;x++){
            out[y][x]=smooth[y]*dev[x];
        }
    }
}
void get_sobel_y(float(*out)[n],int n){
    get_sobel_x(out,n); 
    for(int y=0;y<n;y++){
        for(int x=y+1;x<n;x++){
            float temp=out[y][x];
            out[y][x]=out[x][y]; 
            out[x][y]=temp;
             
        }
    }
}
int get_filters(
    int n,
    unsigned char *output, 
    unsigned char* filter
)
float (*out)[n]=(float(*)[n])output;

if(strncmp(filter,"BOX",5)==0){
    const float denum=1/16.0f;
    for(int y=0;y<n;y++){
        for(int x=0;x<n;x++){
            out[y][x]=denum; 
        }
    }
}
else if(strncmp(filter,"SOBEL_X",7)==0){
    get_sobel_x(out,n); 
}
else if(strncmp(filter,"SOBEL_Y")==0){
    get_sobel_y(out,n); 
}
else if(strncmp(filter,"GAUSSIAN_BLUR",13)==0){

}
else{return -1;}
#endif
#endif
