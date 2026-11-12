// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::iter_move"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_MOVE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_MOVE_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/iterator/detail/is_dereferenceable.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_class.hpp"
#include "arene/base/stdlib_choice/is_enum.hpp"
#include "arene/base/stdlib_choice/is_lvalue_reference.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"

namespace arene {
namespace base {
namespace ranges {

namespace itr_move_detail {

/// @brief Deleted fallback for iter_move to ensure adl only finds function overloads
template <typename>
auto iter_move() -> void = delete;

/// @brief Implementation of @c arene::base::ranges::iter_move
/// @see arene::base::ranges::iter_move
class iter_move_fn {
  /// @brief Result type of the ADL-based @c iter_move for a given iterator type.
  /// @tparam Itr The type of the iterator for which to determine the result type.
  template <typename Itr>
  using adl_itr_move_rt = decltype(iter_move(std::declval<Itr&&>()));
  /// @brief Helper variable to determine if ADL-based @c iter_move is available for a given iterator type.
  /// @tparam Itr The type of the iterator to check for ADL-based @c iter_move availability.
  template <typename Itr>
  static constexpr bool has_adl_iter_move_v = substitution_succeeds<adl_itr_move_rt, Itr>;
  /// @brief Helper type to determine the type obtained by dereferencing an iterator.
  /// @tparam Itr The type of the iterator for which to determine the dereferenced type.
  template <typename Itr>
  using deref_t = decltype(*std::declval<Itr>());

 public:
  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False positive: identifier 'iterator' does not hide anything"
  /// @brief Specialization for when ADL-based @c iter_move is available for the given iterator type.
  /// @tparam Itr The type of the iterator for which to determine the result type.
  /// @param iterator The iterator to apply ADL-based @c iter_move to.
  /// @return The result of applying ADL-based @c iter_move to the iterator.
  template <
      typename Itr,
      constraints<
          std::enable_if_t<
              std::is_class<std::remove_reference_t<Itr>>::value || std::is_enum<std::remove_reference_t<Itr>>::value>,
          std::enable_if_t<has_adl_iter_move_v<Itr>>> = nullptr>
  constexpr auto operator()(Itr&& iterator) const noexcept(noexcept(iter_move(std::declval<Itr>())))
      -> adl_itr_move_rt<Itr> {
    return iter_move(std::forward<Itr>(iterator));
  }

  // parasoft-begin-suppress AUTOSAR-A18_9_2-a "Use of 'forward' here ensures rvalues are correctly moved"
  // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "False Positive: Generic code, moved object is not explicitly const"
  /// @brief Specialization for when dereferenced type is an lvalue reference.
  /// @tparam Itr The type of the iterator for which to determine the result type.
  /// @param iterator The iterator to apply the default implementation of @c iter_move to.
  /// @return @c std::move(*itr)
  template <
      typename Itr,
      typename DerefT = deref_t<Itr&&>,
      constraints<
          std::enable_if_t<!has_adl_iter_move_v<Itr>>,
          std::enable_if_t<iterator_detail::is_dereferenceable_v<Itr>>,
          std::enable_if_t<std::is_lvalue_reference<DerefT>::value>> = nullptr>
  constexpr auto operator()(Itr&& iterator) const noexcept(noexcept(std::move(*std::declval<Itr>())))
      -> decltype(std::move(*std::forward<Itr>(iterator))) {
    return std::move(*std::forward<Itr>(iterator));
  }
  // parasoft-end-suppress AUTOSAR-A18_9_2-a "Use of 'forward' here ensures rvalues are correctly moved"
  // parasoft-end-suppress AUTOSAR-A18_9_3-a-2 "False Positive: Generic code, moved object is not explicitly const"

  /// @brief Specialization for when dereferenced type is not an lvalue reference.
  /// @tparam Itr The type of the iterator for which to determine the result type.
  /// @param iterator The iterator to apply the default implementation of @c iter_move to.
  /// @return @c *itr
  template <
      typename Itr,
      typename DerefT = deref_t<Itr&&>,
      constraints<
          std::enable_if_t<!has_adl_iter_move_v<Itr>>,
          std::enable_if_t<iterator_detail::is_dereferenceable_v<Itr>>,
          std::enable_if_t<!std::is_lvalue_reference<DerefT>::value>> = nullptr>
  constexpr auto operator()(Itr&& iterator) const noexcept(noexcept(*std::declval<Itr>()))
      -> decltype(*std::forward<Itr>(iterator)) {
    return *std::forward<Itr>(iterator);
  }
};

}  // namespace itr_move_detail

/// @brief Performs the move operation on dereferenced value of a dereferenceable type.
/// @tparam Itr The type of the iterator to apply iter_move to.
/// The iterator to move from is passed as @c iterator.
/// @return The result of applying iter_move to the iterator.
///
/// This operation is expression-equivalent to one of the following operations, in order of preference:
/// 1. If @c Itr satisfies either @c std::is_class or @c std::is_enum , and @c iter_move(itr) is well-formed by argument
///    dependent lookup, returns @c iter_move(itr) .
/// 2. Otherwise, if @c *itr is well-formed:
///    - if @c *itr is an lvalue reference, returns @c std::move(*itr) .
///    - otherwise, returns @c *itr .
///
/// If none of the requirements are satisfied, then @c iter_move is ill-formed. This can cause a substitution failure
/// when @c iter_move appears in the context of a template instantiation.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: the referenced constexpr variable is initialized"
ARENE_CPP14_INLINE_VARIABLE(itr_move_detail::iter_move_fn, iter_move);
// parasoft-end-suppress CERT_CPP-DCL56-a
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace ranges
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_MOVE_HPP_
