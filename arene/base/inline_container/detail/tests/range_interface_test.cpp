// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/inline_container/detail/range_interface.hpp"

#include <cstddef>      // for ptrdiff_t, size_t
#include <iterator>     // for bidirectional_iterator_tag
#include <type_traits>  // for is_same, conditional_t, is_b...

#include <gtest/gtest.h>  // for AssertionResult, Message

#include "arene/base/array.hpp"                    // for array
#include "arene/base/compare.hpp"                  // for operator==
#include "arene/base/testing/gtest.hpp"            // for CONSTEXPR_ASSERT, CONSTEXPR_...
#include "arene/base/type_traits/conditional.hpp"  // for CONSTEXPR_ASSERT, CONSTEXPR_...

namespace {

using arene::base::inline_container::detail::range_interface;

template <class T, class Category, bool IsConst>
class testing_iterator {
 public:
  using iterator_category = Category;
  using value_type = T;
  using pointer = arene::base::conditional_t<IsConst, value_type const*, value_type*>;
  using reference = arene::base::conditional_t<IsConst, value_type const&, value_type&>;
  using difference_type = std::ptrdiff_t;

  constexpr testing_iterator() = default;
  constexpr testing_iterator(testing_iterator const&) = default;
  constexpr testing_iterator(testing_iterator&&) = default;
  constexpr auto operator=(testing_iterator const&) -> testing_iterator& = default;
  constexpr auto operator=(testing_iterator&&) -> testing_iterator& = default;
  ~testing_iterator() = default;

  template <class U, class C>
  // Allow implicit conversion from non-const to const
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr testing_iterator(testing_iterator<U, C, false> const&) {}

  auto operator*() noexcept -> reference;
  auto operator->() noexcept -> pointer;
  auto operator++() noexcept -> testing_iterator&;
  auto operator++(int) noexcept -> testing_iterator;
  auto operator--() noexcept -> testing_iterator&;
  auto operator--(int) noexcept -> testing_iterator;
  auto operator==(testing_iterator const&) noexcept -> bool;
  auto operator!=(testing_iterator const&) noexcept -> bool;
};

template <class T, class Category>
class const_view : public range_interface<const_view<T, Category>> {
 public:
  using iterator = testing_iterator<T, Category, true>;
  using const_iterator = testing_iterator<T, Category, true>;

  constexpr const_view()
      : range_interface<const_view<T, Category>>{this} {}
  constexpr auto begin() const noexcept -> const_iterator { return {}; }
  constexpr auto end() const noexcept -> const_iterator { return {}; }
};

template <class T, class Category>
class mutable_view : public range_interface<mutable_view<T, Category>> {
 public:
  using iterator = testing_iterator<T, Category, false>;
  using const_iterator = testing_iterator<T, Category, true>;

