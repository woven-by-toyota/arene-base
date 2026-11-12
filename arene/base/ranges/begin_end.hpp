// parasoft-begin-suppress AUTOSAR-A5_0_3-a "False positive: There is no pointer indirection in return types"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_BEGIN_END_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_BEGIN_END_HPP_

// IWYU pragma: private, include "arene/base/ranges.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes/nodiscard.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/ranges/detail/is_input_or_output_iterator.hpp"
#include "arene/base/ranges/detail/is_sentinel_for.hpp"
#include "arene/base/ranges/enable_borrowed_range.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/extent.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_array.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_lvalue_reference.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_extent.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"  // IWYU pragma: keep (for std::get)
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/type_traits/priority_tag.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {
namespace ranges {

namespace begin_end_detail {

/// @brief The priority of ADL overload for @c begin and @c end .
using adl_priority_tag = priority_tag<0U>;
/// @brief The priority of member overload for @c begin and @c end .
using member_priority_tag = priority_tag<1U>;
/// @brief The priority of @c std::get overload for @c begin and @c end .
using std_array_priority_tag = priority_tag<2U>;
/// @brief The priority of size-zero tuple-like overload for @c begin and @c end .
using size_zero_tuple_like_priority_tag = priority_tag<3U>;
/// @brief The priority of C-array overload for @c begin and @c end .
using c_array_priority_tag = priority_tag<4U>;

/// @brief A helper that invokes the @c impl member function of a given policy, forwarding the range and starting ptag
template <typename Policy>
class accessor_fn {
 public:
  /// @brief Returns an iterator to the input range using the specified policy's implementation.
  ///
  /// @tparam Rng The range type. Must satisfy either @c std::is_lvalue_reference<Rng> or
  ///         @c enable_borrowed_range_v<Rng>.
  /// @param rng The range instance.
  /// @return An iterator to the beginning or end of the range, as determined by the specified policy's implementation.
  template <
      typename Rng,
      constraints<std::enable_if_t<
          std::is_lvalue_reference<Rng>::value || enable_borrowed_range_v<remove_cvref_t<Rng>>>> = nullptr>
  ARENE_NODISCARD constexpr auto operator()(Rng&& rng) const
      noexcept(noexcept(Policy::impl(std::forward<Rng>(rng), c_array_priority_tag{})))
          -> decltype(Policy::impl(std::forward<Rng>(rng), c_array_priority_tag{})) {
    return Policy::impl(std::forward<Rng>(rng), c_array_priority_tag{});
  }
};

/// @brief The implementation policy for @c begin .
/// @see arene::base::ranges::begin
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
class begin_policy {
 public:
  /// @brief Returns a pointer to the first element of a C-style array.
  ///
  /// @tparam Rng The array type.
  /// @param rng The array instance.
  /// @return std::remove_extent_t<Rng>* Pointer to the first element of the array.
  template <typename Rng, constraints<std::enable_if_t<std::is_array<std::remove_reference_t<Rng>>::value>> = nullptr>
  ARENE_NODISCARD static constexpr auto impl(Rng&& rng, c_array_priority_tag) noexcept
      -> std::remove_extent_t<std::remove_reference_t<Rng>>* {
    return &std::forward<Rng>(rng)[0U];
  }

  /// @brief Returns an iterator to the beginning of a tuple-like range with zero size.
  ///
  /// @tparam Rng The tuple-like range type.
  /// @tparam Itr The iterator type, deduced from the range's begin() member function.
  /// @return Itr An iterator to the beginning of the range, constructed from nullptr.
  /// @note This overload lets us work around the fact that @c std::array::begin isn't constexpr in C++14.
  template <
      typename Rng,
      typename Itr = decltype(std::declval<Rng&&>().begin()),
      constraints<
          std::enable_if_t<std::tuple_size<remove_cvref_t<Rng>>::value == 0>,
          std::enable_if_t<std::is_constructible<Itr, std::nullptr_t>::value>> = nullptr>
  ARENE_NODISCARD static constexpr auto impl(Rng&&, size_zero_tuple_like_priority_tag) noexcept -> Itr {
    return Itr{nullptr};
  }

  /// @brief Returns an iterator to the beginning of a tuple-like range with non-zero size.
  ///
  /// @tparam Rng The tuple-like range type. @c BeginItr and @c GetItr must be the same type.
  /// @tparam BeginItr The iterator type, deduced from the range's begin() member function.
  /// @tparam GetItr The iterator type, deduced from the address of the first element of the range.
  /// @param rng The range instance.
  /// @return GetItr An iterator to the beginning of the range, obtained from the address of the first element.
  /// @note This overload lets us work around the fact that @c std::array::begin isn't constexpr in C++14.
  template <
      typename Rng,
      typename BeginItr = decltype(std::declval<Rng&&>().begin()),
      typename GetItr = decltype(&std::get<0>(std::declval<Rng&&>())),
      constraints<
          std::enable_if_t<std::tuple_size<remove_cvref_t<Rng>>::value != 0>,
          std::enable_if_t<std::is_same<BeginItr, GetItr>::value>> = nullptr>
  ARENE_NODISCARD static constexpr auto impl(Rng&& rng, std_array_priority_tag) noexcept -> GetItr {
    return &std::get<0>(std::forward<Rng>(rng));
  }

  /// @brief Returns an iterator to the beginning of a range using its member begin() member function.
  ///
  /// @tparam Rng The range type. Must have a member begin() member function.
  /// @tparam Itr The type of the iterator to the beginning of the range, deduced from @c rng.begin() .
  /// @param rng The range instance.
  /// @return Itr An iterator to the beginning of the range, obtained from the member begin() member function.
  template <
      typename Rng,
      typename Itr = decltype(std::forward<Rng>(std::declval<Rng&&>()).begin()),
      constraints<std::enable_if_t<detail::is_input_or_output_iterator_v<Itr>>> = nullptr>
  ARENE_NODISCARD static constexpr auto impl(Rng&& rng, member_priority_tag) noexcept(
      noexcept(std::forward<Rng>(std::declval<Rng&&>()).begin())
  ) -> Itr {
    return std::forward<Rng>(rng).begin();
  }

  /// @brief Returns an iterator to the beginning of a range using argument-dependent lookup (ADL)
  ///
  /// @tparam Rng The range type. @c begin(rng) must be a valid expression.
  /// @tparam Itr The type of the iterator to the beginning of the range, deduced from @c begin(rng) .
  /// @param rng The range instance.
  /// @return Itr An iterator to the beginning of the range, obtained via an unqualified invocation of @c begin(rng) .
  template <
      typename Rng,
      typename Itr = decltype(begin(std::forward<Rng>(std::declval<Rng&&>()))),
      constraints<std::enable_if_t<detail::is_input_or_output_iterator_v<Itr>>> = nullptr>
  ARENE_NODISCARD static constexpr auto impl(Rng&& rng, adl_priority_tag) noexcept(
      noexcept(begin(std::forward<Rng>(std::declval<Rng&&>())))
  ) -> Itr {
    return begin(std::forward<Rng>(rng));
  }
};

}  // namespace begin_end_detail

/// @brief returns an iterator to the beginning of a range
/// @tparam Rng The type of the range to get the beginning of. Must satisfy either @c std::is_lvalue_reference<Rng> or
///         @c enable_borrowed_range_v<Rng>.
/// The range to inspect is passed as @c rng.
/// @return An iterator to the beginning of the range
/// This operation is expression-equivalent to one of the following operations, in order of preference:
/// 1. If @c Rng satisfies @c std::is_array<Rng> , returns @c &rng[0] .
/// 2. If @c rng.begin() is well-formed, returns @c rng.begin() .
/// 3. Otherwise, if @c begin(rng) is well-formed, returns @c begin(rng) .
///
/// If none of the requirements are satisfied, the function does not participate in overload resolution.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: the referenced constexpr variable is initialized"
ARENE_CPP14_INLINE_VARIABLE(begin_end_detail::accessor_fn<begin_end_detail::begin_policy>, begin);
// parasoft-end-suppress CERT_CPP-DCL56-a
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

/// @brief The type of the iterator for a given range.
/// @tparam Rng The type of the range to extract the iterator for.
template <typename Rng>
using iterator_t = decltype(::arene::base::ranges::begin(std::declval<Rng&>()));

namespace begin_end_detail {

/// @brief The implementation policy for @c end .
/// @see arene::base::ranges::end
class end_policy {
 public:
  /// @brief Returns a pointer to one-past-the-last element of a C-style array.
  ///
  /// @tparam Rng The array type.
  /// @param rng The array instance.
  /// @return std::remove_extent_t<Rng>* Pointer to one-past-the-last element of the array.
  template <typename Rng, constraints<std::enable_if_t<std::is_array<std::remove_reference_t<Rng>>::value>> = nullptr>
  ARENE_NODISCARD static constexpr auto impl(Rng&& rng, c_array_priority_tag) noexcept
      -> std::remove_extent_t<std::remove_reference_t<Rng>>* {
    return &std::forward<Rng>(rng)[std::extent<std::remove_reference_t<Rng>>::value];
  }

  /// @brief Returns an iterator to the end of a tuple-like range with zero size.
  ///
  /// @tparam Rng The tuple-like range type.
  /// @tparam Itr The iterator type, deduced from the range's end() member function.
  /// @return Itr An iterator to the end of the range, constructed from nullptr.
  /// @note This overload lets us work around the fact that @c std::array::end isn't constexpr in C++14.
  /// constexpr.
  template <
      typename Rng,
      typename Itr = decltype(std::declval<Rng&&>().end()),
      constraints<
          std::enable_if_t<std::tuple_size<remove_cvref_t<Rng>>::value == 0>,
          std::enable_if_t<std::is_constructible<Itr, std::nullptr_t>::value>> = nullptr>
  ARENE_NODISCARD static constexpr auto impl(Rng&&, size_zero_tuple_like_priority_tag) noexcept -> Itr {
    return Itr{nullptr};
  }

  /// @brief Returns an iterator to the end of a tuple-like range with non-zero size.
  ///
  /// @tparam Rng The tuple-like range type. @c BeginItr and @c GetItr must be the same type.
  /// @tparam BeginItr The iterator type, deduced from the range's end() member function.
  /// @tparam GetItr The iterator type, deduced from the address of the first element of the range.
  /// @param rng The range instance.
  /// @return GetItr An iterator to the end of the range, obtained from the address of the first element plus the size
  /// of the range.
  /// @note This overload lets us work around the fact that @c std::array::end isn't constexpr in C++14.
  template <
      typename Rng,
      typename BeginItr = decltype(std::declval<Rng&&>().end()),
      typename GetItr = decltype(&std::get<0>(std::declval<Rng&&>())),
      constraints<
          std::enable_if_t<std::tuple_size<remove_cvref_t<Rng>>::value != 0>,
          std::enable_if_t<std::is_same<BeginItr, GetItr>::value>> = nullptr>
  ARENE_NODISCARD static constexpr auto impl(Rng&& rng, std_array_priority_tag) noexcept -> BeginItr {
    // parasoft-begin-suppress AUTOSAR-M5_0_15-a "False positive: the pointer points to the first element of a
    // contiguous tuple-like range"
    return &std::get<0>(std::forward<Rng>(rng)) + std::tuple_size<remove_cvref_t<Rng>>::value;
    // parasoft-end-suppress AUTOSAR-M5_0_15-a
  }

  /// @brief Returns an iterator to the end of a range using its member end() member function.
  ///
  /// @tparam Rng The range type. Must have a member end() member function.
  /// @tparam Sentinel The type of the sentinel (end iterator) for the range, deduced from @c rng.end() .
  /// @param rng The range instance.
  /// @return Sentinel The sentinel to the end of the range, obtained from the member end() member function.
  template <
      typename Rng,
      typename Sentinel = decltype(std::forward<Rng>(std::declval<Rng&&>()).end()),
      constraints<std::enable_if_t<detail::is_sentinel_for_v<Sentinel, iterator_t<Rng>>>> = nullptr>
  ARENE_NODISCARD static constexpr auto impl(Rng&& rng, member_priority_tag) noexcept(
      noexcept(std::forward<Rng>(std::declval<Rng&&>()).end())
  ) -> Sentinel {
    return std::forward<Rng>(rng).end();
  }

  /// @brief Returns an iterator to the end of a range using argument-dependent lookup (ADL)
  ///
  /// @tparam Rng The range type. @c end(rng) must be a valid expression.
  /// @tparam Sentinel The type of the sentinel (end iterator) for the range, deduced from @c end(rng).
  /// @param rng The range instance.
  /// @return Sentinel The sentinel to the end of the range, obtained via an unqualified invocation of @c end(rng) .
  template <
      typename Rng,
      typename Sentinel = decltype(end(std::forward<Rng>(std::declval<Rng&&>()))),
      constraints<std::enable_if_t<detail::is_sentinel_for_v<Sentinel, iterator_t<Rng>>>> = nullptr>
  ARENE_NODISCARD static constexpr auto impl(Rng&& rng, adl_priority_tag) noexcept(
      noexcept(end(std::forward<Rng>(std::declval<Rng&&>())))
  ) -> Sentinel {
    return end(std::forward<Rng>(rng));
  }
};

// parasoft-end-suppress CERT_C-EXP37-a

}  // namespace begin_end_detail

/// @brief returns an iterator to the end of a range
/// @tparam Rng The type of the range to get the end of. Must satisfy either @c std::is_lvalue_reference<Rng> or
///         @c enable_borrowed_range_v<Rng>.
/// The range to inspect is passed as @c rng.
/// @return An iterator to the end of the range
/// This operation is expression-equivalent to one of the following operations, in order of preference:
/// 1. If @c Rng satisfies @c std::is_array<Rng> , returns @c &rng[std::extent<Rng>::value] .
/// 2. If @c rng.end() is well-formed, returns @c rng.end() .
/// 3. Otherwise, if @c end(rng) is well-formed, returns @c end(rng) .
///
/// If none of the requirements are satisfied, the function does not participate in overload resolution.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: the referenced constexpr variable is initialized"
ARENE_CPP14_INLINE_VARIABLE(begin_end_detail::accessor_fn<begin_end_detail::end_policy>, end);
// parasoft-end-suppress CERT_CPP-DCL56-a
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

/// @brief The type of the sentinel (end iterator) for a given range.
/// @tparam Rng The type of the range to extract the sentinel for.
template <typename Rng>
using sentinel_t = decltype(::arene::base::ranges::end(std::declval<Rng&>()));

}  // namespace ranges
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_BEGIN_END_HPP_
