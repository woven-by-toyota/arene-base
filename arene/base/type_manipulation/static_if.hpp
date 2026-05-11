// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_STATIC_IF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_STATIC_IF_HPP_

namespace arene {
namespace base {

/// @brief A metaprogramming facility for selecting a branch statically without evaluating both branches apriori.
///
/// This has uses similar to @c std::conditional except that it allows for lazily evaluated metafunctions. Usage can
/// lead to fewer template instantiations, which can have significant impact on compile times in low level facilities.
///
/// @note This is a rather advanced metaprogramming facility. If you aren't sure if you need it, you probably don't, and
///       should prefer the more conventional @c std::conditional .
///
/// The principle that's followed is that instantiations of type templates are costly in terms of resource usage during
/// compilation and have a tendency to add up surprisingly quickly, however, template aliases are close to free (you
/// only pay for the substitution process of the template arguments). @c static_if minimizes resource usage at
/// compile-time because regardless of how many times a developer uses @c static_if , with any combination of
/// arguments, there are only two types: @c static_if<true> and @c static_if<false> . All branching is done with nested
/// templates aliases.
///
/// In order to handle a variety of branch kinds, there are 4 different forms of "then else" corresponding to when none,
/// one, or both branches take a lazily-evaluated metafunction. The naming convention used is that @c apply in the name
/// corresponds to a parameter that is a metafunction. In other words:
///
///   then_else             // No metafunctions
///   then_apply_else       // The "then" branch is a metafunction
///   then_else_apply       // The "else" branch is a metafunction
///   then_apply_else_apply // Both "then" and "else" branches are metafunctions
///
/// Usage Example:
/// @code {cpp}
///   // Use a lock-free queue if T is trivially-copyable, otherwise a fallback.
///   template <class T>
///   using queue_type
///     = typename static_if<std::is_trivially_copyable_v<T>> // The condition
///       ::template then_apply_else_apply<
///         lockfree_queue,    // A lockfree queue template
///         queue_with_locks,  // A queue-with-locks template
///         T                  // The parameters to pass to the chosen template
///       >;
/// @endcode
template <bool V>
struct static_if;

/// @brief The definition when @c static_if gets a "true" condition
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <>
struct static_if<true> {
  // Note:
  //   This is the "true" specialization, so all branches in this definition
  //   will take the "true" route.

  /// @brief An alias that, itself, takes an "OnTrue" alias, an "OnFalse" alias, and a
  /// pack of type parameters "...P"
  ///
  /// If condition were true (it is)
  ///   results in OnTrue<P..>
  ///   otherwise results in OnFalse<P...>
  template <
      template <class...>
      class OnTrue,  // Alias to apply if in the true branch (we are)
      template <class...>
      class,      // Alias to apply if in the false
                  // branch (we're not)
      class... P  // Parameters to pass to the result alias
      >
  using then_apply_else_apply = OnTrue<P...>;

  /// @brief If condition were true, apply an alias, otherwise result in OnFalse.
  template <
      template <class...>
      class OnTrue,  // Alias to apply if in the true branch (we are)
      class,         // Result if false (we're not false)
      class... P     // Parameters to pass to the result alias
      >
  using then_apply_else = OnTrue<P...>;

  /// @brief If condition were true, result in OnTrue, otherwise apply an alias.
  template <
      class OnTrue,  // Result if true (we are)
      template <class...>
      class,    // Alias to apply if in the false
                // branch (we're not)
      class...  // Parameters to pass to the result alias
      >
  using then_else_apply = OnTrue;

  /// @brief If condition were true, result in OnTrue, otherwise result in OnFalse.
  template <
      class OnTrue,  // Result if in the true branch (we are)
      class          // Result if in the false branch (we're not false)
      >
  using then_else = OnTrue;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief The definition when static_if gets a "false" condition
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <>
struct static_if<false> {
  // Note:
  //   This is the "false" specialization, so all branches in this definition
  //   will take the "false" route.

  /// @brief An alias that, itself, takes an "OnTrue" alias, an "OnFalse" alias, and a
  /// pack of type parameters "...P"
  ///
  /// If condition were true (it is not)
  ///   results in OnTrue<P..>
  ///   otherwise results in OnFalse<P...>
  template <
      template <class...>
      class,  // Alias to apply if in the true branch
              // (we're not)
      template <class...>
      class OnFalse,  // Alias to apply if in the false branch (we are)
      class... P      // Parameters to pass to the result alias
      >
  using then_apply_else_apply = OnFalse<P...>;

  /// @brief If condition were true, apply an alias, otherwise result in OnFalse.
  template <
      template <class...>
      class,          // Alias to apply if in the true branch
                      // (we're not)
      class OnFalse,  // Result if false (we are)
      class...        // Parameters to pass to the result alias
      >
  using then_apply_else = OnFalse;

  /// @brief If condition were true, result in OnTrue, otherwise apply an alias.
  template <
      class,  // Result if true (we're not)
      template <class...>
      class OnFalse,  // Alias to apply if in the false branch (we are)
      class... P      // Parameters to pass to the result alias
      >
  using then_else_apply = OnFalse<P...>;

  /// @brief If condition were true, result in OnTrue, otherwise result in OnFalse.
  template <
      class,         // Result if in the true branch (we're not are)
      class OnFalse  // Result if in the false branch (we are)
      >
  using then_else = OnFalse;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_STATIC_IF_HPP_
