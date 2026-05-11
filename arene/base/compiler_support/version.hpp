// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_VERSION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_VERSION_HPP_

#include "arene/base/detail/raw_c_string.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

#include "arene/base/stdlib_choice/cstdint.hpp"

// AUTOSAR exceptions:
// A16-0-1 The pre-processor shall only be used for unconditional and
// conditional file inclusion and include guards, and using the following
// directives: (1) #ifndef, (2) #ifdef, (3) #if, (4) #if defined, (5) #elif, (6)
// #else, (7) #define, (8) #endif, (9) #include.
//
// Exception Rationale: The code below is used to make user-configurable parts
// of a library that can be used by everyone. It increases safety, as documented
// in "docs/source/implementation/macros_and_configuration.md" and prevents
// excessive, convoluted if-elif definition chains that have resulted in the
// wrong code being included or used in the past. These utilities also offer
// safer ways of checking for the existence of certain macros required for
// interfacing with the C++ standard.

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

//////

// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Macros for the ABI namespace"
#define ARENE_BASE_INLINE_ABI_NAMESPACE_OPEN_I_ inline namespace v0 {
#define ARENE_BASE_INLINE_ABI_NAMESPACE_CLOSE_I_ }
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2

// clang-format on

namespace arene {
namespace base {
ARENE_BASE_INLINE_ABI_NAMESPACE_OPEN_I_

//////
/// @brief The major version for Arene.Base.
constexpr std::uint32_t version_major{0U};

//////
/// @brief The minor version for Arene.Base.
constexpr std::uint32_t version_minor{0U};
//////
/// @brief The patch version for Arene.Base.
constexpr std::uint32_t version_patch{0U};
//////
/// @brief The version of Arene.Base as a string.
// parasoft-begin-suppress AUTOSAR-A27_0_4-d "'const char *' value constructed from string literal always has the
// correct buffer size and a static lifetime"
// NOLINTNEXTLINE(clang-diagnostic-unused-const-variable)
constexpr detail::raw_c_string version_string{"0.0.0"};
// parasoft-end-suppress AUTOSAR-A27_0_4-d

namespace version_detail {
/// @brief The scale factor to use when combining the major version number into @c version
constexpr std::uint32_t major_scale{1'000'000U};
/// @brief The scale factor to use when combining the minor version number into @c version
constexpr std::uint32_t minor_scale{1'000U};
}  // namespace version_detail

//////
/// @brief The version of Arene.Base.
///
/// @remarks Each 100's grouping (in decimal) a part of the version number. For
/// example, a value of @c 5'021'001 is a major version of @c 5 , a minor
/// version of @c 21 , and a patch version of @c 1.
//////
// NOLINTNEXTLINE(clang-diagnostic-unused-const-variable)
constexpr std::uint32_t version{
    ((version_major * version_detail::major_scale) + (version_minor * version_detail::minor_scale) + (version_patch))
};

ARENE_BASE_INLINE_ABI_NAMESPACE_CLOSE_I_
}  // namespace base
}  // namespace arene

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_VERSION_HPP_