  constexpr mutable_view()
      : range_interface<const_view<T, Category>>{this} {}
  constexpr auto begin() const noexcept -> const_iterator { return {}; }
  constexpr auto end() const noexcept -> const_iterator { return {}; }
  constexpr auto begin() noexcept -> iterator { return {}; }
  constexpr auto end() noexcept -> iterator { return {}; }
};

template <typename T>
class ContainerViewProvidedOperationsTest : public ::testing::Test {};

using iterator_category_types = ::testing::Types<
    std::input_iterator_tag,
    std::output_iterator_tag,
    std::forward_iterator_tag,
    std::bidirectional_iterator_tag,
    std::random_access_iterator_tag>;

TYPED_TEST_SUITE(ContainerViewProvidedOperationsTest, iterator_category_types, );

/// @test A view using @c range_interface always has @c cbegin and @c cend provided.
TYPED_TEST(ContainerViewProvidedOperationsTest, TestCBeginAndCEndAlwaysProvided) {
  using Category = TypeParam;
  using T = int;
  using ConstIt = testing_iterator<T, Category, true>;

  auto const cbegin_invoker = [](auto view) -> decltype(view.cbegin()) { return view.cbegin(); };
  auto const cend_invoker = [](auto view) -> decltype(view.cend()) { return view.cend(); };

  ASSERT_TRUE((arene::base::is_invocable_r_v<ConstIt, decltype(cbegin_invoker), const_view<T, Category>>));
  ASSERT_TRUE((arene::base::is_invocable_r_v<ConstIt, decltype(cend_invoker), const_view<T, Category>>));
  ASSERT_TRUE((arene::base::is_invocable_r_v<ConstIt, decltype(cbegin_invoker), mutable_view<T, Category>>));
  ASSERT_TRUE((arene::base::is_invocable_r_v<ConstIt, decltype(cend_invoker), mutable_view<T, Category>>));
}

/// @test A view using @c range_interface has @c rbegin and @c rend provided if it has a bidirectional
/// iterator.
TYPED_TEST(ContainerViewProvidedOperationsTest, TestRBeginAndREndProvidedForBidirectional) {
  using Category = TypeParam;
  using T = int;

  auto const rbegin_invoker = [](auto const& view) -> decltype(view.rbegin()) { return view.rbegin(); };
  auto const rend_invoker = [](auto const& view) -> decltype(view.rend()) { return view.rend(); };

  using ConstReverseIt = std::conditional_t<
      std::is_base_of<std::bidirectional_iterator_tag, Category>::value,
      arene::base::reverse_iterator<testing_iterator<T, Category, true>>,
      void>;

  if (std::is_base_of<std::bidirectional_iterator_tag, Category>::value) {
    ASSERT_TRUE((arene::base::is_invocable_r_v<ConstReverseIt, decltype(rbegin_invoker), const_view<T, Category>>));
    ASSERT_TRUE((arene::base::is_invocable_r_v<ConstReverseIt, decltype(rend_invoker), const_view<T, Category>>));
    ASSERT_TRUE((arene::base::is_invocable_r_v<ConstReverseIt, decltype(rbegin_invoker), mutable_view<T, Category>>));
    ASSERT_TRUE((arene::base::is_invocable_r_v<ConstReverseIt, decltype(rend_invoker), mutable_view<T, Category>>));
  } else {
    ASSERT_FALSE((arene::base::is_invocable_v<decltype(rbegin_invoker), const_view<T, Category>>));
    ASSERT_FALSE((arene::base::is_invocable_v<decltype(rend_invoker), const_view<T, Category>>));
    ASSERT_FALSE((arene::base::is_invocable_v<decltype(rbegin_invoker), mutable_view<T, Category>>));
    ASSERT_FALSE((arene::base::is_invocable_v<decltype(rend_invoker), mutable_view<T, Category>>));
  }
}

/// @test A view using @c range_interface has @c crbegin and @c crend provided if it has a bidirectional
/// iterator.
TYPED_TEST(ContainerViewProvidedOperationsTest, TestCRBeginAndCREndProvidedForBidirectional) {
  using Category = TypeParam;
  using T = int;

  auto const crbegin_invoker = [](auto view) -> decltype(view.crbegin()) { return view.crbegin(); };
  auto const crend_invoker = [](auto view) -> decltype(view.crend()) { return view.crend(); };

  using ConstReverseIt = std::conditional_t<
      std::is_base_of<std::bidirectional_iterator_tag, Category>::value,
      arene::base::reverse_iterator<testing_iterator<T, Category, true>>,
      void>;

  if (std::is_base_of<std::bidirectional_iterator_tag, Category>::value) {
    ASSERT_TRUE((arene::base::is_invocable_r_v<ConstReverseIt, decltype(crbegin_invoker), const_view<T, Category>>));
    ASSERT_TRUE((arene::base::is_invocable_r_v<ConstReverseIt, decltype(crend_invoker), const_view<T, Category>>));
    ASSERT_TRUE((arene::base::is_invocable_r_v<ConstReverseIt, decltype(crbegin_invoker), mutable_view<T, Category>>));
    ASSERT_TRUE((arene::base::is_invocable_r_v<ConstReverseIt, decltype(crend_invoker), mutable_view<T, Category>>));
  } else {
    ASSERT_FALSE((arene::base::is_invocable_v<decltype(crbegin_invoker), const_view<T, Category>>));
    ASSERT_FALSE((arene::base::is_invocable_v<decltype(crend_invoker), const_view<T, Category>>));
    ASSERT_FALSE((arene::base::is_invocable_v<decltype(crbegin_invoker), mutable_view<T, Category>>));
    ASSERT_FALSE((arene::base::is_invocable_v<decltype(crend_invoker), mutable_view<T, Category>>));
  }
}

/// @test A view using @c range_interface has mutable versions @c rbegin and @c rend provided if it has a
/// mutable bidirectional iterator.
TYPED_TEST(ContainerViewProvidedOperationsTest, TestMutableRBeginAndREndProvidedForMutableBidirectional) {
  using Category = TypeParam;
  using T = int;

  auto const rbegin_invoker = [](auto view) -> decltype(view.rbegin()) { return view.rbegin(); };
  auto const rend_invoker = [](auto view) -> decltype(view.rend()) { return view.rend(); };

  using MutableReverseIt = std::conditional_t<
      std::is_base_of<std::bidirectional_iterator_tag, Category>::value,
      arene::base::reverse_iterator<testing_iterator<T, Category, false>>,
      void>;

  if (std::is_base_of<std::bidirectional_iterator_tag, Category>::value) {
    ASSERT_FALSE((arene::base::is_invocable_r_v<MutableReverseIt, decltype(rbegin_invoker), const_view<T, Category>>));
    ASSERT_FALSE((arene::base::is_invocable_r_v<MutableReverseIt, decltype(rend_invoker), const_view<T, Category>>));
    ASSERT_TRUE((arene::base::is_invocable_r_v<MutableReverseIt, decltype(rbegin_invoker), mutable_view<T, Category>>));
    ASSERT_TRUE((arene::base::is_invocable_r_v<MutableReverseIt, decltype(rend_invoker), mutable_view<T, Category>>));
  } else {
    ASSERT_FALSE((arene::base::is_invocable_v<decltype(rbegin_invoker), const_view<T, Category>>));
    ASSERT_FALSE((arene::base::is_invocable_v<decltype(rend_invoker), const_view<T, Category>>));
    ASSERT_FALSE((arene::base::is_invocable_v<decltype(rbegin_invoker), mutable_view<T, Category>>));
    ASSERT_FALSE((arene::base::is_invocable_v<decltype(rend_invoker), mutable_view<T, Category>>));
  }
}

/// @test A view using @c range_interface always has @c empty.
TYPED_TEST(ContainerViewProvidedOperationsTest, TestEmptyIsAlwaysProvided) {
  auto const empty_invoker = [](auto view) -> decltype(view.empty()) { return view.empty(); };

  ASSERT_TRUE((arene::base::is_invocable_r_v<bool, decltype(empty_invoker), const_view<int, TypeParam>>));
  ASSERT_TRUE((arene::base::is_invocable_r_v<bool, decltype(empty_invoker), mutable_view<int, TypeParam>>));
}

class proxy_view : public range_interface<proxy_view> {
  using T = int;
  using Category = std::random_access_iterator_tag;

