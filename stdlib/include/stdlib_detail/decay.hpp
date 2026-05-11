// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_DECAY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_DECAY_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/add_pointer.hpp"
#include "stdlib/include/stdlib_detail/conditional.hpp"
#include "stdlib/include/stdlib_detail/is_array.hpp"
#include "stdlib/include/stdlib_detail/is_function.hpp"
#include "stdlib/include/stdlib_detail/remove_cv.hpp"
#include "stdlib/include/stdlib_detail/remove_extent.hpp"

namespace std {

/// @brief Contains @c type, a conversion equivalent to those performed when passing function arguments by value
/// @tparam Type The type to decay
template <typename Type>
class decay {
 private:
  /// @brief Reference-erased for building @c type
  using reference_erased_type = remove_reference_t<Type>;

 public:
  /// @brief The converted type
  /// For arrays: A reference-erased element pointer type
  /// For functions: A reference-erased pointer type
  /// Otherwise: A reference and cv-erased type
  using type = conditional_t<
      is_array_v<reference_erased_type>,
      add_pointer_t<remove_extent_t<reference_erased_type>>,
      conditional_t<
          is_function_v<reference_erased_type>,
          add_pointer_t<reference_erased_type>,
          remove_cv_t<reference_erased_type>>>;
};

/// @brief A conversion equivalent to those performed when passing function arguments by value
/// @tparam Type The type to decay
template <typename Type>
using decay_t = typename decay<Type>::type;

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_DECAY_HPP_
