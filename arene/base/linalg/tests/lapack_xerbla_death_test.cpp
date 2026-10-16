// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stdlib.h>

#include <cblas.h>
#include <gtest/gtest.h>

namespace {

/// @test Given an invalid leading dimension, when BLAS calls XERBLA, then it reports the argument and exits with
/// failure.
TEST(LapackXerblaDeathTest, InvalidBlasArgumentReportsErrorAndExitsWithFailure) {
  float const matrix = 1.0F;
  float const input = 1.0F;
  float output = 0.0F;

  // Valid layout and transpose values let SGEMV check the invalid leading dimension.
  ASSERT_EXIT(
      cblas_sgemv(CblasColMajor, CblasNoTrans, 1, 1, 1.0F, &matrix, 0, &input, 1, 0.0F, &output, 1),
      ::testing::ExitedWithCode(EXIT_FAILURE),
      "On entry to SGEMV parameter number 6 had an illegal value"
  );
}

}  // namespace
