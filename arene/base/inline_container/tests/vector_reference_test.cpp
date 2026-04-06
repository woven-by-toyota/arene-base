// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/inline_container/vector_reference.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/copy.hpp"
#include "arene/base/algorithm/find.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/byte/byte.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/inline_container/external_vector.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/basic_insert_iterator.hpp"
#include "arene/base/stdlib_choice/begin.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/end.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_if.hpp"
#include "arene/base/stdlib_choice/sort.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/utility/as_const.hpp"

namespace {

/// @brief Helper type for specifying capacity that can be passed as a function argument
/// @tparam S The capacity
template <std::size_t S>
using capacity_t = std::integral_constant<std::size_t, S>;

/// @brief An instantiation of a particular capacity value
/// @tparam S The capacity
template <std::size_t S>
constexpr auto capacity = capacity_t<S>{};

/// @brief First test capacity value
constexpr auto capacity1 = capacity<42>;

/// @brief Second test capacity value
constexpr auto capacity2 = capacity<10>;

/// @brief Tag type indicating to use an inline_vector
struct inline_vector_tag {};

/// @brief Tag type indicating to use an external_vector
struct external_vector_tag {};

auto buffer_counter() -> std::size_t& {
  static auto counter = std::size_t{};
  return counter;
}

/// @brief Helper function to create a buffer sized for the given type and capacity
/// @tparam T The type to be stored in the buffer
/// @tparam Capacity The capacity of the buffer
template <class T, std::size_t Capacity>
auto buffer() -> arene::base::span<arene::base::byte> {
  // The maximum number of vectors that can be created with the same Type and Capacity within a single test.
  constexpr auto max_number_buffers_for_this_size_and_capacity = std::size_t{10};

  using arene::base::array;
  alignas(T) static auto buffers =
      array<array<arene::base::byte, sizeof(T) * Capacity>, max_number_buffers_for_this_size_and_capacity>{};

  auto& buffer = buffers[buffer_counter()++];
  ARENE_INVARIANT(buffer_counter() < max_number_buffers_for_this_size_and_capacity);
  return {buffer};
}

/// @brief Helper to determine the vector type for a given tag
/// @tparam Tag The type indicating which vector to use
/// @tparam T The type stored in the vector
/// @tparam Capacity The capacity of the vector
template <class Tag, class T, std::size_t Capacity>
using vector_type = std::conditional_t<
    std::is_same<Tag, inline_vector_tag>::value,
    arene::base::inline_vector<T, Capacity>,
    arene::base::external_vector<T>>;

/// @brief Construct an inline_vector via tag dispatch
/// @tparam T The type stored in the vector
/// @tparam Capacity The capacity of the vector
/// @param initial_values The list of values used to construct the vector
template <class T, std::size_t Capacity>
constexpr auto construct_test_vector(inline_vector_tag, std::initializer_list<T> initial_values, capacity_t<Capacity>)
    -> vector_type<inline_vector_tag, T, Capacity> {
  return vector_type<inline_vector_tag, T, Capacity>{initial_values};
}

/// @brief Construct an external_vector via tag dispatch
/// @tparam T The type stored in the vector
/// @tparam Capacity The capacity of the vector
/// @param initial_values The list of values used to construct the vector
template <class T, std::size_t Capacity>
auto construct_test_vector(external_vector_tag, std::initializer_list<T> initial_values, capacity_t<Capacity>)
    -> vector_type<external_vector_tag, T, Capacity> {
  return vector_type<external_vector_tag, T, Capacity>{buffer<T, Capacity>(), initial_values};
}

/// @brief Construct a vector of the type dictated by the tag
/// @tparam TestCase The test case containing the tag and value types to use
/// @tparam Capacity The capacity of the vector
/// @param initial_values The list of values used to construct the vector
/// @param capacity Deduction helper indicating the capacity of the vector
template <class TestCase, std::size_t Capacity>
constexpr auto
vector(std::initializer_list<typename TestCase::value_type> initial_values, capacity_t<Capacity> capacity)
    -> vector_type<typename TestCase::tag_type, typename TestCase::value_type, Capacity> {
  return construct_test_vector(typename TestCase::tag_type{}, initial_values, capacity);
}

/// @brief Type trait indicating if the vector represented by the @c Tag can be used in a constant expression.
template <class TestCase>
constexpr auto supports_constexpr_v = std::is_same<typename TestCase::tag_type, inline_vector_tag>::value;

template <typename Tag>
struct VectorReference : ::testing::Test {
  auto TearDown() -> void override { buffer_counter() = 0; }
};

template <class T, class Tag>
struct test_case {
  using value_type = T;
  using tag_type = Tag;
};

using vector_types =
    ::testing::Types<test_case<std::int32_t, inline_vector_tag>, test_case<std::int32_t, external_vector_tag>>;

TYPED_TEST_SUITE(VectorReference, vector_types, );

/// @test A `vector_reference` can be constructed from a vector with any capacity
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanCreateVectorReferenceFromAnySizeVector, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7}, capacity2);

  CONSTEXPR_ASSERT(vec1.data() == arene::base::vector_reference<T>(vec1).data());
  CONSTEXPR_ASSERT(vec2.data() == arene::base::vector_reference<T>(vec2).data());
}

