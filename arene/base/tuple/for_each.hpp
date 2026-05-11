// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_FOR_EACH_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_FOR_EACH_HPP_

// IWYU pragma: private, include "arene/base/tuple.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/functional/invoke.hpp"
#include "arene/base/monostate/monostate.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/tuple/detail/get.hpp"
#include "arene/base/type_manipulation/consume_values.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {

namespace tuple_for_each_detail {
/// @brief Invokes the specified callable with the specified arguments and discard the result, returning @c monostate to
/// be consumable
/// @tparam F The type of the callable to invoke
/// @tparam ArgTypes The types of the arguments
/// @param func The callable to invoke
/// @param args The arguments with which to invoke the callable
/// @return A @c monostate object
/// @note Does not participate in overload resolution if the specified @c F cannot be invoked with the specified @c
/// ArgTypes
template <typename F, typename... ArgTypes, constraints<std::enable_if_t<is_invocable_v<F, ArgTypes...>>> = nullptr>
constexpr auto invoke_for_consume(F&& func, ArgTypes&&... args) noexcept(is_nothrow_invocable_v<F, ArgTypes...>)
    -> monostate {
  static_cast<void>(arene::base::invoke(std::forward<F>(func), std::forward<ArgTypes>(args)...));
  return {};
}

// parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameters 'tuple' and 'operation' are used"

/// @brief The underlying implementation for for_each. Not intended to be
/// called by user code.
/// @tparam Tuple type implementing the tuple protocol.
/// @tparam Op The type of operation to perform on each tuple element.
/// @tparam I The parameter pack of indices.
/// @param tuple The tuple-ish to iterate over.
/// @param operation The operation to perform on each element.
template <typename Tuple, typename Op, std::size_t... I>
constexpr void for_each(Tuple&& tuple, Op&& operation, std::index_sequence<I...>) {
  consume_values({invoke_for_consume(std::forward<Op>(operation), tuple_detail::get<I>(std::forward<Tuple>(tuple)))...}
  );
}

// parasoft-end-suppress AUTOSAR-A0_1_4-a

/// @brief The underlying implementation for for_each_index. Not intended to
/// be called by user code.
/// @tparam Tuple type implementing the tuple protocol.
/// @tparam Op The type of operation to perform on each tuple element, which
/// accepts the index as the first parameter of type std::size_t.
/// @tparam I The parameter pack of indices.
/// @param tuple The tuple-ish to iterate over.
/// @param operation The operation to perform on each element, which should accept
/// a std::size_t as the first argument for the element's index.
template <typename Tuple, typename Op, std::size_t... I>
constexpr void for_each_index(Tuple&& tuple, Op&& operation, std::index_sequence<I...>) {
  consume_values({invoke_for_consume(
      std::forward<Op>(operation),
      std::integral_constant<std::size_t, I>{},
      tuple_detail::get<I>(std::forward<Tuple>(tuple))
  )...});
}
}  // namespace tuple_for_each_detail

/// @brief Perform an operation on each element of a "tuple-ish" type, which
///   includes std::tuple, std::pair, std::array, or arene::base::array.
/// @tparam Tuple type implementing the tuple protocol.
/// @tparam Op The type of operation to perform on each tuple element.
/// @param tuple The tuple-ish to iterate over.
/// @param operation The operation to perform on each element.
///
/// @note A type implements the tuple protocol if it
///   * provides a specialization of @c std::tuple_size
///   * provides a specialization of @c std::tuple_element
///   * provides a function to get an element by index @c I, which is specified
///     as:
///     ** <c> std::forward<Tuple>(tuple).get<I>()  </c> if valid
///     ** otherwise, <c> get<I>(std::forward<Tuple>(tuple))  </c> if valid,
///        where @c get is looked up by ADL only
/// Note that the tuple-like concept defines types that satisfy the tuple
/// protocol but are restricted to a list of types defined in the @c std
/// namespace (@c std::array, @c std::pair, @c std::tuple in C++14). The term
/// tuple-ish is used to define types that satisfy the tuple protocol without
/// the restriction that the type is define in the @c std namespace -- which
/// includes user-defined types.
///
template <typename Tuple, typename Op>
constexpr void for_each(Tuple&& tuple, Op&& operation) {  // CODEQLFP(DCL51-CPP)
  tuple_for_each_detail::for_each(
      std::forward<Tuple>(tuple),
      std::forward<Op>(operation),
      std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{}
  );
}

/// @brief Perform an operation on each element of a "tuple-ish" type, which
///   includes std::tuple, std::pair, std::array, or arene::base::array.
/// @tparam Tuple type implementing the tuple protocol.
/// @tparam Op The type of operation to perform on each tuple element, which
///   accepts the index as the first parameter of type @c std::size_t or
///   @c std::integral_constant<std::size_t, N>.
/// @param tuple The tuple-ish to iterate over.
/// @param operation The operation to perform on each element, which should accept
///   a @c std::size_t or @c std::integral_constant<std::size_t, N> as the first
///   argument for the element's index.
///
/// @note A type implements the tuple protocol if it
///   * provides a specialization of @c std::tuple_size
///   * provides a specialization of @c std::tuple_element
///   * provides a function to get an element by index @c I, which is specified
///     as:
///     ** <c> std::forward<Tuple>(tuple).get<I>()  </c> if valid
///     ** otherwise, <c> get<I>(std::forward<Tuple>(tuple))  </c> if valid,
///        where @c get is looked up by ADL only
/// Note that the tuple-like concept defines types that satisfy the tuple
/// protocol but are restricted to a list of types defined in the @c std
/// namespace (@c std::array, @c std::pair, @c std::tuple in C++14). The term
/// tuple-ish is used to define types that satisfy the tuple protocol without
/// the restriction that the type is defined in the @c std namespace -- which
/// includes user-defined types.
///
template <typename Tuple, typename Op>
constexpr void for_each_index(Tuple&& tuple, Op&& operation) {
  tuple_for_each_detail::for_each_index(
      std::forward<Tuple>(tuple),
      std::forward<Op>(operation),
      std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{}
  );
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_FOR_EACH_HPP_
