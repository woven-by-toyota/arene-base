// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/linalg/matrix_rank_1_update.hpp"
#include "arene/base/linalg/tests/configurable_scalar_value.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

using arene::base::is_invocable_v;
using arene::base::is_nothrow_invocable_v;
using arene::base::linalg::matrix_rank_1_update;

template <typename T>
using vec3_of = arene::base::mdspan<T, arene::base::extents<std::size_t, 3>>;
template <typename T>
using vec2_of = arene::base::mdspan<T, arene::base::extents<std::size_t, 2>>;
template <typename T>
using mat32_of = arene::base::mdspan<T, arene::base::extents<std::size_t, 3, 2>>;

using vec3_t = vec3_of<std::int32_t const>;
using vec2_t = vec2_of<std::int32_t const>;
using mat32_t = mat32_of<std::int32_t>;
using const_mat32_t = mat32_of<std::int32_t const>;

/// @brief Scalar type that is only explicitly constructible from @c std::int32_t
/// @tparam NothrowConversion whether the conversion from @c std::int32_t is @c noexcept
template <bool NothrowConversion>
class explicitly_convertible_scalar {
 public:
  std::int32_t value{};

  constexpr explicitly_convertible_scalar() noexcept = default;

  explicit constexpr explicitly_convertible_scalar(std::int32_t val) noexcept(NothrowConversion)
      : value{val} {}
};

/// @test @c matrix_rank_1_update is not invocable when the vector or matrix extents cannot match
TEST(MatrixRank1UpdateConstraintTest, NotInvocableWithMismatchedExtents) {
  using wrong_vec3_t = vec2_of<std::int32_t const>;
  using wrong_vec2_t = vec3_of<std::int32_t const>;

  // x has the wrong length relative to the number of rows of a_mat
  EXPECT_FALSE((is_invocable_v<decltype(matrix_rank_1_update), wrong_vec3_t, vec2_t, mat32_t>));
  EXPECT_FALSE((is_invocable_v<decltype(matrix_rank_1_update), wrong_vec3_t, vec2_t, const_mat32_t, mat32_t>));

  // y has the wrong length relative to the number of columns of a_mat
  EXPECT_FALSE((is_invocable_v<decltype(matrix_rank_1_update), vec3_t, wrong_vec2_t, mat32_t>));
  EXPECT_FALSE((is_invocable_v<decltype(matrix_rank_1_update), vec3_t, wrong_vec2_t, const_mat32_t, mat32_t>));

  // e_mat has extents that don't match a_mat
  using wrong_mat_t = arene::base::mdspan<std::int32_t const, arene::base::extents<std::size_t, 2, 3>>;
  EXPECT_FALSE((is_invocable_v<decltype(matrix_rank_1_update), vec3_t, vec2_t, wrong_mat_t, mat32_t>));
}

/// @test @c matrix_rank_1_update is not invocable when the output argument is not a matrix, or is not writeable
TEST(MatrixRank1UpdateConstraintTest, NotInvocableWithInvalidOutput) {
  // a_mat is not even an mdspan
  EXPECT_FALSE((is_invocable_v<decltype(matrix_rank_1_update), vec3_t, vec2_t, int>));

  // a_mat is not writeable
  EXPECT_FALSE((is_invocable_v<decltype(matrix_rank_1_update), vec3_t, vec2_t, const_mat32_t>));
  EXPECT_FALSE((is_invocable_v<decltype(matrix_rank_1_update), vec3_t, vec2_t, const_mat32_t, const_mat32_t>));

  // a_mat is rank 1, not a matrix
  EXPECT_FALSE((is_invocable_v<decltype(matrix_rank_1_update), vec3_t, vec2_t, vec3_t>));
}

/// @test @c matrix_rank_1_update is not invocable when the scalar type's multiplication or addition are disabled,
/// for both the overwriting (3-arg) and updating (4-arg) overloads
TEST(MatrixRank1UpdateConstraintTest, NotInvocableWithDisabledOperations) {
  using ::arene::base::linalg_testing::configurable_scalar_value;
  using ::testing::disable;

  using all_enabled = configurable_scalar_value<std::int32_t>;
  using no_mul = configurable_scalar_value<std::int32_t, disable::multiplies>;
  using no_add = configurable_scalar_value<std::int32_t, disable::plus>;
  using no_move = configurable_scalar_value<std::int32_t, disable::move_assign>;

  EXPECT_TRUE((is_invocable_v<
               decltype(matrix_rank_1_update),
               vec3_of<all_enabled>,
               vec2_of<all_enabled>,
               mat32_of<all_enabled>>));

  EXPECT_FALSE((is_invocable_v<decltype(matrix_rank_1_update), vec3_of<no_mul>, vec2_of<no_mul>, mat32_of<no_mul>>));
  EXPECT_FALSE((is_invocable_v<decltype(matrix_rank_1_update), vec3_of<no_add>, vec2_of<no_add>, mat32_of<no_add>>));
  EXPECT_FALSE((is_invocable_v<decltype(matrix_rank_1_update), vec3_of<no_move>, vec2_of<no_move>, mat32_of<no_move>>));

  EXPECT_TRUE((is_invocable_v<
               decltype(matrix_rank_1_update),
               vec3_of<all_enabled>,
               vec2_of<all_enabled>,
               mat32_of<all_enabled>,
               mat32_of<all_enabled>>));

  EXPECT_FALSE((is_invocable_v<
                decltype(matrix_rank_1_update),
                vec3_of<no_mul>,
                vec2_of<no_mul>,
                mat32_of<no_mul>,
                mat32_of<no_mul>>));
  EXPECT_FALSE((is_invocable_v<
                decltype(matrix_rank_1_update),
                vec3_of<no_add>,
                vec2_of<no_add>,
                mat32_of<no_add>,
                mat32_of<no_add>>));

  // The implementation assigns the (rvalue) result of the outer product into the output matrix. Assigning an rvalue
  // prefers move assignment over copy assignment, so disabling move assignment makes the assignment ill-formed even
  // though copy assignment remains available.
  EXPECT_FALSE((is_invocable_v<
                decltype(matrix_rank_1_update),
                vec3_of<no_move>,
                vec2_of<no_move>,
                mat32_of<no_move>,
                mat32_of<no_move>>));
}

