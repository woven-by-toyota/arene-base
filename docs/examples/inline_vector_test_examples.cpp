#include "arene/base/array.hpp"
#include "arene/base/inline_container/testing/vector.hpp"

namespace vector_test {
class my_class {
  std::uint32_t value_;

 public:
  explicit my_class(std::uint32_t val)
      : value_(val) {}

  friend auto operator==(my_class const& lhs, my_class const& rhs) -> bool { return lhs.value_ == rhs.value_; }
  friend auto operator!=(my_class const& lhs, my_class const& rhs) -> bool { return lhs.value_ != rhs.value_; }
};
}  // namespace vector_test

using vector_test::my_class;

template <>
auto const arene::base::testing::test_value_array<my_class> =
    arene::base::to_array<my_class>({my_class{1}, my_class{2}, my_class{3}, my_class{4}, my_class{5}});

// As an arithmetic type, a default array is provided for double; defining this here overrides it.
template <>
constexpr auto arene::base::testing::test_value_array<double> =
    arene::base::to_array<double>({1., 2., -42., 1.23, 99, -10, 77, 65, 4.567, 3.123});

//! [vector_test_values_example]
#include "arene/base/array.hpp"
#include "arene/base/inline_container/testing/vector.hpp"

// For copyable types, the easiest way to bring in test values is by specializing test_value_array.
// For literal types (those which can be used in constexpr) test_value_array must be constexpr.
// As an arithmetic type, a default array is provided for std::int32_t; defining this here overrides it.

template <>
constexpr auto arene::base::testing::test_value_array<std::int32_t> =
    arene::base::to_array<std::int32_t>({1, -2, 6, -24, 120, -720, 5'040, -40'320, 362'880, -3'628'800});

// For non-literal types, the array can be const instead of constexpr.
// As std::string is a non-arithmetic type, defining this specialization is *required* to instantiate the tests for it.

template <>
auto const arene::base::testing::test_value_array<std::string> =
    arene::base::to_array<std::string>({"up", "down", "left", "right", "other"});

// Any tests populating an inline_vector with one or more values will pull them
// from the test_value_array specialization for that type.
//! [vector_test_values_example]

//! [vector_testing_example]
#include <gtest/gtest.h>

#include "arene/base/inline_container/testing/vector.hpp"

// This type list can be renamed, but is used in the instantiation below this
using TestTypes = ::testing::Types<
    arene::base::inline_vector<std::int32_t, 25>,
    arene::base::inline_vector<double, 10>,
    arene::base::inline_vector<std::string, 1>,  // consider using arene::base::optional instead
    arene::base::inline_vector<my_class, 20>>;

// "MyApp" here refers to your instantiation of the tests and can be renamed.
ARENE_INSTANTIATE_TESTS(MyApp, InlineVector, TestTypes);
//! [vector_testing_example]

//! [non_copy_constructible_test_example]
// By default, test_value copies the value from the test_value_array at the specified
// index, but you can specialize it with different behaviour for your type.

// This type can only be constructed by an integer.
struct not_copy_constructible {
  not_copy_constructible(int value_in)
      : internal_value{value_in} {}
  not_copy_constructible(not_copy_constructible const&) = delete;
  not_copy_constructible(not_copy_constructible&&) = default;

  int internal_value{};
};

// You can make a test_value_array specialization for this type if it helps with your
// implementation, but it's not actually required if you don't need it.

template <>
arene::base::array<not_copy_constructible, 3> const arene::base::testing::test_value_array<not_copy_constructible> =
    {not_copy_constructible{1}, not_copy_constructible{-1}, not_copy_constructible{999}};

// This test_value specialization knows how to construct not_copy_constructible.

template <>
auto arene::base::testing::test_value<not_copy_constructible>(std::size_t idx) -> not_copy_constructible {
  auto const& test_values = test_value_array<not_copy_constructible>;
  auto const& test_value_at_index = test_values[idx % test_values.size()];
  return not_copy_constructible{test_value_at_index.internal_value};
}
//! [non_copy_constructible_test_example]
