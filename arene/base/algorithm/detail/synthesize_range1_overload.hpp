// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// parasoft-begin-suppress AUTOSAR-A2_8_1-a CERT_C-EXP37-a "False positive: header defines a matching type and
// operator() is not on this line"
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_SYNTHESIZE_RANGE1_OVERLOAD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_SYNTHESIZE_RANGE1_OVERLOAD_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/ranges/begin_end.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace arene {
namespace base {
namespace algorithm_detail {

/// @brief adaptor that synthesizes a range-taking overload for an
///   iterator-based algorithm function object
/// @tparam F Iterator-based algorithm function object to adapt
///
/// This adaptor is intended for function objects whose primary call operator
/// takes a pair of iterators as its first two arguments, for example:
///
/// @code
/// f(first, last, args...)
/// @endcode
///
/// @c synthesize_range1_overload<F> adds an additional overload that accepts a
/// range as the first argument and dispatches to @c F with @c
/// ranges::begin(range) and @c ranges::end(range):
///
/// @code
/// f(range, args...)  // equivalent to f(ranges::begin(range), ranges::end(range), args...)
/// @endcode
///
/// This is useful for adapting iterator-based algorithms such as @c reduce,
/// @c transform_reduce, and similar function objects into APIs that also accept
/// ranges directly.
///
/// @attention This adaptor should **only** be used in situations where the
/// result cannot dangle. Substituting @c ranges::begin(range) or @c
/// ranges::end(range) for the first two iterator arguments is unsafe when @c F
/// returns an iterator or any other object that refers into the supplied range,
/// because @c range1 may be a temporary that is destroyed at the end of the
/// full expression. Algorithms such as @c reduce and @c transform_reduce, which
/// return values computed from the range, are safe; algorithms such as @c find
/// or @c min_element, which return iterators into the range, are not.
///
/// @note Arguments after the first range argument are deduced **by forwarding
/// reference** and then forwarded into the underlying iterator-based call. This
/// can avoid an additional move relative by deducing the argument by **value**
/// and moving it to the underlying iterator-based function call.
///
/// @note In C++14, the indirection of the range-based overload may cause issues
/// with non-movable types.
template <class F>
class synthesize_range1_overload_t : F {
 public:
  // NOTE: this is currently intended to be used by algorithms in 'arene::base',
  // which are all (currently) default constructible by design.
  static_assert(std::is_default_constructible<F>::value, "'F' must be default constructible");

  // expose iterator-based overloads from F
  using F::operator();

  /// @brief invoke the underlying algorithm with a range
  /// @tparam Range1 range type used to supply the first iterator pair
  /// @tparam Args types of the remaining algorithm arguments
  ///
  /// @param range1 range whose @c begin() and @c end() are passed as the first
  ///   two arguments to @c F
  /// @param args remaining algorithm arguments; moved into the underlying call
  /// @return result of the associated overload of @c F
  ///
  /// Given a call of the form:
  ///
  /// @code
  /// adapted(range1, args...)
  /// @endcode
  ///
  /// this overload dispatches to:
  ///
  /// @code
  /// F::operator()(begin(range1), end(range1), FWD(args)...)
  /// @endcode
  // parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a "False positive: There is no move constructor here"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a "False positive: Arguments are forwarding references, not rvalue
  // references"
  //
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a "'range1' is intentionally treated
  // as an lvalue within the algorithm. Forwarding the range would propagate
  // rvalueness into intermediate operations and may prevent valid begin/end
  // usage. Standard ranges algorithms treat the range as a stable entity rather
  // than forwarding it through each use. Handling of potential dangling is
  // delegated to the entry point, if applicable."
  template <  //
      class Range1,
      class... Args,
      constraints<std::enable_if_t<is_invocable_v<
          F const&,
          decltype(arene::base::ranges::begin(std::declval<Range1&>())),
          decltype(arene::base::ranges::end(std::declval<Range1&>())),
          Args&&...>>> = nullptr>
  constexpr auto operator()(Range1&& range1, Args&&... args) const noexcept(  //
      noexcept(std::declval<F const&>()(                                      //
          arene::base::ranges::begin(std::declval<Range1&>()),
          arene::base::ranges::end(std::declval<Range1&>()),
          std::declval<Args&&>()...
      ))
  )
      -> decltype(std::declval<F const&>()(
          arene::base::ranges::begin(std::declval<Range1&>()),
          arene::base::ranges::end(std::declval<Range1&>()),
          std::declval<Args&&>()...
      )) {
    return F::operator()(  //
        arene::base::ranges::begin(range1),
        arene::base::ranges::end(range1),
        std::forward<Args>(args)...
    );
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a
  // parasoft-end-suppress AUTOSAR-A8_4_5-a
  // parasoft-end-suppress AUTOSAR-A12_8_4-a
  // parasoft-end-suppress CERT_C-EXP37-a
  // parasoft-end-suppress AUTOSAR-M2_10_1-a
};

}  // namespace algorithm_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_SYNTHESIZE_RANGE1_OVERLOAD_HPP_
// parasoft-end-suppress AUTOSAR-A2_8_1-a
