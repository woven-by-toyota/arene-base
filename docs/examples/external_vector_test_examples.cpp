#include "arene/base/array.hpp"
#include "arene/base/inline_container/testing/external_vector.hpp"

namespace external_vector_test {
class my_class {
  std::uint32_t value_;

 public:
  explicit my_class(std::uint32_t val)
      : value_(val) {}

  friend auto operator==(my_class const& lhs, my_class const& rhs) -> bool { return lhs.value_ == rhs.value_; }
  friend auto operator!=(my_class const& lhs, my_class const& rhs) -> bool { return lhs.value_ != rhs.value_; }
};

class my_other_class {
  std::uint32_t value_;

 public:
  explicit my_other_class(std::uint32_t val, std::uint32_t)
      : value_(val) {}

  friend auto operator==(my_other_class const& lhs, my_other_class const& rhs) -> bool {
    return lhs.value_ == rhs.value_;
  }
  friend auto operator!=(my_other_class const& lhs, my_other_class const& rhs) -> bool {
    return lhs.value_ != rhs.value_;
  }
};
}  // namespace external_vector_test

using external_vector_test::my_class;
using external_vector_test::my_other_class;

//! [external_vector_testing_example]
#include <gtest/gtest.h>

#include "arene/base/inline_container/testing/external_vector.hpp"

template <>
auto const arene::base::testing::test_value_array<my_class> =
    arene::base::to_array<my_class>({my_class{1}, my_class{2}, my_class{3}, my_class{4}, my_class{5}});

template <>
auto const arene::base::testing::test_value_array<my_other_class> = arene::base::to_array<my_other_class>(
    {my_other_class{1, 29},
     my_other_class{2, 213},
     my_other_class{3, 99},
     my_other_class{4, 218391},
     my_other_class{5, 324}}
);

template <>
constexpr auto arene::base::testing::test_value_array<std::uint64_t> = arene::base::to_array<std::uint64_t>(
    {1,
     1'234'567'890'123'456'789ULL,
     2,
     6,
     24,
     11'988'127'746'783'356'837ULL,
     120,
     720,
     5'040,
     40'320,
     362'880,
     3'628'800,
     15'234'563'734'998ULL}
);

// This type list can be renamed, but is used in the instantiation below this
using TestTypes = ::testing::Types<std::uint64_t, my_class, my_other_class>;

// "MyApp" here refers to your instantiation of the tests and can be renamed.
ARENE_INSTANTIATE_TESTS(MyApp, ExternalVector, TestTypes);
//! [external_vector_testing_example]
