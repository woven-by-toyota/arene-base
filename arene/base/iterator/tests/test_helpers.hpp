// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file test_helpers.hpp
/// @brief Common helpers used in iterator unit tests.
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_TESTS_TEST_HELPERS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_TESTS_TEST_HELPERS_HPP_

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/stdlib_choice/begin.hpp"
#include "arene/base/stdlib_choice/copy_n.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/end.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "arene/base/type_list/contains.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "testlibs/utilities/iterator_types.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <deque>
#include <forward_list>
#include <list>
#include <sstream>
#include <vector>
#endif

namespace arene {
namespace base {
namespace iterator_tests {
/// @brief Indicates the preincrement operator should be @c noexcept(false).
struct throwing_preincrement {};
/// @brief Indicates the postincrement operator should be @c noexcept(false).
struct throwing_postincrement {};
/// @brief Indicates the increment-assign operator should be @c noexcept(false).
struct throwing_increment_assign {};
/// @brief Indicates the predecrement operator should be @c noexcept(false).
struct throwing_predecrement {};
/// @brief Indicates the postdecrement operator should be @c noexcept(false).
struct throwing_postdecrement {};
/// @brief Indicates the decrement-assign operator should be @c noexcept(false).
struct throwing_decrement_assign {};
/// @brief Indicates the comparison operators should be @c noexcept(false).
struct throwing_comparison {};
/// @brief Indicates the dereference operators should be @c noexcept(false).
struct throwing_dereference {};
/// @brief Indicates the addition operator should be @c noexcept(false).
struct throwing_addition {};
/// @brief Indicates the subtraction operator should be @c noexcept(false).
struct throwing_subtraction {};

///
/// @brief A facade for testing noexcept/invocability of iterator-based algorithms.
///
/// It does not implement any actual behavior, it just declares the correct set of operations for the given
/// @c iterator_category and adjusts their @c noexcept status based on the throwing tags provided.
///
/// @tparam IteratorCateogry The iterator_category that the facade should present as.
/// @tparam ThrowingOps The set of operations that should throw.
///
template <typename IteratorCateogry, typename... ThrowingOps>
struct iterator_facade {
  using iterator_category = IteratorCateogry;
  using difference_type = std::ptrdiff_t;
  using reference = int&;
  using value_type = int;
  using pointer = int*;

  template <typename Op>
  static constexpr bool is_not_throwing_op = !type_lists::contains_v<::arene::base::type_list<ThrowingOps...>, Op>;

  auto operator*() noexcept(is_not_throwing_op<throwing_dereference>) -> reference;
  auto operator->() noexcept(is_not_throwing_op<throwing_dereference>) -> pointer;

  auto operator++() noexcept(is_not_throwing_op<throwing_preincrement>) -> iterator_facade&;
  auto operator++(int) noexcept(is_not_throwing_op<throwing_postincrement>) -> iterator_facade;
  template <
      typename SfinaeIteratorCategory = iterator_category,
      constraints<std::enable_if_t<
          std::is_convertible<SfinaeIteratorCategory, std::bidirectional_iterator_tag>::value>> = nullptr>
  auto operator--() noexcept(is_not_throwing_op<throwing_predecrement>) -> iterator_facade&;
  template <
      typename SfinaeIteratorCategory = iterator_category,
      constraints<std::enable_if_t<
          std::is_convertible<SfinaeIteratorCategory, std::bidirectional_iterator_tag>::value>> = nullptr>
  auto operator--(int) noexcept(is_not_throwing_op<throwing_postdecrement>) -> iterator_facade;

