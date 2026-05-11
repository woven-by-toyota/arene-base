// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/inline_container/external_vector.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <sstream>
#include <type_traits>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/array.hpp"
#include "arene/base/byte.hpp"
#include "arene/base/compare.hpp"
#include "arene/base/span.hpp"
#include "arene/base/type_traits.hpp"
#include "arene/base/utility.hpp"
#include "testlibs/utilities/counting_wrapper.hpp"
#include "testlibs/utilities/has_broken_less_than.hpp"

namespace {

using ::arene::base::external_vector;

template <std::size_t... Is, class... Ts>
auto do_invoke(std::index_sequence<Is...>, std::tuple<Ts&&...> args) {
  return std::get<sizeof...(Is)>(args)(std::get<Is>(args)...);
}

template <class... Args>
auto do_conditional_test(std::true_type, Args&&... args) {
  return do_invoke(std::make_index_sequence<sizeof...(Args) - 1>{}, std::forward_as_tuple(std::forward<Args>(args)...));
}
template <class... Args>
auto do_conditional_test(std::false_type, Args const&...) {}

// similar to ASSERT_THAT(range, ::testing::ElementsAre(...)) but doesn't
// require elements to be copy constructible
template <class Range, class Value = std::decay_t<decltype(*std::declval<Range&>().begin())>, std::size_t N>
auto elements_are(Range const& range, Value const (&values)[N]) {  // NOLINT(hicpp-avoid-c-arrays)
  return std::equal(range.begin(), range.end(), std::begin(values), std::end(values));
}

template <class T>
using is_movable = std::integral_constant<bool, std::is_move_constructible<T>{} && std::is_move_assignable<T>{}>;

// the `std::copyable` concept requires `std::movable`, but we only check copy
// constructible and copy assignable traits with this
template <class T>
using is_copyable = std::integral_constant<bool, std::is_copy_constructible<T>{} && std::is_copy_assignable<T>{}>;

// common values used in tests
constexpr auto values = ::arene::base::to_array({1, 2, 3});

template <class Value, std::size_t Capacity>
struct test_case {
  using value_type = Value;
  static constexpr auto capacity = Capacity;
};

using disable = testing::disable;
using throws_on = testing::throws_on_specifiers;

using value_types = ::testing::Types<  //
    test_case<
        ::testing::counting_wrapper<int, ::testing::throws_on::nothing, ::testing::disable::default_construct>,
        3>,
    test_case<::testing::counting_wrapper<int, ::testing::throws_on::nothing, ::testing::disable::move_construct>, 3>,
    test_case<::testing::counting_wrapper<int, ::testing::throws_on::nothing, ::testing::disable::copy_construct>, 3>,
    test_case<::testing::counting_wrapper<int, ::testing::throws_on::nothing, ::testing::disable::move_assign>, 3>,
    test_case<::testing::counting_wrapper<int, ::testing::throws_on::nothing, ::testing::disable::copy_assign>, 3>,
    test_case<::testing::counting_wrapper<int, ::testing::throws_on::default_construct>, 3>,
    test_case<::testing::counting_wrapper<int, ::testing::throws_on::value_construct>, 3>,
    test_case<::testing::counting_wrapper<int, ::testing::throws_on::copy_construct>, 3>,
    test_case<::testing::counting_wrapper<int, ::testing::throws_on::move_construct>, 3>,
    test_case<::testing::counting_wrapper<int, ::testing::throws_on::copy_assign>, 3>,
    test_case<::testing::counting_wrapper<int, ::testing::throws_on::move_assign>, 3>,
    test_case<::testing::counting_wrapper<int, ::testing::throws_on::nothing>, 3>,
    test_case<::testing::counting_wrapper<int, ::testing::throws_on::nothing>, 5>>;

template <std::size_t N>
constexpr auto constant = std::integral_constant<std::size_t, N>{};

template <class T>
struct ExternalVector : testing::Test {
  template <std::size_t Capacity = T::capacity>
  static constexpr auto make_buffer(std::integral_constant<std::size_t, Capacity> = {}) noexcept {
    using value_type = typename T::value_type;
    return ::arene::base::array<::arene::base::byte, Capacity * sizeof(value_type)>{};
  }
};

template <class T>
struct ExternalVectorDeathTest : ExternalVector<T> {};

TYPED_TEST_SUITE(ExternalVector, value_types, );
TYPED_TEST_SUITE(ExternalVectorDeathTest, value_types, );

/// @test An empty @c external_vector can be default constructed
TYPED_TEST(ExternalVector, DefaultConstructible) {
  using value_type = typename TypeParam::value_type;

  external_vector<value_type> const vec{};

  ASSERT_EQ(vec.size(), 0);
  ASSERT_EQ(vec.capacity(), 0);

  ASSERT_EQ(vec, external_vector<value_type>::try_construct());
}

/// @test An empty @c external_vector can be constructed from a buffer
TYPED_TEST(ExternalVector, ConstructibleFromBuffer) {
  using value_type = typename TypeParam::value_type;
  static constexpr auto capacity = TypeParam::capacity;

  auto buf = TestFixture::make_buffer();
  external_vector<value_type> const vec{buf};

  ASSERT_EQ(vec.size(), 0);
  ASSERT_EQ(vec.capacity(), capacity);

  auto buf2 = TestFixture::make_buffer();
  ASSERT_EQ(vec, external_vector<value_type>::try_construct(buf2));
}

/// @test An @c external_vector can be constructed from a range specified by iterators
TYPED_TEST(ExternalVector, CopyConstructibleIteratorRange) {
  do_conditional_test(
      std::is_copy_constructible<typename TypeParam::value_type>{},
      std::add_pointer_t<TypeParam>{},
      [](auto* dummy) {
        using T = std::remove_pointer_t<decltype(dummy)>;
        using value_type = typename T::value_type;
        auto const local_values = ::arene::base::array<value_type, 3>{1, 2, 3};

        value_type::count = {};

        auto buf = TestFixture::make_buffer();
        external_vector<value_type> const vec{buf, local_values.begin(), local_values.end()};

        ASSERT_EQ(value_type::count.copy_ctor, 3);
        ASSERT_THAT(vec, ::testing::ElementsAre(1, 2, 3));

        auto buf2 = TestFixture::make_buffer();
        value_type::count = {};
        ASSERT_EQ(vec, external_vector<value_type>::try_construct(buf2, local_values.begin(), local_values.end()));
        ASSERT_EQ(value_type::count.copy_ctor, 3);
      }
  );
}

/// @test An @c external_vector can be constructed from a range specified by iterators
/// to a convertible type
TYPED_TEST(ExternalVector, ConversionConstructibleIteratorRange) {
  using value_type = typename TypeParam::value_type;

  value_type::count = {};

  auto buf = TestFixture::make_buffer();
  external_vector<value_type> const vec{buf, values.begin(), values.end()};

  ASSERT_EQ(value_type::count.copy_ctor, 0);
  ASSERT_TRUE(elements_are(vec, {1, 2, 3}));

  auto buf2 = TestFixture::make_buffer();
  value_type::count = {};
  ASSERT_EQ(vec, external_vector<value_type>::try_construct(buf2, values.begin(), values.end()));
  ASSERT_EQ(value_type::count.copy_ctor, 0);
}

/// @test An @c external_vector of default constructed values can be constructed from a count
TYPED_TEST(ExternalVector, CountDefaultConstructedValues) {
  do_conditional_test(
      std::is_default_constructible<typename TypeParam::value_type>{},
      std::add_pointer_t<TypeParam>{},
      [](auto* dummy) {
        using T = std::remove_pointer_t<decltype(dummy)>;
        using value_type = typename T::value_type;

        value_type::count = {};

        auto buf = TestFixture::make_buffer();
        external_vector<value_type> const vec{buf, 3};

        ASSERT_EQ(value_type::count.default_ctor, 3);
        ASSERT_TRUE(elements_are(vec, {{}, {}, {}}));

        auto buf2 = TestFixture::make_buffer();
        value_type::count = {};
        ASSERT_EQ(vec, external_vector<value_type>::try_construct(buf2, 3));
        ASSERT_EQ(value_type::count.default_ctor, 3);
      }
  );
}

/// @test An @c external_vector can be constructed from a count and copies of a value
TYPED_TEST(ExternalVector, CountCopyConstructedValues) {
  do_conditional_test(
      std::is_copy_constructible<typename TypeParam::value_type>{},
      std::add_pointer_t<TypeParam>{},
      [](auto* dummy) {
        using T = std::remove_pointer_t<decltype(dummy)>;
        using value_type = typename T::value_type;

        value_type::count = {};

        auto buf = TestFixture::make_buffer();
        external_vector<value_type> const vec{buf, 3, 4};

        ASSERT_EQ(value_type::count.copy_ctor, 3);
        ASSERT_THAT(vec, ::testing::ElementsAre(4, 4, 4));

        auto buf2 = TestFixture::make_buffer();
        value_type::count = {};
        ASSERT_EQ(vec, external_vector<value_type>::try_construct(buf2, 3, 4));
        ASSERT_EQ(value_type::count.copy_ctor, 3);
      }
  );
}

/// @test An @c external_vector can be constructed by moving values from an
/// @c external_vector managing a different buffer
TYPED_TEST(ExternalVector, ConstructByDeepMoveOfValues) {
  do_conditional_test(
      std::is_move_constructible<typename TypeParam::value_type>{},
      std::add_pointer_t<TypeParam>{},
      [](auto* dummy) {
        using T = std::remove_pointer_t<decltype(dummy)>;
        using value_type = typename T::value_type;

        auto buf1 = TestFixture::make_buffer();
        external_vector<value_type> vec1{buf1, values.begin(), values.end()};

        value_type::count = {};

        auto buf2 = TestFixture::make_buffer();
        external_vector<value_type> const vec2{buf2, std::move(vec1)};

        ASSERT_EQ(value_type::count.move_ctor, 3);
        ASSERT_TRUE(elements_are(vec2, {1, 2, 3}));

        auto buf3 = TestFixture::make_buffer();
        auto buf4 = TestFixture::make_buffer();
        value_type::count = {};
        ASSERT_EQ(
            vec2,
            (external_vector<value_type>::try_construct(
                buf4,
                external_vector<value_type>{buf3, values.begin(), values.end()}
            ))
        );
        ASSERT_EQ(value_type::count.move_ctor, 3);
      }
  );
}

/// @test An @c external_vector be constructed by copying values from an
/// @c external_vector managing a different buffer
TYPED_TEST(ExternalVector, ConstructByDeepCopyOfValues) {
  do_conditional_test(
      std::is_copy_constructible<typename TypeParam::value_type>{},
      std::add_pointer_t<TypeParam>{},
      [](auto* dummy) {
        using T = std::remove_pointer_t<decltype(dummy)>;
        using value_type = typename T::value_type;

        auto buf1 = TestFixture::make_buffer();
        external_vector<value_type> const vec1{buf1, {1, 2, 3}};

        value_type::count = {};

        auto buf2 = TestFixture::make_buffer();
        external_vector<value_type> const vec2{buf2, vec1};

        ASSERT_EQ(value_type::count.copy_ctor, 3);
        ASSERT_THAT(vec2, ::testing::ElementsAre(1, 2, 3));

        auto buf3 = TestFixture::make_buffer();
        value_type::count = {};
        ASSERT_EQ(vec2, (external_vector<value_type>::try_construct(buf3, vec1)));
        ASSERT_EQ(value_type::count.copy_ctor, 3);
      }
  );
}

/// @test An @c external_vector be constructed without specifying a buffer by
/// moving another @c external_vector
TYPED_TEST(ExternalVector, ConstructWithBufferByMoving) {
  using value_type = typename TypeParam::value_type;

  auto buf = TestFixture::make_buffer();
  external_vector<value_type> vec1{buf, values.begin(), values.end()};

  value_type::count = {};

  external_vector<value_type> vec2{std::move(vec1)};

  ASSERT_EQ(value_type::count.copy_ctor, 0);
  ASSERT_EQ(value_type::count.move_ctor, 0);
  ASSERT_TRUE(elements_are(vec2, {1, 2, 3}));

  auto const vec3 = external_vector<value_type>::try_construct(std::move(vec2));
  ASSERT_EQ(value_type::count.copy_ctor, 0);
  ASSERT_EQ(value_type::count.move_ctor, 0);
  ASSERT_TRUE(elements_are(*vec3, {1, 2, 3}));
}

/// @test An @c external_vector can be constructed from an @c initializer_list
TYPED_TEST(ExternalVector, ConstructibleFromInitializerList) {
  do_conditional_test(
      std::is_copy_constructible<typename TypeParam::value_type>{},
      std::add_pointer_t<TypeParam>{},
      [](auto* dummy) {
        using T = std::remove_pointer_t<decltype(dummy)>;
        using value_type = typename T::value_type;

        value_type::count = {};

        auto buf = TestFixture::make_buffer();
        external_vector<value_type> const vec{buf, {1, 2, 3}};

        ASSERT_EQ(value_type::count.copy_ctor, 3);
        ASSERT_THAT(vec, ::testing::ElementsAre(1, 2, 3));

        auto buf2 = TestFixture::make_buffer();
        value_type::count = {};
        ASSERT_EQ(vec, external_vector<value_type>::try_construct(buf2, {1, 2, 3}));
        ASSERT_EQ(value_type::count.copy_ctor, 3);
      }
  );
}

/// @test An @c external_vector provides const and non-const begin and end members
TYPED_TEST(ExternalVector, BeginAndEnd) {
  using value_type = typename TypeParam::value_type;

  auto buf = TestFixture::make_buffer();
  external_vector<value_type> vec{buf};

  static_assert(std::is_same<value_type&, decltype(*vec.begin())>::value, "");
  static_assert(std::is_same<value_type&, decltype(*vec.end())>::value, "");
  static_assert(std::is_same<value_type const&, decltype(*::arene::base::as_const(vec).begin())>::value, "");
  static_assert(std::is_same<value_type const&, decltype(*::arene::base::as_const(vec).end())>::value, "");

  ASSERT_EQ(vec.begin(), ::arene::base::as_const(vec).begin());
  ASSERT_EQ(vec.end(), ::arene::base::as_const(vec).end());
}

/// @test The capacity of an @c external_vector matches the buffer capacity
TYPED_TEST(ExternalVector, CapacityMatchesBuffer) {
  using value_type = typename TypeParam::value_type;
  static constexpr auto capacity = TypeParam::capacity;

  auto buf = TestFixture::make_buffer();
  external_vector<value_type> const vec{buf};

  ASSERT_EQ(vec.capacity(), capacity);
}

/// @test @c begin matches @c end for an empty @c external_vector
TYPED_TEST(ExternalVector, BeginMatchesEndForEmpty) {
  using value_type = typename TypeParam::value_type;

  auto buf = TestFixture::make_buffer();
  external_vector<value_type> vec{buf};

  ASSERT_EQ(vec.size(), 0);
  ASSERT_EQ(vec.begin(), vec.end());
  ASSERT_EQ(::arene::base::as_const(vec).begin(), ::arene::base::as_const(vec).end());
}

/// @test An empty @c external_vector is constructible with an empty buffer
TYPED_TEST(ExternalVector, EmptyBuffer) {
  using value_type = typename TypeParam::value_type;

  auto buf = TestFixture::make_buffer(constant<0>);
  external_vector<value_type> const vec{buf};

  ASSERT_EQ(vec.size(), 0);
  ASSERT_EQ(vec.capacity(), 0);
}

/// @test when move assigning an empty @c external_vector, assignment moves values with move construction
TYPED_TEST(ExternalVector, MoveAssignmentWithConstructedValues) {
  do_conditional_test(is_movable<typename TypeParam::value_type>{}, std::add_pointer_t<TypeParam>{}, [](auto* dummy) {
    using T = std::remove_pointer_t<decltype(dummy)>;
    using value_type = typename T::value_type;

    auto buf1 = TestFixture::make_buffer();
    auto buf2 = TestFixture::make_buffer();
    external_vector<value_type> vec1{buf1, values.begin(), values.end()};
    external_vector<value_type> vec2{buf2};

    value_type::count = {};

    vec2 = std::move(vec1);
    ASSERT_EQ(value_type::count.move_ctor, 3);
    ASSERT_TRUE(elements_are(vec2, {1, 2, 3}));
  });
}

/// @test when move assigning a larger @c external_vector, assignment moves values with move assigment
TYPED_TEST(ExternalVector, MoveAssignmentWithAssignedValues) {
  do_conditional_test(
      std::integral_constant < bool,
      std::is_default_constructible<typename TypeParam::value_type>::value&&
              is_movable<typename TypeParam::value_type>::value > {},
      std::add_pointer_t<TypeParam>{},
      [](auto* dummy) {
        using T = std::remove_pointer_t<decltype(dummy)>;
        using value_type = typename T::value_type;

        auto buf1 = TestFixture::make_buffer();
        auto buf2 = TestFixture::make_buffer(constant<10>);
        external_vector<value_type> vec1{buf1, values.begin(), values.end()};
        external_vector<value_type> vec2{buf2, 10};

        value_type::count = {};

        vec2 = std::move(vec1);
        ASSERT_EQ(value_type::count.move_assign, 3);
        ASSERT_TRUE(elements_are(vec2, {1, 2, 3}));
      }
  );
}

/// @test when copy assigning an empty @c external_vector, assignment copies values with copy construction
TYPED_TEST(ExternalVector, CopyAssignmentWithConstructedValues) {
  do_conditional_test(is_copyable<typename TypeParam::value_type>{}, std::add_pointer_t<TypeParam>{}, [](auto* dummy) {
    using T = std::remove_pointer_t<decltype(dummy)>;
    using value_type = typename T::value_type;

    auto buf1 = TestFixture::make_buffer();
    auto buf2 = TestFixture::make_buffer(constant<10>);
    external_vector<value_type> const vec1{buf1, values.begin(), values.end()};
    external_vector<value_type> vec2{buf2};

    value_type::count = {};

    vec2 = vec1;
    ASSERT_EQ(value_type::count.copy_ctor, 3);
    ASSERT_TRUE(elements_are(vec2, {1, 2, 3}));
  });
}

/// @test when copy assigning a larger @c external_vector, assignment copies values with copy assignment
TYPED_TEST(ExternalVector, CopyAssignmentWithAssignedValues) {
  do_conditional_test(
      std::integral_constant < bool,
      std::is_default_constructible<typename TypeParam::value_type>::value&&
              is_copyable<typename TypeParam::value_type>::value > {},
      std::add_pointer_t<TypeParam>{},
      [](auto* dummy) {
        using T = std::remove_pointer_t<decltype(dummy)>;
        using value_type = typename T::value_type;

        auto buf1 = TestFixture::make_buffer();
        auto buf2 = TestFixture::make_buffer(constant<10>);
        external_vector<value_type> const vec1{buf1, values.begin(), values.end()};
        external_vector<value_type> vec2{buf2, 10};

        value_type::count = {};

        vec2 = vec1;
        ASSERT_EQ(value_type::count.copy_assign, 3);
        ASSERT_TRUE(elements_are(vec2, {1, 2, 3}));
      }
  );
}

/// @test when assigning an empty @c external_vector from an @c std::initializer_list, assignment copies values with
/// copy construction
TYPED_TEST(ExternalVector, AssignmentFromInitializerListWithConstructedValues) {
  do_conditional_test(is_copyable<typename TypeParam::value_type>{}, std::add_pointer_t<TypeParam>{}, [](auto* dummy) {
    using T = std::remove_pointer_t<decltype(dummy)>;
    using value_type = typename T::value_type;

    auto buf = TestFixture::make_buffer(constant<10>);
    external_vector<value_type> vec{buf};

    value_type::count = {};

    vec = {1, 2, 3};
    ASSERT_EQ(value_type::count.copy_ctor, 3);
    ASSERT_TRUE(elements_are(vec, {1, 2, 3}));
  });
}

/// @test when assigning a larger @c external_vector from an @c std::initializer_list, assignment copies values with
/// copy assignment
TYPED_TEST(ExternalVector, AssignmentFromInitializerListWithAssignedValues) {
  do_conditional_test(
      std::integral_constant < bool,
      std::is_default_constructible<typename TypeParam::value_type>::value&&
              is_copyable<typename TypeParam::value_type>::value > {},
      std::add_pointer_t<TypeParam>{},
      [](auto* dummy) {
        using T = std::remove_pointer_t<decltype(dummy)>;
        using value_type = typename T::value_type;

        auto buf = TestFixture::make_buffer(constant<10>);
        external_vector<value_type> vec{buf, 10};

        value_type::count = {};

        vec = {1, 2, 3};
        ASSERT_EQ(value_type::count.copy_assign, 3);
        ASSERT_TRUE(elements_are(vec, {1, 2, 3}));
      }
  );
}

/// @test not move assignable if the value type is not movable
TYPED_TEST(ExternalVector, ConditionalMoveAssignment) {
  using value_type = typename TypeParam::value_type;

  static_assert(
      std::is_move_assignable<external_vector<value_type>>::value == is_movable<value_type>::value,
      "should match"
  );
}

/// @test not copy assignable if the value type is not copyable
TYPED_TEST(ExternalVector, ConditionalCopyAssignment) {
  using value_type = typename TypeParam::value_type;

  static_assert(
      std::is_copy_assignable<external_vector<value_type>>::value == is_copyable<value_type>::value,
      "should match"
  );
}

/// @test not assignable from a @c std::initializer_list if the value type is not copyable
TYPED_TEST(ExternalVector, ConditionalInitializerListAssignment) {
  using value_type = typename TypeParam::value_type;

  static_assert(
      std::is_assignable<external_vector<value_type>&, std::initializer_list<value_type>>::value ==
          is_copyable<value_type>::value,
      "should match"
  );
}

/// @test @c external_vector values are always swappable with member swap
TYPED_TEST(ExternalVector, MemberSwap) {
  using value_type = typename TypeParam::value_type;

  auto buf = TestFixture::make_buffer();
  external_vector<value_type> vec1{buf, values.begin(), values.end()};
  external_vector<value_type> vec2{};

  vec1.swap(vec2);

  ASSERT_EQ(vec1.size(), 0);
  ASSERT_EQ(vec1.capacity(), 0);

  ASSERT_EQ(vec2.size(), 3);
  ASSERT_EQ(static_cast<void*>(&*vec2.begin()), static_cast<void*>(buf.data()));
}

/// @test @c external_vector values are always swappable with adl swap
TYPED_TEST(ExternalVector, AdlSwap) {
  using value_type = typename TypeParam::value_type;

  auto buf = TestFixture::make_buffer();
  external_vector<value_type> vec1{buf, values.begin(), values.end()};
  external_vector<value_type> vec2{};

  swap(vec1, vec2);

  ASSERT_EQ(vec1.size(), 0);
  ASSERT_EQ(vec1.capacity(), 0);

  ASSERT_EQ(vec2.size(), 3);
  ASSERT_EQ(static_cast<void*>(&*vec2.begin()), static_cast<void*>(buf.data()));
}

/// @test Use of an unaligned buffer results in a precondition failure
TYPED_TEST(ExternalVectorDeathTest, UnalignedBuffer) {
  using value_type = typename TypeParam::value_type;
  static constexpr auto capacity = TypeParam::capacity;

  auto buf = TestFixture::make_buffer(constant<1 + capacity>);
  auto const span = static_cast<::arene::base::span<::arene::base::byte>>(buf);

  ASSERT_DEATH(
      (external_vector<value_type>{span.subspan(1, 3 * sizeof(value_type))}),
      "Precondition violation: is_aligned"
  );
}

/// @test Use of an aligned buffer with padding results in a precondition failure
TYPED_TEST(ExternalVectorDeathTest, PaddedBuffer) {
  using value_type = typename TypeParam::value_type;
  static constexpr auto capacity = TypeParam::capacity;

  auto buf = TestFixture::make_buffer(constant<1 + capacity>);
  auto const span = static_cast<::arene::base::span<::arene::base::byte>>(buf);

  ASSERT_DEATH((external_vector<value_type>{span.first(1 + 3 * sizeof(value_type))}), "Precondition violation:");
}

/// @test Construction with a buffer that is too small for the initial values
/// results in a precondition failure
TYPED_TEST(ExternalVectorDeathTest, BufferTooSmall) {
  using value_type = typename TypeParam::value_type;

  auto buf = TestFixture::make_buffer(constant<1>);

  ASSERT_DEATH((external_vector<value_type>{buf, values.begin(), values.end()}), "Precondition violation:");
}

/// @test Construction of an @c external_vector from a buffer and moving another
/// @c external_vector is a precondition violation if the same buffer is given.
TYPED_TEST(ExternalVectorDeathTest, ReusedBufferForMove) {
  do_conditional_test(
      std::is_move_constructible<typename TypeParam::value_type>{},
      std::add_pointer_t<TypeParam>{},
      [](auto* dummy) {
        using T = std::remove_pointer_t<decltype(dummy)>;
        using value_type = typename T::value_type;

        auto buf = TestFixture::make_buffer(constant<1>);

        auto vec = external_vector<value_type>{buf};

        ASSERT_DEATH((external_vector<value_type>{buf, std::move(vec)}), "Precondition violation:");
      }
  );
}

/// @test Construction of an @c external_vector from a buffer and copying another
/// @c external_vector is a precondition violation if the same buffer is given.
TYPED_TEST(ExternalVectorDeathTest, ReusedBufferForCopy) {
  do_conditional_test(
      std::is_copy_constructible<typename TypeParam::value_type>{},
      std::add_pointer_t<TypeParam>{},
      [](auto* dummy) {
        using T = std::remove_pointer_t<decltype(dummy)>;
        using value_type = typename T::value_type;

        auto buf = TestFixture::make_buffer(constant<1>);

        auto const vec = external_vector<value_type>{buf};

        ASSERT_DEATH((external_vector<value_type>{buf, vec}), "Precondition violation:");
      }
  );
}

/// @test Move assignment of an @c external_vector from another @c external_vector
/// is a precondition violation if the buffer is too small
TYPED_TEST(ExternalVectorDeathTest, MoveAssignmentBufferTooSmall) {
  do_conditional_test(is_movable<typename TypeParam::value_type>{}, std::add_pointer_t<TypeParam>{}, [](auto* dummy) {
    using T = std::remove_pointer_t<decltype(dummy)>;
    using value_type = typename T::value_type;

    auto buf = TestFixture::make_buffer();

    auto vec1 = external_vector<value_type>{buf, values.begin(), values.end()};
    auto vec2 = external_vector<value_type>{};

    ASSERT_DEATH(vec2 = std::move(vec1), "Precondition violation:");
  });
}

/// @test Copy assignment of an @c external_vector from another @c external_vector
/// is a precondition violation if the buffer is too small
TYPED_TEST(ExternalVectorDeathTest, CopyAssignmentBufferTooSmall) {
  do_conditional_test(is_copyable<typename TypeParam::value_type>{}, std::add_pointer_t<TypeParam>{}, [](auto* dummy) {
    using T = std::remove_pointer_t<decltype(dummy)>;
    using value_type = typename T::value_type;

    auto buf = TestFixture::make_buffer();

    auto const vec1 = external_vector<value_type>{buf, values.begin(), values.end()};
    auto vec2 = external_vector<value_type>{};

    ASSERT_DEATH(vec2 = vec1, "Precondition violation:");
  });
}

/// @test observer functions are always @c noexcept
TYPED_TEST(ExternalVector, ObserversNoexcept) {
  using value_type = typename TypeParam::value_type;

  using vector_type = external_vector<value_type>;

  static_assert(
      noexcept(std::declval<vector_type&>().begin())&&        //
      noexcept(std::declval<vector_type&>().end())&&          //
      noexcept(std::declval<vector_type const&>().begin())&&  //
      noexcept(std::declval<vector_type const&>().end())&&    //
      noexcept(std::declval<vector_type const&>().size())&&   //
      noexcept(std::declval<vector_type const&>().capacity()),
      "should be noexcept"
  );
}

/// @test constructors that initialize an empty @c external_vector are always @c noexcept
TYPED_TEST(ExternalVector, EmptyConstructorsNoexcept) {
  using value_type = typename TypeParam::value_type;

  using vector_type = external_vector<value_type>;

  static_assert(std::is_nothrow_constructible<vector_type>::value, "should be noexcept");

  static_assert(
      std::is_nothrow_constructible<vector_type, ::arene::base::span<::arene::base::byte>>::value,
      "should be noexcept"
  );
}

/// @test constructors that default construct values are @c noexcept if the value type is nothrow_default_constructible
TYPED_TEST(ExternalVector, DefaultConstructionBasedConstructorsNoexcept) {
  using value_type = typename TypeParam::value_type;

  using vector_type = external_vector<value_type>;

  static_assert(
      std::is_nothrow_default_constructible<value_type>::value ==  //
          std::is_nothrow_constructible<vector_type, ::arene::base::span<::arene::base::byte>, std::size_t>::value,
      "should match"
  );
}

/// @test constructors that move values are @c noexcept if the value type is nothrow_move_constructible
TYPED_TEST(ExternalVector, MoveBasedConstructorsNoexcept) {
  using value_type = typename TypeParam::value_type;

  using vector_type = external_vector<value_type>;

  static_assert(
      std::is_nothrow_move_constructible<value_type>::value ==  //
          std::is_nothrow_constructible<vector_type, ::arene::base::span<::arene::base::byte>, vector_type&&>::value,
      "should match"
  );
}

/// @test constructors that copy values are @c noexcept if the value type is nothrow_copy_constructible
TYPED_TEST(ExternalVector, CopyBasedConstructorsNoexcept) {
  using value_type = typename TypeParam::value_type;

  using vector_type = external_vector<value_type>;

  static_assert(
      std::is_nothrow_copy_constructible<value_type>::value ==  //
          std::is_nothrow_constructible<
              vector_type,
              ::arene::base::span<::arene::base::byte>,
              value_type const*,
              value_type const*>::value,
      "should match"
  );

  static_assert(
      std::is_nothrow_copy_constructible<value_type>::value ==  //
          std::is_nothrow_constructible<
              vector_type,
              ::arene::base::span<::arene::base::byte>,
              std::size_t,
              value_type const&>::value,
      "should match"
  );

  static_assert(
      std::is_nothrow_copy_constructible<value_type>::value ==  //
          std::is_nothrow_constructible<
              vector_type,
              ::arene::base::span<::arene::base::byte>,
              std::initializer_list<value_type>>::value,
      "should match"
  );

  static_assert(
      std::is_nothrow_copy_constructible<value_type>::value ==  //
          std::is_nothrow_constructible<vector_type, ::arene::base::span<::arene::base::byte>, vector_type const&>::
              value,
      "should match"
  );
}

/// @test assignment operators are conditionally @c noexcept
TYPED_TEST(ExternalVector, AssignmentNoexcept) {
  using value_type = typename TypeParam::value_type;

  using vector_type = external_vector<value_type>;

  static_assert(
      (std::is_nothrow_move_constructible<value_type>::value && std::is_nothrow_move_assignable<value_type>::value) ==
          std::is_nothrow_move_assignable<vector_type>::value,
      "should match"
  );

  static_assert(
      (std::is_nothrow_copy_constructible<value_type>::value && std::is_nothrow_copy_assignable<value_type>::value) ==
          std::is_nothrow_copy_assignable<vector_type>::value,
      "should match"
  );

  static_assert(
      (std::is_nothrow_copy_constructible<value_type>::value && std::is_nothrow_copy_assignable<value_type>::value) ==
          std::is_nothrow_assignable<vector_type&, std::initializer_list<value_type>>::value,
      "should match"
  );
}

/// @test swap functions are always @c noexcept
TYPED_TEST(ExternalVector, SwapNoexcept) {
  using value_type = typename TypeParam::value_type;

  using vector_type = external_vector<value_type>;

  static_assert(noexcept(std::declval<vector_type&>().swap(std::declval<vector_type&>())), "should be noexcept");
  static_assert(noexcept(swap(std::declval<vector_type&>(), std::declval<vector_type&>())), "should be noexcept");
}

/// @test values are destroyed when the @c external_vector is destroyed
TEST(ExternalVector, ValuesDestroyedWhenVectorIsDestroyed) {
  using value_type = ::testing::counting_wrapper<int>;

  auto buf = ::arene::base::array<::arene::base::byte, 5 * sizeof(value_type)>{};

  value_type::count = {};

  {
    ASSERT_EQ(value_type::count.dtor, 0);

    external_vector<value_type> const vec{buf, {1, 2, 3}};

    // elements in the initializer list are destroyed at the end of the previous expression
    ASSERT_EQ(value_type::count.dtor, 3);

    ASSERT_EQ(vec.size(), 3);
  }

  ASSERT_EQ(value_type::count.dtor, 6);
}

/// @test iterator range constructors are not invocable if the value type is not constructible from the iterator
/// reference type
TEST(ExternalVector, IteratorRangeConstructorRequiresConstructiblityFromReference) {
  struct dummy {};

  static_assert(
      !std::is_constructible<external_vector<int>, ::arene::base::span<::arene::base::byte>, dummy*, dummy*>::value,
      "should not be constructible"
  );
}

/// @test @c initializer_list constructors are not invocable if the value type is not copy constructible
TEST(ExternalVector, InitializerListConstructorRequiresCopyConstructible) {
  // NOLINTNEXTLINE(hicpp-special-member-functions)
  struct not_copy_constructible {
    not_copy_constructible() = default;
    not_copy_constructible(not_copy_constructible const&) = delete;
  };

  static_assert(
      !std::is_constructible<
          external_vector<not_copy_constructible>,
          ::arene::base::span<::arene::base::byte>,
          std::initializer_list<not_copy_constructible>>::value,
      "should not be constructible"
  );
}

template <class T>
struct ExternalVectorTryConstruct : ExternalVector<T> {
  using external_vector = ::arene::base::external_vector<typename T::value_type>;

