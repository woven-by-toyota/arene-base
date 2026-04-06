// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_CUSTOMIZATION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_CUSTOMIZATION_HPP_

#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/inline_container/detail/iterator_interface.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/is_array.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_floating_point.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/is_unsigned.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/make_signed.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace arene {
namespace base {
namespace testing {

//////////////////////////////////////////////////////////////////////
// Customization points for generating values of user-defined types //
//////////////////////////////////////////////////////////////////////

namespace customization_detail {
/// @brief A version of arene::base::to_array where the individual elements are cast to the specified type
/// @tparam OutType The element type of the output array, specified by the caller
/// @tparam InType The element type of the input C-array, deduced from the first argument
/// @tparam Indices A pack of indices the same size as the input and output arrays, deduced from the second argument
/// @param arr A C-Array containing the values as instances of some type @c InType
/// @return An <c>arene::base::array<OutType, sizeof...(Indices)></c> where the elements are constructed from @c arr
template <typename OutType, typename InType, std::size_t... Indices>
constexpr auto to_array_cast(
    // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
    InType (&&arr)[sizeof...(Indices)],
    std::index_sequence<Indices...>
) noexcept(std::is_nothrow_constructible<OutType, InType&&>::value) -> array<base::remove_cvref_t<OutType>, sizeof...(Indices)> {
  return {{std::move(arr[Indices])...}};
}
}  // namespace customization_detail

/// @brief A version of arene::base::to_array where the individual elements are cast to the specified type
/// @tparam OutType The element type of the output array, specified by the caller
/// @tparam InType The element type of the input C-array, deduced from the argument
/// @tparam Size The size of the input and output arrays, deduced from the argument
/// @param arr A C-Array containing the values as instances of some type @c InType
/// @return An <c>arene::base::array<OutType, Size></c> where the elements are constructed from the elements of @c arr
template <
    typename OutType,
    typename InType,
    std::size_t Size,
    constraints<
        std::enable_if_t<!std::is_array<InType>::value>,
        std::enable_if_t<std::is_constructible<OutType, InType&&>::value>> = nullptr>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
constexpr auto to_array_cast(InType (&&arr)[Size]) noexcept(std::is_nothrow_constructible<OutType, InType&&>::value)
    -> array<base::remove_cvref_t<OutType>, Size> {
  return customization_detail::to_array_cast<OutType>(std::move(arr), std::make_index_sequence<Size>());
}

/// @brief An array used in the default implementation of @c ::test_value ; by default it's empty and unusable
/// @note This must be explicitly specialized to be usable; specializations can use different array sizes
/// @tparam T The type of which this is an array of test values
template <typename T, typename = ::arene::base::constraints<>>
extern constexpr ::arene::base::array<T, 0UL> test_value_array{};

/// @brief A partial specialization of @c test_value_array for built-in unsigned integers to make it easier to use
/// @tparam T The type of which this is an array of test values
template <typename T>
extern constexpr auto test_value_array<
    T,
    ::arene::base::
        constraints<std::enable_if_t<std::is_integral<T>::value>, std::enable_if_t<std::is_unsigned<T>::value>>> =
    to_array_cast<T>({0U, 8U, 19U, 6612U, 777124U, 7U, 609124U, 1224545862U, 2561U, 976173U});

/// @brief A partial specialization of @c test_value_array for built-in signed integers to make it easier to use
/// @tparam T The type of which this is an array of test values
template <typename T>
extern constexpr auto test_value_array<
    T,
    ::arene::base::
        constraints<std::enable_if_t<std::is_integral<T>::value>, std::enable_if_t<std::is_signed<T>::value>>> =
    to_array_cast<T>({1, -2, 6, -24, 120, -720, 5'040, -40'320, 362'880, -3'628'800});

/// @brief A partial specialization of @c test_value_array for built-in floating point numbers to make it easier to use
/// @tparam T The type of which this is an array of test values
template <typename T>
extern constexpr auto
    test_value_array<T, ::arene::base::constraints<std::enable_if_t<std::is_floating_point<T>::value>>> =
        to_array_cast<T>({0.0, -1.0, 3.14, 2.72, 1.41, 0.577});

namespace customization_detail {
/// @brief A class to hold the default generation function so that it can be partially specialized for our test classes
/// @tparam T The type to generate a test value of
template <typename T>
struct test_value_generator {
  /// @brief The default implementation of <tt>test_value</tt>, returning a value copied from @c test_value_array
  /// @param idx The index of the test value to be generated
  static constexpr auto test_value(std::size_t idx) noexcept(std::is_nothrow_copy_constructible<T>::value) -> T {
    static_assert(
        !test_value_array<T>.empty(),
        "To use a type T for parameterized testing, you must specialize arene::base::testing::test_value_array<T> for "
        "copyable types or arene::base::testing::test_value<T> for non-copyable types"
    );
    return test_value_array<T>[idx % test_value_array<T>.size()];
  }
};

/// @brief A partial specialization of @c test_value for @c configurable_value
/// @tparam T The underlying value type held by the @c configurable_value
/// @tparam ThrowsOn A specification of which operations should throw
/// @tparam Disable A specification of which operations to disable entirely
/// @tparam IsConstexpr A specification of whether or not the @c configurable_value should be usable in @c constexpr
template <typename T, ::testing::throws_on ThrowsOn, ::testing::disable Disable, ::testing::is_constexpr IsConstexpr>
struct test_value_generator<::testing::configurable_value<T, ThrowsOn, Disable, IsConstexpr>> {
  /// @brief An implementation of @c test_value for @c configurable_value which generates based on the value type
  /// @param idx The index of the test value to be generated
  static constexpr auto test_value(std::size_t idx
  ) noexcept((ThrowsOn & ::testing::throws_on::value_construct) == ::testing::throws_on::nothing) -> T {
    return {test_value_generator<T>::test_value(idx)};
  }
};
}  // namespace customization_detail

/// @brief Get the @c idx 'th test value of type @c T
/// @note The default implementation copies the @c idx 'th element of @c test_value_array , wrapping to stay in bounds
/// @note For non-copyable types, this function can be specialized to construct the test value without copying
/// @tparam T The type to return test values of
/// @param idx The index of the test value to be generated
/// @return A test value of type @c T ; test values with different @c idx may be different from each other, but may not
template <typename T>
constexpr auto test_value(std::size_t idx
) noexcept(noexcept(customization_detail::test_value_generator<T>::test_value(idx))) -> T {
  return customization_detail::test_value_generator<T>::test_value(idx);
}

/// @brief Check whether or not a given type's @c test_value function works in @c constexpr contexts
/// @tparam T The type to check
template <typename T, typename = std::integral_constant<bool, true>>
constexpr bool has_constexpr_test_value = false;

/// @brief Check whether or not a given type's @c test_value function works in @c constexpr contexts
/// @tparam T The type to check
// This uses the comma operator to instantiate a test_value<T> and then discard it; if it compiles the trait is true.
template <typename T>
constexpr bool has_constexpr_test_value<T, std::integral_constant<bool, (test_value<T>(0), true)>> = true;

namespace customization_detail {

/// @brief A function object to provide a @c std::initializer_list implemented differently for @c constexpr and runtime
/// @tparam T The type for which an initializer list is requested
/// @tparam IsConstexpr Whether or not the provided initializer list should be declared @c constexpr
/// @tparam Indices A pack of indices, one for each test value in the initializer list
template <typename T, bool IsConstexpr, std::size_t... Indices>
class init_list_holder {
  /// @brief @c constexpr functions can not have static variables until C++23 so this has to be outside the function
  static constexpr std::initializer_list<T> list{test_value<T>(Indices)...};

 public:
  /// @brief Call the function object, returning a reference to the statically-allocated @c std::initializer_list
  /// @return A copy of a statically-allocated @c std::initializer_list<T> containing one test value per index
  constexpr auto operator()() const noexcept -> std::initializer_list<T> { return list; }
};

// This static variable definition is required until C++17.
template <typename T, bool IsConstexpr, std::size_t... Indices>
constexpr std::initializer_list<T> init_list_holder<T, IsConstexpr, Indices...>::list;

/// @brief A function object to provide a @c std::initializer_list implemented differently for @c constexpr and runtime
/// @tparam T The type for which an initializer list is requested
/// @tparam IsConstexpr Whether or not the provided initializer list should be declared @c constexpr
/// @tparam Indices A pack of indices, one for each test value in the initializer list
template <typename T, std::size_t... Indices>
class init_list_holder<T, false, Indices...> {
 public:
  /// @brief Call the function object, returning a reference to the statically-allocated @c std::initializer_list
  /// @return A copy of a statically-allocated @c std::initializer_list<T> containing one test value per index
  auto operator()() const noexcept -> std::initializer_list<T> {
    // For non-constexpr T, this needs to be *inside* the function to ensure it's initialized after test_value_array
    static std::initializer_list<T> const list{test_value<T>(Indices)...};
    return list;
  }
};

}  // namespace customization_detail

/// @brief A function object returning a @c std::initializer_list of one @c test_value per index in the given pack
/// @tparam T The type for which an initializer list is requested
/// @tparam Indices A pack of indices, one for each test value in the initializer list
/// @return A copy of a statically-allocated @c std::initializer_list<T> containing one test value per index
template <typename T, std::size_t... Indices>
extern constexpr customization_detail::init_list_holder<T, has_constexpr_test_value<T>, Indices...>
    test_initializer_list{};

/// @brief An iterator whose i'th position yields an rvalue reference to @c test_value(i) of the given type
/// @tparam T The type to yield
template <typename T>
class rvalue_iterator : public inline_container_detail::iterator_interface<rvalue_iterator<T>> {
  /// @brief A type trait indicating whether or not retrieving a @c test_value<T> can throw an exception
  static constexpr bool test_value_is_noexcept = noexcept(test_value<T>(0));

  /// @brief Alias for the iterator mixin, used to initialize it in various constructors
  using iterator_interface = inline_container_detail::iterator_interface<rvalue_iterator<T>>;

 public:
  /// @brief The value type of the iterator; this is the type whose test values are being retrieved
  using value_type = T;
  /// @brief The pointer type of the iterator; basically not meaningful, and it would be ill-formed to use it
  using pointer = T*;
  /// @brief The reference type of the iterator; a value of the expected type, since there's nothing to rvalue-refer to
  using reference = T&&;
  /// @brief The difference type of the iterator; test_value takes @c std::size_t so the difference is that but signed
  using difference_type = std::make_signed_t<std::size_t>;
  /// @brief The iterator category; random access because we just store an index and yield from it
  using iterator_category = std::random_access_iterator_tag;

  /// @brief Construct an rvalue_iterator containing the test value with the given index (defaulting to 0)
  /// @param index Index to retrieve the test value with
  /// @pre <c>index >= 0</c>, otherwise ARENE_PRECONDITION violation
  explicit constexpr rvalue_iterator(difference_type index = 0) noexcept(test_value_is_noexcept)
      : iterator_interface(),
        current_index_(index),
        current_value_(get_test_value(current_index_)) {}

  /// @brief Copy construct an rvalue_iterator, retrieving a new test value so that the value type can be move-only
  /// @param other Iterator to copy from
  constexpr rvalue_iterator(rvalue_iterator const& other) noexcept(test_value_is_noexcept)
      : iterator_interface(),
        current_index_(other.current_index_),
        current_value_(get_test_value(current_index_)) {}

  /// @brief Move construct an rvalue_iterator
  /// @param other Iterator to move from
  // NOLINTNEXTLINE(hicpp-noexcept-move) noexcept-specification accurately reflects that of the underlying type
  constexpr rvalue_iterator(rvalue_iterator&& other) noexcept(std::is_nothrow_move_constructible<T>::value)
      : iterator_interface(),
        current_index_(other.current_index_),
        current_value_(std::move(other.current_value_)) {}

  /// @brief Copy assign an rvalue_iterator, retrieving a new test value so that the value type can be move-only
  /// @param other Iterator to copy from
  constexpr auto operator=(rvalue_iterator const& other) noexcept(test_value_is_noexcept) -> rvalue_iterator& {
    current_index_ = other.current_index_;
    current_value_ = get_test_value(current_index_);
  }

  /// @brief Move assign an rvalue_iterator
  /// @param other Iterator to move from
  // NOLINTNEXTLINE(hicpp-noexcept-move) noexcept-specification accurately reflects that of the underlying type
  constexpr auto operator=(rvalue_iterator&& other) noexcept(std::is_nothrow_move_assignable<T>::value)
      -> rvalue_iterator& {
    current_index_ = other.current_index_;
    current_value_ = std::move(other.current_value_);
  }

  /// @brief Destroy an rvalue_iterator
  ~rvalue_iterator() = default;

  /// @brief Dereference an rvalue_iterator; data may no longer be meaningful after the first use
  /// @return An rvalue reference to the current test value
  constexpr auto operator*() noexcept -> reference { return std::move(current_value_); }

  /// @brief Shift an rvalue_iterator by the given amount
  /// @param diff Amount by which to shift the iterator
  /// @return A reference to @c *this
  /// @pre <c>current_index_ + diff >= 0</c>, otherwise ARENE_PRECONDITION violation
  constexpr auto operator+=(difference_type diff) noexcept(test_value_is_noexcept) -> rvalue_iterator& {
    current_index_ += diff;
    current_value_ = get_test_value(current_index_);
    return *this;
  }

  /// @brief Get the difference between two rvalue_iterators
  /// @param left Left side of the difference
  /// @param right Right side of the difference
  /// @return The difference between the operands' respective current indices
  friend constexpr auto operator-(rvalue_iterator const& left, rvalue_iterator const& right) noexcept
      -> difference_type {
    return left.current_index_ - right.current_index_;
  }

 private:
  /// @brief Current index whose @c test_value is yielded upon dereferencing
  difference_type current_index_;
  /// @brief Current value which will be yielded upon dereferencing; needs to be saved so we can get an rvalue ref to it
  T current_value_;

  /// @brief Get the @c test_value<T> with the given index, checking to make sure the index hasn't become negative
  /// @param Index to retrieve the test value with
  /// @return @c test_value<T>(index)
  /// @pre <c>index >= 0</c>, otherwise ARENE_PRECONDITION violation
  static constexpr auto get_test_value(difference_type index) noexcept(test_value_is_noexcept) -> T {
    ARENE_PRECONDITION(index >= 0);
    return test_value<T>(static_cast<std::size_t>(index));
  }
};

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_CUSTOMIZATION_HPP_
