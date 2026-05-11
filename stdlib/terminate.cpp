// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/terminate.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// parasoft-begin-suppress AUTOSAR-A18_0_1-a-2 "This is currently used for an incomplete implementation of
// std::terminate"
// parasoft-begin-suppress AUTOSAR-A1_1_1-d-2 "This is currently used for an incomplete implementation
// of std::terminate"
// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stdlib.h>
// parasoft-end-suppress AUTOSAR-A1_1_1-d-2
// parasoft-end-suppress AUTOSAR-A18_0_1-a-2

namespace std {

/// @brief function called when exception handling fails
///
/// @note This is an incomplete implementation of @c std::terminate that is only
/// enough to allow use of @c //arene/base:contracts within @c //stdlib.
///
[[noreturn]] void terminate() noexcept {
  // This does not yet call the terminate handler
  // parasoft-begin-suppress CERT_CPP-ERR50-l AUTOSAR-A15_5_2-b AUTOSAR-M18_0_3-a "std::abort used because
  // std::terminate is part of the standard library"
  ::abort();
  // parasoft-end-suppress CERT_CPP-ERR50-l AUTOSAR-A15_5_2-b AUTOSAR-M18_0_3-a
}

}  // namespace std