  struct try_construct_fn {
    template <class... Args>
    auto operator()(Args&&...) const noexcept(noexcept(external_vector::try_construct(std::declval<Args&&>()...)))
        -> decltype(external_vector::try_construct(std::declval<Args&&>()...)) {
      return {};
    }
  };

  template <class... Args>
  static constexpr auto is_try_constructible_v = ::arene::base::is_invocable_v<try_construct_fn, Args...>;

  template <class... Args>
  static constexpr auto is_nothrow_try_constructible_v =
      ::arene::base::is_nothrow_invocable_v<try_construct_fn, Args...>;

  auto SetUp() -> void override { T::value_type::count = {}; }
};

TYPED_TEST_SUITE(ExternalVectorTryConstruct, value_types, );

/// @test each @c try_construct overload is not invocable if the associated constructor is not invocable
TYPED_TEST(ExternalVectorTryConstruct, Invocable) {
  using value_type = typename TypeParam::value_type;
  using vector_type = external_vector<value_type>;

  static_assert(
      std::is_constructible<vector_type>::value ==  //
          TestFixture::template is_try_constructible_v<>,
      "should match"
  );
  static_assert(
      std::is_move_constructible<vector_type>::value ==  //
          TestFixture::template is_try_constructible_v<vector_type&&>,
      "should match"
  );
  static_assert(
      std::is_copy_constructible<vector_type>::value ==  //
          TestFixture::template is_try_constructible_v<vector_type const&>,
      "should match"
  );

  using buffer_type = ::arene::base::span<::arene::base::byte>;
  using converting_iterator = int const*;
  using copying_iterator = value_type const*;
  using moving_iterator = std::move_iterator<value_type*>;
  using converting_input_iterator = std::istream_iterator<int const*>;
  using copying_input_iterator = std::istream_iterator<value_type const*>;
  using moving_input_iterator = std::move_iterator<std::istream_iterator<value_type*>>;
  using size_type = typename vector_type::size_type;

  static_assert(
      std::is_constructible<vector_type, buffer_type>::value ==  //
          TestFixture::template is_try_constructible_v<buffer_type>,
      "should match"
  );
  static_assert(
      std::is_constructible<vector_type, buffer_type, converting_iterator, converting_iterator>::value ==
          TestFixture::template is_try_constructible_v<buffer_type, converting_iterator, converting_iterator>,
      "should match"
  );
  static_assert(
      std::is_constructible<vector_type, buffer_type, copying_iterator, copying_iterator>::value ==
          TestFixture::template is_try_constructible_v<buffer_type, copying_iterator, copying_iterator>,
      "should match"
  );
  static_assert(
      std::is_constructible<vector_type, buffer_type, moving_iterator, moving_iterator>::value ==
          TestFixture::template is_try_constructible_v<buffer_type, moving_iterator, moving_iterator>,
      "should match"
  );
  static_assert(
      std::is_constructible<vector_type, buffer_type, converting_input_iterator, converting_input_iterator>::value ==
          TestFixture::
              template is_try_constructible_v<buffer_type, converting_input_iterator, converting_input_iterator>,
      "should match"
  );
  static_assert(
      std::is_constructible<vector_type, buffer_type, copying_input_iterator, copying_input_iterator>::value ==
          TestFixture::template is_try_constructible_v<buffer_type, copying_input_iterator, copying_input_iterator>,
      "should match"
  );
  static_assert(
      std::is_constructible<vector_type, buffer_type, moving_input_iterator, moving_input_iterator>::value ==
          TestFixture::template is_try_constructible_v<buffer_type, moving_input_iterator, moving_input_iterator>,
      "should match"
  );
  static_assert(
      std::is_constructible<vector_type, buffer_type, size_type>::value ==
          TestFixture::template is_try_constructible_v<buffer_type, size_type>,
      "should match"
  );
  static_assert(
      std::is_constructible<vector_type, buffer_type, size_type, value_type const&>::value ==
          TestFixture::template is_try_constructible_v<buffer_type, size_type, value_type const&>,
      "should match"
  );
  static_assert(
      std::is_constructible<vector_type, buffer_type, vector_type&&>::value ==
          TestFixture::template is_try_constructible_v<buffer_type, vector_type&&>,
      "should match"
  );
  static_assert(
      std::is_constructible<vector_type, buffer_type, vector_type const&>::value ==
          TestFixture::template is_try_constructible_v<buffer_type, vector_type const&>,
      "should match"
  );
  static_assert(
      std::is_constructible<vector_type, buffer_type, std::initializer_list<value_type>>::value ==
          TestFixture::template is_try_constructible_v<buffer_type, std::initializer_list<value_type>>,
      "should match"
  );
}

/// @test each @c try_construct overload is noexcept if the associated constructor is noexcept
TYPED_TEST(ExternalVectorTryConstruct, Noexcept) {
  using value_type = typename TypeParam::value_type;
  using vector_type = external_vector<value_type>;

  static_assert(
      std::is_nothrow_constructible<vector_type>::value ==  //
          TestFixture::template is_nothrow_try_constructible_v<>,
      "should match"
  );
  static_assert(
      std::is_nothrow_move_constructible<vector_type>::value ==  //
          TestFixture::template is_nothrow_try_constructible_v<vector_type&&>,
      "should match"
  );
  static_assert(
      std::is_nothrow_copy_constructible<vector_type>::value ==  //
          TestFixture::template is_nothrow_try_constructible_v<vector_type const&>,
      "should match"
  );

  using buffer_type = ::arene::base::span<::arene::base::byte>;
  using converting_iterator = int const*;
  using copying_iterator = value_type const*;
  using moving_iterator = std::move_iterator<value_type*>;
  using converting_input_iterator = std::istream_iterator<int const*>;
  using copying_input_iterator = std::istream_iterator<value_type const*>;
  using moving_input_iterator = std::move_iterator<std::istream_iterator<value_type*>>;
  using size_type = typename vector_type::size_type;

  static_assert(
      std::is_nothrow_constructible<vector_type, buffer_type>::value ==  //
          TestFixture::template is_nothrow_try_constructible_v<buffer_type>,
      "should match"
  );
  static_assert(
      std::is_nothrow_constructible<vector_type, buffer_type, converting_iterator, converting_iterator>::value ==
          TestFixture::template is_nothrow_try_constructible_v<buffer_type, converting_iterator, converting_iterator>,
      "should match"
  );
  static_assert(
      std::is_nothrow_constructible<vector_type, buffer_type, copying_iterator, copying_iterator>::value ==
          TestFixture::template is_nothrow_try_constructible_v<buffer_type, copying_iterator, copying_iterator>,
      "should match"
  );
  static_assert(
      std::is_nothrow_constructible<vector_type, buffer_type, moving_iterator, moving_iterator>::value ==
          TestFixture::template is_nothrow_try_constructible_v<buffer_type, moving_iterator, moving_iterator>,
      "should match"
  );
  static_assert(
      std::is_nothrow_constructible<vector_type, buffer_type, converting_input_iterator, converting_input_iterator>::
              value == TestFixture::template is_nothrow_try_constructible_v<
                           buffer_type,
                           converting_input_iterator,
                           converting_input_iterator>,
      "should match"
  );
  static_assert(
      std::is_nothrow_constructible<vector_type, buffer_type, copying_input_iterator, copying_input_iterator>::value ==
          TestFixture::
              template is_nothrow_try_constructible_v<buffer_type, copying_input_iterator, copying_input_iterator>,
      "should match"
  );
  static_assert(
      std::is_nothrow_constructible<vector_type, buffer_type, moving_input_iterator, moving_input_iterator>::value ==
          TestFixture::
              template is_nothrow_try_constructible_v<buffer_type, moving_input_iterator, moving_input_iterator>,
      "should match"
  );
  static_assert(
      std::is_nothrow_constructible<vector_type, buffer_type, size_type>::value ==
          TestFixture::template is_nothrow_try_constructible_v<buffer_type, size_type>,
      "should match"
  );
  static_assert(
      std::is_nothrow_constructible<vector_type, buffer_type, size_type, value_type const&>::value ==
          TestFixture::template is_nothrow_try_constructible_v<buffer_type, size_type, value_type const&>,
      "should match"
  );
  static_assert(
      std::is_nothrow_constructible<vector_type, buffer_type, vector_type&&>::value ==
          TestFixture::template is_nothrow_try_constructible_v<buffer_type, vector_type&&>,
      "should match"
  );
  static_assert(
      std::is_nothrow_constructible<vector_type, buffer_type, vector_type const&>::value ==
          TestFixture::template is_nothrow_try_constructible_v<buffer_type, vector_type const&>,
      "should match"
  );
  static_assert(
      std::is_nothrow_constructible<vector_type, buffer_type, std::initializer_list<value_type>>::value ==
          TestFixture::template is_nothrow_try_constructible_v<buffer_type, std::initializer_list<value_type>>,
      "should match"
  );
}

/// @test @c external_vector::try_construct returns an engaged optional if the
/// number of elements in the input iterator-pair range is within capacity
TYPED_TEST(ExternalVectorTryConstruct, InputIteratorRangeSucceeds) {
  using value_type = typename TypeParam::value_type;

  auto str = std::istringstream{"1 2 3"};

  auto buf = TestFixture::make_buffer();
  auto vec =
      external_vector<value_type>::try_construct(buf, std::istream_iterator<int>(str), std::istream_iterator<int>());

  ASSERT_TRUE(vec);

  auto test_count = typename value_type::count_t{};
  test_count.value_ctor = 3;

  // possible copy/move elision in this test case
  value_type::count.move_ctor = 0;
  value_type::count.copy_ctor = 0;

  ASSERT_EQ(value_type::count, test_count);

  ASSERT_TRUE(elements_are(*vec, {1, 2, 3}));
}

/// @test @c external_vector::try_construct returns an empty optional if the
/// input iterator-pair range has too many elements
TYPED_TEST(ExternalVectorTryConstruct, InputIteratorRangeFails) {
  using value_type = typename TypeParam::value_type;

  auto str = std::istringstream{"1 2 3"};

  auto buf = TestFixture::make_buffer(constant<1>);
  auto vec =
      external_vector<value_type>::try_construct(buf, std::istream_iterator<int>(str), std::istream_iterator<int>());

  ASSERT_FALSE(vec);

  auto test_count = typename value_type::count_t{};
  test_count.dtor = 1;
  test_count.value_ctor = 1;

  // possible copy/move elision in this test case
  value_type::count.move_ctor = 0;
  value_type::count.copy_ctor = 0;

  ASSERT_EQ(value_type::count, test_count);
}

/// @test @c external_vector::try_construct returns an empty optional if the
/// random access iterator-pair range has too many elements
TYPED_TEST(ExternalVectorTryConstruct, RandomAccessIteratorRange) {
  using value_type = typename TypeParam::value_type;

  auto buf = TestFixture::make_buffer(constant<1>);
  auto vec = external_vector<value_type>::try_construct(buf, values.begin(), values.end());

  ASSERT_FALSE(vec);
  ASSERT_EQ(value_type::count, typename value_type::count_t{});
}

/// @test @c external_vector::try_construct returns an empty optional if the
/// requested size is too large when using the size overload
TYPED_TEST(ExternalVectorTryConstruct, Size) {
  do_conditional_test(
      std::is_default_constructible<typename TypeParam::value_type>{},
      std::add_pointer_t<TypeParam>{},
      [](auto* dummy) {
        using T = std::remove_pointer_t<decltype(dummy)>;
        using value_type = typename T::value_type;

        auto buf = TestFixture::make_buffer(constant<1>);
        auto vec = external_vector<value_type>::try_construct(buf, TypeParam::capacity);

        ASSERT_FALSE(vec);
        ASSERT_EQ(value_type::count, typename value_type::count_t{});
      }
  );
}

/// @test @c external_vector::try_construct returns an empty optional if the
/// requested size is too large when using the size-value overload
TYPED_TEST(ExternalVectorTryConstruct, SizeAndValuetoCopy) {
  do_conditional_test(is_copyable<typename TypeParam::value_type>{}, std::add_pointer_t<TypeParam>{}, [](auto* dummy) {
    using T = std::remove_pointer_t<decltype(dummy)>;
    using value_type = typename T::value_type;

    auto const value = value_type{42};

    value_type::count = {};

    auto buf = TestFixture::make_buffer(constant<1>);
    auto vec = external_vector<value_type>::try_construct(buf, TypeParam::capacity, value);

    ASSERT_FALSE(vec);

    ASSERT_EQ(value_type::count, typename value_type::count_t{});
  });
}

/// @test @c external_vector::try_construct returns an empty optional if the
/// @c external_vector to deep-move from has too many elements
TYPED_TEST(ExternalVectorTryConstruct, MoveWithBufferAndExternalVector) {
  do_conditional_test(is_movable<typename TypeParam::value_type>{}, std::add_pointer_t<TypeParam>{}, [](auto* dummy) {
    using T = std::remove_pointer_t<decltype(dummy)>;
    using value_type = typename T::value_type;

    auto buf1 = TestFixture::make_buffer();
    auto vec1 = external_vector<value_type>{buf1, values.begin(), values.end()};

    value_type::count = {};

    auto buf2 = TestFixture::make_buffer(constant<1>);
    auto const vec2 = external_vector<value_type>::try_construct(buf2, std::move(vec1));

    ASSERT_FALSE(vec2);
    ASSERT_EQ(value_type::count, typename value_type::count_t{});
  });
}

/// @test @c external_vector::try_construct returns an empty optional if the
/// @c external_vector to deep-copy from has too many elements
TYPED_TEST(ExternalVectorTryConstruct, CopyWithBufferAndExternalVector) {
  do_conditional_test(is_copyable<typename TypeParam::value_type>{}, std::add_pointer_t<TypeParam>{}, [](auto* dummy) {
    using T = std::remove_pointer_t<decltype(dummy)>;
    using value_type = typename T::value_type;

    auto buf1 = TestFixture::make_buffer();
    auto const vec1 = external_vector<value_type>{buf1, values.begin(), values.end()};

    value_type::count = {};

    auto buf2 = TestFixture::make_buffer(constant<1>);
    auto const vec2 = external_vector<value_type>::try_construct(buf2, vec1);

    ASSERT_FALSE(vec2);
    ASSERT_EQ(value_type::count, typename value_type::count_t{});
  });
}

/// @test @c external_vector::try_construct returns an empty optional if the
/// @c std::initializer_list has too many elements
TYPED_TEST(ExternalVectorTryConstruct, WithInitializerList) {
  do_conditional_test(is_copyable<typename TypeParam::value_type>{}, std::add_pointer_t<TypeParam>{}, [](auto* dummy) {
    using T = std::remove_pointer_t<decltype(dummy)>;
    using value_type = typename T::value_type;

    auto buf = TestFixture::make_buffer(constant<1>);
    auto const vec = external_vector<value_type>::try_construct(buf, {1, 2, 3});

    ASSERT_FALSE(vec);

    auto test_count = typename value_type::count_t{};
    test_count.dtor = 3;
    test_count.value_ctor = 3;
    ASSERT_EQ(value_type::count, test_count);
  });
}

struct three_way_comparable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr three_way_comparable(int input)
      : val_{input} {}

