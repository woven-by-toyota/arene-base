// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_VISIT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_VISIT_HPP_

// IWYU pragma: private, include "arene/base/variant.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/functional/invoke.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/decay.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/variant/traits.hpp"
#include "arene/base/variant/variant.hpp"  // IWYU pragma: keep
// parasoft-end-suppress AUTOSAR-A16_2_2-a

namespace arene {
namespace base {

namespace visit_detail {

/// @brief Trait to determine the return type of invoking a visitor with a given alternative of a variant.
///
/// @tparam VisitorRefT The type of the visitor. Should include cref qualification.
/// @tparam VariantRefT The type of the variant. Should include cref qualification.
/// @tparam Idx The index of the alternative to consider
template <typename VisitorRefT, typename VariantRefT, std::size_t Idx>
using visitor_invoke_result_at_t =
    invoke_result_t<VisitorRefT, decltype(::arene::base::get<Idx>(std::declval<VariantRefT>()))>;

/// @brief Trait to determine what the return type of invoking a visitor on a variant is.
///
/// Equivalent to @c visitor_invoke_result_at_t<VisitorT,VariantT,0U> since all overloads must return the same type.
///
/// @tparam VisitorRefT The type of the visitor. Should include cref qualification.
/// @tparam VariantRefT The type of the variant. Should include cref qualification.
template <typename VisitorRefT, typename VariantRefT>
using visitor_invoke_result_t = visitor_invoke_result_at_t<VisitorRefT, VariantRefT, 0U>;

/// @brief Trait to determine if a visitor has the same return type when invoked with all alternatives of a variant.
///
/// @tparam VisitorRefT The type of the visitor. Should include cref qualification.
/// @tparam VariantRefT The type of the variant. Should include cref qualification.
/// @tparam Indexes an index sequence the size of VariantT
template <
    typename VisitorRefT,
    typename VariantRefT,
    typename Indexes = std::make_index_sequence<::arene::base::variant_size_v<std::decay_t<VariantRefT>>>>
class are_visitor_returns_the_same_type;

/// @brief Empty variant specialization of trait to determine if a visitor has the same return type when invoked with
/// all alternatives of a variant.
///
/// @tparam VisitorRefT The type of the visitor. Should include cref qualification.
/// @tparam VariantRefT The type of the variant. Should include cref qualification.
template <typename VisitorRefT, typename VariantRefT>
class are_visitor_returns_the_same_type<VisitorRefT, VariantRefT, std::index_sequence<>> {};

/// @brief Trait to determine if a visitor has the same return type when invoked with all alternatives of a variant.
///
/// @tparam VisitorRefT The type of the visitor. Should include cref qualification.
/// @tparam VariantRefT The type of the variant. Should include cref qualification.
/// @tparam Idxs The set of indexs for alternatives in @c VariantT
template <typename VisitorRefT, typename VariantRefT, std::size_t Idx, std::size_t... Idxs>
class are_visitor_returns_the_same_type<VisitorRefT, VariantRefT, std::index_sequence<Idx, Idxs...>>
    : are_visitor_returns_the_same_type<VisitorRefT, VariantRefT, std::index_sequence<Idxs...>> {
  static_assert(
      std::is_same<
          visitor_invoke_result_t<VisitorRefT, VariantRefT>,
          visitor_invoke_result_at_t<VisitorRefT, VariantRefT, Idx>>::value,
      "visit requires all visitor overloads to return exactly the same type."
  );
};

/// @brief Trait to determine if a visitor is noexcept when invoked with a given alternative of a variant and the return
/// is coerced into a given type.
///
/// @tparam Ret The return type to coerce to.
/// @tparam VisitorRefT The type of the visitor. Should include cref qualification.
/// @tparam VariantRefT The type of the variant. Should include cref qualification.
/// @tparam Idx The index of the alternative to consider
/// @return bool The result of
///         @c is_nothrow_invocable_v<VisitorT,decltype(::arene::base::get<Idx>(std::declval<VariantT>()))> .
template <typename Ret, typename VisitorRefT, typename VariantRefT, std::size_t Idx>
constexpr bool is_visitor_nothrow_invocable_r_at_v =
    is_nothrow_invocable_r_v<Ret, VisitorRefT, decltype(::arene::base::get<Idx>(std::declval<VariantRefT>()))> &&
    std::is_nothrow_constructible<Ret, visitor_invoke_result_at_t<VisitorRefT, VariantRefT, Idx>>::value;

/// @brief Trait to determine if a visitor is noexcept when invoked with all alternatives of a variant and coerced to a
/// given return type.
///
/// @tparam Ret The return type to coerce to.
/// @tparam VisitorRefT The type of the visitor. Should include cref qualification.
/// @tparam VariantRefT The type of the variant. Should include cref qualification.
/// @tparam Indexes an index sequence the size of VariantT
template <
    typename Ret,
    typename VisitorRefT,
    typename VariantRefT,
    typename Indexes = std::make_index_sequence<::arene::base::variant_size_v<std::decay_t<VariantRefT>>>>
constexpr bool is_visitor_nothrow_invocable_r_v{false};

/// @brief Trait to determine if a visitor is noexcept when invoked with all alternatives of a variant and coerced to a
/// given return type.
///
/// @tparam Ret The return type to coerce to.
/// @tparam VisitorRefT The type of the visitor. Should include cref qualification.
/// @tparam VariantRefT The type of the variant. Should include cref qualification.
/// @tparam Idxs The set of indexs for alternatives in @c VariantT
template <typename Ret, typename VisitorRefT, typename VariantRefT, std::size_t... Idxs>
constexpr bool is_visitor_nothrow_invocable_r_v<
    Ret,
    VisitorRefT,
    VariantRefT,
    std::index_sequence<Idxs...>>{::arene::base::all_of_v<
    is_visitor_nothrow_invocable_r_at_v<Ret, VisitorRefT, VariantRefT, Idxs>...>};

/// @brief Helper for mapping visitor overloads to variant alternatives and dispatching via a jump table.
///
/// @tparam VisitorRefT The type of the visitor. Should include cref qualification.
/// @tparam VariantRefT The type of the variant. Should include cref qualification.
/// @tparam Indices An index sequence the size of VariantT
template <
    typename VisitorRefT,
    typename VariantRefT,
    typename RetT = visitor_invoke_result_t<VisitorRefT, VariantRefT>,
    typename Indexes = std::make_index_sequence<::arene::base::variant_size_v<std::decay_t<VariantRefT>>>>
class visit_dispatcher;

/// @brief Helper for mapping visitor overloads to variant alternatives and dispatching via a jump table.
///
/// @tparam VisitorT The type of the visitor. Should include cref qualification.
/// @tparam VariantT The type of the variant. Should include cref qualification.
/// @tparam Idxs The set of indexs for alternatives in @c VariantT
template <typename VisitorRefT, typename VariantRefT, typename RetT, std::size_t... Idxs>
class visit_dispatcher<VisitorRefT, VariantRefT, RetT, std::index_sequence<Idxs...>> {
  /// @brief Trampoline function which invokes the visitor using the value at the given alternative in the variant.
  ///
  /// @tparam Idx The index of the alternative in the variant to invoke visitor with.
  /// @param visitor The visitor to invoke with the variant.
  /// @param vrnt The variant to invoke the visitor with.
  /// @invariant @c holds_alternative<Idx>(vrnt)==true .
  /// @return return_type The result of invoking the variant with the given alternative.
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a "False positive, the static_cast is equivalent to move."
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a "False positive, the static_cast is equivalent to move."
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a "False positive, the static_cast is equivalent to move."
  template <std::size_t Idx>
  static constexpr auto invoke_visitor(
      VisitorRefT visitor,
      VariantRefT vrnt
  ) noexcept(is_visitor_nothrow_invocable_r_at_v<RetT, VisitorRefT, VariantRefT, Idx>) -> RetT {
    return ::arene::base::invoke(
        static_cast<VisitorRefT>(visitor),
        ::arene::base::get<Idx>(static_cast<VariantRefT>(vrnt))
    );
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a "False positive, the static_cast is equivalent to move."
  // parasoft-end-suppress AUTOSAR-A8_4_5-a "False positive, the static_cast is equivalent to move."
  // parasoft-end-suppress AUTOSAR-A12_8_4-a "False positive, the static_cast is equivalent to move."

  /// @brief The signature of a trampoline function for invoking a visitor with a variant.
  using invoke_visitor_fn_t = RetT (*)(VisitorRefT visitor, VariantRefT vrnt);
  /// @brief The type of a jump table of trampoline functions.
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) Better codegen, and compile-time invariants prevent oob access.
  using jump_table_t = invoke_visitor_fn_t[::arene::base::variant_size_v<std::decay_t<VariantRefT>>];