/// @test @c matrix_rank_1_update is invocable when the sum is only explicitly convertible to the value type of the
/// output matrix
TEST(MatrixRank1UpdateConstraintTest, InvocableWithExplicitConversionToOutputValueType) {
  using explicit_scalar = explicitly_convertible_scalar<true>;

  EXPECT_FALSE((std::is_assignable<explicit_scalar&, std::int32_t>::value));
  EXPECT_TRUE((is_invocable_v<decltype(matrix_rank_1_update), vec3_t, vec2_t, const_mat32_t, mat32_of<explicit_scalar>>)
  );
}

/// @test @c matrix_rank_1_update is not noexcept when the scalar type's addition, multiplication, or assignment are
/// not noexcept, for both the overwriting (3-arg) and updating (4-arg) overloads
TEST(MatrixRank1UpdateConstraintTest, NoexceptIsCorrect) {
  using non_throwing = ::testing::configurable_value<std::int32_t>;
  using throwing_add = ::testing::configurable_value<std::int32_t, ::testing::throws_on::plus>;
  using throwing_mul = ::testing::configurable_value<std::int32_t, ::testing::throws_on::multiply>;
  using throwing_move = ::testing::configurable_value<std::int32_t, ::testing::throws_on::move_assign>;

  EXPECT_TRUE((is_invocable_v<
               decltype(matrix_rank_1_update),
               vec3_of<non_throwing>,
               vec2_of<non_throwing>,
               mat32_of<non_throwing>>));
  EXPECT_TRUE((is_nothrow_invocable_v<
               decltype(matrix_rank_1_update),
               vec3_of<non_throwing>,
               vec2_of<non_throwing>,
               mat32_of<non_throwing>>));

  EXPECT_TRUE((is_invocable_v<
               decltype(matrix_rank_1_update),
               vec3_of<throwing_add>,
               vec2_of<throwing_add>,
               mat32_of<throwing_add>>));
  EXPECT_FALSE((is_nothrow_invocable_v<
                decltype(matrix_rank_1_update),
                vec3_of<throwing_add>,
                vec2_of<throwing_add>,
                mat32_of<throwing_add>>));

  EXPECT_TRUE((is_invocable_v<
               decltype(matrix_rank_1_update),
               vec3_of<throwing_mul>,
               vec2_of<throwing_mul>,
               mat32_of<throwing_mul>>));
  EXPECT_FALSE((is_nothrow_invocable_v<
                decltype(matrix_rank_1_update),
                vec3_of<throwing_mul>,
                vec2_of<throwing_mul>,
                mat32_of<throwing_mul>>));

  EXPECT_TRUE((is_invocable_v<
               decltype(matrix_rank_1_update),
               vec3_of<non_throwing>,
               vec2_of<non_throwing>,
               mat32_of<non_throwing>,
               mat32_of<non_throwing>>));
  EXPECT_TRUE((is_nothrow_invocable_v<
               decltype(matrix_rank_1_update),
               vec3_of<non_throwing>,
               vec2_of<non_throwing>,
               mat32_of<non_throwing>,
               mat32_of<non_throwing>>));

  EXPECT_TRUE((is_invocable_v<
               decltype(matrix_rank_1_update),
               vec3_of<throwing_add>,
               vec2_of<throwing_add>,
               mat32_of<throwing_add>,
               mat32_of<throwing_add>>));
  EXPECT_FALSE((is_nothrow_invocable_v<
                decltype(matrix_rank_1_update),
                vec3_of<throwing_add>,
                vec2_of<throwing_add>,
                mat32_of<throwing_add>,
                mat32_of<throwing_add>>));

  EXPECT_TRUE((is_invocable_v<
               decltype(matrix_rank_1_update),
               vec3_of<throwing_mul>,
               vec2_of<throwing_mul>,
               mat32_of<throwing_mul>,
               mat32_of<throwing_mul>>));
  EXPECT_FALSE((is_nothrow_invocable_v<
                decltype(matrix_rank_1_update),
                vec3_of<throwing_mul>,
                vec2_of<throwing_mul>,
                mat32_of<throwing_mul>,
                mat32_of<throwing_mul>>));

  EXPECT_TRUE((is_invocable_v<
               decltype(matrix_rank_1_update),
               vec3_of<throwing_move>,
               vec2_of<throwing_move>,
               mat32_of<throwing_move>,
               mat32_of<throwing_move>>));
  EXPECT_FALSE((is_nothrow_invocable_v<
                decltype(matrix_rank_1_update),
                vec3_of<throwing_move>,
                vec2_of<throwing_move>,
                mat32_of<throwing_move>,
                mat32_of<throwing_move>>));

  EXPECT_TRUE((is_nothrow_invocable_v<
               decltype(matrix_rank_1_update),
               vec3_t,
               vec2_t,
               const_mat32_t,
               mat32_of<explicitly_convertible_scalar<true>>>));
  EXPECT_FALSE((is_nothrow_invocable_v<
                decltype(matrix_rank_1_update),
                vec3_t,
                vec2_t,
                const_mat32_t,
                mat32_of<explicitly_convertible_scalar<false>>>));
}

}  // namespace
