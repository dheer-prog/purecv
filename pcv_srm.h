/*
Currently uses internal approximations for log and square root.

use:- 
#define PCV_SRM_IMPLEMENT 
#include "pcv_srm.h"
Now you can use the functions of this file 
*/


#ifndef SRM_H
#define SRM_H

#ifdef __cplusplus
extern "C" {
#endif

int pcv_srm_segment(const unsigned char *original_data, int width, int height, int q, unsigned char *output);

#ifdef __cplusplus
}
#endif

#ifdef PCV_SRM_IMPLEMENT
 

#ifndef ABS 
#define ABS(a)(((a)<0)?(-a):(a))
#endif
#ifndef FLOAT_MAX
#define FLOAT_MAX 3.402823466e+38f
#endif
static float pcv_sqrtf(float x) {
    if (x <= 0.0f) {
        return 0.0f;
    }

    union {
        float f;
        unsigned int i;
    } converter;

    
    converter.f = x;

    //Needed to convert to uint32_t to make use of bitshifting hardware in the CPU
    converter.i = 0x1FBD1DF5 + (converter.i >> 1);
    //I don't know how the random hex helps reduce error but 
    //But the unint32 format of the float is actually the 
    //roughly equal to the log value of the float  

 
    float y = converter.f;

    y = 0.5f * (y + x / y);
    y = 0.5f * (y + x / y);

    return y;
}

static float pcv_cross_platform_fast_sqrtf(float x) {
    // #if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)
    //     // 1. Intel/AMD x86 implementation
    //     __m128 reg = _mm_set_ss(x);
    //     reg = _mm_rsqrt_ss(reg); 
    //     float rsqrt;
    //     _mm_store_ss(&rsqrt, reg);
    //     return x * rsqrt;

    // #elif defined(__ARM_NEON) || defined(__ARM_NEON__)
    //     // 2. ARM (Apple Silicon, Android, Raspberry Pi) implementation
    //     // frsqrte provides the same 12-bit approximate reciprocal square root step
    //     float32x4_t reg = vdupq_n_f32(x);
    //     float32x4_t rsqrt_reg = vrsqrteq_f32(reg);
    //     float rsqrt = vgetq_lane_f32(rsqrt_reg, 0);
    //     return x * rsqrt;

    // #elif defined(__wasm_simd128__)
    //     // 3. WebAssembly SIMD implementation
    //     // Note: Wasm doesn't have a crude estimate, so we use the exact hardware sqrt step
    //     v128_t reg = wasm_f32x4_splat(x);
    //     v128_t result_reg = wasm_f32x4_sqrt(reg);
    //     return wasm_f32x4_extract_lane(result_reg, 0);

    // #else
    //     // 4. Fallback for any other unknown CPU (RISC-V, PowerPC, etc.)
    return pcv_sqrtf(x);

}

 
 

static void pcv_swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

// partition function
static int pcv_partition(int arr[], int low, int high) {
    
    // Choose the pivot
    int pivot = arr[high];
    
    // Index of smaller element and indicates 
    // the right position of pivot found so far
    int i = low - 1;

    // Traverse arr[low..high] and move all smaller
    // elements to the left side. Elements from low to 
    // i are smaller after every iteration
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            pcv_swap(&arr[i], &arr[j]);
        }
    }
    
    // Move pivot after smaller elements and
    // return its position
    pcv_swap(&arr[i + 1], &arr[high]);  
    return i + 1;
}

// The QuickSort function implementation
static void pcv_qsort(int arr[], int low, int high) {
    if (low < high) {
        
        // pi is the partition return index of pivot
        int pi = pcv_partition(arr, low, high);

        // recursion calls for smaller elements
        // and greater or equals elements
        pcv_qsort(arr, low, pi - 1);
        pcv_qsort(arr, pi + 1, high);
    }
}

