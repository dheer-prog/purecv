#include <criterion/criterion.h>
#include <stdlib.h>
#include <string.h>

#include "stb_image.h"

#include "../pcv_convert_gray.h"
#include "../pcv_convolution.h"
#include "test_support.h"

#define SOBEL_SIZE 5

static float pcv_absf(float value) {
    return value < 0.0f ? -value : value;
}

static void float_to_u8_scaled(const float *input, unsigned char *output, int count) {
    float max_value = 0.0f;
    int i;

    for (i = 0; i < count; ++i) {
        float value = input[i];
        if (value > max_value) {
            max_value = value;
        }
    }

    if (max_value <= 0.0f) {
        memset(output, 0, (size_t)count);
        return;
    }

    for (i = 0; i < count; ++i) {
        float scaled = (input[i] * 255.0f) / max_value;
        if (scaled < 0.0f) {
            scaled = 0.0f;
        } else if (scaled > 255.0f) {
            scaled = 255.0f;
        }
        output[i] = (unsigned char)scaled;
    }
}

static void non_max_suppress(
    const float *gx,
    const float *gy,
    const float *magnitude,
    int width,
    int height,
    float *suppressed
) {
    int y;

    memset(suppressed, 0, (size_t)width * (size_t)height * sizeof(float));
    for (y = 1; y < (height - 1); ++y) {
        int x;
        for (x = 1; x < (width - 1); ++x) {
            int index = (y * width) + x;
            float ax = pcv_absf(gx[index]);
            float ay = pcv_absf(gy[index]);
            float current = magnitude[index];
            float prev;
            float next;

            if ((ay * 2.0f) <= ax) {
                prev = magnitude[index - 1];
                next = magnitude[index + 1];
            } else if ((ax * 2.0f) <= ay) {
                prev = magnitude[index - width];
                next = magnitude[index + width];
            } else if ((gx[index] >= 0.0f && gy[index] >= 0.0f) ||
                       (gx[index] < 0.0f && gy[index] < 0.0f)) {
                prev = magnitude[index - width - 1];
                next = magnitude[index + width + 1];
            } else {
                prev = magnitude[index - width + 1];
                next = magnitude[index + width - 1];
            }

            if (current >= prev && current >= next) {
                suppressed[index] = current;
            }
        }
    }
}

static void hysteresis_threshold(
    const float *suppressed,
    int width,
    int height,
    unsigned char *edges
) {
    int pixel_count = width * height;
    int *stack = (int *)malloc((size_t)pixel_count * sizeof(int));
    float max_value = 0.0f;
    float high;
    float low;
    int top = 0;
    int i;

    cr_assert_not_null(stack, "Could not allocate hysteresis stack");
    memset(edges, 0, (size_t)pixel_count);

    for (i = 0; i < pixel_count; ++i) {
        if (suppressed[i] > max_value) {
            max_value = suppressed[i];
        }
    }

    high = max_value * 0.25f;
    low = high * 0.5f;

    for (i = 0; i < pixel_count; ++i) {
        if (suppressed[i] >= high) {
            edges[i] = 255;
            stack[top++] = i;
        } else if (suppressed[i] >= low) {
            edges[i] = 128;
        }
    }

    while (top > 0) {
        int index = stack[--top];
        int x = index % width;
        int y = index / width;
        int dy;

        for (dy = -1; dy <= 1; ++dy) {
            int dx;
            for (dx = -1; dx <= 1; ++dx) {
                int nx = x + dx;
                int ny = y + dy;
                int neighbor;

                if ((dx == 0 && dy == 0) || nx < 0 || nx >= width || ny < 0 || ny >= height) {
                    continue;
                }

                neighbor = (ny * width) + nx;
                if (edges[neighbor] == 128) {
                    edges[neighbor] = 255;
                    stack[top++] = neighbor;
                }
            }
        }
    }

    for (i = 0; i < pixel_count; ++i) {
        if (edges[i] != 255) {
            edges[i] = 0;
        }
    }

    free(stack);
}

Test(filters, sobel_5x5_kernels) {
    static const float expected_x[SOBEL_SIZE * SOBEL_SIZE] = {
        -1, -2, 0, 2, 1,
        -4, -8, 0, 8, 4,
        -6, -12, 0, 12, 6,
        -4, -8, 0, 8, 4,
        -1, -2, 0, 2, 1
    };
    static const float expected_y[SOBEL_SIZE * SOBEL_SIZE] = {
        -1, -4, -6, -4, -1,
        -2, -8, -12, -8, -2,
        0, 0, 0, 0, 0,
        2, 8, 12, 8, 2,
        1, 4, 6, 4, 1
    };
    float sobel_x[SOBEL_SIZE * SOBEL_SIZE];
    float sobel_y[SOBEL_SIZE * SOBEL_SIZE];
    int i;

    test_support_get_sobel_x(SOBEL_SIZE, sobel_x);
    test_support_get_sobel_y(SOBEL_SIZE, sobel_y);

    for (i = 0; i < (SOBEL_SIZE * SOBEL_SIZE); ++i) {
        cr_expect_eq(sobel_x[i], expected_x[i], "Unexpected 5x5 Sobel X kernel at %d", i);
        cr_expect_eq(sobel_y[i], expected_y[i], "Unexpected 5x5 Sobel Y kernel at %d", i);
    }
}

