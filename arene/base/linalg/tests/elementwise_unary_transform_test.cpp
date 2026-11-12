// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/elementwise_unary_transform.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/functional/identity.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/negate.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {
using arene::base::array;
using arene::base::extents;
using arene::base::identity;
using arene::base::is_invocable_v;
using arene::base::is_nothrow_invocable_v;
using arene::base::mdspan;
using arene::base::linalg::elementwise_unary_transform;

// Unary operator with a specified input and output type; this is for testing invocability so it just returns zero
template <typename In, typename Out>
struct return_zero {
  constexpr auto operator()(In const&) const noexcept -> Out { return {}; }
};

// Unary operator that returns the argument squared
struct square {
  template <typename T>
  constexpr auto operator()(T value) const noexcept -> T {
    return value * value;
  }
};

/// @test Ensure cannot invoke @c elementwise_unary_transform if the span arguments are not mdspans
TEST(ElementwiseUnaryTransform, CannotInvokeWithNonMdspans) {
  ASSERT_FALSE((is_invocable_v<decltype(elementwise_unary_transform), int, int, identity>));
}

/// @test Ensure can invoke @c elementwise_unary_transform if the span arguments are matching mdspans
TEST(ElementwiseUnaryTransform, CanInvokeWithIdenticalMdSpansAndUnaryOperation) {
  using span1_t = mdspan<std::int32_t, extents<std::size_t, 1, 2, 3>>;
  using span2_t = mdspan<float, extents<std::size_t, 7, 2, 4>>;

  ASSERT_TRUE((is_invocable_v<decltype(elementwise_unary_transform), span1_t, span1_t, identity>));
  ASSERT_TRUE((is_invocable_v<decltype(elementwise_unary_transform), span2_t, span2_t, identity>));
}

/// @test Ensure cannot invoke @c elementwise_unary_transform if the span extents are not matching
TEST(ElementwiseUnaryTransform, CannotInvokeWithMismatchedMdSpansAndUnaryOperation) {
  using span1_t = mdspan<std::int32_t, extents<std::size_t, 1, 2, 3>>;
  using span2_t = mdspan<std::int32_t, extents<std::size_t, 7, 2, 4>>;

  ASSERT_FALSE((is_invocable_v<decltype(elementwise_unary_transform), span1_t, span2_t, identity>));
}

/// @test Ensure can invoke @c elementwise_unary_transform if the unary operation cannot be invoked
TEST(ElementwiseUnaryTransform, CannotInvokeWithIdenticalMdSpansAndWrongUnaryOperation) {
  using span1_t = mdspan<std::int32_t, extents<std::size_t, 1, 2, 3>>;
  using span2_t = mdspan<float, extents<std::size_t, 7, 2, 4>>;

  struct local {};

  ASSERT_FALSE(
      (is_invocable_v<decltype(elementwise_unary_transform), span1_t, span1_t, return_zero<local, std::int32_t>>)
  );
  ASSERT_FALSE((is_invocable_v<decltype(elementwise_unary_transform), span2_t, span2_t, return_zero<local, float>>));
}

/// @test Ensure can invoke @c elementwise_unary_transform if the result of the unary operation cannot be assigned
TEST(ElementwiseUnaryTransform, CannotInvokeWithIdenticalMdSpansAndWrongUnaryOperationOutput) {
  struct local {};
  using span1_t = mdspan<std::int32_t, extents<std::size_t, 1, 2, 3>>;
  using span1a_t = mdspan<local, extents<std::size_t, 7, 2, 4>>;
  using span2_t = mdspan<float, extents<std::size_t, 7, 2, 4>>;
  using span2a_t = mdspan<local, extents<std::size_t, 7, 2, 4>>;

  ASSERT_FALSE((is_invocable_v<decltype(elementwise_unary_transform), span1_t, span1a_t, identity>));
  ASSERT_FALSE((is_invocable_v<decltype(elementwise_unary_transform), span2_t, span2a_t, identity>));
}