/// @test A `const_vector_reference` can be constructed from a vector with any capacity
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanCreateConstVectorReferenceFromAnySizeVector, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7}, capacity2);

  CONSTEXPR_ASSERT(vec1.data() == arene::base::const_vector_reference<T>(vec1).data());
  CONSTEXPR_ASSERT(vec2.data() == arene::base::const_vector_reference<T>(vec2).data());
}

/// @test `size` can be invoked on a `vector_reference` or `const_vector_reference`, and yields the
/// result of invoking `size` on the referenced vector.
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanCheckSizeOfVectors, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({}, capacity2);

  CONSTEXPR_ASSERT(vec1.size() == arene::base::vector_reference<T>(vec1).size());
  CONSTEXPR_ASSERT(vec2.size() == arene::base::vector_reference<T>(vec2).size());
  CONSTEXPR_ASSERT(vec1.size() == arene::base::const_vector_reference<T>(vec1).size());
  CONSTEXPR_ASSERT(vec2.size() == arene::base::const_vector_reference<T>(vec2).size());
}

/// @test `capacity` and `max_size` can be invoked on a `vector_reference` or `const_vector_reference`,
/// and yields the result of invoking `capacity` on the referenced vector.
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanCheckCapacityOfVectors, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({}, capacity2);

  CONSTEXPR_ASSERT(vec1.capacity() == arene::base::vector_reference<T>(vec1).capacity());
  CONSTEXPR_ASSERT(vec2.capacity() == arene::base::vector_reference<T>(vec2).capacity());
  CONSTEXPR_ASSERT(vec1.capacity() == arene::base::const_vector_reference<T>(vec1).capacity());
  CONSTEXPR_ASSERT(vec2.capacity() == arene::base::const_vector_reference<T>(vec2).capacity());

  CONSTEXPR_ASSERT(vec1.max_size() == arene::base::vector_reference<T>(vec1).max_size());
  CONSTEXPR_ASSERT(vec2.max_size() == arene::base::vector_reference<T>(vec2).max_size());
  CONSTEXPR_ASSERT(vec1.max_size() == arene::base::const_vector_reference<T>(vec1).max_size());
  CONSTEXPR_ASSERT(vec2.max_size() == arene::base::const_vector_reference<T>(vec2).max_size());
}

/// @test `empty` can be invoked on a `vector_reference` or `const_vector_reference`, and yields the
/// result of invoking `empty` on the referenced vector.
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanCheckForEmptyVectors, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({}, capacity2);

  CONSTEXPR_ASSERT(vec1.empty() == arene::base::vector_reference<T>(vec1).empty());
  CONSTEXPR_ASSERT(vec2.empty() == arene::base::vector_reference<T>(vec2).empty());
  CONSTEXPR_ASSERT(vec1.empty() == arene::base::const_vector_reference<T>(vec1).empty());
  CONSTEXPR_ASSERT(vec2.empty() == arene::base::const_vector_reference<T>(vec2).empty());
}

/// @test `push_back` can be invoked on a `vector_reference`, and yields the same consequences as invoking
/// `push_back` on the referenced vector with the same argument.
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanPushBack, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({}, capacity2);

  T const elem = 42;

  auto ref1 = arene::base::vector_reference<T>(vec1);
  auto ref2 = arene::base::vector_reference<T>(vec2);

  auto size_before1 = vec1.size();
  auto size_before2 = vec2.size();
  ref1.push_back(elem);
  ref2.push_back(elem);

  CONSTEXPR_ASSERT(ref1.size() == size_before1 + 1);
  CONSTEXPR_ASSERT(ref2.size() == size_before2 + 1);
  CONSTEXPR_ASSERT(ref1.back() == elem);
  CONSTEXPR_ASSERT(ref2.back() == elem);
}

