// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/type_list/size.hpp"
#include "stdlib/include/cstddef"
#include "stdlib/include/functional"
#include "stdlib/include/tuple"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/throws_on.hpp"

// NOLINTBEGIN(hicpp-avoid-c-arrays,readability-identifier-length,llvm-qualified-auto)

namespace {

/// @brief Incomplete base declaration of test fixture, used for partial specialization below
/// @tparam T The type parameter for the tests, expected to be a type-list
template <class T, class U>
class MakeTupleWithIndices;

/// @brief Partial specialization of the test fixture used to extract a pack of type parameters
/// @tparam Elements The elements of a type-list passed as a @c TypeParam
/// @tparam Indices Indices for the elements of a type-list passed as a @c TypeParam
template <class... Elements, std::size_t... Indices>
class MakeTupleWithIndices<std::tuple<Elements...>, std::index_sequence<Indices...>> : testing::Test {
 protected:
  using value_tuple = std::tuple<Elements...>;
  using ref_tuple = std::tuple<Elements&...>;
  using const_ref_tuple = std::tuple<Elements const&...>;
  using pointer_tuple = std::tuple<Elements*...>;
  using function_pointer_tuple = std::tuple<Elements (*)(Elements)...>;

  /// @brief Calls @c std::make_tuple with a pack of default-constructed @c Elements
  /// @return A @c std::tuple whose values are all default-constructed from @c Elements
  static constexpr auto default_tuple() noexcept(noexcept(std::make_tuple(Elements{}...))) -> value_tuple {
    static_assert(
        noexcept(std::make_tuple(Elements{}...)) == noexcept(value_tuple{Elements{}...}),
        "Noexcept specification should match direct construction"
    );
    return std::make_tuple(Elements{}...);
  }

  static constexpr auto make_tuple(Elements&... args) -> value_tuple {
    static_assert(
        noexcept(std::make_tuple(args...)) == noexcept(value_tuple{args...}),
        "Noexcept specification should match direct construction"
    );
    return std::make_tuple(args...);
  }

  static constexpr auto make_tuple(Elements&&... args) -> value_tuple {
    static_assert(
        noexcept(std::make_tuple(std::move(args)...)) == noexcept(value_tuple{std::move(args)...}),
        "Noexcept specification should match direct construction"
    );
    return std::make_tuple(std::move(args)...);
  }

  static constexpr auto make_tuple_from_lvalue_refs(value_tuple& tuple) -> value_tuple {
    static_assert(
        noexcept(std::make_tuple(std::get<Indices>(tuple)...)) == noexcept(value_tuple{std::get<Indices>(tuple)...}),
        "Noexcept specification should match direct construction"
    );
    return std::make_tuple(std::get<Indices>(tuple)...);
  }

  static constexpr auto make_tuple_from_rvalue_refs(value_tuple&& tuple) -> value_tuple {
    static_assert(
        noexcept(std::make_tuple(std::get<Indices>(std::move(tuple))...)) ==
            noexcept(value_tuple{std::get<Indices>(std::move(tuple))...}),
        "Noexcept specification should match direct construction"
    );
    // NOLINTNEXTLINE(bugprone-use-after-move) Each move only gets resolved on a single tuple member
    return std::make_tuple(std::get<Indices>(std::move(tuple))...);
  }

  static constexpr auto make_tuple_from_ref_wrappers(value_tuple& tuple) -> ref_tuple {
    static_assert(
        noexcept(std::make_tuple(std::ref(std::get<Indices>(tuple))...)) ==
            noexcept(ref_tuple{std::ref(std::get<Indices>(tuple))...}),
        "Noexcept specification should match direct construction"
    );
    return std::make_tuple(std::ref(std::get<Indices>(tuple))...);
  }

  static constexpr auto make_tuple_from_const_ref_wrappers(value_tuple& tuple) -> const_ref_tuple {
    static_assert(
        noexcept(std::make_tuple(std::cref(std::get<Indices>(tuple))...)) ==
            noexcept(const_ref_tuple{std::cref(std::get<Indices>(tuple))...}),
        "Noexcept specification should match direct construction"
    );
    return std::make_tuple(std::cref(std::get<Indices>(tuple))...);
  }

  static constexpr auto make_tuple_from_arrays() -> pointer_tuple {
    std::tuple<Elements[1]...> array_tuple{};
    static_assert(
        noexcept(std::make_tuple(std::get<Indices>(array_tuple)...)) ==
            noexcept(pointer_tuple{std::get<Indices>(array_tuple)...}),  // NOLINT(hicpp-no-array-decay) needed for test
        "Noexcept specification should match direct construction"
    );
    return std::make_tuple(std::get<Indices>(array_tuple)...);
  }

