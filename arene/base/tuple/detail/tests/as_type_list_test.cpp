// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/tuple/detail/as_type_list.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {

/// @test `as_type_list_t` is an identity operation for std::tuple
TEST(AsTypeList, PreservesStdTupleElementTypes) {
  ::testing::StaticAssertTypeEq<arene::base::tuple_detail::as_type_list_t<std::tuple<>>, std::tuple<>>();

  ::testing::StaticAssertTypeEq<
      arene::base::tuple_detail::as_type_list_t<std::tuple<int, char, double>>,
      std::tuple<int, char, double>>();
}

/// @test `as_type_list_t` works on std::pair
TEST(AsTypeList, StdPairYieldsTwoElementTypeList) {
  ::testing::StaticAssertTypeEq<
      arene::base::tuple_detail::as_type_list_t<std::pair<int, char>>,
      arene::base::type_list<int, char>>();
}

/// @test `as_type_list_t` works on arene::base::array
TEST(AsTypeList, AreneArrayYieldsRepeatedElementType) {
  ::testing::StaticAssertTypeEq<
      arene::base::tuple_detail::as_type_list_t<arene::base::array<int, 3>>,
      arene::base::type_list<int, int, int>>();
}

/// @test `as_type_list_t` on an empty array yields an empty type_list
TEST(AsTypeList, EmptyTupleYieldsEmptyTypeList) {
  ::testing::StaticAssertTypeEq<
      arene::base::tuple_detail::as_type_list_t<arene::base::array<int, 0>>,
      arene::base::type_list<>>();
}

/// @test `as_type_list_t` preserves cv-ref qualifiers on tuple elements
TEST(AsTypeList, PreservesCvRefQualifiersOnElements) {
  ::testing::StaticAssertTypeEq<
      arene::base::tuple_detail::as_type_list_t<std::pair<int&, int&&>>,
      arene::base::type_list<int&, int&&>>();

  ::testing::StaticAssertTypeEq<
      arene::base::tuple_detail::as_type_list_t<std::pair<int const&, int const&&>>,
      arene::base::type_list<int const&, int const&&>>();
}

}  // namespace
