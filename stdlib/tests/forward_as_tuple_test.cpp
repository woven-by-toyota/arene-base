// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/cstddef"
#include "stdlib/include/tuple"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

using udt = testing::configurable_value<int>;

using arg1t = int;
using arg2t = double;
using arg3t = udt;

/// @test Given a set of arguments `Args&&... args`, then the return type of `std::forward_as_tuple(args...)` is
/// `std::tuple<Args&&...>`
TEST(ForwardAsTuple, ReferenceQualificationOfInputsIsPreserved) {
  ::testing::StaticAssertTypeEq<
      decltype(std::forward_as_tuple(std::declval<arg1t const&>(), std::declval<arg2t&>(), std::declval<arg3t const&>())
      ),
      std::tuple<arg1t const&, arg2t&, arg3t const&>>();

  ::testing::StaticAssertTypeEq<
      decltype(std::forward_as_tuple(
          std::declval<arg2t&>(),
          std::declval<arg3t const&>(),
          std::declval<arg1t const&>()

      )),
      std::tuple<arg2t&, arg3t const&, arg1t const&>>();

  ::testing::StaticAssertTypeEq<
      decltype(std::forward_as_tuple(std::declval<arg1t&&>(), std::declval<arg2t const&&>(), std::declval<arg3t&&>())),
      std::tuple<arg1t&&, arg2t const&&, arg3t&&>>();

  ::testing::StaticAssertTypeEq<
      decltype(std::forward_as_tuple(std::declval<arg2t const&&>(), std::declval<arg3t&&>(), std::declval<arg1t&&>())),
      std::tuple<arg2t const&&, arg3t&&, arg1t&&>>();
}

/// @test Given instances of set of arguments `args...`, when `std::forward_as_tuple(args...)` is invoked, the returned
/// tuple is constructed as if by the expression `std::tuple<Args&&...>(std::forward<Args>(args)...)`.
CONSTEXPR_TEST(ForwardAsTuple, ArgumentsArePerfectlyForwarded) {
  arg1t value0{42};
  arg2t value1{3.14F};
  arg3t value2{7};

  CONSTEXPR_ASSERT(&std::get<0>(std::forward_as_tuple(value0, value1, value2)) == &value0);
  CONSTEXPR_ASSERT(&std::get<1>(std::forward_as_tuple(value0, value1, value2)) == &value1);
  CONSTEXPR_ASSERT(&std::get<2>(std::forward_as_tuple(value0, value1, value2)) == &value2);

  arg1t const& value0_cref = value0;
  arg2t const& value1_cref = value1;
  arg3t const& value2_cref = value2;
  CONSTEXPR_ASSERT(&std::get<0>(std::forward_as_tuple(value0_cref, value1_cref, value2_cref)) == &value0);
  CONSTEXPR_ASSERT(&std::get<1>(std::forward_as_tuple(value0_cref, value1_cref, value2_cref)) == &value1);
  CONSTEXPR_ASSERT(&std::get<2>(std::forward_as_tuple(value0_cref, value1_cref, value2_cref)) == &value2);
}

}  // namespace
