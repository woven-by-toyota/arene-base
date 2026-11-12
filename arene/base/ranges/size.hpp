// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::ranges::size"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_SIZE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_SIZE_HPP_

// IWYU pragma: private, include "arene/base/ranges.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes/nodiscard.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/iterator/iter_difference_t.hpp"
#include "arene/base/ranges/begin_end.hpp"
#include "arene/base/ranges/detail/is_sized_sentinel_for.hpp"
#include "arene/base/ranges/disable_sized_range.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/priority_tag.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace ranges {

namespace size_detail {

/// @brief Poison-pill declaration so that the unqualified call to @c size below performs argument-dependent lookup
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
template <class>
auto size() -> void = delete;
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

/// @brief Alias to the result of @c t.size() , well-formed only when that member call is valid.
/// @tparam Range the type of the range
template <typename Range>
using member_size_t = decltype(std::declval<Range>().size());

/// @brief Alias to the result of the ADL call @c size(t) , well-formed only when that call is valid.
/// @tparam Range the type of the range
template <typename Range>
using adl_size_t = decltype(size(std::declval<Range>()));

/// @brief @c true when @c Range opts out of a constant-time @c size() via @c disable_sized_range .
/// @tparam Range the type of the range
template <typename Range>
extern constexpr bool disabled_v = disable_sized_range_v<remove_cvref_t<Range>>;

/// @brief @c true when @c T models the integer-like requirement
/// @tparam T the candidate result type of a @c size() call
///
/// @note Implementing the integer-like requirement is extremely difficult in C++14, so the stricter @c std::is_integral
/// trait is used instead.
template <typename T>
extern constexpr bool is_integer_like_v = std::is_integral<remove_cvref_t<T>>::value;

/// @brief @c true when the member @c size() call is valid
/// @tparam Range the type of the range
template <typename Range>
extern constexpr bool can_use_member_size_v = substitution_succeeds<member_size_t, Range>;

/// @brief @c true when the ADL @c size() call is valid
/// @tparam Range the type of the range
template <typename Range>
extern constexpr bool can_use_adl_size_v = substitution_succeeds<adl_size_t, Range>;

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
/// @brief Function object implementing @c arene::base::ranges::size .
class size_fn {
 private:
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
  /// @brief Returns the size of a range that provides a @c size() member function which returns an @c integer-like
  /// @tparam Range the type of the range
  /// @param range the range argument
  /// @return a decay-copy of @c range.size()
  template <
      typename Range,
      constraints<std::enable_if_t<
          can_use_member_size_v<Range> && is_integer_like_v<member_size_t<Range>> && !disabled_v<Range>>> = nullptr>
  static constexpr auto impl(Range&& range, priority_tag<2>) noexcept(
      noexcept(remove_cvref_t<member_size_t<Range>>(std::declval<Range>().size()))
  ) -> remove_cvref_t<member_size_t<Range>> {
    return std::forward<Range>(range).size();
  }

  /// @brief Returns the size of a range that provides an ADL-findable @c size() free function which returns an @c
  /// integer-like
  /// @tparam Range the type of the range
  /// @param range the range argument
  /// @return a decay-copy of the ADL call @c size(range)
  template <
      typename Range,
      constraints<std::enable_if_t<
          can_use_adl_size_v<Range> && is_integer_like_v<adl_size_t<Range>> && !disabled_v<Range>>> = nullptr>
  static constexpr auto impl(Range&& range, priority_tag<1>) noexcept(
      noexcept(remove_cvref_t<adl_size_t<Range>>(size(std::declval<Range>())))
  ) -> remove_cvref_t<adl_size_t<Range>> {
    return size(std::forward<Range>(range));
  }

  /// @brief Returns the size of a range computed as @c end(range) - begin(range) .
  /// @tparam Range the type of the range
  /// @param range the range argument
  /// @return @c end(range) - begin(range) , cast to the unsigned version of the iterator's difference type
  // Unlike @c std::ranges::size , no dedicated overload for C arrays is provided: this random-access
  // <c> end - begin </c> path already gives the extent of a C array of known bound. A dedicated array overload could be
  // added if a future need arises.
  //
  // This gate deliberately requires only @c is_sized_sentinel_for_v and omits the @c forward_iterator<I> requirement
  // that @c std::ranges::size also imposes, so it accepts sized input-iterator ranges the standard would reject. There
  // is no ITER_CONCEPT-based @c forward_iterator trait yet; one can be added and required here when broader
  // @c std::ranges backporting or proxy-returning views make that distinction matter.
  template <
      typename Range,
      constraints<std::enable_if_t<detail::is_sized_sentinel_for_v<sentinel_t<Range>, iterator_t<Range>>>> = nullptr>
  static constexpr auto impl(Range&& range, priority_tag<0>) noexcept(
      noexcept(arene::base::ranges::end(std::declval<Range>()) - arene::base::ranges::begin(std::declval<Range>()))
  ) -> std::make_unsigned_t<iter_difference_t<iterator_t<Range>>> {
    // parasoft-begin-suppress AUTOSAR-M5_0_15-a "False positive: first is an iterator, bounded by last"
    return static_cast<std::make_unsigned_t<iter_difference_t<iterator_t<Range>>>>(
        arene::base::ranges::end(std::forward<Range>(range)) - arene::base::ranges::begin(std::forward<Range>(range))
    );
    // parasoft-end-suppress AUTOSAR-M5_0_15-a
    // parasoft-end-suppress CERT_C-EXP37-a
  }

 public:
  /// @brief Returns the size of a range
  /// @tparam Range the type of the range
  /// @param range the range argument
  /// @return the size of the range
  template <typename Range>
  ARENE_NODISCARD constexpr auto operator()(Range&& range) const
      noexcept(noexcept(impl(std::forward<Range>(range), priority_tag<2>{})))
          -> decltype(impl(std::forward<Range>(range), priority_tag<2>{})) {
    return impl(std::forward<Range>(range), priority_tag<2>{});
  }
};
// parasoft-end-suppress AUTOSAR-M2_10_1-a

}  // namespace size_detail

/// @brief Returns an integer equal to the size of a range.
///
/// @c size is intended to be a constant time operation. Callers of this function should generally assume this runs in
/// constant time and is ill-formed for types which don't have a constant-time size, but we can not enforce this for
/// range types that we don't control. Use @c arene::base::distance if you want to disclaim this constant time
/// guarantee.
///
/// To container implementers: if your container has member or ADL size functions then @c size will
/// pick them up and assume that they can operate in constant time. If your type's size is not constant time, specialize
/// @c arene::base::ranges::disable_sized_range for it so that @c size ignores those functions.
///
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(size_detail::size_fn, size);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace ranges
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_SIZE_HPP_
