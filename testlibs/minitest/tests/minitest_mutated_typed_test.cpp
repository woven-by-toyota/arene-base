// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/mutate.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"

namespace minitest_modified_typed_tests {

using types_for_testing = ::testing::Types<char, double>;

using all_types = ::testing::extend_with_all_t<types_for_testing>;

template <typename T>
class TestSuiteForAllTypesTests : public testing::Test {};

TYPED_TEST_SUITE(TestSuiteForAllTypesTests, all_types, );

TYPED_TEST(TestSuiteForAllTypesTests, AllTypesTest) { ASSERT_TRUE(true); }

using volatile_types = ::testing::mutate_t<types_for_testing, ::testing::add_volatile, ::testing::add_const_volatile>;
using volatile_non_rvalue_types =
    ::testing::mutate_t<volatile_types, ::testing::type_identity, ::testing::add_lvalue_reference>;

template <typename T>
class TestSuiteForVolatileNonRvalueReferenceTests : public testing::Test {};

TYPED_TEST_SUITE(TestSuiteForVolatileNonRvalueReferenceTests, volatile_non_rvalue_types, );

TYPED_TEST(TestSuiteForVolatileNonRvalueReferenceTests, VolatileNonRvalueTest) { ASSERT_TRUE(true); }

using non_volatile_types = ::testing::mutate_t<types_for_testing, ::testing::type_identity, ::testing::add_const>;

template <typename T>
class TestSuiteForNonVolatileTests : public testing::Test {};

TYPED_TEST_SUITE(TestSuiteForNonVolatileTests, non_volatile_types, );

TYPED_TEST(TestSuiteForNonVolatileTests, NonVolatileTest) { ASSERT_TRUE(true); }

using rvalue_reference_types = ::testing::mutate_t<types_for_testing, ::testing::add_rvalue_reference>;

template <typename T>
class TestSuiteForRvalueReferenceTests : public testing::Test {};

TYPED_TEST_SUITE(TestSuiteForRvalueReferenceTests, rvalue_reference_types, );

TYPED_TEST(TestSuiteForRvalueReferenceTests, RRefTest) { ASSERT_TRUE(true); }

using pre_referenced_types_for_testing = ::testing::Types<char&, double&>;
using mutated_pre_referenced_types = arene::base::type_lists::remove_duplicates_t<::testing::extend_with_all_t<pre_referenced_types_for_testing>>;

template <typename T>
class TestSuiteForPreReferencedTypedTests : public testing::Test {};

TYPED_TEST_SUITE(TestSuiteForPreReferencedTypedTests, mutated_pre_referenced_types, );

TYPED_TEST(TestSuiteForPreReferencedTypedTests, PreRefTest) { ASSERT_TRUE(true); }

}  // namespace minitest_modified_typed_tests
