#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define PCV_CONVERT_GRAY
#include "../pcv_convert_gray.h"
#define PCV_CONVOLUTION
#include "../pcv_convolution.h"
#define PCV_GET_FILTERS
#include "../pcv_get_filters.h"
#define PCV_FLOODFILL
#include "../pcv_floodfill.h"
#define PCV_SRM_IMPLEMENT
#include "../pcv_srm.h"

#include "test_support.h"

unsigned char *test_support_load_rgb_image(const char *filename, int *width, int *height, int *channels) {
    char path[256];

    snprintf(path, sizeof(path), "test_images/%s", filename);
    return stbi_load(path, width, height, channels, 3);
}

int test_support_write_png_gray(const char *filename, const unsigned char *data, int width, int height) {
    char path[256];

    snprintf(path, sizeof(path), "test_images/%s", filename);
    return stbi_write_png(path, width, height, 1, data, width);
}

void test_support_get_gaussian(int n, float *out) {
    pcv_get_gaussian(n, (float (*)[n])out);
}

void test_support_get_sobel_x(int n, float *out) {
    pcv_get_sobel_x(n, (float (*)[n])out);
}

void test_support_get_sobel_y(int n, float *out) {
    pcv_get_sobel_y(n, (float (*)[n])out);
}