  template <
      typename SfinaeIteratorCategory = iterator_category,
      constraints<std::enable_if_t<
          std::is_convertible<SfinaeIteratorCategory, std::random_access_iterator_tag>::value>> = nullptr>
  auto operator+(difference_type) noexcept(is_not_throwing_op<throwing_addition>) -> iterator_facade;
  template <
      typename SfinaeIteratorCategory = iterator_category,
      constraints<std::enable_if_t<
          std::is_convertible<SfinaeIteratorCategory, std::random_access_iterator_tag>::value>> = nullptr>
  auto operator-(difference_type) noexcept(is_not_throwing_op<throwing_subtraction>) -> iterator_facade;
  template <
      typename SfinaeIteratorCategory = iterator_category,
      constraints<std::enable_if_t<
          std::is_convertible<SfinaeIteratorCategory, std::random_access_iterator_tag>::value>> = nullptr>
  auto operator+=(difference_type) noexcept(is_not_throwing_op<throwing_increment_assign>) -> iterator_facade&;
  template <
      typename SfinaeIteratorCategory = iterator_category,
      constraints<std::enable_if_t<
          std::is_convertible<SfinaeIteratorCategory, std::random_access_iterator_tag>::value>> = nullptr>
  auto operator-=(difference_type) noexcept(is_not_throwing_op<throwing_decrement_assign>) -> iterator_facade;
  template <
      typename SfinaeIteratorCategory = iterator_category,
      constraints<std::enable_if_t<
          std::is_convertible<SfinaeIteratorCategory, std::random_access_iterator_tag>::value>> = nullptr>
  auto operator-(iterator_facade const&) noexcept(is_not_throwing_op<throwing_decrement_assign>) -> difference_type;

  auto operator==(iterator_facade const&) noexcept(is_not_throwing_op<throwing_comparison>) -> bool;
  auto operator!=(iterator_facade const&) noexcept(is_not_throwing_op<throwing_comparison>) -> bool;
};
/// @brief An alias for a facade to a @c std::radom_access_tag presenting iterator.
template <typename... Ops>
using random_access_iterator_facade = iterator_facade<std::random_access_iterator_tag, Ops...>;
/// @brief An alias for a facade to a @c std::bidirectional_iterator_tag presenting iterator.
template <typename... Ops>
using bidirectional_iterator_facade = iterator_facade<std::bidirectional_iterator_tag, Ops...>;
/// @brief An alias for a facade to a @c std::forward_iterator presenting iterator.
template <typename... Ops>
using forward_iterator_facade = iterator_facade<std::forward_iterator_tag, Ops...>;
/// @brief An alias for a facade to a @c std::input_iterator presenting iterator.
template <typename... Ops>
using input_iterator_facade = iterator_facade<std::input_iterator_tag, Ops...>;

namespace type_lists = ::arene::base::type_lists;

/// Helper for casting a type_list to testing::Types
template <typename Tl>
using as_testing_types = type_lists::apply_all_t<Tl, ::testing::Types>;

/// @brief the size of constant-size containers used in @c backing_data .
static constexpr std::size_t data_size = 5U;

template <typename Category, typename Value = int, std::size_t Size = data_size>
class demoted_iterator_array {
  using array_type = arene::base::array<Value, Size>;
  array_type data_;

 public:
  using value_type = Value;
  using reference = value_type&;
  using const_reference = value_type const&;
  using difference_type = typename array_type::difference_type;
  using size_type = typename array_type::size_type;

  using iterator = testing::demoted_iterator<typename array_type::iterator, Category>;
  using const_iterator = testing::demoted_iterator<typename array_type::const_iterator, Category>;

  using reverse_iterator = arene::base::reverse_iterator<iterator>;
  using const_reverse_iterator = arene::base::reverse_iterator<const_iterator>;

  demoted_iterator_array() = default;
  ~demoted_iterator_array() = default;
  demoted_iterator_array(demoted_iterator_array const&) = default;
  demoted_iterator_array(demoted_iterator_array&&) = default;
  auto operator=(demoted_iterator_array const&) -> demoted_iterator_array& = default;
  auto operator=(demoted_iterator_array&&) -> demoted_iterator_array& = default;
  demoted_iterator_array(std::initializer_list<value_type> init) {
    ARENE_PRECONDITION(init.size() <= data_.size());
    std::copy_n(init.begin(), init.size(), data_.begin());
  }

  auto begin() noexcept -> iterator { return testing::make_demoted_iterator<Category>(data_.begin()); }
  auto begin() const noexcept -> const_iterator { return testing::make_demoted_iterator<Category>(data_.begin()); }
  auto cbegin() const noexcept -> const_iterator { return begin(); }

  auto end() noexcept -> iterator { return testing::make_demoted_iterator<Category>(data_.end()); }
  auto end() const noexcept -> const_iterator { return testing::make_demoted_iterator<Category>(data_.end()); }
  auto cend() const noexcept -> const_iterator { return end(); }