  template <class T>
  static auto identity_function(T) -> T {
    return {};
  }

  static constexpr auto make_tuple_from_function_refs() -> function_pointer_tuple {
    return std::make_tuple(identity_function<Elements>...);
  }

  static constexpr auto make_tuple_from_lambda_refs() -> function_pointer_tuple {
    return std::make_tuple(*[](Elements) { return Elements{}; }...);
  }

  template <class Tuple>
  static constexpr auto elements_are_default_constructed(Tuple const& values) noexcept -> bool {
    ARENE_IGNORE_START();
    ARENE_IGNORE_ALL("-Wfloat-equal", "No arithmetic is performed so float comparison is valid");
    return arene::base::all_of((std::get<Indices>(values) == Elements{})...);
    ARENE_IGNORE_END();
  }
};

template <class T>
class MakeTuple : protected MakeTupleWithIndices<T, std::make_index_sequence<std::tuple_size<T>{}>> {};

// clang-format off
using tuple_args = ::testing::Types<
  std::tuple<int>,
  std::tuple<float>,
  std::tuple<void*>,
  std::tuple<int, float, void*>,
  std::tuple<int, int, int, int>,
  std::tuple<void*, char, int, float, double>,
  std::tuple<testing::configurable_value<int, testing::throws_on::default_construct>>,
  std::tuple<testing::configurable_value<int, testing::throws_on::copy_construct>>,
  std::tuple<testing::configurable_value<int, testing::throws_on::move_construct>>
>;
// clang-format on

TYPED_TEST_SUITE(MakeTuple, tuple_args, );

/// @test @c make_tuple works with all elements default-constructed
CONSTEXPR_TYPED_TEST(MakeTuple, CanMakeTupleWithDefaultValues) {
  auto const tuple = TestFixture::default_tuple();
  EXPECT_TRUE(TestFixture::elements_are_default_constructed(tuple));
}

/// @test @c make_tuple on packs of references returns a tuple of values, not references
CONSTEXPR_TYPED_TEST(MakeTuple, UnwrappedReferencesResolveToValues) {
  TypeParam default_values = TestFixture::default_tuple();
  TypeParam const copied_values = TestFixture::make_tuple_from_lvalue_refs(default_values);
  EXPECT_TRUE(TestFixture::elements_are_default_constructed(copied_values));

  TypeParam const moved_values = TestFixture::make_tuple_from_rvalue_refs(std::move(default_values));
  EXPECT_TRUE(TestFixture::elements_are_default_constructed(moved_values));
}

/// @test @c make_tuple on packs of @c std::reference_wrapper s returns a tuple of references
TYPED_TEST(MakeTuple, WrappedReferencesResolveToReferences) {
  // This test case can't be CONSTEXPR_ because std::reference_wrapper is not constexpr until C++20.
  using const_ref_tup = typename TestFixture::const_ref_tuple;
  using ref_tup = typename TestFixture::ref_tuple;

  TypeParam default_values = TestFixture::default_tuple();
  const_ref_tup const const_refs = TestFixture::make_tuple_from_const_ref_wrappers(default_values);
  EXPECT_TRUE(TestFixture::elements_are_default_constructed(const_refs));

  ref_tup const mutable_refs = TestFixture::make_tuple_from_ref_wrappers(default_values);
  EXPECT_TRUE(TestFixture::elements_are_default_constructed(mutable_refs));
}

/// @test @c make_tuple on packs of arrays returns a tuple of decayed pointers
CONSTEXPR_TYPED_TEST(MakeTuple, ArraysResolveToPointers) { std::ignore = TestFixture::make_tuple_from_arrays(); }

/// @test @c make_tuple on packs of function references returns a tuple of decayed function pointers
CONSTEXPR_TYPED_TEST(MakeTuple, FunctionReferencesResolveToFunctionPointers) {
  std::ignore = TestFixture::make_tuple_from_function_refs();
}

/// @test @c make_tuple on packs of lambda references returns a tuple of decayed function pointers
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MakeTuple, LambdaReferencesResolveToFunctionPointers, __cplusplus >= 201703L) {
  // This test case uses a lambda which can't be constexpr until C++17.
  std::ignore = TestFixture::make_tuple_from_lambda_refs();
}

}  // namespace

// NOLINTEND(hicpp-avoid-c-arrays,readability-identifier-length,llvm-qualified-auto)
