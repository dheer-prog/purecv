# purecv

`purecv` is a single-header-style C library for classical computer vision
operations.

Implementation pattern:

```c
#define PCV_GET_FILTERS
#include "pcv_get_filters.h"
```

Note:
- The current codebase is primarily grayscale-oriented.

## Current API

`pcv_get_filters.h`
- Define before include: `PCV_GET_FILTERS`
- Public function:
  `int pcv_get_filters(int n, float *output, char *filter)`
- What it does:
  Generates an `n x n` filter kernel into `output`.
- Supported filter names:
  `BOX`, `SOBEL_X`, `SOBEL_Y`, `GAUSSIAN_BLUR`

`pcv_convolution.h`
- Define before include: `PCV_CONVOLUTION`
- Public function:
  `int pcv_convolve_U8(const unsigned char *input, int width, int height, const float *kernel, int k_width, int k_height, int stride, unsigned char *output, int padding)`
- What it does:
  Convolves a grayscale `unsigned char` image and writes clamped
  `unsigned char` output.

`pcv_convolution.h`
- Define before include: `PCV_CONVOLUTION`
- Public function:
  `int pcv_convolve_F(const unsigned char *input, int width, int height, const float *kernel, int k_width, int k_height, int stride, float *output, int padding)`
- What it does:
  Convolves a grayscale `unsigned char` image and writes floating-point
  output.

`pcv_convert_gray.h`
- Define before include: `PCV_CONVERT_GRAY`
- Public function:
  `int pcv_convert_gray(unsigned char *input, int width, int height, unsigned char *output)`
- What it does:
  Converts a flat `RGBRGB...` byte array into a grayscale image.

`pcv_floodfill.h`
- Define before include: `PCV_FLOODFILL`
- Public function:
  `int pcv_floodfill(const unsigned char *input, int width, int height, unsigned char *output, int (*seeds)[2], int num_seeds, int threshold, unsigned char new_val)`
- What it does:
  Performs flood fill on a grayscale image using seed points in `(y, x)`
  order.

`pcv_srm.h`
- Define before include: `PCV_SRM_IMPLEMENT`
- Public function:
  `int pcv_srm_segment(const unsigned char *original_data, int width, int height, int q, unsigned char *output)`
- What it does:
  Runs Statistical Region Merging (SRM) segmentation and writes a
  boundary-marked output image.

`pcv_normalize.h`
- Define before include: `PCV_NORMALIZE`
- Public function:
  `int pcv_general_norm(void *input, int height, int width, int bottom, int upp, int input_type)`
- What it does:
  Normalizes the input image in place to the range `[bottom, upp]`.
- Current input type convention:
  `input_type == 1` means the buffer is treated as `float`.
  Any other value means the buffer is treated as `unsigned char`.
- Current behavior note:
  The function mutates the input buffer directly instead of writing to a
  separate output buffer.

## Build / Usage Notes

`pcv_convert_gray.h`
- Expects a flat RGB byte layout:
  `RGBRGBRGB...`
  
`pcv_floodfill.h`
- Seed points are expected in `(y, x)` order.
- Current implementation supports 4-way flood fill.
`pcv_normalize.h`
- Assumes IEEE 754 floating-point layout.