  friend auto operator<(three_way_comparable const& lhs, three_way_comparable const& rhs) noexcept -> bool {
    return lhs.val_ < rhs.val_;
  }
  friend auto operator==(three_way_comparable const& lhs, three_way_comparable const& rhs) noexcept -> bool {
    return lhs.val_ == rhs.val_;
  }

 private:
  int val_;
};

using three_way_compare_types = ::testing::Types<int, float, three_way_comparable>;

template <class T>
struct ExternalVectorComparisonOperators : testing::Test {
  static constexpr auto vector_capacity = 5;

  ::arene::base::array<::arene::base::byte, vector_capacity * sizeof(T)> base_buf{};
  ::arene::base::array<::arene::base::byte, vector_capacity * sizeof(T)> equal_buf{};
  ::arene::base::array<::arene::base::byte, vector_capacity * sizeof(T)> not_equal_buf{};
  ::arene::base::array<::arene::base::byte, vector_capacity * sizeof(T)> longer_buf{};
  ::arene::base::array<::arene::base::byte, vector_capacity * sizeof(T)> shorter_buf{};
  ::arene::base::array<::arene::base::byte, vector_capacity * sizeof(T)> greater_buf{};
  ::arene::base::array<::arene::base::byte, vector_capacity * sizeof(T)> lesser_buf{};

