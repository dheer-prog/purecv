#include <criterion/criterion.h>
#include <stdlib.h>
#include <string.h>

#include "stb_image.h"

#include "../pcv_convert_gray.h"
#include "../pcv_srm.h"
#include "test_support.h"

static void expect_image_eq(
    const unsigned char *actual,
    const unsigned char *expected,
    size_t count,
    const char *message
) {
    size_t index;

    for (index = 0; index < count; ++index) {
        if (actual[index] != expected[index]) {
            cr_assert_fail(
                "%s: first mismatch at index %zu: got %u expected %u",
                message,
                index,
                (unsigned)actual[index],
                (unsigned)expected[index]
            );
        }
    }
}

Test(srm, rejects_invalid_arguments) {
    unsigned char pixel = 0;

    cr_expect_eq(pcv_srm_segment(NULL, 1, 1, 32, &pixel), 1, "NULL input should fail");
    cr_expect_eq(pcv_srm_segment(&pixel, 1, 1, 32, NULL), 1, "NULL output should fail");
    cr_expect_eq(pcv_srm_segment(&pixel, 0, 1, 32, &pixel), 1, "zero width should fail");
    cr_expect_eq(pcv_srm_segment(&pixel, 1, 0, 32, &pixel), 1, "zero height should fail");
    cr_expect_eq(pcv_srm_segment(&pixel, 1, 1, 0, &pixel), 1, "non-positive q should fail");
}

Test(srm, uniform_image_is_unchanged) {
    static const unsigned char input[9] = {
        7, 7, 7,
        7, 7, 7,
        7, 7, 7
    };
    unsigned char output[9];

    cr_assert_eq(pcv_srm_segment(input, 3, 3, 32, output), 0, "uniform image should segment");
    expect_image_eq(output, input, sizeof(input), "uniform image should remain unchanged");
}

Test(srm, vertical_split_marks_boundaries) {
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

    cr_assert_eq(pcv_srm_segment(input, 4, 4, 32, output), 0, "split image should segment");
    expect_image_eq(output, expected, sizeof(expected), "split image should mark detected boundaries");
}

Test(srm, writes_segmented_output_from_image) {
    unsigned char *rgb = NULL;
    unsigned char *gray = NULL;
    unsigned char *segmented = NULL;
    int width;
    int height;
    int channels;

    rgb = test_support_load_rgb_image("coins.png", &width, &height, &channels);
    cr_assert_not_null(rgb, "failed to read coins.png");

    gray = (unsigned char *)malloc((size_t)width * (size_t)height);
    segmented = (unsigned char *)malloc((size_t)width * (size_t)height);
    cr_assert(gray != NULL && segmented != NULL, "failed to allocate image buffers");

    cr_assert_eq(pcv_convert_gray(rgb, width, height, gray), 0, "pcv_convert_gray failed");
    cr_assert_eq(pcv_srm_segment(gray, width, height, 32, segmented), 0, "pcv_srm_segment failed");
    cr_assert_neq(
        test_support_write_png_gray("coins_srm_result.png", segmented, width, height),
        0,
        "failed to write SRM image"
    );

    free(segmented);
    free(gray);
    stbi_image_free(rgb);
}
