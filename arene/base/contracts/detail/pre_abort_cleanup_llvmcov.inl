// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Disabling linting on next line since there's no workaround to dumping
// gcov logs apart from calling __gcov_dump that clang-tidy considers a reserved
// identifier because of the double underscore. Directly including "gcov.h"
// will resolve this issue but opens up another - the "gcov.h" declaration of
// __gcov_dump is missing the "C" linkage and we hit a linker error.
// NOLINTBEGIN(bugprone-reserved-identifier, readability-identifier-naming)
extern "C" {
__attribute__((weak)) int __llvm_profile_write_file(void);
}
// NOLINTEND(bugprone-reserved-identifier, readability-identifier-naming)

namespace arene {
namespace base {
namespace contracts_detail {

/**
 * Performs operations that need to be done just before a call to abort.
 * Used for special cases like in coverage runs where we need to explicitly
 * dump/flush the logs before abort as a log dump/flush is only called on a
 * graceful exit.
 *
 * @param None
 * @returns Nothing.
 */
inline void pre_abort_cleanup() { __llvm_profile_write_file(); }
}  // namespace contracts_detail
}  // namespace base
}  // namespace arene