/// @test `operator[]` can be invoked on a `vector_reference` or `const_vector_reference`, and yields the
/// result of invoking `operator[]` on the referenced vector with the same argument.
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanAccessElementsByIndex, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7}, capacity2);

  for (std::size_t i = 0; i < vec1.size(); ++i) {
    CONSTEXPR_ASSERT(&vec1[i] == &arene::base::vector_reference<T>(vec1)[i]);
    CONSTEXPR_ASSERT(&vec1[i] == &arene::base::const_vector_reference<T>(vec1)[i]);
  }

  for (std::size_t i = 0; i < vec2.size(); ++i) {
    CONSTEXPR_ASSERT(&vec2[i] == &arene::base::vector_reference<T>(vec2)[i]);
    CONSTEXPR_ASSERT(&vec2[i] == &arene::base::const_vector_reference<T>(vec2)[i]);
  }
}

#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
constexpr bool exceptions_enabled = true;

template <typename TypeParam, bool AreExceptionsEnabled = arene::base::detail::are_exceptions_enabled_v>
struct AtCanAccessElementsByIndexImpl {
  constexpr void operator()() {
    using T = typename TypeParam::value_type;
    auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
    auto vec2 = vector<TypeParam>({4, 5, 6, 7}, capacity2);

    for (std::size_t i = 0; i < vec1.size(); ++i) {
      CONSTEXPR_ASSERT(&vec1.at(i) == &arene::base::vector_reference<T>(vec1).at(i));
      CONSTEXPR_ASSERT(&vec1.at(i) == &arene::base::const_vector_reference<T>(vec1).at(i));
    }

    for (std::size_t i = 0; i < vec2.size(); ++i) {
      CONSTEXPR_ASSERT(&vec2.at(i) == &arene::base::vector_reference<T>(vec2).at(i));
      CONSTEXPR_ASSERT(&vec2.at(i) == &arene::base::const_vector_reference<T>(vec2).at(i));
    }
  }
};

template <typename T>
struct AtCanAccessElementsByIndexImpl<T, false> {
  constexpr void operator()() { GTEST_SKIP() << "Exceptions are disabled, thus at is not defined"; }
};
#else
constexpr bool exceptions_enabled = false;
#endif

/// @test When exceptions are enabled, `at` can be invoked on a `vector_reference` or `const_vector_reference`, and
/// yields the result of invoking `at` on the referenced vector with the same argument.
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    VectorReference,
    AtCanAccessElementsByIndex,
    supports_constexpr_v<TypeParam>&& exceptions_enabled
) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
  AtCanAccessElementsByIndexImpl<TypeParam>{}();
#else
  GTEST_SKIP() << "Exceptions are disabled, thus at is not defined";
#endif
}

/// @test `front` can be invoked on a `vector_reference` or `const_vector_reference`, and yields the
/// result of invoking `front` on the referenced vector
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanAccessFrontElement, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7}, capacity2);

  CONSTEXPR_ASSERT(&vec1.front() == &arene::base::vector_reference<T>(vec1).front());
  CONSTEXPR_ASSERT(&vec2.front() == &arene::base::vector_reference<T>(vec2).front());
  CONSTEXPR_ASSERT(&vec1.front() == &arene::base::const_vector_reference<T>(vec1).front());
  CONSTEXPR_ASSERT(&vec2.front() == &arene::base::const_vector_reference<T>(vec2).front());
}

/// @test `back` can be invoked on a `vector_reference` or `const_vector_reference`, and yields the
/// result of invoking `back` on the referenced vector
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanAccessBackElement, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7}, capacity2);

  CONSTEXPR_ASSERT(&vec1.back() == &arene::base::vector_reference<T>(vec1).back());
  CONSTEXPR_ASSERT(&vec2.back() == &arene::base::vector_reference<T>(vec2).back());
  CONSTEXPR_ASSERT(&vec1.back() == &arene::base::const_vector_reference<T>(vec1).back());
  CONSTEXPR_ASSERT(&vec2.back() == &arene::base::const_vector_reference<T>(vec2).back());
}

