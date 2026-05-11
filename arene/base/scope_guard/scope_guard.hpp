// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SCOPE_GUARD_SCOPE_GUARD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SCOPE_GUARD_SCOPE_GUARD_HPP_

// IWYU pragma: private, include "arene/base/scope_guard.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/exchange.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {

/// @brief RAII utility for executing logic on scope exit
///
/// The @c scope_guard type is a utility that helps writing exception-safe code by ensuring that some code gets run on
/// scope-exit, regardless of how the scope is exited (return, exception, flowing off end, etc). The destructor of the
/// @c scope_guard class invokes the function passed to the constructor.
///
/// @see arene::base::on_scope_exit for a helper function for creating @c scope_guard instances.
///
/// @tparam Func The functor to be called on scope exit. Must have signature <c> void(void) noexcept </c>. It also must
///         be nothrow move constructable for scope_guard to be move-constructable.
template <typename Func>
class ARENE_NODISCARD scope_guard {
  static_assert(is_nothrow_invocable_r_v<void, Func>, "Func must have signature void(void) noexcept");

 public:
  /// @brief Construct a new scope_guard that decay-copies the
  /// function and ensure that it is called upon destruction of the
  /// scope_guard object.
  /// @tparam Func2 The type of the supplied callable
  /// @param supplied_callable The supplied callable
  template <
      typename Func2,
      constraints<
          std::enable_if_t<std::is_constructible<Func, Func2>::value>,
          std::enable_if_t<!std::is_same<remove_cvref_t<Func2>, scope_guard>::value>> = nullptr>
  // This has been guarded against by the !is_same<...> constraint above.
  // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
  explicit scope_guard(Func2&& supplied_callable) noexcept(std::is_nothrow_constructible<Func, Func2>::value)
      : func_(std::forward<Func2>(supplied_callable)),
        cancelled_(false) {}

  /// @brief Move constructor.
  ///
  /// Moves ownership of the function and responsibility for calling the function on destruction to the newly
  /// constructed scope_guard object.
  ///
  /// @param other The scope_guard to move from.
  /// @post The held function will be the one from @c other
  /// @post The cancel state will be the one from @c other
  /// @post The moved-from @c scope_guard can be safely destructed.
  scope_guard(scope_guard&& other) noexcept
      : func_(std::move(other.func_)),
        cancelled_(std::exchange(other.cancelled_, true)) {
    static_assert(std::is_nothrow_move_constructible<Func>::value, "Func must be nothrow move constructible");
  }

  /// @brief Move assignment is deleted as allowing it would overwrite the existing guard function without executing it.
  auto operator=(scope_guard&& other) -> scope_guard& = delete;

  /// @brief copy-construction is deleted as two guards cannot own the same function.
  scope_guard(scope_guard const&) = delete;
  /// @brief copy-assignment is deleted as two guards cannot own the same function.
  auto operator=(scope_guard const&) -> scope_guard& = delete;

  /// @brief Destroy the scope guard and invoke the function if it should be executed
  /// @post The held function is executed as if via @c invoke_now() .
  ~scope_guard() { invoke_now(); }

  /// @brief Declares the held function should not be invoked.
  /// @post Calls to @c invoke_now() will be noops.
  void cancel() noexcept { cancelled_ = true; }

  /// @brief Invokes the held function if it has not be otherwise canceled.
  /// @post If @c cancel() has not been called, invokes the held functor.
  /// @post Subsequent calls to @c invoke_now() are noops as if @c cancel() had been called.
  void invoke_now() noexcept {
    if (!cancelled_) {
      cancelled_ = true;
      func_();
    }
  }

 private:
  /// @brief The function to invoke
  Func func_;

  /// @brief Flag to signal if the functor should be invoked.
  bool cancelled_;  // NOLINT(modernize-use-default-member-init)
};

/// @brief Declarative helper for constructing @c scope_guard instances.
///
/// A simple factory function for @c scope_guard with a declarative name that makes its purpose clear. Example usage:
/// \snippet docs/examples/scope_guard_examples.cpp basic_usage
///
/// @tparam CallableType a callable with signature <c> void(void) noexcept </c>. Must be move-copyable and decay
/// constructible.
/// @param func The callable to invoke upon destruction of the returned scope_guard object.
/// @return scope_guard A scope_guard instance initialized from @c func through forwarding.
template <typename CallableType, constraints<std::enable_if_t<is_nothrow_invocable_r_v<void, CallableType>>> = nullptr>
ARENE_NODISCARD auto on_scope_exit(CallableType&& func
) noexcept(std::is_nothrow_constructible<remove_cvref_t<CallableType>, CallableType>::value)
    -> scope_guard<remove_cvref_t<CallableType>> {
  return scope_guard<remove_cvref_t<CallableType>>{std::forward<CallableType>(func)};
}

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SCOPE_GUARD_SCOPE_GUARD_HPP_
