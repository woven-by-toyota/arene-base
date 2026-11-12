// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/elementwise_binary_transform.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/minus.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {
/// @test Ensure cannot invoke @c elementwise_binary_transform if the span arguments are not mdspans
TEST(ElementwiseBinaryTransform, CannotInvokeWithNonMdspans) {
  ASSERT_FALSE(
      (arene::base::
           is_invocable_v<decltype(arene::base::linalg::elementwise_binary_transform), int, int, int, std::plus<>>)
  );
}

/// @test Ensure can invoke @c elementwise_binary_transform if the span arguments are matching mdspans
TEST(ElementwiseBinaryTransform, CanInvokeWithIdenticalMdSpansAndBinaryOperation) {
  using span1_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3>>;
  using span2_t = arene::base::mdspan<float, arene::base::extents<std::size_t, 7, 2, 4>>;

  ASSERT_TRUE((arene::base::is_invocable_v<
               decltype(arene::base::linalg::elementwise_binary_transform),
               span1_t,
               span1_t,
               span1_t,
               std::plus<>>));
  ASSERT_TRUE((arene::base::is_invocable_v<
               decltype(arene::base::linalg::elementwise_binary_transform),
               span2_t,
               span2_t,
               span2_t,
               std::plus<>>));
}

/// @test Ensure cannot invoke @c elementwise_binary_transform if the span extents are not matching
TEST(ElementwiseBinaryTransform, CannotInvokeWithMismatchedMdSpansAndBinaryOperation) {
  using span1_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3>>;
  using span2_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 7, 2, 4>>;

  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(arene::base::linalg::elementwise_binary_transform),
                span2_t,
                span1_t,
                span1_t,
                std::plus<>>));
  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(arene::base::linalg::elementwise_binary_transform),
                span1_t,
                span2_t,
                span1_t,
                std::plus<>>));
  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(arene::base::linalg::elementwise_binary_transform),
                span1_t,
                span1_t,
                span2_t,
                std::plus<>>));
}

/// @test Ensure can invoke @c elementwise_binary_transform if the binary operation cannot be invoked
TEST(ElementwiseBinaryTransform, CannotInvokeWithIdenticalMdSpansAndWrongBinaryOperation) {
  using span1_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3>>;
  using span2_t = arene::base::mdspan<float, arene::base::extents<std::size_t, 7, 2, 4>>;

  struct local {};

  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(arene::base::linalg::elementwise_binary_transform),
                span1_t,
                span1_t,
                span1_t,
                std::plus<local>>));
  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(arene::base::linalg::elementwise_binary_transform),
                span2_t,
                span2_t,
                span2_t,
                std::plus<local>>));
}

/// @test Ensure can invoke @c elementwise_binary_transform if the result of the binary operation cannot be assigned
TEST(ElementwiseBinaryTransform, CannotInvokeWithIdenticalMdSpansAndWrongBinaryOperationOutput) {
  struct local {};
  using span1_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3>>;
  using span1a_t = arene::base::mdspan<local, arene::base::extents<std::size_t, 7, 2, 4>>;
  using span2_t = arene::base::mdspan<float, arene::base::extents<std::size_t, 7, 2, 4>>;
  using span2a_t = arene::base::mdspan<local, arene::base::extents<std::size_t, 7, 2, 4>>;

  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(arene::base::linalg::elementwise_binary_transform),
                span1_t,
                span1_t,
                span1a_t,
                std::plus<>>));
  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(arene::base::linalg::elementwise_binary_transform),
                span2_t,
                span2_t,
                span2a_t,
                std::plus<>>));
}