  ::arene::base::external_vector<T> base_vec{base_buf, {1, 2, 3}};
  ::arene::base::external_vector<T> equal_vec{equal_buf, {1, 2, 3}};
  ::arene::base::external_vector<T> longer_vec{longer_buf, {1, 2, 3, 4}};
  ::arene::base::external_vector<T> shorter_vec{shorter_buf, {1, 2}};
  ::arene::base::external_vector<T> not_equal_vec{not_equal_buf, {1, 2, 4}};
  ::arene::base::external_vector<T> greater_vec{greater_buf, {2, 3, 4}};
  ::arene::base::external_vector<T> lesser_vec{lesser_buf, {0, 1, 2}};
};

TYPED_TEST_SUITE(ExternalVectorComparisonOperators, three_way_compare_types, );

/// @test @c external_vector has the equality operator defined from three_way_compare
TYPED_TEST(ExternalVectorComparisonOperators, EqualityDefinedFromThreeWayCompare) {
  ASSERT_TRUE(this->base_vec == this->equal_vec);
  ASSERT_FALSE(this->base_vec == this->not_equal_vec);
  ASSERT_FALSE(this->base_vec == this->longer_vec);
  ASSERT_FALSE(this->base_vec == this->shorter_vec);
  ASSERT_FALSE(this->base_vec == this->greater_vec);
  ASSERT_FALSE(this->base_vec == this->lesser_vec);
}

