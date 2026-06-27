/*
Currently requires maths library hence requires the -lm flag

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

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

struct srm_p_error {
    int p1;
    int p2;
    int error;
};

struct srm_region {
    int parent;
    int mean;
    int size;
    bool is_boundary;
};

static int srm_max(int a, int b) {
    return (a > b) ? a : b;
}

static int srm_min(int a, int b) {
    return (a < b) ? a : b;
}

static float srm_delta_log(int height, int width) {
    float pixel_count = (float)height * (float)width;
    return -(logf(6.0f) + 2.0f * logf(pixel_count));
}

static float srm_br(int height, int width, struct srm_region region, int q) {
    float log_term = logf((float)region.size) - srm_delta_log(height, width);
    float denom = 2.0f * (float)q * (float)region.size;
    return 256.0f * sqrtf(log_term / denom);
}

static int srm_cmp_p_error(const void *a, const void *b) {
    const struct srm_p_error *left = (const struct srm_p_error *)a;
    const struct srm_p_error *right = (const struct srm_p_error *)b;
    return (left->error > right->error) - (left->error < right->error);
}

static int srm_find(struct srm_region *regions, int index) {
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

static void srm_unite(struct srm_region *regions, int left_root, int right_root) {
    int size_left;
    int size_right;

    left_root = srm_find(regions, left_root);
    right_root = srm_find(regions, right_root);
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

static void srm_compare(struct srm_region *regions, int left_root, int right_root, int height, int width, int q) {
    float bound_left = srm_br(height, width, regions[left_root], q);
    float bound_right = srm_br(height, width, regions[right_root], q);
    float threshold = sqrtf((bound_left * bound_left) + (bound_right * bound_right));

    if (fabsf((float)regions[left_root].mean - (float)regions[right_root].mean) <= threshold) {
        srm_unite(regions, left_root, right_root);
    }
}

static void srm_smooth_img(
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
                int sample_y = srm_max(0, srm_min(height - 1, y + delta_y));
                int delta_x;

                for (delta_x = -1; delta_x <= 1; ++delta_x) {
                    int sample_x = srm_max(0, srm_min(width - 1, x + delta_x));
                    gray_sum += img[sample_y][sample_x];
                }
            }

            gray_sum = (gray_sum + ((center_weight - 1) * img[y][x])) / (8 + center_weight);
            smoothed[y][x] = (unsigned char)gray_sum;
        }
    }
}

static void srm_calc_deriv(
    int height,
    int width,
    unsigned char (*img)[width],
    int (*diff)[width][2]
) {
    int y;

    for (y = 0; y < height; ++y) {
        int x;

        for (x = 0; x < width; ++x) {
            diff[y][x][0] = (-2 * (int)img[srm_max(0, y - 1)][x]) +
                            (2 * (int)img[srm_min(height - 1, y + 1)][x]);
            diff[y][x][1] = (-2 * (int)img[y][srm_max(0, x - 1)]) +
                            (2 * (int)img[y][srm_min(width - 1, x + 1)]);
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
    srm_smooth_img(1, width, height, img, smoothed);
    srm_calc_deriv(height, width, smoothed, diff);

    for (y = 0; y < height; ++y) {
        int x;

        for (x = 0; x < width; ++x) {
            if (x < width - 1) {
                graph[edge_count].p1 = pixel_index;
                graph[edge_count].p2 = pixel_index + 1;
                graph[edge_count].error = abs(diff[y][x][1]);
                ++edge_count;
            }

            if (y < height - 1) {
                graph[edge_count].p1 = pixel_index;
                graph[edge_count].p2 = pixel_index + width;
                graph[edge_count].error = abs(diff[y][x][0]);
                ++edge_count;
            }

            ++pixel_index;
        }
    }

    qsort(graph, (size_t)edge_count, sizeof(*graph), srm_cmp_p_error);

    pixel_index = 0;
    for (y = 0; y < height; ++y) {
        int x;

        for (x = 0; x < width; ++x) {
            regions[pixel_index].parent = pixel_index;
            regions[pixel_index].mean = smoothed[y][x];
            regions[pixel_index].size = 1;
            regions[pixel_index].is_boundary = false;
            ++pixel_index;
        }
    }

    {
        int edge_index;

        for (edge_index = 0; edge_index < edge_count; ++edge_index) {
            int root_left = srm_find(regions, graph[edge_index].p1);
            int root_right = srm_find(regions, graph[edge_index].p2);

            if (root_left != root_right) {
                srm_compare(regions, root_left, root_right, height, width, q);
            }
        }
    }

    pixel_index = 0;
    for (y = 0; y < height; ++y) {
        int x;

        for (x = 0; x < width; ++x) {
            bool is_boundary = false;

            if (y < height - 1 && srm_find(regions, pixel_index) != srm_find(regions, pixel_index + width)) {
                is_boundary = true;
            }
            if (x < width - 1 && srm_find(regions, pixel_index) != srm_find(regions, pixel_index + 1)) {
                is_boundary = true;
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
