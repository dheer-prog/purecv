#include <criterion/criterion.h>
#include <string.h>

#include "../pcv_convolution.h"

static void expect_u8_output(const unsigned char *actual, const unsigned char *expected, int count) {
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

static void expect_f_output(const float *actual, const unsigned char *expected, int count) {
    int index;

    for (index = 0; index < count; ++index) {
        cr_expect_eq(
            actual[index],
            (float)expected[index],
            "Float mismatch at index %d: expected %.1f, got %.1f",
            index,
            (float)expected[index],
            actual[index]
        );
    }
}

Test(convolution, averaging_kernel_u8) {
    const unsigned char input[] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };
    const float kernel[] = {
        1, 1, 1,
        1, 1, 1,
        1, 1, 1
    };
    const unsigned char expected[] = {
        1, 2, 1,
        3, 5, 3,
        2, 4, 3
    };
    unsigned char output[sizeof(expected)];
    int rc;

    memset(output, 0, sizeof(output));
    rc = pcv_convolve_U8(input, 3, 3, kernel, 3, 3, 1, output, 1);
    cr_assert_eq(rc, 0, "pcv_convolve_U8 failed with code %d", rc);
    expect_u8_output(output, expected, (int)sizeof(expected));
}

Test(convolution, averaging_kernel_f32) {
    const unsigned char input[] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };
    const float kernel[] = {
        1, 1, 1,
        1, 1, 1,
        1, 1, 1
    };
    const unsigned char expected[] = {
        1, 2, 1,
        3, 5, 3,
        2, 4, 3
    };
    float output_f[sizeof(expected)];
    int rc;

    memset(output_f, 0, sizeof(output_f));
    rc = pcv_convolve_F(input, 3, 3, kernel, 3, 3, 1, output_f, 1);
    cr_assert_eq(rc, 0, "pcv_convolve_F failed with code %d", rc);
    expect_f_output(output_f, expected, (int)sizeof(expected));
}