/// @test @c external_vector has the inequality operator defined from three_way_compare
TYPED_TEST(ExternalVectorComparisonOperators, InequalityDefinedFromThreeWayCompare) {
  ASSERT_FALSE(this->base_vec != this->equal_vec);
  ASSERT_TRUE(this->base_vec != this->not_equal_vec);
  ASSERT_TRUE(this->base_vec != this->longer_vec);
  ASSERT_TRUE(this->base_vec != this->shorter_vec);
  ASSERT_TRUE(this->base_vec != this->greater_vec);
  ASSERT_TRUE(this->base_vec != this->lesser_vec);
}

/// @test @c external_vector has greater than operator defined from three_way_compare
TYPED_TEST(ExternalVectorComparisonOperators, GreaterDefinedFromThreeWayCompare) {
  ASSERT_FALSE(this->base_vec > this->equal_vec);
  ASSERT_FALSE(this->base_vec > this->not_equal_vec);
  ASSERT_FALSE(this->base_vec > this->longer_vec);
  ASSERT_TRUE(this->base_vec > this->shorter_vec);
  ASSERT_FALSE(this->base_vec > this->greater_vec);
  ASSERT_TRUE(this->base_vec > this->lesser_vec);
}

/// @test @c external_vector has the greater than or equal to operator defined from three_way_compare
TYPED_TEST(ExternalVectorComparisonOperators, GreaterEqualDefinedFromThreeWayCompare) {
  ASSERT_TRUE(this->base_vec >= this->equal_vec);
  ASSERT_FALSE(this->base_vec >= this->not_equal_vec);
  ASSERT_FALSE(this->base_vec >= this->longer_vec);
  ASSERT_TRUE(this->base_vec >= this->shorter_vec);
  ASSERT_FALSE(this->base_vec >= this->greater_vec);
  ASSERT_TRUE(this->base_vec >= this->lesser_vec);
}