  /// @brief The jump table of trampoline functions. The entry at @c Idx is @c invoke_visitor<Idx> .
  // parasoft-begin-suppress AUTOSAR-A18_1_1-a "Better codegen, and compile-time invariants prevent oob access."
  static constexpr jump_table_t visitor_fns{invoke_visitor<Idxs>...};
  // parasoft-end-suppress AUTOSAR-A18_1_1-a

 public:
  /// @brief Invokes the given visitor with the active alternative in the given variant.
  ///
  /// @param visitor The visitor to invoke with the variant.
  /// @param vrnt The variant to invoke the visitor with.
  /// @return return_type Equivalent to @c invoke_visitor<Idx>(visitor,variant) where @c Idx is equal to
  ///         @c variant.index() .
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a "False positive, the static_cast is equivalent to move."
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a "False positive, the static_cast is equivalent to move."
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a "False positive, the static_cast is equivalent to move."
  constexpr auto operator()(VisitorRefT visitor, VariantRefT vrnt) const
      noexcept(is_visitor_nothrow_invocable_r_v<RetT, VisitorRefT, VariantRefT>) -> RetT {
    return visitor_fns[vrnt.index()](static_cast<VisitorRefT>(visitor), static_cast<VariantRefT>(vrnt));
  }
  // parasoft-end-suppress AUTOSAR-A12_8_4-a
  // parasoft-end-suppress AUTOSAR-A8_4_5-a
  // parasoft-end-suppress AUTOSAR-A8_4_6-a
};

template <typename VisitorRefT, typename VariantRefT, typename RetT, std::size_t... Idxs>
constexpr typename visit_dispatcher<VisitorRefT, VariantRefT, RetT, std::index_sequence<Idxs...>>::jump_table_t
    visit_dispatcher<VisitorRefT, VariantRefT, RetT, std::index_sequence<Idxs...>>::visitor_fns;

}  // namespace visit_detail

/// @brief Invokes a given visitor with the active alternative in a given variant.
///
/// @tparam VisitorT The type of the visitor to invoke.
/// @tparam VariantT The type of the variant to unwrap. Given @c variant<Ts...> , @c is_invocable<VisitorT,Ts> must hold
///                  where @c Visitor and @c Ts maintain the cref qualification of Ts.
/// @param visitor The visitor to invoke.
/// @param vrnt The variant to take the active alternative of.
/// @return The result of invoking @c visitor with the active alternative in @c vrnt .
/// @pre @c vrnt.valueless_by_exception()==false , else @c ARENE_PRECONDITION violation.
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive, this is not a member function and thus cannot be static.""
template <typename VisitorT, typename VariantT>
auto visit(VisitorT&& visitor, VariantT&& vrnt) noexcept(visit_detail::is_visitor_nothrow_invocable_r_v<
                                                         visit_detail::
                                                             visitor_invoke_result_t<decltype(visitor), decltype(vrnt)>,
                                                         decltype(visitor),
                                                         decltype(vrnt)>)
    -> visit_detail::visitor_invoke_result_t<decltype(visitor), decltype(vrnt)> {
  // We do this indirection to get better compiler messages if this condition is violated; the user sees the actual
  // overload that violates the constraint. The validation struct has to be assigned to ignore to avoid
  // discard-of-return static analysis warnings.
  std::ignore = visit_detail::are_visitor_returns_the_same_type<decltype(visitor), decltype(vrnt)>{};
  ARENE_PRECONDITION(!vrnt.valueless_by_exception());
  return visit_detail::visit_dispatcher<decltype(visitor), decltype(vrnt)>{
  }(std::forward<VisitorT>(visitor), std::forward<VariantT>(vrnt));
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a "False positive, this is not a member function and thus cannot be static."

/// @brief Invokes a given visitor with the active alternative in a given variant.
///
/// @tparam RetT The type to coerce all return types to.
/// @tparam VisitorT The type of the visitor to invoke.
/// @tparam VariantT The type of the variant to unwrap. Given @c variant<Ts...> , @c is_invocable_r<RetT,VisitorT,Ts>
///                  must hold where @c Visitor and @c Ts maintain the cref qualification @c visitor and @c variant
///                  respectively.
/// @param visitor The visitor to invoke.
/// @param vrnt The variant to take the active alternative of.
/// @return RetT The result of invoking @c visitor with the active alternative in @c vrnt , converted to @c RetT .
/// @pre @c vrnt.valueless_by_exception()==false , else @c ARENE_PRECONDITION violation.
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive, this is not a member function and thus cannot be static."
template <typename RetT, typename VisitorT, typename VariantT>
auto visit(
    VisitorT&& visitor,
    VariantT&& vrnt
) noexcept(visit_detail::is_visitor_nothrow_invocable_r_v<RetT, decltype(visitor), decltype(vrnt)>) -> RetT {
  ARENE_PRECONDITION(!vrnt.valueless_by_exception());
  return visit_detail::visit_dispatcher<decltype(visitor), decltype(vrnt), RetT>{
  }(std::forward<VisitorT>(visitor), std::forward<VariantT>(vrnt));
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a "False positive, this is not a member function and thus cannot be static."

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_VISIT_HPP_
