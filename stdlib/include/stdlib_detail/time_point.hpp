// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TIME_POINT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TIME_POINT_HPP_

// IWYU pragma: private, include <chrono>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

#include "arene/base/constraints.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
#include "stdlib/include/stdlib_detail/common_type.hpp"
#include "stdlib/include/stdlib_detail/duration.hpp"
#include "stdlib/include/stdlib_detail/is_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_copy_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_copy_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_move_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_move_constructible.hpp"

namespace std {
namespace chrono {

// parasoft-begin-suppress AUTOSAR-A12_1_5-a "False positive: delegating constructors would not reduce duplication"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A14_5_1-a "Template constructor is sufficiently constrained to not hide default ones"

/// @brief A class representing a single point in time for the given clock
/// @tparam Clock The clock on which this is a time point
/// @tparam Duration The type representing time since the clock's epoch; must be an instance of @c std::chrono::duration
template <class Clock, class Duration = typename Clock::duration>
class time_point {
  // The standard requires that Clock be a clock in the sense of [time.clock], but none of the major implementations
  // actually enforce this requirement, and it was removed in C++23, so we will not enforce it either for compatibility.
  static_assert(
      ::arene::base::is_instantiation_of_v<Duration, duration>,
      "The Duration of a std::time_point must be an instance of std::chrono::duration"
  );

 public:
  /// @brief The clock on which this is a time point
  // parasoft-begin-suppress CERT_CPP-DCL51-f "This name is required by the C++14 standard"
  using clock = Clock;
  // parasoft-end-suppress CERT_CPP-DCL51-f
  /// @brief A @c std::chrono::duration specialization to be used to represent the time since the clock's epoch
  using duration = Duration;
  /// @brief The underlying arithmetic type used to represent the time since the clock's epoch
  using rep = typename duration::rep;
  /// @brief The period (length of one tick) used by <c>duration</c>, relative to 1 second
  using period = typename duration::period;

 private:
  /// @brief The time since the clock's epoch represented by a particular @c time_point instance
  duration time_since_epoch_;

 public:
  /// @brief Default-construct a @c time_point referring to <c>clock</c>'s epoch
  constexpr time_point() noexcept(noexcept(duration(duration::zero())))
      : time_since_epoch_{duration::zero()} {}

  /// @brief Construct a @c time_point referring to the point a certain length of time after <c>clock</c>'s epoch
  /// @param epoch_time The amount of time after <c>clock</c>'s epoch which the new @c time_point will represent
  constexpr explicit time_point(duration const& epoch_time) noexcept(is_nothrow_copy_constructible_v<duration>)
      : time_since_epoch_{epoch_time} {}

  /// @brief Implicitly convert a @c time_point for the same clock, potentially even if it has a different @c duration
  /// @tparam Duration2 The duration representation for the other time point; must be convertible to @c duration
  /// @param other The time point to convert
  template <class Duration2, arene::base::constraints<enable_if_t<is_convertible_v<Duration2, duration>>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions) C++14 specifies this constructor as implicit
  constexpr time_point(time_point<clock, Duration2> const& other
  ) noexcept(is_nothrow_constructible_v<duration, Duration2>)
      : time_since_epoch_{other.time_since_epoch()} {}

  /// @brief Return the time of a particular @c time_point as the duration since its clock's epoch
  /// @return The time between this @c time_point and <c>clock</c>'s epoch
  constexpr auto time_since_epoch() const noexcept(is_nothrow_copy_constructible_v<duration>) -> duration {
    return time_since_epoch_;
  }

  /// @brief Move this @c time_point forward by a certain duration
  /// @param dur The duration to move forward by
  /// @note May overflow if @c duration::operator+= can overflow
  auto operator+=(duration const& dur) noexcept(noexcept(time_since_epoch_ += dur)) -> time_point& {
    time_since_epoch_ += dur;
    return *this;
  }

  /// @brief Move this @c time_point backward by a certain duration
  /// @param dur The duration to move backward by
  /// @note May underflow if @c duration::operator-= can underflow
  auto operator-=(duration const& dur) noexcept(noexcept(time_since_epoch_ -= dur)) -> time_point& {
    time_since_epoch_ -= dur;
    return *this;
  }

