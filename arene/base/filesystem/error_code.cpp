// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/filesystem/error_code.hpp"

#include <cerrno>
#include <cstring>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/expect.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/contracts.hpp"
#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/inline_string.hpp"
#include "arene/base/string_algorithms.hpp"
#include "arene/base/string_view.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "Everything is documented in the header file"
// parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "Everything is documented in the header file"
// parasoft-begin-suppress AUTOSAR-A15_4_5-a-2 "Everything is documented in the header file"

namespace {

/// @brief The raw error code type
using raw_error_code = arene::base::filesystem::error_code::raw_error_code;

/// @brief Helper which SFINAE-dispatches to the GNU-specific @c char* return variant of @c strerror_r to get the string
/// representation of an error code
/// @tparam Err The type of the error code
/// @param error The error code
/// @param res The string to store the result in
/// @return The error code, if any
template <
    typename Err,
    arene::base::constraints<std::enable_if_t<std::is_same<
        decltype(strerror_r(
            std::declval<Err>(),
            std::declval<arene::base::detail::character*>(),
            std::declval<size_t>()
        )),
        arene::base::detail::character*>::value>> = nullptr>
auto helper_strerror_r(Err error, arene::base::filesystem::error_string& res) noexcept -> raw_error_code {
  // parasoft-begin-suppress AUTOSAR-M19_3_1-a "strerror_r uses errno to report errors"
  // parasoft-begin-suppress AUTOSAR-A17_1_1-a "strerror_r uses errno to report errors"
  errno = 0;
  arene::base::detail::raw_c_string const str{strerror_r(error, res.data(), res.size())};
  raw_error_code const local_error{errno};
  // parasoft-end-suppress AUTOSAR-M19_3_1-a
  // parasoft-end-suppress AUTOSAR-A17_1_1-a
  if (str != res.data()) {
    std::ignore = strncpy(res.data(), str, res.size() - 1U);
  }
  return local_error;
}

/// @brief Helper which SFINAE-dispatches to the XSI-compliant @c raw_error_code return variant of @c strerror_r to get
/// the string representation of an error code
/// @tparam Err The type of the error code
/// @param error The error code
/// @param res The string to store the result in
/// @return The error code, if any
template <
    typename Err,
    arene::base::constraints<std::enable_if_t<std::is_same<
        decltype(strerror_r(
            std::declval<Err>(),
            std::declval<arene::base::detail::character*>(),
            std::declval<size_t>()
        )),
        raw_error_code>::value>> = nullptr>
auto helper_strerror_r(Err error, arene::base::filesystem::error_string& res) noexcept -> raw_error_code {
  return strerror_r(error, res.data(), res.size());
}

}  // namespace

namespace arene {
namespace base {
namespace filesystem {

// parasoft-begin-suppress AUTOSAR-M19_3_1-a "This function encapsulates the use of errno"
// parasoft-begin-suppress AUTOSAR-A17_1_1-a "This function encapsulates the use of errno"
auto error_code::from_errno() noexcept -> error_code { return error_code(std::exchange(errno, 0)); }
// parasoft-end-suppress AUTOSAR-M19_3_1-a
// parasoft-end-suppress AUTOSAR-A17_1_1-a

// NOLINTNEXTLINE(bugprone-exception-escape)
auto error_code::message() const noexcept -> error_string {
  error_string res;
  res.resize(max_error_length);
  raw_error_code const local_error{helper_strerror_r(error_, res)};
  if (local_error != 0) {
    res = "Unable to get message for error ";
    res += to_inline_string(error_);
  } else {
    auto const actual_length = string_length(res.c_str());
    ARENE_INVARIANT(actual_length <= max_error_length);
    res.resize(actual_length);
  }

  return res;
}

ARENE_NORETURN void error_code::throw_error() const { throw std::system_error(error_, std::system_category()); }

ARENE_NORETURN void error_code::throw_error_with_prefix(null_terminated_string_view const message_prefix) const {
  throw std::system_error(error_, std::system_category(), message_prefix.c_str());
}

}  // namespace filesystem
}  // namespace base
}  // namespace arene