/// @test `begin` can be invoked on a `vector_reference` or `const_vector_reference`, and yields the
/// result of invoking `begin` on the referenced vector
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, BeginIterators, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7}, capacity2);

  testing::StaticAssertTypeEq<decltype(vec1.begin()), decltype(arene::base::vector_reference<T>(vec1).begin())>();
  testing::StaticAssertTypeEq<decltype(vec1.begin()), typename arene::base::vector_reference<T>::iterator>();

  CONSTEXPR_ASSERT(vec1.begin() == arene::base::vector_reference<T>(vec1).begin());
  CONSTEXPR_ASSERT(vec2.begin() == arene::base::vector_reference<T>(vec2).begin());

  testing::StaticAssertTypeEq<decltype(vec1.cbegin()), decltype(arene::base::const_vector_reference<T>(vec1).begin())>(
  );
  testing::StaticAssertTypeEq<decltype(vec1.cbegin()), typename arene::base::const_vector_reference<T>::iterator>();

  CONSTEXPR_ASSERT(vec1.begin() == arene::base::const_vector_reference<T>(vec1).begin());
  CONSTEXPR_ASSERT(vec2.begin() == arene::base::const_vector_reference<T>(vec2).begin());
}

/// @test `end` can be invoked on a `vector_reference` or `const_vector_reference`, and yields the
/// result of invoking `end` on the referenced vector
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, EndIterators, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7}, capacity2);

  testing::StaticAssertTypeEq<decltype(vec1.end()), decltype(arene::base::vector_reference<T>(vec1).end())>();

  CONSTEXPR_ASSERT(vec1.end() == arene::base::vector_reference<T>(vec1).end());
  CONSTEXPR_ASSERT(vec2.end() == arene::base::vector_reference<T>(vec2).end());

  testing::StaticAssertTypeEq<decltype(vec1.cend()), decltype(arene::base::const_vector_reference<T>(vec1).end())>();

  CONSTEXPR_ASSERT(vec1.end() == arene::base::const_vector_reference<T>(vec1).end());
  CONSTEXPR_ASSERT(vec2.end() == arene::base::const_vector_reference<T>(vec2).end());
}

/// @test `cbegin` can be invoked on a `vector_reference` or `const_vector_reference`, and yields the
/// result of invoking `cbegin` on the referenced vector
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, ConstBeginIterators, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7}, capacity2);

  testing::StaticAssertTypeEq<decltype(vec1.cbegin()), decltype(arene::base::vector_reference<T>(vec1).cbegin())>();
  testing::StaticAssertTypeEq<decltype(vec1.cbegin()), typename arene::base::vector_reference<T>::const_iterator>();

  CONSTEXPR_ASSERT(vec1.cbegin() == arene::base::vector_reference<T>(vec1).cbegin());
  CONSTEXPR_ASSERT(vec2.cbegin() == arene::base::vector_reference<T>(vec2).cbegin());

  testing::StaticAssertTypeEq<decltype(vec1.cbegin()), typename arene::base::const_vector_reference<T>::const_iterator>(
  );

  CONSTEXPR_ASSERT(vec1.cbegin() == arene::base::const_vector_reference<T>(vec1).cbegin());
  CONSTEXPR_ASSERT(vec2.cbegin() == arene::base::const_vector_reference<T>(vec2).cbegin());
}

/// @test `cend` can be invoked on a `vector_reference` or `const_vector_reference`, and yields the
/// result of invoking `cend` on the referenced `inline_vector`
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, ConstEndIterators, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7}, capacity2);

  testing::StaticAssertTypeEq<decltype(vec1.cend()), decltype(arene::base::vector_reference<T>(vec1).cend())>();
  testing::StaticAssertTypeEq<decltype(vec1.cend()), typename arene::base::vector_reference<T>::const_iterator>();

  CONSTEXPR_ASSERT(vec1.cend() == arene::base::vector_reference<T>(vec1).cend());
  CONSTEXPR_ASSERT(vec2.cend() == arene::base::vector_reference<T>(vec2).cend());

  testing::StaticAssertTypeEq<decltype(vec1.cend()), decltype(arene::base::const_vector_reference<T>(vec1).cend())>();
  testing::StaticAssertTypeEq<decltype(vec1.cend()), typename arene::base::const_vector_reference<T>::const_iterator>();

  CONSTEXPR_ASSERT(vec1.cend() == arene::base::const_vector_reference<T>(vec1).cend());
  CONSTEXPR_ASSERT(vec2.cend() == arene::base::const_vector_reference<T>(vec2).cend());
}

