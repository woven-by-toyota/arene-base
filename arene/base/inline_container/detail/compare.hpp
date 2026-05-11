// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_COMPARE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_COMPARE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/inline_container/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/type_manipulation/ebo_holder.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace inline_container {
namespace detail {

/// @brief tag struct for @c compare_wrapper EBO access.
class compare_storage_tag {};

/// @brief A helper class which converts any comparison operator into a three way comparison operator.
///
/// @tparam Compare The type of the comparator to wrap. Must satisfy one of <c>is_invocable_r_v<strong_ordering, Compare
/// const&, Key const&, Key const&></c> or <c>is_invocable_r_v<bool, Compare const&, Key const&, Key const&></c>
/// @tparam Key The key type to use as the left hand operand of the comparisons, and the presumed right hand operand.
/// @note As this type is intended as a detail helper, it exposes a transparent comparison interface but does not itself
/// handle declaring it is a transparent comparator. It is up to the user to deduce this information on their own.
template <typename Compare, typename Key>
class compare_wrapper : ebo_holder<compare_storage_tag, Compare> {
  /// @brief Is the comparator a three-way comparison?
  /// @tparam OtherKey The type of the right hand operand in the comparison invocation.
  /// @return bool equivalent to <c>is_invocable_r_v<strong_ordering, Compare const&, Key const&, OtherKey const&></c>
  template <typename OtherKey>
  static constexpr bool
      is_three_way_comparison_with{is_invocable_r_v<strong_ordering, Compare const&, Key const&, OtherKey const&>};

  /// @brief Is the comparator a binary comparison?
  /// @tparam OtherKey The type of the right hand operand in the comparison invocation.
  /// @return bool equivalent to <c>is_invocable_r_v<bool, Compare const&, Key const&, OtherKey const&></c>
  template <typename OtherKey>
  static constexpr bool is_binary_comparison_with{is_invocable_r_v<bool, Compare const&, Key const&, OtherKey const&>};

  static_assert(
      is_three_way_comparison_with<Key> || is_binary_comparison_with<Key>,
      "Comparison result must be strong_ordering or bool"
  );

 public:
  /// @brief alias for base class.
  using ebo_holder<compare_storage_tag, Compare>::ebo_holder;

  /// @brief Can comparisons throw?
  /// @tparam OtherKey The type of the right hand operand in the comparison invocation.
  /// @return bool Equivalent to <c>noexcept(std::declval<Compare const&>()(std::declval<Key const&>(),
  /// std::declval<OtherKey const&>()))</c>
  template <typename OtherKey = Key>
  static constexpr bool comparison_is_noexcept{
      noexcept(std::declval<Compare const&>()(std::declval<Key const&>(), std::declval<OtherKey const&>()))
  };

  // parasoft-begin-suppress CERT_CPP-ERR55-a-2 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress CERT_CPP-ERR50-h-3 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-A15_5_3-h-2 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress CERT_CPP-ERR51-b-3 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-A15_3_2-a-2 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-A15_5_3-g-2 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress CERT_CPP-ERR50-g-3 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-M15_3_4-b-2 "False positive: Exception is caught"
  // parasoft-begin-suppress AUTOSAR-A15_2_1-b-2 "False positive: Throwing constructor not invoked"
  /// @brief Check the relative ordering of two keys, using the comparison function as a three-way-comparison.
  ///
  /// @tparam OtherKey The type of the right hand operand to the comparison.
  /// @param lhs The first key value to compare
  /// @param rhs The second key value to compare
  /// @return strong_ordering Equivalent to invoking an instance of the underlying comparator with the input arguments.
  template <typename OtherKey, constraints<std::enable_if_t<is_three_way_comparison_with<OtherKey>>> = nullptr>
  constexpr auto key_ordering(Key const& lhs, OtherKey const& rhs) const noexcept(comparison_is_noexcept<OtherKey>)
      -> strong_ordering {
    return this->get_comparator()(lhs, rhs);
  }
  // parasoft-end-suppress AUTOSAR-A15_2_1-b-2
  // parasoft-end-suppress AUTOSAR-M15_3_4-b-2
  // parasoft-end-suppress CERT_CPP-ERR50-g-3
  // parasoft-end-suppress AUTOSAR-A15_5_3-g-2
  // parasoft-end-suppress AUTOSAR-A15_3_2-a-2
  // parasoft-end-suppress CERT_CPP-ERR51-b-3
  // parasoft-end-suppress AUTOSAR-A15_5_3-h-2
  // parasoft-end-suppress CERT_CPP-ERR55-a-2
  // parasoft-end-suppress CERT_CPP-ERR50-h-3

  /// @brief Check the relative ordering of two keys, using the comparison function as a binary ordering comparison
  ///        equivalent to "less-than".
  ///
  /// @tparam OtherKey The type of the right hand operand to the comparison.
  /// @param lhs The first key value to compare
  /// @param rhs The second key value to compare
  /// @return strong_ordering::less if @c lhs is before @c rhs
  /// @return strong_ordering::equal if @c lhs and @c rhs are equivalent
  /// @return strong_ordering::greater if @c lhs is after @c rhs
  template <typename OtherKey, constraints<std::enable_if_t<is_binary_comparison_with<OtherKey>>> = nullptr>
  constexpr auto key_ordering(Key const& lhs, OtherKey const& rhs) const noexcept(comparison_is_noexcept<OtherKey>)
      -> strong_ordering {
    auto const& comp = get_comparator();
    if (comp(lhs, rhs)) {
      return strong_ordering::less;
    }
    // NOLINTNEXTLINE(readability-suspicious-call-argument) this is intentional, reversed-argument less-than is greater.
    if (comp(rhs, lhs)) {
      return strong_ordering::greater;
    }
    return strong_ordering::equal;
  }

 protected:
  /// @brief Get the underlying stored comparator
  /// @return A reference to the stored comparator
  constexpr auto get_comparator() const noexcept -> Compare const& { return this->get_value(compare_storage_tag{}); }
};

}  // namespace detail
}  // namespace inline_container
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_COMPARE_HPP_
