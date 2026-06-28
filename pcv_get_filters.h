/*
For gaussian I am going to use
std=1 for both x and y
and mean=0
*/

#ifndef IP_FILTERS_H
#define IP_FILTERS_H

#ifdef __cplusplus
extern "C" {
#endif

int pcv_get_filters(
    int n,
    float *output,
    char* filter
);

#ifdef __cplusplus
}
#endif

#ifdef PCV_GET_FILTERS

 
 
float pcv_get_e_power(float p){
    float num=1.0f;
    float denum=1.0f;
    float ans=0.0f; 
    for(int i=0;i<10;i++){
        ans=ans+(num/denum); 
        num=num*p; 
        if(i==0){continue;}
        denum=denum*(float)i; 
    }
    return ans;
}
void pcv_get_gaussian(int n,float (*out)[n]){
    float PI=3.14159265f; 
    for(int y=-(n/2);y<=(n/2);y++){
        for(int x=-(n/2);x<=(n/2);x++){
            float pow=((float)(x*x)+(float)(y*y))/2.0f; 
            float denum=(2.0f)*(PI)*(pcv_get_e_power(pow)); 
            out[y+(n/2)][x+(n/2)]=(1/denum); 
        }
    }
}

void pcv_get_pascal_row(float* smooth,int n){
    float num=1.0f; 
    float denum=1.0f;
    smooth[0]=1; 
    for(int i=1;i<n;i++){
        num=num*((float)(n-i));
        denum=denum*(float)i; 
        smooth[i]=num/denum;
    }
}

void pcv_get_derv(float* derv,int n){
    int i;
    float smoother[n - 2];

    pcv_get_pascal_row(smoother, n - 2);
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

void pcv_get_sobel_x(int n,float (*out)[n]){
    float dev[n]; 
    float smooth[n];
    if(n==3){
        dev[0]=-1; 
        dev[1]=0 ;
        dev[2]=1; 
        smooth[0]=1; 
        smooth[1]=2; 
        smooth[2]=1; 
    }
    else{
        pcv_get_pascal_row(smooth,n); 
        pcv_get_derv(dev,n); 
    }
    for(int y=0;y<n;y++){
        for(int x=0;x<n;x++){
            out[y][x]=smooth[y]*dev[x];
        }
    }
}
void pcv_get_sobel_y(int n,float(*out)[n]){
    pcv_get_sobel_x(n,out); 
    for(int y=0;y<n;y++){
        for(int x=y+1;x<n;x++){
            float temp=out[y][x];
            out[y][x]=out[x][y]; 
            out[x][y]=temp;
             
        }
    }
}
int pcv_compare_string(char* n1,char* n2){
    int i =0;
    while(n1[i]!='\0' && n2[i]!='\0'){
        if(n1[i]!=n2[i]){return 1;}
    }
    if(n1[i]=='\0' && n2[i]=='\0'){return 0;}
    return 1;
}
int pcv_get_filters(
    int n,
    float *output, 
    char* filter
){
float (*out)[n]=(float(*)[n])output;

if(pcv_compare_string(filter,"BOX")==0){
    const float denum=1.0f/(float)(n*n);
    for(int y=0;y<n;y++){
        for(int x=0;x<n;x++){
            out[y][x]=denum; 
        }
    }
}
else if(pcv_compare_string(filter,"SOBEL_X")==0){
    pcv_get_sobel_x(n,out); 
}
else if(pcv_compare_string(filter,"SOBEL_Y")==0){
    pcv_get_sobel_y(n,out); 
}
else if(pcv_compare_string(filter,"GAUSSIAN_BLUR")==0){
    pcv_get_gaussian(n,out); 
}
else{return -1;}
return 0;
}
#endif
#endif
