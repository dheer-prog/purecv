/*


Currently only supports 
4 way floodfill 

The seeds are expected in y,x format 
*/

#ifndef FF_H
#define FF_H

#ifdef __cplusplus
extern "C" {
#endif
int floodfill(
    const unsigned char* input, 
    int width, 
    int height,
    unsigned char* output,
    int (*seeds)[2],
    int num_seeds,
    int threshold,
    unsigned char new_val
);

#ifdef __cplusplus
}
#endif

#ifdef PCV_FLOODFILL 
int floodfill(
    const unsigned char* input, 
    int width, 
    int height,
    unsigned char* output,
    int (*seeds)[2],
    int num_seeds,
    int threshold,
    unsigned char new_val
){
    if (width <= 0 || height <= 0 || !input || !output) return 1;
    unsigned char (*out)[width]=(unsigned char (*)[width])output;
    const unsigned char (*in)[width]=(const unsigned char (*)[width])input;  
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            out[y][x]=in[y][x]; 
        }
    }
    for(int i=0;i<num_seeds;i++){
        if(seeds[i][0]<0 || seeds[i][0]>=height || seeds[i][1]<0 || seeds[i][1]>=width){
            return 1; 
        }
        int q[width*height][2]; 
        int last_index=0; 
        int curr_index=0;
        unsigned char visited[height][width];
        for(int y=0;y<height;y++){
            for(int x=0;x<width;x++){
                visited[y][x]=0; 
            }
        }
        visited[seeds[i][0]][seeds[i][1]]=1;
        unsigned char seed_val=in[seeds[i][0]][seeds[i][1]]; 
        q[curr_index][0]=seeds[i][0]; 
        q[curr_index][1]=seeds[i][1]; 
        while(curr_index<=last_index){
            int y=q[curr_index][0]; 
            int x=q[curr_index][1]; 
            curr_index++; 
            if(in[y][x]<seed_val){
                if(seed_val-in[y][x]>threshold){continue;}
            } 
            if(in[y][x]>=seed_val && in[y][x]-seed_val>threshold){continue;}
            out[y][x]=new_val; 
            if(y<height-1 && visited[y+1][x]!=1){
                visited[y+1][x]=1; 
                q[++last_index][0]=y+1; 
                q[last_index][1]=x;
            }
            if(y>0 && visited[y-1][x]!=1){
                visited[y-1][x]=1;
                q[++last_index][0]=y-1;
                q[last_index][1]=x;
            }
            if(x<width-1 && visited[y][x+1]!=1){
                visited[y][x+1]=1; 
                q[++last_index][0]=y; 
                q[last_index][1]=x+1;
            }
            if(x>0 && visited[y][x-1]!=1){
                visited[y][x-1]=1;
                q[++last_index][0]=y;
                q[last_index][1]=x-1;       
            }
        }
    }
    return 0; 
}
#endif 
#endif