 public:
  using iterator = testing_iterator<T, Category, false>;
  using const_iterator = testing_iterator<T, Category, true>;

  constexpr proxy_view()
      : range_interface<proxy_view>{this} {}
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto begin() const noexcept -> iterator { return {}; }
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto end() const noexcept -> iterator { return {}; }
};

/// @test A proxy view which always returns a mutable iterator gets the correct return type on generated members.
CONSTEXPR_TEST(ContainerViewProvidedOperationsTest, TestProxyViewTypesAreForwardedCorrectly) {
  using iterator = proxy_view::iterator;
  using const_iterator = proxy_view::const_iterator;
  using reverse_iterator = ::arene::base::reverse_iterator<iterator>;
  using const_reverse_iterator = ::arene::base::reverse_iterator<const_iterator>;

  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view&>().begin()), iterator>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view const&>().begin()), iterator>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view&>().end()), iterator>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view const&>().end()), iterator>::value);

  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view&>().cbegin()), const_iterator>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view const&>().cbegin()), const_iterator>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view&>().cend()), const_iterator>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view const&>().cend()), const_iterator>::value);

  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view&>().rbegin()), reverse_iterator>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view const&>().rbegin()), reverse_iterator>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view&>().rend()), reverse_iterator>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view const&>().rend()), reverse_iterator>::value);

  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view&>().crbegin()), const_reverse_iterator>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view const&>().crbegin()), const_reverse_iterator>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view&>().crend()), const_reverse_iterator>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(std::declval<proxy_view const&>().crend()), const_reverse_iterator>::value);
}