/// @test @c elementwise_unary_transform applies the supplied transform to the input to yield the output
CONSTEXPR_TEST(ElementwiseUnaryTransform, AppliesTranform) {
  using in_span_t = mdspan<std::int32_t const, extents<std::size_t, 1, 2, 3>>;
  using out_span_t = mdspan<std::int32_t, extents<std::size_t, 1, 2, 3>>;

  array<std::int32_t, 6U> const in_data{1, 3, 7, 42, -100, -200};
  array<std::int32_t, 6U> out_data{};

  in_span_t const in_span{in_data.data()};
  out_span_t const out_span{out_data.data()};

  elementwise_unary_transform(in_span, out_span, std::negate<>{});

  array<std::int32_t, 6U> const expected{-1, -3, -7, -42, 100, 200};

  CONSTEXPR_ASSERT_EQ(out_data, expected);

  elementwise_unary_transform(in_span, out_span, square{});

  array<std::int32_t, 6U> const expected2{1, 9, 49, 1764, 10'000, 40'000};

  CONSTEXPR_ASSERT_EQ(out_data, expected2);
}

class throwing_op {
 public:
  template <typename T>
  auto operator()(T& value) const -> T& {
    return value;
  }
};

class non_throwing_op {
 public:
  template <typename T>
  auto operator()(T& value) const noexcept -> T& {
    return value;
  }
};

class throwing_assign {
 public:
  template <typename T>
  auto operator=(T const&) -> throwing_assign& {
    return *this;
  }
};

/// @test Ensure that the noexcept-ness of elementwise unary transform depends on the transform and assign of result
TEST(ElementwiseUnaryTransform, IsNoexceptIfOpsAre) {
  using span_t = mdspan<std::int32_t, extents<std::size_t, 1, 2, 3>>;

  ASSERT_TRUE((is_invocable_v<decltype(elementwise_unary_transform), span_t, span_t, throwing_op>));
  ASSERT_FALSE((is_nothrow_invocable_v<decltype(elementwise_unary_transform), span_t, span_t, throwing_op>));
  ASSERT_TRUE((is_nothrow_invocable_v<decltype(elementwise_unary_transform), span_t, span_t, non_throwing_op>));

  using throwing_out_span_t = mdspan<throwing_assign, extents<std::size_t, 1, 2, 3>>;

  ASSERT_TRUE((is_invocable_v<decltype(elementwise_unary_transform), span_t, throwing_out_span_t, non_throwing_op>));
  ASSERT_FALSE(
      (is_nothrow_invocable_v<decltype(elementwise_unary_transform), span_t, throwing_out_span_t, non_throwing_op>)
  );
}

/// @test Ensure @c elementwise_unary_transform correctly handles a transform that changes type
CONSTEXPR_TEST(ElementwiseUnaryTransform, TransformChangesType) {
  struct wrapper {
    std::int32_t data;

    constexpr auto operator==(wrapper const& other) const noexcept -> bool { return data == other.data; }
  };

  class transform {
   public:
    constexpr auto operator()(std::int32_t value) const noexcept -> wrapper { return wrapper{value}; }
  };

  using in_span_t = mdspan<std::int32_t const, extents<std::size_t, 1, 2, 3>>;
  using out_span_t = mdspan<wrapper, extents<std::size_t, 1, 2, 3>>;

  array<std::int32_t, 6U> const in_data{1, 3, 7, 42, -100, -200};
  array<wrapper, 6U> out_data{};

  in_span_t const in_span{in_data.data()};
  out_span_t const out_span{out_data.data()};

  elementwise_unary_transform(in_span, out_span, transform{});

  array<wrapper, 6U> const expected{{{1}, {3}, {7}, {42}, {-100}, {-200}}};

  CONSTEXPR_ASSERT_EQ(out_data, expected);
}

}  // namespace
