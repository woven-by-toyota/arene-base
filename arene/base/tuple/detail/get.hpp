// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::tuple_detail::get"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_GET_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_GET_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/tuple/.*"

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/type_traits/priority_tag.hpp"

namespace arene {
namespace base {

namespace tuple_detail {
namespace get_detail {

/// @brief declaration used by the function object below
template <class>
auto get() -> void = delete;

/// @brief function object implementing index-based get for a tuple-ish type
/// @tparam Index index to get
///
template <std::size_t Index>
class get_index_fn  //
{
  /// @brief obtain an element of a tuple-ish by index with member get
  /// @tparam Tuple type implementing the tuple-protocol
  /// @param value tuple-ish
  /// @return obtains @c Index 'th element of @c value
  ///
  template <class Tuple>
  static constexpr auto impl(priority_tag<1>, Tuple&& value)             //
      noexcept(noexcept(std::declval<Tuple&&>().template get<Index>()))  //
      -> decltype(std::forward<Tuple>(value).template get<Index>())      //
  {
    return std::forward<Tuple>(value).template get<Index>();
  }

  /// @brief obtain an element of a tuple-ish by index with ADL get
  /// @tparam Tuple type implementing the tuple-protocol
  /// @param value tuple-ish
  /// @return obtains @c Index 'th element of @c value
  ///
  template <class Tuple>
  static constexpr auto impl(priority_tag<0>, Tuple&& value)   //
      noexcept(noexcept(get<Index>(std::declval<Tuple&&>())))  //
      -> decltype(get<Index>(std::forward<Tuple>(value)))      //
  {
    return get<Index>(std::forward<Tuple>(value));
  }

 public:
  /// @brief obtain an element of a tuple by index
  /// @tparam Tuple type implementing the tuple-protocol
  /// @param value A tuple-ish object
  /// @return obtains @c Index 'th element of @c value
  ///
  /// Expression equivalent to:
  /// * <c> std::forward<Tuple>(value).get<Index>() </c> if valid
  /// * otherwise, <c> get<Index>(std::forward<Tuple>(value)) </c> if valid, where
  ///   @c get is looked up by ADL only
  ///
  /// @note Constraints:
  ///   * <c> Index < std::tuple_size<std::remove_reference_t<Tuple>>::value </c>
  ///
  template <  //
      class Tuple,
      constraints<  //
          std::enable_if_t<(Index < std::tuple_size<std::remove_reference_t<Tuple>>::value)>> = nullptr>
  constexpr auto operator()(Tuple&& value) const                                        //
      noexcept(noexcept(impl(std::declval<priority_tag<1>>(), std::declval<Tuple>())))  //
      -> decltype(impl(priority_tag<1>{}, std::forward<Tuple>(value)))                  //
  {
    return impl(priority_tag<1>{}, std::forward<Tuple>(value));
  }
};
}  // namespace get_detail

/// @def arene::base::tuple_detail::get
/// @brief obtain an element of a tuple-ish by index
/// @tparam Index index of the element to access
/// @tparam Tuple type implementing the tuple-protocol
/// @param value tuple-ish
/// @return obtains @c Index 'th element of @c value
///
/// Expression equivalent to:
/// * <c> std::forward<Tuple>(value).get<Index>() </c> if valid
/// * otherwise, <c> get<Index>(std::forward<Tuple>(value)) </c> if valid, where
///   @c get is looked up by ADL only
///
/// @note Constraints:
///   * <c> Index < std::tuple_size<std::remove_reference_t<Tuple>>::value </c>
///
/// @note This prioritization matches lookup of @c get in structured bindings,
///   as specified in [dcl.struct.bind].
///
/// @note A type implements the tuple protocol if it
///   * provides a specialization of @c std::tuple_size
///   * provides a specialization of @c std::tuple_element
///   * provides a function to get an element by index, where that can be
///     specified in detailed above
/// Note that the tuple-like concept defines types that satisfy the tuple
/// protocol but are restricted to a list of types defined in the @c std
/// namespace (@c std::array, @c std::pair, @c std::tuple in C++14). The term
/// tuple-ish is used to define types that satisfy the tuple protocol without
/// the restriction that the type is define in the @c std namespace -- which
/// includes user-defined types.
///
template <std::size_t Index>
extern constexpr auto get = get_detail::get_index_fn<Index>{};

}  // namespace tuple_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_GET_HPP_