/// @test A `vector_reference` can be converted to a `const_vector_reference` with the same element type
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanConvertNonConstReferenceToConstReference, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7}, capacity2);

  arene::base::vector_reference<T> const non_const_ref1{vec1};
  arene::base::const_vector_reference<T> const const_ref1{non_const_ref1};
  CONSTEXPR_ASSERT((const_ref1.data() == vec1.data()) && (const_ref1.size() == vec1.size()));

  arene::base::vector_reference<T> const non_const_ref2{vec2};
  arene::base::const_vector_reference<T> const const_ref2{non_const_ref2};
  CONSTEXPR_ASSERT((const_ref2.data() == vec2.data()) && (const_ref2.size() == vec2.size()));
}

/// @test `insert` can be invoked on a `vector_reference` with an iterator and element, and yields the same
/// consequences as invoking `insert` on the referenced vector with the same arguments
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanInsertViaIterator, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7}, capacity2);

  constexpr T element{42};

  auto const expected1 = vector<TypeParam>({1, 2, element, 3}, capacity1);
  auto const expected2 = vector<TypeParam>({4, element, 5, 6, 7}, capacity2);

  auto const ref1 = arene::base::vector_reference<T>{vec1};
  auto const ref2 = arene::base::vector_reference<T>{vec2};

  CONSTEXPR_ASSERT(ref1.insert(vec1.cbegin() + 2, element) == vec1.begin() + 2);
  CONSTEXPR_ASSERT(ref2.insert(vec2.cbegin() + 1, element) == vec2.begin() + 1);
  CONSTEXPR_ASSERT(vec1 == expected1);
  CONSTEXPR_ASSERT(vec2 == expected2);
}

/// @test `insert` can be invoked on a `vector_reference` with an iterator and source range, and yields the same
/// consequences as invoking `insert` on the referenced vector with the same arguments
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanInsertRange, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({1, 2, 3}, capacity2);
  auto vec3 = vector<TypeParam>({4, 5, 6, 7}, capacity2);

  auto const expected1 = vector<TypeParam>({1, 2, 3, 4, 5, 6, 7}, capacity1);
  auto const expected2 = vector<TypeParam>({4, 5, 6, 7, 1, 2, 3}, capacity2);

  auto ref1 = arene::base::vector_reference<T>{vec1};
  auto ref2 = arene::base::vector_reference<T>{vec2};

  ref1.insert(vec1.cbegin() + static_cast<std::ptrdiff_t>(vec1.size()), vec3.begin(), vec3.end());
  ref2.insert(vec2.cbegin(), vec3.begin(), vec3.end());

  CONSTEXPR_ASSERT(vec1 == expected1);
  CONSTEXPR_ASSERT(vec2 == expected2);
}

/// @test `insert` can be invoked on a `vector_reference` with an iterator and
/// source range where the range iterator type is not the same as a vector
/// iterator
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanInsertRangeWithDifferentIterator, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({1, 2, 3}, capacity2);
  T arr[] = {4, 5, 6, 7};  // NOLINT(hicpp-avoid-c-arrays)
  static_assert(
      !std::is_same<decltype(vec1.begin()), decltype(std::begin(arr))>::value,
      "iterators expected to be different types"
  );

  auto const expected1 = vector<TypeParam>({1, 2, 3, 4, 5, 6, 7}, capacity1);
  auto const expected2 = vector<TypeParam>({4, 5, 6, 7, 1, 2, 3}, capacity2);

  auto ref1 = arene::base::vector_reference<T>{vec1};
  auto ref2 = arene::base::vector_reference<T>{vec2};

  ref1.insert(vec1.cbegin() + static_cast<std::ptrdiff_t>(vec1.size()), std::begin(arr), std::end(arr));
  ref2.insert(vec2.cbegin(), std::begin(arr), std::end(arr));

  CONSTEXPR_ASSERT(vec1 == expected1);
  CONSTEXPR_ASSERT(vec2 == expected2);
}

/// @test `erase` can be invoked on a `vector_reference` with an iterator, and yields the same consequences as
/// invoking `erase` on the referenced vector with the same arguments
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanEraseViaIterator, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7}, capacity2);

  auto const expected1 = vector<TypeParam>({1, 2}, capacity1);
  auto const expected2 = vector<TypeParam>({4, 6, 7}, capacity2);

  auto ref1 = arene::base::vector_reference<T>{vec1};
  auto ref2 = arene::base::vector_reference<T>{vec2};

  CONSTEXPR_ASSERT(ref1.erase(vec1.cbegin() + 2) == vec1.begin() + 2);
  CONSTEXPR_ASSERT(ref2.erase(vec2.cbegin() + 1) == vec2.begin() + 1);
  CONSTEXPR_ASSERT(vec1 == expected1);
  CONSTEXPR_ASSERT(vec2 == expected2);
}

