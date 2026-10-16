// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>

#include "arene/base/testing/prng_xoshiro.hpp"

auto main(int argc, char** argv) -> int {
  if (argc != 2) {
    // NOLINTNEXTLINE(hicpp-vararg)
    std::fprintf(  //
        stderr,
        "usage: bazel run %s -- <seed>\n",
        "//arene/base/testing/tests:prng_xoshiro_dump"
    );
    return EXIT_FAILURE;
  }

  char* end = nullptr;
  errno = 0;
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  auto const seed = std::strtoull(argv[1], &end, 0);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  if (errno != 0 || end == argv[1] || *end != '\0') {
    // NOLINTNEXTLINE(hicpp-vararg,cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::fprintf(stderr, "error: invalid seed: %s\n", argv[1]);
    return EXIT_FAILURE;
  }

  auto prng = arene::base::testing::prng_xoshiro{seed};

  std::signal(SIGPIPE, SIG_IGN);

  // write 64-bit values until the process is killed
  while (true) {
    auto const value = prng();
    auto const written = std::fwrite(&value, sizeof(value), 1, stdout);
    if (written == 0U) {
      if (errno == EPIPE) {
        return EXIT_SUCCESS;
      }

      std::perror("fwrite");
      return EXIT_FAILURE;
    }
  }
}
