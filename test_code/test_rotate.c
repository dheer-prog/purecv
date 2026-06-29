#ifdef PCV_ROTATE_STANDALONE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define PCV_CONVERT_GRAY
#include "../pcv_convert_gray.h"
#define PCV_ROTATE
#include "../pcv_rotate.h"

int main(int argc, char **argv) {
    const char *input_path = "test_images/coins.png";
    const char *output_path = "test_images/coins_rotate.png";
    float theta=90.0f;
    unsigned char *rgb = NULL;
    unsigned char *gray = NULL;
    unsigned char *rotated = NULL;
    int width;
    int height;
    int channels;
    int ok;

    if (argc >= 2) {
        theta = atoi(argv[1]);
    }
    if (argc >= 3) {
        output_path = argv[2];
    }

    rgb = stbi_load(input_path, &width, &height, &channels, 3);
    if (!rgb) {
        fprintf(stderr, "could not load %s\n", input_path);
        return 1;
    }

    gray = (unsigned char *)malloc((size_t)width * (size_t)height);
    rotated = (unsigned char *)malloc((size_t)width * (size_t)height);
    if (!gray || !rotated) {
        fprintf(stderr, "could not allocate rotate buffers\n");
        free(rotated);
        free(gray);
        stbi_image_free(rgb);
        return 1;
    }

    if (pcv_convert_gray(rgb, width, height, gray) != 0) {
        fprintf(stderr, "pcv_convert_gray failed\n");
        free(rotated);
        free(gray);
        stbi_image_free(rgb);
        return 1;
    }
    if (pcv_rotate(gray, width, height, rotated, theta) != 0) {
        fprintf(stderr, "pcv_rotate failed\n");
        free(rotated);
        free(gray);
        stbi_image_free(rgb);
        return 1;
    }

    ok = stbi_write_png(output_path, width, height, 1, rotated, width);
    free(rotated);
    free(gray);
    stbi_image_free(rgb);

    if (!ok) {
        fprintf(stderr, "could not write %s\n", output_path);
        return 1;
    }

    printf("wrote rotated image to %s with theta=%d\n", output_path, theta);
    return 0;
}

#else

#include <criterion/criterion.h>
#include <string.h>

#define PCV_ROTATE
#include "../pcv_rotate.h"

static void expect_u8_image_eq(
    const unsigned char *actual,
    const unsigned char *expected,
    int count
) {
    int index;

    for (index = 0; index < count; ++index) {
        cr_expect_eq(
            actual[index],
            expected[index],
            "Mismatch at index %d: expected %u, got %u",
            index,
            (unsigned int)expected[index],
            (unsigned int)actual[index]
        );
    }
}

Test(rotate, zero_degrees_keeps_image_unchanged) {
    const unsigned char input[] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };
    const unsigned char expected[] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };
    unsigned char output[sizeof(expected)];
    int rc;

    memset(output, 0, sizeof(output));
    rc = pcv_rotate((unsigned char *)input, 3, 3, output, 0);

    cr_assert_eq(rc, 0, "pcv_rotate failed with code %d", rc);
    expect_u8_image_eq(output, expected, (int)sizeof(expected));
}

Test(rotate, full_turn_keeps_image_unchanged) {
    const unsigned char input[] = {
        10, 20, 30,
        40, 50, 60,
        70, 80, 90
    };
    const unsigned char expected[] = {
        10, 20, 30,
        40, 50, 60,
        70, 80, 90
    };
    unsigned char output[sizeof(expected)];
    int rc;

    memset(output, 0, sizeof(output));
    rc = pcv_rotate((unsigned char *)input, 3, 3, output, 360);

    cr_assert_eq(rc, 0, "pcv_rotate failed with code %d", rc);
    expect_u8_image_eq(output, expected, (int)sizeof(expected));
}

#endif