/// @test `erase` can be invoked on a `vector_reference` with an iterator range, and yields the same
/// consequences as invoking `erase` on the referenced `inline_vector` with the same arguments
TYPED_TEST(VectorReference, CanEraseRangeViaIterator) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3, 4, 5, 6}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7, 8, 9, 10}, capacity2);

  auto const expected1 = vector<TypeParam>({1, 2, 6}, capacity1);
  auto const expected2 = vector<TypeParam>({4, 9, 10}, capacity2);

  auto ref1 = arene::base::vector_reference<T>{vec1};
  auto ref2 = arene::base::vector_reference<T>{vec2};

  CONSTEXPR_ASSERT(ref1.erase(vec1.cbegin() + 2, vec1.cbegin() + 5) == vec1.begin() + 2);
  CONSTEXPR_ASSERT(ref2.erase(vec2.cbegin() + 1, vec2.cbegin() + 5) == vec2.begin() + 1);
  CONSTEXPR_ASSERT(vec1 == expected1);
  CONSTEXPR_ASSERT(vec2 == expected2);
}

/// @test `pop_back` can be invoked on a `vector_reference` or `const_vector_reference`, and yields the
/// same consequences as invoking `pop_back` on the referenced vector
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanPopBack, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  auto vec2 = vector<TypeParam>({4}, capacity2);

  auto const expected1 = vector<TypeParam>({1, 2}, capacity1);
  auto const expected2 = vector<TypeParam>({}, capacity2);

  arene::base::vector_reference<T>{vec1}.pop_back();
  arene::base::vector_reference<T>{vec2}.pop_back();

  CONSTEXPR_ASSERT(vec1 == expected1);
  CONSTEXPR_ASSERT(vec2 == expected2);
}

/// @test `clear` can be invoked on a `vector_reference` or `const_vector_reference`, and yields the
/// same consequences as invoking `clear` on the referenced vector
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanClear, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3, 4, 5, 6}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7, 8, 9, 10}, capacity2);

  arene::base::vector_reference<T>{vec1}.clear();
  arene::base::vector_reference<T>{vec2}.clear();

  CONSTEXPR_ASSERT(vec1.size() == 0);
  CONSTEXPR_ASSERT(vec2.size() == 0);
}

/// @test `resize` can be invoked on a `vector_reference` or `const_vector_reference`, and yields the
/// same consequences as invoking `resize` on the referenced vector
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanResize, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3, 4, 5, 6}, capacity1);
  auto vec2 = vector<TypeParam>({4, 5, 6, 7, 8, 9, 10}, capacity2);

  std::size_t const vec1_size = vec1.size();
  std::size_t const vec2_size = vec2.size();

  auto ref1 = arene::base::vector_reference<T>{vec1};
  auto ref2 = arene::base::vector_reference<T>{vec2};

  ref1.resize(vec1_size + 1);
  ref2.resize(vec2_size + 1);
  CONSTEXPR_ASSERT(vec1.size() == vec1_size + 1);
  CONSTEXPR_ASSERT(vec2.size() == vec2_size + 1);

  ref1.resize(vec1_size - 1);
  ref2.resize(vec2_size - 1);
  CONSTEXPR_ASSERT(vec1.size() == vec1_size - 1);
  CONSTEXPR_ASSERT(vec2.size() == vec2_size - 1);

  ref1.resize(0);
  ref2.resize(0);
  CONSTEXPR_ASSERT(vec1.empty());
  CONSTEXPR_ASSERT(vec2.empty());
}

/// @test A `const_vector_reference` can be used with a range-based `for`-loop to iterate over the referenced
/// vector.
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, CanUseRangedForLoop, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;
  auto const vec = vector<TypeParam>({1, 2, 3, 4, 5, 6}, capacity1);

  arene::base::const_vector_reference<T> const ref{vec};

  auto itr = vec.begin();
  for (T const& elem : ref) {
    CONSTEXPR_ASSERT(elem == *itr++);
  }
}

// Note: The find test cannot easily share the implementation between inline and external vector. This is because the
// inline_vector needs to be made `static` to use find in a constant expression.

