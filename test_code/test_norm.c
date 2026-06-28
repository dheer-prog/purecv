#include <criterion/criterion.h>

#define PCV_NORMALIZE
#include "../pcv_normalize.h"

Test(normalize, rejects_invalid_ranges) {
    float input[4] = {1.0f, 2.0f, 3.0f, 4.0f};

    cr_expect_eq(pcv_general_norm(input, 0, 2, 1, 255, 1), 1);
    cr_expect_eq(pcv_general_norm(input, 2, 0, 1, 255, 1), 1);
    cr_expect_eq(pcv_general_norm(input, 2, 2, 255, 1, 1), 1);
}

Test(normalize, scales_float_input_in_place) {
    float input[2][2] = {
        {0.0f, 5.0f},
        {10.0f, 15.0f}
    };
    int rc = pcv_general_norm(input, 2, 2, 1, 255, 1);

    cr_assert_eq(rc, 0);
    cr_expect_float_eq(input[0][0], 1.0f, 1e-5f);
    cr_expect_float_eq(input[1][1], 255.0f, 1e-5f);
}

Test(normalize, scales_u8_input_in_place) {
    unsigned char input[2][2] = {
        {10, 20},
        {30, 40}
    };
    int rc = pcv_general_norm(input, 2, 2, 1, 255, 0);

    cr_assert_eq(rc, 0);
    cr_expect_eq(input[0][0], 1);
    cr_expect_eq(input[1][1], 255);
}