  friend auto operator==(demoted_iterator_array const& lhs, demoted_iterator_array const& rhs) noexcept -> bool {
    return lhs.data_ == rhs.data_;
  }
  friend auto operator!=(demoted_iterator_array const& lhs, demoted_iterator_array const& rhs) noexcept -> bool {
    return lhs.data_ != rhs.data_;
  }

  void swap(demoted_iterator_array& other) noexcept { return data_.swap(other.data_); }
  auto size() const noexcept -> size_type { return data_.size(); }
  auto max_size() const noexcept -> size_type { return data_.max_size(); }
  auto empty() const noexcept -> bool { return data_.empty(); }

  auto data() noexcept -> value_type* { return data_.data(); }
  auto data() const noexcept -> value_type const* { return data_.data(); }
};

/// @brief A list of containers whose iterators are random_access_iterator .
using random_access_iterator_containers = ::testing::Types<
    arene::base::array<int, data_size>,
    // NOLINTNEXTLINE(hicpp-avoid-c-arrays) explicitly need to test c-array compatibility.
    int[data_size]>;
/// @brief A list of containers whose iterators are bidirectional_iterator .
using bidirectional_iterator_containers = ::testing::Types<
    demoted_iterator_array<std::bidirectional_iterator_tag>
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    ,
    std::list<int>,
    std::deque<int>,
    std::set<int>
#endif
    >;
/// @brief A list of containers whose iterators are forward_iterator .
using forward_iterator_containers = ::testing::Types<
    demoted_iterator_array<std::forward_iterator_tag>
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    ,
    std::forward_list<int>
#endif
    >;
/// @brief A list of containers whose iterators are input_iterator .
using input_iterator_containers = ::testing::Types<
    demoted_iterator_array<std::input_iterator_tag>
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    ,
    std::stringstream
#endif
    >;
/// @brief A list of containers whose iterators are at least bidirectional .
using at_least_bidirectional_iterator_containers =
    type_lists::concat_unique_t<random_access_iterator_containers, bidirectional_iterator_containers>;
/// @brief A list of containers whose iterators are at least forward_iterator .
using at_least_forward_iterator_containers =
    type_lists::concat_unique_t<at_least_bidirectional_iterator_containers, forward_iterator_containers>;
/// @brief A list of containers whose iterators are either forward_iterator or input_iterator .
using only_incrementable_iterator_containers =
    type_lists::concat_unique_t<forward_iterator_containers, input_iterator_containers>;
/// @brief A list of containers that includes at least one from every iterator_category.
using all_iterator_category_containers =
    type_lists::concat_unique_t<at_least_forward_iterator_containers, only_incrementable_iterator_containers>;

///
/// @brief A helper for initializing backing data in a container to produce iterators from.
///
/// @tparam Container The type of the container to produce.
///
template <typename Container>
class backing_data {
 protected:
  Container container_;

 public:
  constexpr backing_data()
      : container_{1, 2, 3, 4, 5} {}
  constexpr auto begin() const { return container_.begin(); }
  constexpr auto end() const { return container_.end(); }
  static auto ssize() -> int { return static_cast<int>(data_size); }
};

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)

/// @brief specialization for @c std::stringstream .
template <>
class backing_data<std::stringstream> {
 protected:
  std::string backing_{"1 2 3 4 5"};
  std::stringstream container_{backing_};

 public:
  auto begin() { return std::istream_iterator<int>{container_}; }
  static auto end() { return std::istream_iterator<int>{}; }
  static auto ssize() -> int { return static_cast<int>(data_size); }
};
#endif

/// @brief specialization for a c-array of ints .
// NOLINTBEGIN(hicpp-avoid-c-arrays) explicitly need to test c-array compatibility.
template <>
class backing_data<int[data_size]> {
 protected:
  int container_[data_size]{1, 2, 3, 4, 5};

 public:
  constexpr auto begin() const { return std::begin(container_); }
  constexpr auto end() const { return std::end(container_); }
  static auto ssize() -> int { return static_cast<int>(data_size); }
};
// NOLINTEND(hicpp-avoid-c-arrays) explicitly need to test c-array compatibility.

}  // namespace iterator_tests
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_TESTS_TEST_HELPERS_HPP_
