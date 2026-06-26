/*
The caller is responsible for allocating the output buffer for the current
image dimensions and selected stride.

Currently this only works for grayscale images.
*/

#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#ifdef __cplusplus
extern "C" {
#endif

int convolve_U8(
    const unsigned char *input,
    int width,
    int height,
    const float *kernel,
    int k_width,
    int k_height,
    int stride,
    unsigned char *output,
    int padding
);

int convolve_F(
    const unsigned char *input,
    int width,
    int height,
    const float *kernel,
    int k_width,
    int k_height,
    int stride,
    float *output,
    int padding
);

#ifdef __cplusplus
}
#endif

#ifdef PCV_CONVOLUTION

static unsigned char pcv_convolution_sample(
    const unsigned char *input,
    int width,
    int height,
    int x,
    int y,
    int padding
) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        if (padding) {
            return 0;
        }

        if (x < 0) {
            x = 0;
        } else if (x >= width) {
            x = width - 1;
        }

        if (y < 0) {
            y = 0;
        } else if (y >= height) {
            y = height - 1;
        }
    }

    return input[(y * width) + x];
}
int convolve_U8(
    const unsigned char *input,
    int width,
    int height,
    const float *kernel,
    int k_width,
    int k_height,
    int stride,
    unsigned char *output,
    int padding
) {
    int output_index = 0;
    int kernel_sum = 0;
    int kernel_half_width;
    int kernel_half_height;
    int y;
    if (!input || !kernel || !output || width <= 0 || height <= 0 ||
        k_width <= 0 || k_height <= 0 || stride <= 0) {
        return 1;
    }
    for (y = 0; y < k_height; ++y) {
        int x;
        for (x = 0; x < k_width; ++x) {
            kernel_sum += kernel[(y * k_width) + x];
        }
    }
    if (kernel_sum == 0) {
        kernel_sum = 1;
    }
    kernel_half_width = k_width / 2;
    kernel_half_height = k_height / 2;

    for (y = 0; y < height; y += stride) {
        int x;
        for (x = 0; x < width; x += stride) {
            int total_val = 0;
            int ky;
            for (ky = 0; ky < k_height; ++ky) {
                int kx;
                for (kx = 0; kx < k_width; ++kx) {
                    int sample_x = x + kx - kernel_half_width;
                    int sample_y = y + ky - kernel_half_height;
                    int kernel_index = (ky * k_width) + kx;
                    unsigned char sample =
                        pcv_convolution_sample(input, width, height, sample_x, sample_y, padding);

                    total_val += (int)kernel[kernel_index] * (int)sample;
                }
            }

            total_val /= kernel_sum;
            if (total_val < 0) {
                total_val = 0;
            } else if (total_val > 255) {
                total_val = 255;
            }

            output[output_index++] = (unsigned char)total_val;
        }
    }

    return 0;
}

int convolve_F(
    const unsigned char *input,
    int width,
    int height,
    const float *kernel,
    int k_width,
    int k_height,
    int stride,
    float *output,
    int padding
) {
    int output_index = 0;
    int kernel_sum = 0;
    int kernel_half_width;
    int kernel_half_height;
    int y;
    if (!input || !kernel || !output || width <= 0 || height <= 0 ||
        k_width <= 0 || k_height <= 0 || stride <= 0) {
        return 1;
    }
    for (y = 0; y < k_height; ++y) {
        int x;
        for (x = 0; x < k_width; ++x) {
            kernel_sum += kernel[(y * k_width) + x];
        }
    }
    if (kernel_sum == 0) {
        kernel_sum = 1;
    }
    kernel_half_width = k_width / 2;
    kernel_half_height = k_height / 2;

    for (y = 0; y < height; y += stride) {
        int x;
        for (x = 0; x < width; x += stride) {
            int total_val = 0;
            int ky;
            for (ky = 0; ky < k_height; ++ky) {
                int kx;
                for (kx = 0; kx < k_width; ++kx) {
                    int sample_x = x + kx - kernel_half_width;
                    int sample_y = y + ky - kernel_half_height;
                    int kernel_index = (ky * k_width) + kx;
                    unsigned char sample =
                        pcv_convolution_sample(input, width, height, sample_x, sample_y, padding);

                    total_val += (int)kernel[kernel_index] * (int)sample;
                }
            }

            total_val /= kernel_sum;
            if (total_val < 0) {
                total_val = 0;
            } else if (total_val > 255) {
                total_val = 255;
            }

            output[output_index++] = (float)total_val;
        }
    }

    return 0;
}

#endif
#endif
