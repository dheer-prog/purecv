#include <assert.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define PCV_FLOODFILL
#include "pcv_floodfill.h"
#define PCV_CONVERT_GRAY
#include "pcv_convert_gray.h"
int main(void) {
    unsigned char all1[] = {1,1,1,1,1,1,1,1,1}, out1[9];
    unsigned char bin[] = {1,1,0,1,0,0,1,1,1}, out2[9];
    int s1[1][2] = {{1,1}}, s2[1][2] = {{0,0}};
    int w, h, c, si[1][2], ok = 1;
    unsigned char *img = stbi_load("coins.png", &w, &h, &c, 3), *out;
    unsigned char* img_gray = (unsigned char*)calloc((size_t)w * (size_t)h, sizeof(unsigned char));
    convert_gray(img, w, h, img_gray);
    floodfill(all1, 3, 3, out1, s1, 1, 1, 9);
    for (int i = 0; i < 9; ++i){
        if(out1[i] != 9) {
            printf("%d\n", out1[i]);
            fprintf(stderr, "floodfill failed on all1 test\n");
            return 1;
        }
    };

    floodfill(bin, 3, 3, out2, s2, 1, 0, 7);
    assert(out2[0] == 7 && out2[1] == 7 && out2[2] == 0);
    assert(out2[3] == 7 && out2[4] == 0 && out2[5] == 0);
    assert(out2[6] == 7 && out2[7] == 7 && out2[8] == 7);

    if (!img) return fprintf(stderr, "could not load coins.png\n"), 1;
    unsigned char* outer= (unsigned char*)malloc((size_t)w * (size_t)h);
    if (!out) return fprintf(stderr, "alloc failed\n"), 1;
    si[0][0] = h / 2; si[0][1] = w / 2;
    floodfill(img_gray , w, h, outer, si, 1, 18, 255);
    ok = stbi_write_png("coins_floodfill_result.png", w, h, 1, outer, w);
    free(outer); stbi_image_free(img);
    if (!ok) return fprintf(stderr, "could not write result image\n"), 1;
    puts("test_ff: ok");
    return 0;
}