template <bool NoExcept>
class maybe_throwable_view : public range_interface<maybe_throwable_view<NoExcept>> {
  using T = int;
  using Category = std::random_access_iterator_tag;

 public:
  using iterator = testing_iterator<T, Category, false>;
  using const_iterator = testing_iterator<T, Category, true>;

  constexpr auto begin() const noexcept(NoExcept) -> const_iterator { return {}; }
  constexpr auto end() const noexcept(NoExcept) -> const_iterator { return {}; }
  constexpr auto begin() noexcept(NoExcept) -> iterator { return {}; }
  constexpr auto end() noexcept(NoExcept) -> iterator { return {}; }
};

/// @test Opertions provided by @c range_interface are noexcept if the basis functions are noexcept.
CONSTEXPR_TEST(ContainerViewInterfaceTest, CheckOperationsAreNonThrowing) {
  using non_throwing_view = maybe_throwable_view<true>;

  CONSTEXPR_ASSERT(noexcept(std::declval<non_throwing_view&>().cbegin()));
  CONSTEXPR_ASSERT(noexcept(std::declval<non_throwing_view&>().cend()));
  CONSTEXPR_ASSERT(noexcept(std::declval<non_throwing_view&>().rbegin()));
  CONSTEXPR_ASSERT(noexcept(std::declval<non_throwing_view&>().rend()));
  CONSTEXPR_ASSERT(noexcept(std::declval<non_throwing_view const&>().rbegin()));
  CONSTEXPR_ASSERT(noexcept(std::declval<non_throwing_view const&>().rend()));
  CONSTEXPR_ASSERT(noexcept(std::declval<non_throwing_view&>().crbegin()));
  CONSTEXPR_ASSERT(noexcept(std::declval<non_throwing_view&>().crend()));
  CONSTEXPR_ASSERT(noexcept(std::declval<non_throwing_view&>().empty()));
}

/// @test Opertions provided by @c range_interface are not noexcept if the basis functions are not noexcept.
CONSTEXPR_TEST(ContainerViewInterfaceTest, CheckOperationsAreThrowing) {
  using throwing_view = maybe_throwable_view<false>;

  CONSTEXPR_ASSERT(!noexcept(std::declval<throwing_view&>().cbegin()));
  CONSTEXPR_ASSERT(!noexcept(std::declval<throwing_view&>().cend()));
  CONSTEXPR_ASSERT(!noexcept(std::declval<throwing_view&>().rbegin()));
  CONSTEXPR_ASSERT(!noexcept(std::declval<throwing_view&>().rend()));
  CONSTEXPR_ASSERT(!noexcept(std::declval<throwing_view const&>().rbegin()));
  CONSTEXPR_ASSERT(!noexcept(std::declval<throwing_view const&>().rend()));
  CONSTEXPR_ASSERT(!noexcept(std::declval<throwing_view&>().crbegin()));
  CONSTEXPR_ASSERT(!noexcept(std::declval<throwing_view&>().crend()));
  CONSTEXPR_ASSERT(!noexcept(std::declval<throwing_view&>().empty()));
}

/// A basic view type which defines the minimum operations for the range_interface
template <class T, std::size_t N>
class mutable_array_view : public range_interface<mutable_array_view<T, N>> {
  using base_type = arene::base::array<T, N>;

 public:
  using value_type = T;
  using iterator = typename base_type::iterator;
  using const_iterator = typename base_type::const_iterator;

  constexpr explicit mutable_array_view(base_type& arr)
      : range_interface<mutable_array_view<T, N>>{this},
        data_{arr} {}

  /// @brief Obtain the mutable iterator for the start of the range
  /// @return The mutable iterator
  constexpr auto begin() noexcept -> iterator { return data_.begin(); }

  /// @brief Obtain the const iterator for the start of the range
  /// @return The const iterator
  constexpr auto begin() const noexcept -> const_iterator { return data_.cbegin(); }