Test(filters, canny_like_pipeline_writes_expected_images) {
    unsigned char *rgb;
    unsigned char *gray;
    unsigned char *blurred;
    unsigned char *sobel_x_vis;
    unsigned char *sobel_y_vis;
    unsigned char *magnitude_vis;
    unsigned char *edges;
    float *grad_x;
    float *grad_y;
    float *magnitude;
    float *suppressed;
    float gaussian_5x5[25];
    float sobel_x[SOBEL_SIZE * SOBEL_SIZE];
    float sobel_y[SOBEL_SIZE * SOBEL_SIZE];
    int width;
    int height;
    int channels;
    int pixel_count;
    int i;

    test_support_get_gaussian(5, gaussian_5x5);
    rgb = test_support_load_rgb_image("coins.png", &width, &height, &channels);
    cr_assert_not_null(rgb, "Could not load coins.png");

    pixel_count = width * height;
    gray = (unsigned char *)malloc((size_t)pixel_count);
    blurred = (unsigned char *)malloc((size_t)pixel_count);
    sobel_x_vis = (unsigned char *)malloc((size_t)pixel_count);
    sobel_y_vis = (unsigned char *)malloc((size_t)pixel_count);
    magnitude_vis = (unsigned char *)malloc((size_t)pixel_count);
    edges = (unsigned char *)malloc((size_t)pixel_count);
    grad_x = (float *)malloc((size_t)pixel_count * sizeof(float));
    grad_y = (float *)malloc((size_t)pixel_count * sizeof(float));
    magnitude = (float *)malloc((size_t)pixel_count * sizeof(float));
    suppressed = (float *)malloc((size_t)pixel_count * sizeof(float));

    cr_assert(gray && blurred && sobel_x_vis && sobel_y_vis && magnitude_vis && edges,
              "Could not allocate image buffers");
    cr_assert(grad_x && grad_y && magnitude && suppressed,
              "Could not allocate float buffers");

    cr_assert_eq(pcv_convert_gray(rgb, width, height, gray), 0, "Grayscale conversion failed");
    cr_assert_eq(pcv_convolve_U8(gray, width, height, gaussian_5x5, 5, 5, 1, blurred, 1), 0,
                 "Gaussian blur convolution failed");

    test_support_get_sobel_x(SOBEL_SIZE, sobel_x);
    test_support_get_sobel_y(SOBEL_SIZE, sobel_y);

    cr_assert_eq(pcv_convolve_F(blurred, width, height, sobel_x, SOBEL_SIZE, SOBEL_SIZE, 1, grad_x, 1), 0,
                 "Sobel X convolution failed");
    cr_assert_eq(pcv_convolve_F(blurred, width, height, sobel_y, SOBEL_SIZE, SOBEL_SIZE, 1, grad_y, 1), 0,
                 "Sobel Y convolution failed");

    for (i = 0; i < pixel_count; ++i) {
        magnitude[i] = pcv_absf(grad_x[i]) + pcv_absf(grad_y[i]);
    }

    non_max_suppress(grad_x, grad_y, magnitude, width, height, suppressed);
    hysteresis_threshold(suppressed, width, height, edges);

    for (i = 0; i < pixel_count; ++i) {
        grad_x[i] = pcv_absf(grad_x[i]);
        grad_y[i] = pcv_absf(grad_y[i]);
    }

    float_to_u8_scaled(grad_x, sobel_x_vis, pixel_count);
    float_to_u8_scaled(grad_y, sobel_y_vis, pixel_count);
    float_to_u8_scaled(magnitude, magnitude_vis, pixel_count);

    cr_expect_neq(test_support_write_png_gray("coins_blur_5x5.png", blurred, width, height), 0,
                  "Could not write blurred image");
    cr_expect_neq(test_support_write_png_gray("coins_sobel_x_5x5.png", sobel_x_vis, width, height), 0,
                  "Could not write Sobel X image");
    cr_expect_neq(test_support_write_png_gray("coins_sobel_y_5x5.png", sobel_y_vis, width, height), 0,
                  "Could not write Sobel Y image");
    cr_expect_neq(test_support_write_png_gray("coins_gradient_mag_5x5.png", magnitude_vis, width, height), 0,
                  "Could not write magnitude image");
    cr_expect_neq(test_support_write_png_gray("coins_canny_like_5x5.png", edges, width, height), 0,
                  "Could not write canny-like image");

    free(suppressed);
    free(magnitude);
    free(grad_y);
    free(grad_x);
    free(edges);
    free(magnitude_vis);
    free(sobel_y_vis);
    free(sobel_x_vis);
    free(blurred);
    free(gray);
    stbi_image_free(rgb);
}
