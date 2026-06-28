#include <stdio.h>
#include <string.h>

#define PCV_CONVOLUTION
#include "pcv_convolution.h"

static int check_output(const unsigned char *actual, const unsigned char *expected, int count) {
    int index;

    for (index = 0; index < count; ++index) {
        if (actual[index] != expected[index]) {
            fprintf(
                stderr,
                "Mismatch at index %d: expected %u, got %u\n",
                index,
                (unsigned int)expected[index],
                (unsigned int)actual[index]
            );
            return 1;
        }
    }

    return 0;
}

static int check_output_f(const float *actual, const unsigned char *expected, int count) {
    int index;

    for (index = 0; index < count; ++index) {
        if (actual[index] != (float)expected[index]) {
            fprintf(
                stderr,
                "Float mismatch at index %d: expected %.1f, got %.1f\n",
                index,
                (float)expected[index],
                actual[index]
            );
            return 1;
        }
    }

    return 0;
}

int main(void) {
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
    float output_f[sizeof(expected)];
    int rc;

    memset(output, 0, sizeof(output));
    memset(output_f, 0, sizeof(output_f));

    rc = convolve_U8(input, 3, 3, kernel, 3, 3, 1, output, 1);
    if (rc != 0) {
        fprintf(stderr, "convolve_U8 failed with code %d\n", rc);
        return 1;
    }

    if (check_output(output, expected, (int)sizeof(expected)) != 0) {
        return 1;
    }

    rc = convolve_F(input, 3, 3, kernel, 3, 3, 1, output_f, 1);
    if (rc != 0) {
        fprintf(stderr, "convolve_F failed with code %d\n", rc);
        return 1;
    }

    if (check_output_f(output_f, expected, (int)sizeof(expected)) != 0) {
        return 1;
    }

    printf("Averaging kernel tests passed.\n");
    return 0;
}
