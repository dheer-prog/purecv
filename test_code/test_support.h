#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

unsigned char *test_support_load_rgb_image(const char *filename, int *width, int *height, int *channels);
int test_support_write_png_gray(const char *filename, const unsigned char *data, int width, int height);
void test_support_get_gaussian(int n, float *out);
void test_support_get_sobel_x(int n, float *out);
void test_support_get_sobel_y(int n, float *out);

#endif
