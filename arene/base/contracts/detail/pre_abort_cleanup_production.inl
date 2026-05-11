// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

namespace arene {
namespace base {
namespace contracts_detail {
/**
 * Performs operations that need to be done just before a call to abort.
 * Used for special cases like in coverage runs where we need to explicitly
 * flush the logs before abort as log flush is only called on a graceful exit.
 * Currently does not do anything in production builds.
 *
 * @param None
 * @returns Nothing.
 */
inline void pre_abort_cleanup() noexcept {}
}  // namespace contracts_detail
}  // namespace base
}  // namespace arene
