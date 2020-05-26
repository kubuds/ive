#include "kernel_generator.hpp"
#include "utils.hpp"

static inline IveKernel createGaussianKernel(bmctx_t *ctx, uint32_t img_c, uint32_t k_h,
                                             uint32_t k_w) {
  CviImg cimg(ctx, img_c, k_h, k_w, CVK_FMT_I8);
  IveKernel kernel;
  kernel.img = cimg;
  uint8_t *v_addr = cimg.GetVAddr();
  if (k_h == 3 && k_w == 3) {
    for (uint32_t i = 0; i < img_c; i++) {
      for (uint32_t j = 0; j < k_h; j++) {
        for (uint32_t k = 0; k < k_w; k++) {
          int val = 1;
          if (j == 1 && k == 1) {
            val = 4;
          } else if ((j == 1 && (k == 0 || k == k_w - 1)) || (k == 1 && (j == 0 || j == k_h - 1))) {
            val = 2;
          }
          v_addr[i * k_h * k_w + j * k_w + k] = val;
        }
      }
    }
    kernel.multiplier.f = 1.f / 16;
  } else {
    std::cerr << "Not supported kernel shape. ( " << k_h << ", " << k_w << ")" << std::endl;
  }
  QuantizeMultiplierSmallerThanOne(kernel.multiplier.f, &kernel.multiplier.base,
                                   &kernel.multiplier.shift);
  return kernel;
}

// clang-format off
static int8_t sobel_y_kernel_1x1[] = { 0, -1, 0, 0, 0, 0, 0, 1, 0 };
static int8_t sobel_x_kernel_1x1[] = { 0, 0, 0, -1, 0, 1, 0, 0, 0 };
static int8_t sobel_y_kernel_3x3[] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
static int8_t sobel_x_kernel_3x3[] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
static int8_t sobel_y_kernel_5x5[] = { 1,  4,  6,   4,  1, \
                                   2,  8, 12,   8,  2, \
                                   0,  0,  0,   0,  0, \
                                  -2, -8,-12,  -8, -2, \
                                   1, -4, -6,  -4, -1};
static int8_t sobel_x_kernel_5x5[] = { 1,  2,  0,  -2, -1, \
                                   4,  8,  0,  -8, -4, \
                                   6, 12,  0, -12, -6, \
                                   4,  8,  0,  -8, -4, \
                                   1,  2,  0,  -2, -1};
static int8_t scharr_y_kernel_3x3[] = { -3, -10, -3, 0, 0, 0, 3, 10, 3 };
static int8_t scharr_x_kernel_3x3[] = { -3, 0, 3, -10, 0, 10, -3, 0, 3 };
static int8_t morph_rect_kernel_3x3[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1};
static int8_t morph_cross_kernel_3x3[] = { 0, 1, 0, 1, 1, 1, 0, 1, 0};
static int8_t morph_ellipse_kernel_3x3[] = { 0, 1, 0, 1, 1, 1, 0, 1, 0};
static int8_t morph_rect_kernel_5x5[] = { 1, 1, 1, 1, 1, \
                                      1, 1, 1, 1, 1, \
                                      1, 1, 1, 1, 1, \
                                      1, 1, 1, 1, 1, \
                                      1, 1, 1, 1, 1};
static int8_t morph_cross_kernel_5x5[] = { 0, 0, 1, 0, 0, \
                                       0, 0, 1, 0, 0, \
                                       1, 1, 1, 1, 1, \
                                       0, 0, 1, 0, 0, \
                                       0, 0, 1, 0, 0};
static int8_t morph_ellipse_kernel_5x5[] = { 0, 1, 1, 1, 0, \
                                         1, 1, 1, 1, 1, \
                                         1, 1, 1, 1, 1, \
                                         1, 1, 1, 1, 1, \
                                         0, 1, 1, 1, 0};
// clang-format on

static inline IveKernel createKernel(bmctx_t *ctx, uint32_t img_c, uint32_t k_h, uint32_t k_w,
                                     IVE_KERNEL kernel_type, float multiplir_val = 1.f) {
  bool is_1x1 = false;
  if (k_h == 1 && k_w == 1) {
    is_1x1 = true;
    k_h = 3;
    k_w = 3;
  }
  CviImg cimg(ctx, img_c, k_h, k_w, CVK_FMT_I8);
  IveKernel kernel;
  kernel.img = cimg;
  kernel.multiplier.f = multiplir_val;
  int8_t *filter = nullptr;
  if (is_1x1) {
    switch (kernel_type) {
      case IVE_KERNEL::SOBEL_Y:
        filter = sobel_y_kernel_1x1;
        break;
      case IVE_KERNEL::SOBEL_X:
        filter = sobel_x_kernel_1x1;
        break;
      default:
        std::cerr << "Not supported kernel type." << std::endl;
        break;
    }
  } else if (k_h == 3 && k_w == 3) {
    switch (kernel_type) {
      case IVE_KERNEL::SOBEL_Y:
        filter = sobel_y_kernel_3x3;
        break;
      case IVE_KERNEL::SOBEL_X:
        filter = sobel_x_kernel_3x3;
        break;
      case IVE_KERNEL::SCHARR_Y:
        filter = scharr_y_kernel_3x3;
        break;
      case IVE_KERNEL::SCHARR_X:
        filter = scharr_x_kernel_3x3;
        break;
      case IVE_KERNEL::MORPH_RECT:
        filter = morph_rect_kernel_3x3;
        break;
      case IVE_KERNEL::MORPH_CROSS:
        filter = morph_cross_kernel_3x3;
        break;
      case IVE_KERNEL::MORPH_ELLIPSE:
        filter = morph_ellipse_kernel_3x3;
        break;
      default:
        std::cerr << "Not supported kernel type." << std::endl;
        break;
    }
  } else if (k_h == 5 && k_w == 5) {
    switch (kernel_type) {
      case IVE_KERNEL::SOBEL_Y:
        filter = sobel_y_kernel_5x5;
        break;
      case IVE_KERNEL::SOBEL_X:
        filter = sobel_x_kernel_5x5;
        break;
      case IVE_KERNEL::MORPH_RECT:
        filter = morph_rect_kernel_5x5;
        break;
      case IVE_KERNEL::MORPH_CROSS:
        filter = morph_cross_kernel_5x5;
        break;
      case IVE_KERNEL::MORPH_ELLIPSE:
        filter = morph_ellipse_kernel_5x5;
        break;
      default:
        std::cerr << "Not supported kernel type." << std::endl;
        break;
    }
  } else {
    std::cerr << "Not supported kernel shape. ( " << k_h << ", " << k_w << ")" << std::endl;
  }

  uint8_t *v_addr = cimg.GetVAddr();
  for (uint32_t i = 0; i < img_c; i++) {
    for (uint32_t j = 0; j < k_h; j++) {
      for (uint32_t k = 0; k < k_w; k++) {
        v_addr[i * k_h * k_w + j * k_w + k] = (uint8_t)filter[j * k_w + k];
      }
    }
  }
  QuantizeMultiplierSmallerThanOne(kernel.multiplier.f, &kernel.multiplier.base,
                                   &kernel.multiplier.shift);
  return kernel;
}

IveKernel createKernel(bmctx_t *ctx, uint32_t img_c, uint32_t k_h, uint32_t k_w, IVE_KERNEL type) {
  IveKernel kernel;
  switch (type) {
    case IVE_KERNEL::GAUSSIAN:
      return createGaussianKernel(ctx, img_c, k_h, k_w);
      break;
    default:
      return createKernel(ctx, img_c, k_h, k_w, type, 1.f);
  }
  return kernel;
}