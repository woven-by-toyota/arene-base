// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstdint>
#include <initializer_list>

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/byte/byte.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/inline_container/external_vector.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/inline_container/vector_reference.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"

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

template <typename Tag>
struct VectorReference : ::testing::Test {
  auto TearDown() -> void override { buffer_counter() = 0; }
};

template <typename Tag>
struct VectorReferenceDeathTest : VectorReference<Tag> {};

template <class T, class Tag>
struct test_case {
  using value_type = T;
  using tag_type = Tag;
};

using vector_types =
    ::testing::Types<test_case<std::int32_t, inline_vector_tag>, test_case<std::int32_t, external_vector_tag>>;

TYPED_TEST_SUITE(VectorReferenceDeathTest, vector_types, );

/// @test `pop_back` invoked on an empty `vector` via a `vector_reference` is a precondition violation.
TYPED_TEST(VectorReferenceDeathTest, PopBackPrecondition) {
  using T = typename TypeParam::value_type;
  auto vec = vector<TypeParam>({}, capacity1);

  ASSERT_DEATH(arene::base::vector_reference<T>(vec).pop_back(), "Precondition violation: !empty()");
}

/// @test `resize` to a size over the capacity invoked on a `vector_reference` is a precondition violation.
TYPED_TEST(VectorReferenceDeathTest, ResizePrecondition) {
  using T = typename TypeParam::value_type;
  auto vec = vector<TypeParam>({}, capacity1);

  ASSERT_DEATH(
      arene::base::vector_reference<T>{vec}.resize(vec.capacity() + 1),
      "Precondition violation: count <= this->capacity()"
  );
}

}  // namespace