  /// @brief Obtain the mutable iterator for the end of the range
  /// @return The mutable iterator
  constexpr auto end() noexcept -> iterator { return data_.end(); }

  /// @brief Obtain the const iterator for the end of the range
  /// @return The const iterator
  constexpr auto end() const noexcept -> const_iterator { return data_.cend(); }

 private:
  base_type& data_;
};

/// @test The @c range_interface provides @c cbegin() and @c cend() correctly.
CONSTEXPR_TEST(ContainerViewInterfaceTest, TestCBeginAndCEnd) {
  auto arr = arene::base::array<int, 10>{};
  auto const view = mutable_array_view<int, 10>{arr};

  static_assert(
      std::is_same<decltype(view.cbegin()), decltype(arr.cbegin())>::value,
      "View provides same iterator type for cbegin"
  );
  static_assert(
      std::is_same<decltype(view.cend()), decltype(arr.cend())>::value,
      "View provides same iterator type for cend"
  );
  CONSTEXPR_ASSERT(view.cbegin() == arr.cbegin());
  CONSTEXPR_ASSERT(view.cend() == arr.cend());
}

/// @test The @c range_interface provides @c rbegin() const and mutable variants correctly.
CONSTEXPR_TEST(ContainerViewInterfaceTest, TestRBegin) {
  auto arr = arene::base::array<int, 10>{};
  auto mutable_view = mutable_array_view<int, 10>{arr};

  static_assert(
      std::is_same<decltype(mutable_view.rbegin()), decltype(arr.rbegin())>::value,
      "mutable_view provides same iterator type for rbegin"
  );
  CONSTEXPR_ASSERT(mutable_view.rbegin() == arr.rbegin());

  auto const const_view = mutable_array_view<int, 10>{arr};

  static_assert(
      std::is_same<decltype(const_view.rbegin()), decltype(arr.crbegin())>::value,
      "const_view provides same iterator type for rbegin"
  );
  CONSTEXPR_ASSERT(const_view.rbegin() == arr.crbegin());
}

/// @test The @c range_interface provides @c rend() const and mutable variants correctly.
CONSTEXPR_TEST(ContainerViewInterfaceTest, TestREnd) {
  auto arr = arene::base::array<int, 10>{};
  auto mutable_view = mutable_array_view<int, 10>{arr};

  static_assert(
      std::is_same<decltype(mutable_view.rend()), decltype(arr.rend())>::value,
      "mutable_view provides same iterator type for rend"
  );
  CONSTEXPR_ASSERT(mutable_view.rend() == arr.rend());

  auto const const_view = mutable_array_view<int, 10>{arr};

  static_assert(
      std::is_same<decltype(const_view.rend()), decltype(arr.crend())>::value,
      "const_view provides same iterator type for rend"
  );
  CONSTEXPR_ASSERT(const_view.rend() == arr.crend());
}

/// @test The @c range_interface provides @c crbegin() and @c crend() correctly.
CONSTEXPR_TEST(ContainerViewInterfaceTest, TestCRBeginAndCREnd) {
  auto arr = arene::base::array<int, 10>{};
  auto const view = mutable_array_view<int, 10>{arr};

  static_assert(
      std::is_same<decltype(view.crbegin()), decltype(arr.crbegin())>::value,
      "View provides same iterator type for crbegin"
  );
  static_assert(
      std::is_same<decltype(view.crend()), decltype(arr.crend())>::value,
      "View provides same iterator type for crend"
  );
  CONSTEXPR_ASSERT(view.crbegin() == arr.crbegin());
  CONSTEXPR_ASSERT(view.crend() == arr.crend());
}

/// @test The @c range_interface provides @c empty correctly.
CONSTEXPR_TEST(ContainerViewInterfaceTest, TestEmpty) {
  auto empty_arr = arene::base::array<int, 0>{};
  auto const empty_view = mutable_array_view<int, 0>{empty_arr};

  CONSTEXPR_ASSERT(empty_view.empty());

  auto not_empty_arr = arene::base::array<int, 1>{};
  auto const not_empty_view = mutable_array_view<int, 1>{not_empty_arr};

  CONSTEXPR_ASSERT(!not_empty_view.empty());
}
}  // namespace