  /// @brief Get the minimum @c time_point representable by this class
  /// @return The @c time_point corresponding to the earliest representable time
  static constexpr auto min() noexcept(noexcept(time_point{duration::min()})) -> time_point {
    return time_point{duration::min()};
  }
  /// @brief Get the maximum @c time_point representable by this class
  /// @return The @c time_point corresponding to the latest representable time
  static constexpr auto max() noexcept(noexcept(time_point{duration::max()})) -> time_point {
    return time_point{duration::max()};
  }
};

// parasoft-end-suppress AUTOSAR-A14_5_1-a
// parasoft-end-suppress AUTOSAR-M2_10_1-a
// parasoft-end-suppress AUTOSAR-A12_1_5-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: these functions have external linkage"

/// @brief Add a @c time_point to a @c duration when the two are compatible
/// @tparam Clock The clock of the <c>time_point</c>, deduced from @c left
/// @tparam Duration1 The @c duration type of the <c>time_point</c>, deduced from @c left
/// @tparam Rep2 The arithmetic representation of the <c>duration</c>, deduced from @c right
/// @tparam Period2 The tick period of the <c>duration</c>, deduced from @c right
/// @param left The @c time_point to be added
/// @param right The @c duration to be added
/// @return A time point representing @c left shifted by @c right
// parasoft-begin-suppress AUTOSAR-M5_17_1-a "This behavior is mandated explicitly by the C++14 standard"
template <class Clock, class Duration1, class Rep2, class Period2>
constexpr auto operator+(time_point<Clock, Duration1> const& left, duration<Rep2, Period2> const& right) noexcept(
    noexcept(time_point<Clock, common_type_t<Duration1, duration<Rep2, Period2>>>(left.time_since_epoch() + right))
) -> time_point<Clock, common_type_t<Duration1, duration<Rep2, Period2>>> {
  using return_type = time_point<Clock, common_type_t<Duration1, duration<Rep2, Period2>>>;
  return return_type(left.time_since_epoch() + right);
}
// parasoft-end-suppress AUTOSAR-M5_17_1-a

/// @brief Add a @c time_point to a @c duration when the two are compatible
/// @tparam Rep1 The arithmetic representation of the <c>duration</c>, deduced from @c left
/// @tparam Period1 The tick period of the <c>duration</c>, deduced from @c left
/// @tparam Clock The clock of the <c>time_point</c>, deduced from @c right
/// @tparam Duration2 The @c duration type of the <c>time_point</c>, deduced from @c right
/// @param left The @c duration to be added
/// @param right The @c time_point to be added
/// @return A time point representing @c right shifted by @c left
template <class Rep1, class Period1, class Clock, class Duration2>
constexpr auto operator+(duration<Rep1, Period1> const& left, time_point<Clock, Duration2> const& right) noexcept(
    noexcept(right + left)
) -> time_point<Clock, common_type_t<duration<Rep1, Period1>, Duration2>> {
  return right + left;
}

/// @brief Subtract a @c duration from a @c time_point when the two are compatible
/// @tparam Clock The clock of the <c>time_point</c>, deduced from @c left
/// @tparam Duration1 The @c duration type of the <c>time_point</c>, deduced from @c left
/// @tparam Rep2 The arithmetic representation of the <c>duration</c>, deduced from @c right
/// @tparam Period2 The tick period of the <c>duration</c>, deduced from @c right
/// @param left The @c time_point to be subtracted from
/// @param right The @c duration to be subtracted
/// @return A time point representing @c left shifted by the inverse of @c right
// parasoft-begin-suppress AUTOSAR-M5_17_1-a "This behavior is mandated explicitly by the C++14 standard"
template <class Clock, class Duration1, class Rep2, class Period2>
constexpr auto operator-(time_point<Clock, Duration1> const& left, duration<Rep2, Period2> const& right) noexcept(
    noexcept(left + (-right))
) -> time_point<Clock, common_type_t<Duration1, duration<Rep2, Period2>>> {
  return left + (-right);
}
// parasoft-end-suppress AUTOSAR-M5_17_1-a

/// @brief Get the @c duration between two <c>time_point</c>s when the two are compatible
/// @tparam Clock The clock shared by both <c>time_point</c>s, deduced from @c left and @c right
/// @tparam Duration1 The @c duration type of the first <c>time_point</c>, deduced from @c left
/// @tparam Duration2 The @c duration type of the second <c>time_point</c>, deduced from @c right
/// @param left The first <c>time_point</c>, where the return value is "duration from here"
/// @param right The second <c>time_point</c>, where the return value is "duration to here"
/// @return A @c duration representing the time from @c left to @c right
template <class Clock, class Duration1, class Duration2>
constexpr auto operator-(time_point<Clock, Duration1> const& left, time_point<Clock, Duration2> const& right) noexcept(
    noexcept(common_type_t<Duration1, Duration2>(left.time_since_epoch() - right.time_since_epoch()))
) -> common_type_t<Duration1, Duration2> {
  return left.time_since_epoch() - right.time_since_epoch();
}

// parasoft-begin-suppress AUTOSAR-A13_5_5-b "Comparisons are conditionally noexcept based on underlying comparison"

/// @brief Compare two time points for equality if they're both from the same clock
/// @tparam Clock The clock shared by both <c>time_point</c>s, deduced from @c left and @c right
/// @tparam Duration1 The @c duration type of the first <c>time_point</c>, deduced from @c left
/// @tparam Duration2 The @c duration type of the second <c>time_point</c>, deduced from @c right
/// @param left The first <c>time_point</c>
/// @param right The second <c>time_point</c>
/// @return @c true if @c left and @c right refer to the same time point, otherwise @c false
template <class Clock, class Duration1, class Duration2>
constexpr auto operator==(time_point<Clock, Duration1> const& left, time_point<Clock, Duration2> const& right) noexcept(
    noexcept(left.time_since_epoch() == right.time_since_epoch())
) -> bool {
  return left.time_since_epoch() == right.time_since_epoch();
}

/// @brief Compare two time points for inequality if they're both from the same clock
/// @tparam Clock The clock shared by both <c>time_point</c>s, deduced from @c left and @c right
/// @tparam Duration1 The @c duration type of the first <c>time_point</c>, deduced from @c left
/// @tparam Duration2 The @c duration type of the second <c>time_point</c>, deduced from @c right
/// @param left The first <c>time_point</c>
/// @param right The second <c>time_point</c>
/// @return @c true if @c left and @c right refer to different time points, otherwise @c false
template <class Clock, class Duration1, class Duration2>
constexpr auto operator!=(time_point<Clock, Duration1> const& left, time_point<Clock, Duration2> const& right) noexcept(
    noexcept(left == right)
) -> bool {
  return !(left == right);
}

/// @brief Compare two time points for ordering if they're both from the same clock
/// @tparam Clock The clock shared by both <c>time_point</c>s, deduced from @c left and @c right
/// @tparam Duration1 The @c duration type of the first <c>time_point</c>, deduced from @c left
/// @tparam Duration2 The @c duration type of the second <c>time_point</c>, deduced from @c right
/// @param left The first <c>time_point</c>
/// @param right The second <c>time_point</c>
/// @return @c true if @c left refers to a time point earlier than <c>right</c>, otherwise @c false
template <class Clock, class Duration1, class Duration2>
constexpr auto operator<(time_point<Clock, Duration1> const& left, time_point<Clock, Duration2> const& right) noexcept(
    noexcept(left.time_since_epoch() < right.time_since_epoch())
) -> bool {
  return left.time_since_epoch() < right.time_since_epoch();
}

/// @brief Compare two time points for ordering if they're both from the same clock
/// @tparam Clock The clock shared by both <c>time_point</c>s, deduced from @c left and @c right
/// @tparam Duration1 The @c duration type of the first <c>time_point</c>, deduced from @c left
/// @tparam Duration2 The @c duration type of the second <c>time_point</c>, deduced from @c right
/// @param left The first <c>time_point</c>
/// @param right The second <c>time_point</c>
/// @return @c true if @c left refers to a time point no later than <c>right</c>, otherwise @c false
template <class Clock, class Duration1, class Duration2>
constexpr auto operator<=(time_point<Clock, Duration1> const& left, time_point<Clock, Duration2> const& right) noexcept(
    noexcept(right < left)
) -> bool {
  return !(right < left);
}

/// @brief Compare two time points for ordering if they're both from the same clock
/// @tparam Clock The clock shared by both <c>time_point</c>s, deduced from @c left and @c right
/// @tparam Duration1 The @c duration type of the first <c>time_point</c>, deduced from @c left
/// @tparam Duration2 The @c duration type of the second <c>time_point</c>, deduced from @c right
/// @param left The first <c>time_point</c>
/// @param right The second <c>time_point</c>
/// @return @c true if @c left refers to a time point later than <c>right</c>, otherwise @c false
template <class Clock, class Duration1, class Duration2>
constexpr auto operator>(time_point<Clock, Duration1> const& left, time_point<Clock, Duration2> const& right) noexcept(
    noexcept(right < left)
) -> bool {
  return right < left;
}

/// @brief Compare two time points for ordering if they're both from the same clock
/// @tparam Clock The clock shared by both <c>time_point</c>s, deduced from @c left and @c right
/// @tparam Duration1 The @c duration type of the first <c>time_point</c>, deduced from @c left
/// @tparam Duration2 The @c duration type of the second <c>time_point</c>, deduced from @c right
/// @param left The first <c>time_point</c>
/// @param right The second <c>time_point</c>
/// @return @c true if @c left refers to a time point no earlier than <c>right</c>, otherwise @c false
template <class Clock, class Duration1, class Duration2>
constexpr auto operator>=(time_point<Clock, Duration1> const& left, time_point<Clock, Duration2> const& right) noexcept(
    noexcept(left < right)
) -> bool {
  return !(left < right);
}

// parasoft-end-suppress AUTOSAR-A13_5_5-b

/// @brief Cast a @c time_point into another type with the same clock but a different duration type
/// @tparam ToDuration The @c duration type to be used after the cast
/// @tparam Clock The clock of the <c>time_point</c>, deduced from @c point
/// @tparam Duration The @c duration type of the existing <c>time_point</c>, deduced from @c point
/// @param point The time point to cast to the new duration @c ToDuration
/// @return A representation of the same time point represented by <c>point</c>, but using duration type @c ToDuration
template <
    class ToDuration,
    class Clock,
    class Duration,
    arene::base::constraints<enable_if_t<::arene::base::is_instantiation_of_v<ToDuration, duration>>> = nullptr>
constexpr auto time_point_cast(time_point<Clock, Duration> const& point
) noexcept(noexcept(time_point<Clock, ToDuration>(duration_cast<ToDuration>(point.time_since_epoch()))))
    -> time_point<Clock, ToDuration> {
  return time_point<Clock, ToDuration>(duration_cast<ToDuration>(point.time_since_epoch()));
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace chrono
}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TIME_POINT_HPP_