/// @test `find` can be invoked on a `vector_reference` or `const_vector_reference`, and yields the
/// result of invoking `find` on the referenced vector
TEST(VectorReference, CanUseFindInlineVector) {
  using T = std::int32_t;
  static constexpr arene::base::inline_vector<T, capacity1> vec{1, 2, 3, 4, 5, 6};
  constexpr arene::base::const_vector_reference<T> ref{vec};

  ASSERT_EQ(*arene::base::find(ref.begin(), ref.end(), 1), ref.front());
  ASSERT_EQ(*arene::base::find(ref.begin(), ref.end(), 5), 5);
  ASSERT_EQ(*arene::base::find(ref.begin(), ref.end(), 6), ref.back());
  ASSERT_EQ(arene::base::find(ref.begin(), ref.end(), 9), ref.end());
}

/// @test `find` can be invoked on a `vector_reference` or `const_vector_reference`, and yields the
/// result of invoking `find` on the referenced vector
TEST(VectorReference, CanUseFindExternalVector) {
  using T = std::int32_t;
  arene::base::external_vector<T> const vec{buffer<T, capacity1>(), {1, 2, 3, 4, 5, 6}};
  arene::base::const_vector_reference<T> const ref{vec};

  ASSERT_EQ(*arene::base::find(ref.begin(), ref.end(), 1), ref.front());
  ASSERT_EQ(*arene::base::find(ref.begin(), ref.end(), 5), 5);
  ASSERT_EQ(*arene::base::find(ref.begin(), ref.end(), 6), ref.back());
  ASSERT_EQ(arene::base::find(ref.begin(), ref.end(), 9), ref.end());
}

/// @test Iterators retrieved from a `vector_reference` can be passed to `std::sort` to sort the elements in the
/// referenced vector
TYPED_TEST(VectorReference, CanUseSort) {
  using T = typename TypeParam::value_type;

  auto vec = vector<TypeParam>({4, 5, 6, 1, 2, 3}, capacity1);
  arene::base::vector_reference<T> const ref{vec};

  auto const expected = vector<TypeParam>({1, 2, 3, 4, 5, 6}, capacity1);

  std::sort(ref.begin(), ref.end());
  CONSTEXPR_ASSERT(vec == expected);
}

/// @test Iterators retrieved from an `const_vector_reference` can be passed to `arene::base::copy` to copy the elements
/// in the referenced vector to another vector via a `std::back_inserter` constructed with a `vector_reference`
/// to that second vector.
TYPED_TEST(VectorReference, CanUseCopy) {
  using T = typename TypeParam::value_type;

  auto vec1 = vector<TypeParam>({1, 2, 3}, capacity1);
  arene::base::vector_reference<T> ref1{vec1};

  auto const vec2 = vector<TypeParam>({4, 5, 6}, capacity2);
  arene::base::const_vector_reference<T> const ref2{vec2};

  auto const expected = vector<TypeParam>({1, 2, 3, 4, 5, 6}, capacity1);

  arene::base::copy(ref2.begin(), ref2.end(), std::back_inserter(ref1));
  ASSERT_EQ(vec1, expected);
}

/// @test Can use `erase` and `std::remove_if` on a `vector_reference` as-if on the referenced vector
TYPED_TEST(VectorReference, CanUseEraseRemove) {
  using T = typename TypeParam::value_type;

  auto vec = vector<TypeParam>({1, 2, 3, 4, 5, 6}, capacity1);
  arene::base::vector_reference<T> const ref{vec};

  auto const expected = vector<TypeParam>({1, 3, 5}, capacity1);

  ref.erase(std::remove_if(ref.begin(), ref.end(), [](auto val) { return val % 2 == 0; }), ref.end());
  ASSERT_EQ(vec, expected);
}

/// @brief helper function to test equivalence of constructing a container
/// reference value and using a 'make_*' helper function
/// @tparam ExpectedRef the expected container reference type (e.g. @c
///     vector_reference or @c const_vector_reference)
/// @tparam Vec inline vector type
/// @tparam RefFunc function object that creates a container reference type
/// @param vec reference to an inline vector
/// @param ref_func 'make_*' helper function
///
/// Test that asserts @c ExpectedRef{vec} and @c ref_func(vec) are equivalent.
///
template <typename ExpectedRef, typename Vec, typename RefFunc>
constexpr void check_data_transparency(Vec& vec, RefFunc const& ref_func) noexcept {
  ExpectedRef ref1{vec};
  auto ref2 = ref_func(vec);

  // check equality by comparing the data pointer and checking the type
  CONSTEXPR_ASSERT(ref1.data() == ref2.data());
  CONSTEXPR_ASSERT(std::is_same<decltype(ref1), decltype(ref2)>::value);
}

