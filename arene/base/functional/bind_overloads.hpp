// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::bind_overloads"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_BIND_OVERLOADS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_BIND_OVERLOADS_HPP_

// IWYU pragma: private, include "arene/base/functional.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/stdlib_choice/decay.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

namespace arene {
namespace base {

namespace bind_overloads_detail {

/// @brief Helper structure for creating an invocable that is the sum-type of N invocables.
/// @tparam OverloadTs The set of invocables to combine.
/// @note This has to be implemented as a recursive template because C++14 doesn't support argument pack expansion for
///       @c using declarations.
template <typename... OverloadTs>
class bind_overloads_impl;

/// @brief Specialization for a single invocable.
///
/// @tparam OverloadT The invocable to bind.
template <typename OverloadT>
class bind_overloads_impl<OverloadT> : OverloadT {
 public:
  /// @brief lvalue consuming ctor
  /// @param overload The invocable to bind.
  explicit bind_overloads_impl(OverloadT const& overload) noexcept(std::is_nothrow_copy_constructible<OverloadT>::value)
      : OverloadT(overload) {}

  /// @brief rvalue consuming ctor
  /// @param overload The invocable to bind.
  explicit bind_overloads_impl(OverloadT&& overload) noexcept(std::is_nothrow_move_constructible<OverloadT>::value)
      : OverloadT(std::move(overload)) {}

  /// @brief default destructor
  ~bind_overloads_impl() = default;

  /// @brief aliases in the underlying call operator.
  using OverloadT::operator();

 protected:
  /// @brief default copy ctor
  constexpr bind_overloads_impl(bind_overloads_impl const& copy) = default;
  /// @brief default move ctor
  constexpr bind_overloads_impl(bind_overloads_impl&& move) = default;
  /// @brief default copy assignment operator
  constexpr auto operator=(bind_overloads_impl const& copy) -> bind_overloads_impl& = default;
  /// @brief default move assignment operator
  constexpr auto operator=(bind_overloads_impl&& move) -> bind_overloads_impl& = default;
};

/// @brief Specialization for more than one invocable.
///
/// @tparam OverloadT The invocable to bind.
/// @tparam OverloadTs The remaining invocables to bind.
// parasoft-begin-suppress AUTOSAR-A10_1_1-a "Multiple base classes needed to implement semantics."
template <typename OverloadT, typename... OverloadTs>
class bind_overloads_impl<OverloadT, OverloadTs...>
    : OverloadT
    , bind_overloads_impl<OverloadTs...> {
 public:
  /// @brief copy ctor for current invocable.
  /// @tparam UOverloads The types of the remaining invocables to bind.
  /// @param overload The invocable to bind.
  /// @param overloads The remaining invocables to bind.
  template <typename... UOverloads>
  constexpr explicit bind_overloads_impl(OverloadT const& overload, UOverloads&&... overloads) noexcept(
      std::is_nothrow_copy_constructible<OverloadT>::value &&
      std::is_nothrow_constructible<bind_overloads_impl<OverloadTs...>, UOverloads&&...>::value
  )
      : OverloadT(overload),
        bind_overloads_impl<OverloadTs...>(std::forward<UOverloads>(overloads)...) {}

  /// @brief move ctor for current invocable.
  /// @tparam UOverloadTs The types of the remaining invocables to bind.
  /// @param overload The invocable to bind.
  /// @param overloads The remaining invocables to bind.
  template <typename... UOverloadTs>
  constexpr explicit bind_overloads_impl(OverloadT&& overload, UOverloadTs&&... overloads) noexcept(
      std::is_nothrow_move_constructible<OverloadT>::value &&
      std::is_nothrow_constructible<bind_overloads_impl<OverloadTs...>, UOverloadTs&&...>::value
  )
      : OverloadT(std::move(overload)),
        bind_overloads_impl<OverloadTs...>(std::forward<UOverloadTs>(overloads)...) {}

  /// @brief default destructor
  ~bind_overloads_impl() = default;

  /// @brief aliases in the underlying call operator.
  using OverloadT::operator();
  /// @brief aliases in the other underlying call operators.
  using bind_overloads_impl<OverloadTs...>::operator();

 protected:
  /// @brief default copy ctor
  constexpr bind_overloads_impl(bind_overloads_impl const& copy) = default;
  /// @brief default move ctor
  constexpr bind_overloads_impl(bind_overloads_impl&& move) = default;
  /// @brief default copy assignment operator
  constexpr auto operator=(bind_overloads_impl const& copy) -> bind_overloads_impl& = default;
  /// @brief default move assignment operator
  constexpr auto operator=(bind_overloads_impl&& move) -> bind_overloads_impl& = default;
};
// parasoft-begin-suppress AUTOSAR-A10_1_1-a "Multiple base classes needed to implement semantics."

/// @brief A type which is the sum-type of all the input invocables.
/// This indirection is needed to satisfy CppTestPro static analysis around slicing constructors while keeping a
/// recursive template copyable/movable as they don't consider the visibility of inheritance of the base class.
template <typename... OverloadTs>
class bind_overloads_t : bind_overloads_impl<OverloadTs...> {
 public:
  /// @brief alias in underlying constructors.
  using bind_overloads_impl<OverloadTs...>::bind_overloads_impl;
  /// @brief alias in underlying call operator.
  using bind_overloads_impl<OverloadTs...>::operator();
};

}  // namespace bind_overloads_detail

///
/// @brief Given a sequence of invocables, creates a type which is the sum-type of all the input invocables.
///
/// Typically, @c bind_overloads is used to produce a variant-visitor inline from a sequence of lambdas, like so:
///
/// \snippet docs/examples/subpackages/variant_examples.cpp bind_overloads_usage
///
/// @tparam OverloadsT The types of the invocables to combine.
/// @param overloads The invocables to combine. They will be perfect-forwarded into the resulting object.
/// @return An implementation-defined type which is the summation of the input callables and can be invoked as if it
///         were any of them.
///
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive, this is not a member function and thus cannot be static."
template <typename... OverloadsT>
constexpr auto bind_overloads(OverloadsT&&... overloads) noexcept(
    std::is_nothrow_constructible<
        bind_overloads_detail::bind_overloads_t<std::decay_t<OverloadsT>...>,
        OverloadsT&&...>::value
) -> bind_overloads_detail::bind_overloads_t<std::decay_t<OverloadsT>...> {
  return bind_overloads_detail::bind_overloads_t<std::decay_t<OverloadsT>...>{std::forward<OverloadsT>(overloads)...};
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a "False positive, this is not a member function and thus cannot be static."

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_BIND_OVERLOADS_HPP_
