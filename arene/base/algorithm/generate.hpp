// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::generate"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_GENERATE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_GENERATE_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/detail/synthesize_range1_overload.hpp"  // IWYU pragma: export
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/iterator/iter_reference_t.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/type_traits/denotes_range.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace generate_detail {

/// @brief function object implementing 'generate'
class generate_fn {
 public:
  /// @brief Assigns each element in a sequence the result of a successive invocation of a generator.
  ///
  /// @tparam Itr The type of the iterator for the sequence. It must be a forward iterator whose @c reference is
  /// assignable from the result of invoking @c Generator
  /// @tparam Sent The type of the sentinel to mark the end of the sequence. It must be inequality comparable with
  /// @c Itr
  /// @tparam Generator The type of the generator. It must be invocable with no arguments.
  /// @param first Iterator to the first position in the sequence to generate into.
  /// @param last Sentinel to mark the end of the sequence to generate into.
  /// @param gen The generator invoked once per element of the sequence.
  /// @post All elements in the sequence @c [first,last) will have been assigned the result of invoking @c gen
  /// @pre Repeated incrementing of @c first must eventually produce an iterator which compares equal to @c last ,
  /// else behaviour is undefined.
  ///
  /// Performs exactly @c std::distance(first,last) invocations of @c gen and the same number of assignments.
  /// Backport of the @c constexpr [std::generate](https://en.cppreference.com/w/cpp/algorithm/generate) from C++20 .
  template <
      typename Itr,
      typename Sent,
      typename Generator,
      constraints<
          std::enable_if_t<is_forward_iterator_v<Itr>>,
          std::enable_if_t<denotes_range_v<Itr, Sent>>,
          std::enable_if_t<is_invocable_v<Generator&>>,
          std::enable_if_t<std::is_assignable<iter_reference_t<Itr>, invoke_result_t<Generator&>>::value>> = nullptr>
  constexpr auto operator()(Itr first, Sent last, Generator gen) const
      noexcept(std::is_nothrow_assignable<iter_reference_t<Itr>, invoke_result_t<Generator&>>::value && is_nothrow_invocable_v<Generator&> && denotes_nothrow_iterable_range_v<Itr, Sent>)
          -> void {
    // parasoft-begin-suppress AUTOSAR-A6_5_2-a "False Positive: The loop counter is 'first'"
    for (; first != last; ++first) {
      *first = gen();
    }
    // parasoft-end-suppress AUTOSAR-A6_5_2-a
  }
};

}  // namespace generate_detail

/// @brief Assigns each element in a sequence the result of a successive invocation of a generator.
///
/// An additional overload accepting a range in place of the iterator/sentinel pair is provided, which forwards to the
/// iterator-based overload with the @c begin and @c end of the range.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variable is initialized"
ARENE_CPP14_INLINE_VARIABLE(algorithm_detail::synthesize_range1_overload_t<generate_detail::generate_fn>, generate);
// parasoft-end-suppress CERT_CPP-DCL56-a
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_GENERATE_HPP_
// parasoft-end-suppress AUTOSAR-A2_8_1-a-2