/// @test Check that both @c make_vector_reference and @c
/// make_const_vector_reference can be invoked on an lvalue reference to
/// an @c inline_vector and that the created value is equal to calling the
/// constructor on the argument.
CONDITIONALLY_CONSTEXPR_TYPED_TEST(VectorReference, TemplateDeductionHelper, supports_constexpr_v<TypeParam>) {
  using T = typename TypeParam::value_type;
  // NOLINTNEXTLINE(misc-const-correctness)
  auto vec = vector<TypeParam>({1, 2, 3, 4, 5, 6}, capacity1);

  check_data_transparency<arene::base::vector_reference<T>>(vec, arene::base::make_vector_reference);
  check_data_transparency<arene::base::const_vector_reference<T>>(
      arene::base::as_const(vec),
      arene::base::make_vector_reference
  );

  check_data_transparency<arene::base::const_vector_reference<T>>(vec, arene::base::make_const_vector_reference);
  check_data_transparency<arene::base::const_vector_reference<T>>(
      arene::base::as_const(vec),
      arene::base::make_const_vector_reference
  );
}

/// @test Check that both @c make_vector_reference and @c
/// make_const_vector_reference are @c noexcept when invoked with lvalue
/// references
TYPED_TEST(VectorReference, NoexceptWithLvalueReference) {
  using T = typename TypeParam::value_type;
  using vector_test_type = vector_type<TypeParam, T, capacity1>;

  ASSERT_TRUE(noexcept(arene::base::make_vector_reference(std::declval<vector_test_type&>())));
  ASSERT_TRUE(noexcept(arene::base::make_vector_reference(std::declval<vector_test_type const&>())));

  ASSERT_TRUE(noexcept(arene::base::make_const_vector_reference(std::declval<vector_test_type&>())));
  ASSERT_TRUE(noexcept(arene::base::make_const_vector_reference(std::declval<vector_test_type const&>())));
}

/// @test Check that both @c make_vector_reference and @c
/// make_const_vector_reference cannot be invoked with rvalue references
TYPED_TEST(VectorReference, NotInvocableWithRvalueReference) {
  using T = typename TypeParam::value_type;
  using vector_test_type = vector_type<TypeParam, T, capacity1>;
  using arene::base::is_invocable_v;

  ASSERT_FALSE((is_invocable_v<decltype(arene::base::make_vector_reference), vector_test_type&&>));
  ASSERT_FALSE((is_invocable_v<decltype(arene::base::make_vector_reference), vector_test_type const&&>));

  ASSERT_FALSE((is_invocable_v<decltype(arene::base::make_const_vector_reference), vector_test_type&&>));
  ASSERT_FALSE((is_invocable_v<decltype(arene::base::make_const_vector_reference), vector_test_type const&&>));
}

/// @test member functions of @c vector_reference are invocable with a @c const qualified @c vector_reference
TYPED_TEST(VectorReference, MemberFunctionsAreConstInvocable) {
  using value_type = typename TypeParam::value_type;
  using vector_reference = arene::base::vector_reference<value_type>;
  using const_iterator = typename vector_reference::const_iterator;
  using input_iterator = value_type*;
  using size_type = typename vector_reference::size_type;

  using arene::base::is_invocable_v;

  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::data), vector_reference const&>));

  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::size), vector_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::capacity), vector_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::max_size), vector_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::empty), vector_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::clear), vector_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::operator[]), vector_reference const&, size_type>));
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::template at<>), vector_reference const&, size_type>));
#endif
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::front), vector_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::back), vector_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::begin), vector_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::end), vector_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::cbegin), vector_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::cend), vector_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::push_back), vector_reference const&, value_type const&>));

  auto const insert = [](auto const& ref, auto... args) { return ref.insert(args...); };
  ASSERT_TRUE((is_invocable_v<decltype(insert), vector_reference const&, const_iterator, value_type const&>));
  ASSERT_TRUE(
      (is_invocable_v<decltype(insert), vector_reference const&, const_iterator, input_iterator, input_iterator>)
  );

  auto const erase = [](auto const& ref, auto... args) { return ref.erase(args...); };
  ASSERT_TRUE((is_invocable_v<decltype(erase), vector_reference const&, const_iterator>));
  ASSERT_TRUE((is_invocable_v<decltype(erase), vector_reference const&, const_iterator, const_iterator>));

  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::pop_back), vector_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::clear), vector_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&vector_reference::resize), vector_reference const&, size_type>));
}

}  // namespace
