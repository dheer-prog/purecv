#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define PCV_CONVERT_GRAY
#include "../pcv_convert_gray.h"
#define PCV_SRM_IMPLEMENT
#include "../pcv_srm.h"

static void expect_true(int condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        exit(1);
    }
}

static void expect_image_eq(
    const unsigned char *actual,
    const unsigned char *expected,
    size_t count,
    const char *message
) {
    if (memcmp(actual, expected, count) != 0) {
        size_t index;

        fprintf(stderr, "FAIL: %s\n", message);
        for (index = 0; index < count; ++index) {
            if (actual[index] != expected[index]) {
                fprintf(
                    stderr,
                    "  first mismatch at index %zu: got %u expected %u\n",
                    index,
                    (unsigned)actual[index],
                    (unsigned)expected[index]
                );
                break;
            }
        }
        exit(1);
    }
}

static void test_rejects_invalid_arguments(void) {
    unsigned char pixel = 0;

    expect_true(pcv_srm_segment(NULL, 1, 1, 32, &pixel) == 1, "NULL input should fail");
    expect_true(pcv_srm_segment(&pixel, 1, 1, 32, NULL) == 1, "NULL output should fail");
    expect_true(pcv_srm_segment(&pixel, 0, 1, 32, &pixel) == 1, "zero width should fail");
    expect_true(pcv_srm_segment(&pixel, 1, 0, 32, &pixel) == 1, "zero height should fail");
    expect_true(pcv_srm_segment(&pixel, 1, 1, 0, &pixel) == 1, "non-positive q should fail");
}

static void test_uniform_image_is_unchanged(void) {
    static const unsigned char input[9] = {
        7, 7, 7,
        7, 7, 7,
        7, 7, 7
    };
    unsigned char output[9];

    expect_true(pcv_srm_segment(input, 3, 3, 32, output) == 0, "uniform image should segment");
    expect_image_eq(output, input, sizeof(input), "uniform image should remain unchanged");
}

static void test_vertical_split_marks_boundaries(void) {
    static const unsigned char input[16] = {
        0, 0, 255, 255,
        0, 0, 255, 255,
        0, 0, 255, 255,
        0, 0, 255, 255
    };
    static const unsigned char expected[16] = {
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255
    };
    unsigned char output[16];

    expect_true(pcv_srm_segment(input, 4, 4, 32, output) == 0, "split image should segment");
    expect_image_eq(output, expected, sizeof(expected), "split image should mark detected boundaries");
}

static int run_image_pipeline(const char *input_path, const char *output_path, int q) {
    unsigned char *rgb = NULL;
    unsigned char *gray = NULL;
    unsigned char *segmented = NULL;
    int width;
    int height;
    int channels;
    int status = 1;

    rgb = stbi_load(input_path, &width, &height, &channels, 3);
    if (!rgb) {
        fprintf(stderr, "failed to read image: %s\n", input_path);
        goto cleanup;
    }

    gray = (unsigned char *)malloc((size_t)width * (size_t)height);
    segmented = (unsigned char *)malloc((size_t)width * (size_t)height);
    if (!gray || !segmented) {
        fprintf(stderr, "failed to allocate image buffers\n");
        goto cleanup;
    }

    if (pcv_convert_gray(rgb, width, height, gray) != 0) {
        fprintf(stderr, "pcv_convert_gray failed\n");
        goto cleanup;
    }
    if (pcv_srm_segment(gray, width, height, q, segmented) != 0) {
        fprintf(stderr, "pcv_srm_segment failed\n");
        goto cleanup;
    }
    if (!stbi_write_png(output_path, width, height, 1, segmented, width)) {
        fprintf(stderr, "failed to write image: %s\n", output_path);
        goto cleanup;
    }

    status = 0;

cleanup:
    free(segmented);
    free(gray);
    stbi_image_free(rgb);
    return status;
}

int main(int argc, char **argv) {
    if (argc == 3 || argc == 4) {
        int q = 32;

        if (argc == 4) {
            q = atoi(argv[3]);
            if (q <= 0) {
                fprintf(stderr, "q must be positive\n");
                return 1;
            }
        }
        return run_image_pipeline(argv[1], argv[2], q);
    }
    if (argc != 1) {
        fprintf(stderr, "usage: %s [input_image output_image [q]]\n", argv[0]);
        return 1;
    }

    test_rejects_invalid_arguments();
    test_uniform_image_is_unchanged();
    test_vertical_split_marks_boundaries();
    puts("test_srm: all tests passed");
    return 0;
}