static int pcv_get_digits(float* num){ 
    int p=1; 
    int count=0;
    while((int)(*num)/p>0){
        count++; 
        p=p*10; 
    } 
    *num=(*num)/(float)p; 
    return count;
}
static float pcv_logf(float num){
    if(num==0){
        return -FLOAT_MAX; 
    }
    int mul=pcv_get_digits(&num);
    float x=num-1;
    float og_x=x; 
    float total=0.0f; 
    for(int i=1;i<=100;i++){
        if(i%2==0){
            total=total-(x/(float)i); 
        }
        else{
            total=total+(x/(float)i); 
        }
        x=x*og_x; 
    } 
    static const float  len_ten=2.30258509299f; 
    return total+((float)mul*len_ten); 
}


struct srm_p_error {
    int p1;
    int p2;
    int error;
};

struct srm_region {
    int parent;
    int mean;
    int size;
    int is_boundary;
};

static float pcv_ABSf(float value) {
    return (value < 0.0f) ? -value : value;
}

static int pcv_srm_max(int a, int b) {
    return (a > b) ? a : b;
}

static int pcv_srm_min(int a, int b) {
    return (a < b) ? a : b;
}

static float pcv_srm_delta_log(int height, int width) {
    float pixel_count = (float)height * (float)width;
    return -(pcv_logf(6.0f) + 2.0f * pcv_logf(pixel_count));
}

static float pcv_srm_br(int height, int width, struct srm_region region, int q) {
    float log_term = pcv_logf((float)region.size) - pcv_srm_delta_log(height, width);
    float denom = 2.0f * (float)q * (float)region.size;
    return 256.0f * pcv_cross_platform_fast_sqrtf(log_term / denom);
}

static int pcv_srm_cmp_p_error(const void *a, const void *b) {
    const struct srm_p_error *left = (const struct srm_p_error *)a;
    const struct srm_p_error *right = (const struct srm_p_error *)b;
    return (left->error > right->error) - (left->error < right->error);
}

static int pcv_srm_find(struct srm_region *regions, int index) {
    int root = index;

    while (regions[root].parent != root) {
        root = regions[root].parent;
    }

    while (regions[index].parent != index) {
        int parent = regions[index].parent;
        regions[index].parent = root;
        index = parent;
    }

    return root;
}

static void pcv_srm_unite(struct srm_region *regions, int left_root, int right_root) {
    int size_left;
    int size_right;

    left_root = pcv_srm_find(regions, left_root);
    right_root = pcv_srm_find(regions, right_root);
    if (left_root == right_root) {
        return;
    }

    if (regions[left_root].size < regions[right_root].size) {
        int temp = left_root;
        left_root = right_root;
        right_root = temp;
    }

    regions[right_root].parent = left_root;
    size_left = regions[left_root].size;
    size_right = regions[right_root].size;
    regions[left_root].mean =
        (regions[left_root].mean * size_left + regions[right_root].mean * size_right) /
        (size_left + size_right);
    regions[left_root].size += size_right;
}

static void pcv_srm_compare(struct srm_region *regions, int left_root, int right_root, int height, int width, int q) {
    float bound_left = pcv_srm_br(height, width, regions[left_root], q);
    float bound_right = pcv_srm_br(height, width, regions[right_root], q);
    float threshold = pcv_cross_platform_fast_sqrtf((bound_left * bound_left) + (bound_right * bound_right));

    if (pcv_ABSf((float)regions[left_root].mean - (float)regions[right_root].mean) <= threshold) {
        pcv_srm_unite(regions, left_root, right_root);
    }
}

static void pcv_srm_smooth_img(
    int center_weight,
    int width,
    int height,
    const unsigned char (*img)[width],
    unsigned char (*smoothed)[width]
) {
    int y;

    for (y = 0; y < height; ++y) {
        int x;

        for (x = 0; x < width; ++x) {
            int gray_sum = 0;
            int delta_y;

            for (delta_y = -1; delta_y <= 1; ++delta_y) {
                int sample_y = pcv_srm_max(0, pcv_srm_min(height - 1, y + delta_y));
                int delta_x;

                for (delta_x = -1; delta_x <= 1; ++delta_x) {
                    int sample_x = pcv_srm_max(0, pcv_srm_min(width - 1, x + delta_x));
                    gray_sum += img[sample_y][sample_x];
                }
            }

            gray_sum = (gray_sum + ((center_weight - 1) * img[y][x])) / (8 + center_weight);
            smoothed[y][x] = (unsigned char)gray_sum;
        }
    }
}

