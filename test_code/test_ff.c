#include <criterion/criterion.h>
#include <stdlib.h>

#include "stb_image.h"

#include "../pcv_convert_gray.h"
#include "../pcv_floodfill.h"
#include "test_support.h"

Test(floodfill, fills_simple_regions) {
    unsigned char all1[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    unsigned char out1[9];
    unsigned char bin[] = {1, 1, 0, 1, 0, 0, 1, 1, 1};
    unsigned char out2[9];
    int s1[1][2] = {{1, 1}};
    int s2[1][2] = {{0, 0}};
    int i;

    cr_assert_eq(pcv_floodfill(all1, 3, 3, out1, s1, 1, 1, 9), 0);
    for (i = 0; i < 9; ++i) {
        cr_expect_eq(out1[i], 9, "floodfill failed on all1 test at index %d", i);
    }

    cr_assert_eq(pcv_floodfill(bin, 3, 3, out2, s2, 1, 0, 7), 0);
    cr_expect_eq(out2[0], 7);
    cr_expect_eq(out2[1], 7);
    cr_expect_eq(out2[2], 0);
    cr_expect_eq(out2[3], 7);
    cr_expect_eq(out2[4], 0);
    cr_expect_eq(out2[5], 0);
    cr_expect_eq(out2[6], 7);
    cr_expect_eq(out2[7], 7);
    cr_expect_eq(out2[8], 7);
}

Test(floodfill, writes_coins_result) {
    int w;
    int h;
    int c;
    int seeds[1][2];
    int ok;
    unsigned char *img = test_support_load_rgb_image("coins.png", &w, &h, &c);
    unsigned char *img_gray;
    unsigned char *outer;

    cr_assert_not_null(img, "could not load coins.png");

    img_gray = (unsigned char *)calloc((size_t)w * (size_t)h, sizeof(unsigned char));
    outer = (unsigned char *)malloc((size_t)w * (size_t)h);
    cr_assert_not_null(img_gray, "alloc failed for grayscale image");
    cr_assert_not_null(outer, "alloc failed for floodfill output");

    cr_assert_eq(pcv_convert_gray(img, w, h, img_gray), 0);
    seeds[0][0] = h / 2;
    seeds[0][1] = w / 2;

    cr_assert_eq(pcv_floodfill(img_gray, w, h, outer, seeds, 1, 18, 255), 0);
    ok = test_support_write_png_gray("coins_floodfill_result.png", outer, w, h);
    cr_assert_neq(ok, 0, "could not write result image");

    free(outer);
    free(img_gray);
    stbi_image_free(img);
}
