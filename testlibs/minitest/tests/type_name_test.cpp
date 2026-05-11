// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/type_name.hpp"

#include "testlibs/minitest/cstring.hpp"

struct type1 {};

namespace {
struct type2 {};
}  // namespace

namespace ns {
struct type3 {};
}  // namespace ns

auto main() -> int {
  using ::testing::cstring::equal;
  using ::testing::type_name::type_name;

  static_assert(equal(type_name<int>, "int"), "the type name of int is 'int'");
  static_assert(equal(type_name<double>, "double"), "the type name of double is 'double'");
  static_assert(equal(type_name<type1>, "type1"), "the type name of type1 is 'type1'");

  static_assert(
      equal(
          type_name<type2>,
#if defined(__clang__)
          "(anonymous namespace)::type2"
#else
          "{anonymous}::type2"
#endif
      ),
      "the type name of type2 includes the anonymous namespace"
  );

  static_assert(equal(type_name<ns::type3>, "ns::type3"), "the type name of type3 is 'ns::type3'");
}