static void pcv_srm_calc_deriv(
    int height,
    int width,
    unsigned char (*img)[width],
    int (*diff)[width][2]
) {
    int y;

    for (y = 0; y < height; ++y) {
        int x;

        for (x = 0; x < width; ++x) {
            diff[y][x][0] = (-2 * (int)img[pcv_srm_max(0, y - 1)][x]) +
                            (2 * (int)img[pcv_srm_min(height - 1, y + 1)][x]);
            diff[y][x][1] = (-2 * (int)img[y][pcv_srm_max(0, x - 1)]) +
                            (2 * (int)img[y][pcv_srm_min(width - 1, x + 1)]);
        }
    }
}

int pcv_srm_segment(const unsigned char *original_data, int width, int height, int q, unsigned char *output) {
    if (!original_data || !output || width <= 0 || height <= 0 || q <= 0) {
        return 1;
    }
    const unsigned char (*img)[width];
    const int edge_capacity = (height * (width - 1)) + ((height - 1) * width);
    unsigned char smoothed[height][width];
    int diff[height][width][2];
    struct srm_p_error graph[edge_capacity];
    struct srm_region regions[height * width];
    int edge_count = 0;
    int pixel_index = 0;
    int y;

    img = (const unsigned char (*)[width])original_data;
    pcv_srm_smooth_img(1, width, height, img, smoothed);
    pcv_srm_calc_deriv(height, width, smoothed, diff);

    for (y = 0; y < height; ++y) {
        int x;

        for (x = 0; x < width; ++x) {
            if (x < width - 1) {
                graph[edge_count].p1 = pixel_index;
                graph[edge_count].p2 = pixel_index + 1;
                graph[edge_count].error = ABS(diff[y][x][1]);
                ++edge_count;
            }

            if (y < height - 1) {
                graph[edge_count].p1 = pixel_index;
                graph[edge_count].p2 = pixel_index + width;
                graph[edge_count].error = ABS(diff[y][x][0]);
                ++edge_count;
            }

            ++pixel_index;
        }
    }

    pcv_qsort(graph, (size_t)edge_count, sizeof(*graph), pcv_srm_cmp_p_error);

    pixel_index = 0;
    for (y = 0; y < height; ++y) {
        int x;

        for (x = 0; x < width; ++x) {
            regions[pixel_index].parent = pixel_index;
            regions[pixel_index].mean = smoothed[y][x];
            regions[pixel_index].size = 1;
            regions[pixel_index].is_boundary = 0;
            ++pixel_index;
        }
    }

    {
        int edge_index;

        for (edge_index = 0; edge_index < edge_count; ++edge_index) {
            int root_left = pcv_srm_find(regions, graph[edge_index].p1);
            int root_right = pcv_srm_find(regions, graph[edge_index].p2);

            if (root_left != root_right) {
                pcv_srm_compare(regions, root_left, root_right, height, width, q);
            }
        }
    }

    pixel_index = 0;
    for (y = 0; y < height; ++y) {
        int x;

        for (x = 0; x < width; ++x) {
            int is_boundary = 0;

            if (y < height - 1 && pcv_srm_find(regions, pixel_index) != pcv_srm_find(regions, pixel_index + width)) {
                is_boundary = 1;
            }
            if (x < width - 1 && pcv_srm_find(regions, pixel_index) != pcv_srm_find(regions, pixel_index + 1)) {
                is_boundary = 1;
            }

            regions[pixel_index].is_boundary = is_boundary;
            ++pixel_index;
        }
    }

    for (pixel_index = 0; pixel_index < width * height; ++pixel_index) {
        output[pixel_index] = regions[pixel_index].is_boundary ? 255 : original_data[pixel_index];
    }
    return 0;
}

#endif

#endif