/// @test @c external_vector has the less than operator defined from three_way_compare
TYPED_TEST(ExternalVectorComparisonOperators, LessDefinedFromThreeWayCompare) {
  ASSERT_FALSE(this->base_vec < this->equal_vec);
  ASSERT_TRUE(this->base_vec < this->not_equal_vec);
  ASSERT_TRUE(this->base_vec < this->longer_vec);
  ASSERT_FALSE(this->base_vec < this->shorter_vec);
  ASSERT_TRUE(this->base_vec < this->greater_vec);
  ASSERT_FALSE(this->base_vec < this->lesser_vec);
}

/// @test @c external_vector has the less than or equal to operator defined from three_way_compare
TYPED_TEST(ExternalVectorComparisonOperators, LessEqualDefinedFromThreeWayCompare) {
  ASSERT_TRUE(this->base_vec <= this->equal_vec);
  ASSERT_TRUE(this->base_vec <= this->not_equal_vec);
  ASSERT_TRUE(this->base_vec <= this->longer_vec);
  ASSERT_FALSE(this->base_vec <= this->shorter_vec);
  ASSERT_TRUE(this->base_vec <= this->greater_vec);
  ASSERT_FALSE(this->base_vec <= this->lesser_vec);
}

class only_equality {
  int value_{};

 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr only_equality(int input)
      : value_(input) {}
  constexpr auto operator==(only_equality const& rhs) const -> bool { return value_ == rhs.value_; }
};

template <class T>
struct ExternalVectorEqualityOnly : testing::Test {};

using equality_only_types = ::testing::Types<only_equality, testing::has_broken_less_than>;

TYPED_TEST_SUITE(ExternalVectorEqualityOnly, equality_only_types, );

/// @test Two instances of `external_vector` can be compared for equality and inequality, even when the element type
/// only supports equality comparison
TYPED_TEST(ExternalVectorEqualityOnly, CanCompareForEqualityAndInequality) {
  constexpr auto capacity = 5;
  ::arene::base::array<::arene::base::byte, capacity * sizeof(TypeParam)> buf{};
  ::arene::base::array<::arene::base::byte, capacity * sizeof(TypeParam)> buf2{};

  using vector_t = ::arene::base::external_vector<TypeParam>;
  vector_t const vec{buf, {1, 2, 3}};

  ASSERT_TRUE((vec == vector_t{buf2, {1, 2, 3}}));
  ASSERT_TRUE((vec != vector_t{buf2, {4, 5, 6}}));
  ASSERT_TRUE((vec != vector_t{buf2, {1, 2}}));
  ASSERT_TRUE((vec != vector_t{buf2, {1, 2, 3, 4}}));
}

}  // namespace
