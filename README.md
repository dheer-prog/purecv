# purecv

`purecv` is a single-header-style C library for classical computer vision operations.

To use an implementation from a header, define its implementation macro in one C file before including the header.

Example:

```c
#define PCV_GET_FILTERS
#include "pcv_get_filters.h"
```

## Current API

| Header | Define before include | Public function | What it does |
| --- | --- | --- | --- |
| `pcv_get_filters.h` | `PCV_GET_FILTERS` | `int pcv_get_filters(int n, float *output, char *filter)` | Generates an `n x n` filter kernel into `output`. Supported filter names in the current code are `BOX`, `SOBEL_X`, `SOBEL_Y`, and `GAUSSIAN_BLUR`. | 
| `pcv_convolution.h` | `PCV_CONVOLUTION` | `int pcv_convolve_U8(const unsigned char *input, int width, int height, const float *kernel, int k_width, int k_height, int stride, unsigned char *output, int padding)` | Convolves a grayscale `unsigned char` image and writes clamped `unsigned char` output. | `input`, `kernel`, and `output` must be valid. `stride` must be positive. Current code is grayscale-only. |
| `pcv_convolution.h` | `PCV_CONVOLUTION` | `int pcv_convolve_F(const unsigned char *input, int width, int height, const float *kernel, int k_width, int k_height, int stride, float *output, int padding)` | Convolves a grayscale `unsigned char` image and writes floating-point output. | Useful when you want to normalize or post-process the raw convolution result yourself. |
| `pcv_convert_gray.h` | `PCV_CONVERT_GRAY` | `int pcv_convert_gray(unsigned char *input, int width, int height, unsigned char *output)` | Converts an RGB image stored as a flat `RGBRGB...` byte array into a grayscale image. | `output` must have room for `width * height` bytes. |
| `pcv_floodfill.h` | `PCV_FLOODFILL` | `int pcv_floodfill(const unsigned char *input, int width, int height, unsigned char *output, int (*seeds)[2], int num_seeds, int threshold, unsigned char new_val)` | Performs flood fill on a grayscale image using seed points in `(y, x)` format and writes the filled image to `output`. | Current implementation supports 4-way flood fill. `seeds` must contain valid coordinates. |
| `pcv_srm.h` | `PCV_SRM_IMPLEMENT` | `int pcv_srm_segment(const unsigned char *original_data, int width, int height, int q, unsigned char *output)` | Runs Statistical Region Merging (SRM) segmentation and writes a boundary-marked output image. | Requires linking with the math library: `-lm`. |
|  |  |  |  |  |

## Build notes

| Header | Extra compile / link note |
| --- | --- |
| `pcv_srm.h` | Link with `-lm` because it uses `<math.h>` functions such as `logf`, `sqrtf`, and `fabsf`. |
| `pcv_convolution.h` | Expects grayscale input buffers. |
| `pcv_convert_gray.h` | Expects a flat RGB byte layout: `RGBRGBRGB...`. |
| `pcv_floodfill.h` | Seed points are expected in `(y, x)` order. |