/// @test @c elementwise_binary_transform applies the supplied transform to the input to yield the output
CONSTEXPR_TEST(ElementwiseBinaryTransform, AppliesTranform) {
  using in_span_t = arene::base::mdspan<std::int32_t const, arene::base::extents<std::size_t, 1, 2, 3>>;
  using out_span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3>>;

  arene::base::array<std::int32_t, 6U> const lhs{1, 3, 7, 42, -100, -200};
  arene::base::array<std::int32_t, 6U> const rhs{100, 200, 300, 400, -500, 600};
  arene::base::array<std::int32_t, 6U> out{};

  in_span_t const lhs_span{lhs.data()};
  in_span_t const rhs_span{rhs.data()};
  out_span_t const out_span{out.data()};

  arene::base::linalg::elementwise_binary_transform(lhs_span, rhs_span, out_span, std::plus<>{});

  arene::base::array<std::int32_t, 6U> const expected{101, 203, 307, 442, -600, 400};

  CONSTEXPR_ASSERT_EQ(out, expected);

  arene::base::linalg::elementwise_binary_transform(lhs_span, rhs_span, out_span, std::minus<>{});

  arene::base::array<std::int32_t, 6U> const expected2{-99, -197, -293, -358, 400, -800};

  CONSTEXPR_ASSERT_EQ(out, expected2);
}

class throwing_op {
 public:
  template <typename T>
  auto operator()(T& lhs, T&) const -> T& {
    return lhs;
  }
};

class non_throwing_op {
 public:
  template <typename T>
  auto operator()(T& lhs, T&) const noexcept -> T& {
    return lhs;
  }
};

class throwing_assign {
 public:
  template <typename T>
  auto operator=(T const&) -> throwing_assign& {
    return *this;
  }
};

/// @test Ensure that the noexcept-ness of elementwise binary transform depends on the transform and assign of result
TEST(ElementwiseBinaryTransform, IsNoexceptIfOpsAre) {
  using span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3>>;

  ASSERT_TRUE((arene::base::is_invocable_v<
               decltype(arene::base::linalg::elementwise_binary_transform),
               span_t,
               span_t,
               span_t,
               throwing_op>));
  ASSERT_FALSE((arene::base::is_nothrow_invocable_v<
                decltype(arene::base::linalg::elementwise_binary_transform),
                span_t,
                span_t,
                span_t,
                throwing_op>));
  ASSERT_TRUE((arene::base::is_nothrow_invocable_v<
               decltype(arene::base::linalg::elementwise_binary_transform),
               span_t,
               span_t,
               span_t,
               non_throwing_op>));

  using throwing_out_span_t = arene::base::mdspan<throwing_assign, arene::base::extents<std::size_t, 1, 2, 3>>;

  ASSERT_TRUE((arene::base::is_invocable_v<
               decltype(arene::base::linalg::elementwise_binary_transform),
               span_t,
               span_t,
               throwing_out_span_t,
               non_throwing_op>));
  ASSERT_FALSE((arene::base::is_nothrow_invocable_v<
                decltype(arene::base::linalg::elementwise_binary_transform),
                span_t,
                span_t,
                throwing_out_span_t,
                non_throwing_op>));
}

/// @test Ensure @c elementwise_binary_transform correctly handles a transform that changes type
CONSTEXPR_TEST(ElementwiseBinaryTransform, TransformChangesType) {
  struct wrapper {
    std::int32_t data;

    constexpr auto operator==(wrapper const& other) const noexcept -> bool { return data == other.data; }
  };

  class transform {
   public:
    constexpr auto operator()(std::int32_t lhs, std::int32_t rhs) const noexcept -> wrapper {
      return wrapper{lhs + rhs};
    }
  };

  using in_span_t = arene::base::mdspan<std::int32_t const, arene::base::extents<std::size_t, 1, 2, 3>>;
  using out_span_t = arene::base::mdspan<wrapper, arene::base::extents<std::size_t, 1, 2, 3>>;

  arene::base::array<std::int32_t, 6U> const lhs{1, 3, 7, 42, -100, -200};
  arene::base::array<std::int32_t, 6U> const rhs{100, 200, 300, 400, -500, 600};
  arene::base::array<wrapper, 6U> out{};

  in_span_t const lhs_span{lhs.data()};
  in_span_t const rhs_span{rhs.data()};
  out_span_t const out_span{out.data()};

  arene::base::linalg::elementwise_binary_transform(lhs_span, rhs_span, out_span, transform{});

  arene::base::array<wrapper, 6U> const expected{{{101}, {203}, {307}, {442}, {-600}, {400}}};

  CONSTEXPR_ASSERT_EQ(out, expected);
}

}  // namespace
