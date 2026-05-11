// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Declare BLAS saxpy directly rather than pulling in cblas.h or lapacke.h,
// which require hosted C library headers unavailable on all target platforms.
extern "C" {
// NOLINTNEXTLINE(readability-identifier-naming,readability-identifier-length)
void saxpy_(int const* n, float const* alpha, float const* x, int const* incx, float* y, int const* incy);
}

// NOLINTBEGIN(hicpp-avoid-c-arrays,readability-magic-numbers,readability-identifier-length,cppcoreguidelines-pro-bounds-constant-array-index,hicpp-no-array-decay)

auto main() -> int {
  // y = alpha * x + y
  float x[4] = {1.0F, 2.0F, 3.0F, 4.0F};
  float y[4] = {5.0F, 6.0F, 7.0F, 8.0F};
  float const alpha = 2.0F;
  int const n = 4;
  int const inc = 1;

  // NOLINTNEXTLINE(readability-identifier-naming)
  saxpy_(&n, &alpha, x, &inc, y, &inc);

  // Expected: y = {7, 10, 13, 16}
  constexpr int expected[4] = {7, 10, 13, 16};
  for (int i = 0; i != 4; ++i) {
    if (static_cast<int>(y[i]) != expected[i]) {
      return 1;
    }
  }
  return 0;
}

// NOLINTEND(hicpp-avoid-c-arrays,readability-magic-numbers,readability-identifier-length,cppcoreguidelines-pro-bounds-constant-array-index,hicpp-no-array-decay)
