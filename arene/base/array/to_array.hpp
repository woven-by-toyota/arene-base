// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: this header intentionally defines both an implementation
// class and function object API." Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ARRAY_TO_ARRAY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ARRAY_TO_ARRAY_HPP_

// IWYU pragma: private, include "arene/base/array.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/functional/construct.hpp"
#include "arene/base/functional/identity.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_array.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_object.hpp"
#include "arene/base/stdlib_choice/is_void.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace to_array_detail {

/// @brief function object implementing the @c to_array and @c to_array_of helpers
/// @tparam D destination element type, or @c void to request that the element type be deduced from the source array
/// (with cvref qualifiers removed)
///
/// Constructs an @c array from a C-array, deducing the size from the source array and either deducing the element type
/// (when @c D is @c void) or converting each element to the explicitly requested type @c D.
template <class D>
class to_array_fn {
  static_assert(std::is_void<D>::value || std::is_object<D>::value, "'D' must an object type");

  /// @brief type of the conversion function object applied to each element
  using convert_fn = std::conditional_t<std::is_void<D>::value, identity, decltype(construct<D>)>;

  /// @brief type of the resulting array's element type, deduced from the source array's element type if @c D is @c void
  /// @tparam From reference-qualified element type of the source array
  template <typename From>
  using to_t = std::conditional_t<std::is_void<D>::value, remove_cvref_t<From>, D>;

  /// @brief Implementation for both the lvalue and rvalue overloads
  /// @tparam From reference-qualified element type of the source array, used to propagate value category
  /// @tparam Is parameter pack of indices of the elements to read from @c arr
  /// @tparam Iterator iterator type providing indexed access to the source array's elements
  /// @tparam To element type of the resulting @c array
  /// @param arr iterator-like handle to the source array's elements.
  /// @return @c array<Result, sizeof...(Is)> whose elements are constructed from
  /// <c> convert_fn{}(static_cast<From>(arr[Is]))... </c>
  ///
  /// Builds the resulting @c array by indexing into @c arr with each element of the index sequence and converting
  /// through @c convert_fn. The cast to @c From propagates the value category of the source array's elements so that
  /// rvalue overloads move from their elements while lvalue overloads copy from theirs.
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
  template <typename From, std::size_t... Is, typename Iterator>
  static constexpr auto impl(std::index_sequence<Is...>, Iterator arr) noexcept(
      noexcept(array<to_t<From>, sizeof...(Is)>{{convert_fn{}(static_cast<From>(arr[Is]))...}})
  ) -> array<to_t<From>, sizeof...(Is)> {
    // parasoft-begin-suppress AUTOSAR-M5_0_15-a "False positive: the pointer points to an array"
    return array<to_t<From>, sizeof...(Is)>{{convert_fn{}(static_cast<From>(arr[Is]))...}};
    // parasoft-end-suppress AUTOSAR-M5_0_15-a
  }
  // parasoft-end-suppress CERT_C-EXP37-a

 public:
  // NOLINTBEGIN(hicpp-avoid-c-arrays) Specifically providing overloads for c-array support.
  /// @brief construct an @c array from an lvalue C-array
  /// @param arr source values to copy from
  /// @return @c array of the deduced or requested element type
  template <
      typename T,
      std::size_t Size,
      constraints<
          std::enable_if_t<!std::is_array<T>::value>,
          std::enable_if_t<std::is_constructible<remove_cvref_t<T>, T&>::value>,
          std::enable_if_t<std::is_constructible<to_t<T&>, invoke_result_t<convert_fn, T&>>::value>> = nullptr>
  constexpr auto operator()(T (&arr)[Size]) const noexcept(  //
      noexcept(impl<T&>(std::make_index_sequence<Size>{}, arr))
  ) -> array<to_t<T&>, Size> {
    return impl<T&>(std::make_index_sequence<Size>{}, arr);
  }
  /// @brief construct an @c array from an rvalue C-array
  /// @param arr source values to move from
  /// @return @c array of the deduced or requested element type
  template <
      typename T,
      std::size_t Size,
      constraints<
          std::enable_if_t<!std::is_array<T>::value>,
          std::enable_if_t<std::is_move_constructible<remove_cvref_t<T>>::value>,
          std::enable_if_t<std::is_constructible<to_t<T&&>, invoke_result_t<convert_fn, T&&>>::value>> = nullptr>
  constexpr auto operator()(T (&&arr)[Size]) const noexcept(  //
      noexcept(impl<T&&>(std::make_index_sequence<Size>{}, std::move(arr)))
  ) -> array<to_t<T&&>, Size> {
    // If constexpr move_iterator were available, we could use that. Since it is not, we propagate the ref-qualifier.
    return impl<T&&>(std::make_index_sequence<Size>{}, std::move(arr));
  }
  // NOLINTEND(hicpp-avoid-c-arrays)
};

}  // namespace to_array_detail

/// @brief construct an @c array from a C-array
/// Deduces the resulting element type and copies or moves the source elements as appropriate.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(to_array_detail::to_array_fn<void>, to_array);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

/// @brief construct an @c array with a desired element type from a C-array.
/// @tparam T element type of the resulting @c array
///
/// Like @c to_array, but the element type of the resulting @c array is specified. Each source element is converted to
/// @c T before being used to initialize the corresponding element of the result.
template <typename T>
constexpr auto to_array_of = to_array_detail::to_array_fn<T>{};

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A2_8_1-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ARRAY_TO_ARRAY_HPP_
