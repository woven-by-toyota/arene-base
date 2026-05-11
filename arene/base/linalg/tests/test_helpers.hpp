// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_TEST_HELPERS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_TEST_HELPERS_HPP_

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/filter.hpp"
#include "arene/base/type_list/flat_map.hpp"
#include "arene/base/type_list/size.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/disjunction.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace arene {
namespace base {
namespace linalg_testing {

/// @brief Get the rank of an object which may or may not be an mdspan. Non-mdspan objects are taken to have rank 0.
/// @{
template <typename T>
constexpr std::size_t rank_v{0U};

template <typename T>
constexpr std::size_t rank_v<T const>{rank_v<T>};

template <typename T>
constexpr std::size_t rank_v<T volatile>{rank_v<T>};

template <typename T>
constexpr std::size_t rank_v<T const volatile>{rank_v<T>};

template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
constexpr std::size_t rank_v<arene::base::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>>{Extents::rank()};
/// @}

/// @brief An expectation category for a type: is it a linalg object or not, and what kind?
enum class category { nothing, scalar, just_mdspan, in_vector, inout_vector, in_matrix, inout_matrix };

/// @brief A test parameter with an expected category, for testing whether or not type traits apply
template <typename Type, category Expectation>
struct param {
  using type = Type;
  static constexpr category expectation = Expectation;
};

using ::arene::base::dynamic_extent;
using ::testing::configurable_value;
using ::testing::disable;
using ::testing::throws_on;

/// @brief A generic uncopyable type for testing
using not_copyable =
    configurable_value<std::int32_t, throws_on::nothing, disable::copy_construct | disable::copy_assign>;

/// @brief A generic unmovable type for testing
using not_movable =
    configurable_value<std::int32_t, throws_on::nothing, disable::move_construct | disable::move_assign>;

/// @brief Simplified implementation of std::linalg::scaled_accessor which always scales by std::int32_t{2}.
/// @note mdspans using this accessor are only input-eligible (not output-eligible) because the reference typedef is not
/// actually assignable.
class doubled_accessor : private arene::base::default_accessor<std::int32_t> {
 public:
  using element_type = std::int32_t;  // This is const for scaled_accessor but non-const also meets the accessor reqs.
  using reference = std::int32_t;
  using data_handle_type = std::int32_t*;
  using offset_policy = doubled_accessor;

  constexpr auto access(data_handle_type hdl, std::size_t idx) const noexcept -> reference {
    return std::int32_t{2} * arene::base::default_accessor<std::int32_t>::access(hdl, idx);
  }
  constexpr auto offset(data_handle_type hdl, std::size_t idx) const noexcept -> data_handle_type {
    return arene::base::default_accessor<std::int32_t>::offset(hdl, idx);
  }
};

/// @brief A list of un-cv-qualified mdspan types along with the object categories we expect them to be in
using unqualified_mdspan_types = ::testing::Types<
    // Types that aren't even scalars
    param<configurable_value<std::int16_t, throws_on::nothing, disable::default_construct>, category::nothing>,
    param<configurable_value<std::int32_t, throws_on::nothing, disable::copy_construct>, category::nothing>,
    param<configurable_value<std::int64_t, throws_on::nothing, disable::move_construct>, category::nothing>,
    param<configurable_value<std::uint8_t, throws_on::nothing, disable::copy_assign>, category::nothing>,
    param<configurable_value<float, throws_on::nothing, disable::move_assign>, category::nothing>,
    // Scalar types
    param<std::int8_t, category::scalar>,
    param<std::uint16_t, category::scalar>,
    param<std::int32_t, category::scalar>,
    param<std::uint64_t, category::scalar>,
    param<float, category::scalar>,
    param<double, category::scalar>,
    param<configurable_value<std::int32_t>, category::scalar>,
    param<configurable_value<double, throws_on::nothing, disable::swap>, category::scalar>,  // still swappable w/ move
    // Mdspans that are neither vectors nor matrices
    param<arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t>>, category::just_mdspan>,
    param<arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3>>, category::just_mdspan>,
    param<arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3, 4>>, category::just_mdspan>,
    param<arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3, 4, 5>>, category::just_mdspan>,
    param<arene::base::mdspan<double, arene::base::dextents<std::size_t, 0>>, category::just_mdspan>,
    param<arene::base::mdspan<double, arene::base::dextents<std::size_t, 3>>, category::just_mdspan>,
    param<arene::base::mdspan<double, arene::base::dextents<std::size_t, 4>>, category::just_mdspan>,
    param<arene::base::mdspan<double, arene::base::dextents<std::size_t, 5>>, category::just_mdspan>,
    param<
        arene::base::mdspan<std::uint32_t, arene::base::dextents<std::size_t, 5>, arene::base::layout_left>,
        category::just_mdspan>,
    param<
        arene::base::mdspan<std::uint32_t, arene::base::extents<std::size_t, 6, 7, 8>, arene::base::layout_stride>,
        category::just_mdspan>,
    // Even when the dimensions are correct, an mdspan whose value type is not a scalar is not a vector or matrix
    param<arene::base::mdspan<not_movable, arene::base::extents<std::size_t, 1>>, category::just_mdspan>,
    param<arene::base::mdspan<not_movable, arene::base::dextents<std::size_t, 1>>, category::just_mdspan>,
    param<arene::base::mdspan<not_copyable, arene::base::extents<std::size_t, 1>>, category::just_mdspan>,
    param<arene::base::mdspan<not_copyable, arene::base::dextents<std::size_t, 1>>, category::just_mdspan>,
    param<
        arene::base::mdspan<not_movable, arene::base::extents<std::size_t, dynamic_extent, 2>>,
        category::just_mdspan>,
    param<arene::base::mdspan<not_movable, arene::base::dextents<std::size_t, 2>>, category::just_mdspan>,
    param<arene::base::mdspan<not_copyable, arene::base::extents<std::size_t, 1, 2>>, category::just_mdspan>,
    param<arene::base::mdspan<not_copyable, arene::base::dextents<std::size_t, 2>>, category::just_mdspan>,
    // Mdspans that are input vectors only but not writeable
    param<arene::base::mdspan<double const, arene::base::extents<std::size_t, 1>>, category::in_vector>,
    param<arene::base::mdspan<double const, arene::base::dextents<std::size_t, 1>>, category::in_vector>,
    param<
        arene::base::mdspan<std::uint32_t const, arene::base::extents<std::size_t, 4>, arene::base::layout_left>,
        category::in_vector>,
    param<
        arene::base::
            mdspan<std::int32_t, arene::base::extents<std::size_t, 1>, arene::base::layout_right, doubled_accessor>,
        category::in_vector>,
    // Mdspans that are input/output vectors
    param<arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 3>>, category::inout_vector>,
    param<arene::base::mdspan<double, arene::base::extents<std::size_t, 1>>, category::inout_vector>,
    param<arene::base::mdspan<double, arene::base::dextents<std::size_t, 1>>, category::inout_vector>,
    param<
        arene::base::mdspan<std::uint32_t, arene::base::extents<std::size_t, 4>, arene::base::layout_left>,
        category::inout_vector>,
    param<
        arene::base::mdspan<std::uint32_t, arene::base::dextents<std::size_t, 1>, arene::base::layout_stride>,
        category::inout_vector>,
    // Mdspans that are input matrices only but not writeable
    param<arene::base::mdspan<std::int32_t const, arene::base::extents<std::size_t, 3, 4>>, category::in_matrix>,
    param<arene::base::mdspan<double const, arene::base::extents<std::size_t, 1, dynamic_extent>>, category::in_matrix>,
    param<
        arene::base::mdspan<
            std::uint32_t const,
            arene::base::extents<std::size_t, 6, arene::base::dynamic_extent>,
            arene::base::layout_stride>,
        category::in_matrix>,
    param<
        arene::base::
            mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2>, arene::base::layout_right, doubled_accessor>,
        category::in_matrix>,
    // Mdspans that are input/output matrices
    param<arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 3, 4>>, category::inout_matrix>,
    param<arene::base::mdspan<double, arene::base::extents<std::size_t, 1, dynamic_extent>>, category::inout_matrix>,
    param<arene::base::mdspan<double, arene::base::dextents<std::size_t, 2>>, category::inout_matrix>,
    param<
        arene::base::mdspan<
            std::uint32_t,
            arene::base::extents<std::size_t, arene::base::dynamic_extent, 4>,
            arene::base::layout_left>,
        category::inout_matrix>,
    param<
        arene::base::mdspan<
            std::uint32_t,
            arene::base::extents<std::size_t, 6, arene::base::dynamic_extent>,
            arene::base::layout_stride>,
        category::inout_matrix>>;

/// @brief Given a @c param type, emit a list of equivalent @c param specializations with all cv-qualifications
template <typename T>
using all_cv_variants = arene::base::type_list<
    param<typename T::type, T::expectation>,
    param<typename T::type const, T::expectation>,
    param<typename T::type volatile, T::expectation>,
    param<typename T::type const volatile, T::expectation>>;

/// @brief A list of mdspan types along with the object categories we expect them to be in, with all cv-qualifications
using mdspan_types = arene::base::type_lists::flat_map_t<unqualified_mdspan_types, all_cv_variants>;

static_assert(
    arene::base::type_lists::size_v<mdspan_types> == 4U * arene::base::type_lists::size_v<unqualified_mdspan_types>,
    "cv-qualified list should be 4 times the size of the unqualified list"
);

/// @brief Unpack a @c param instance to just the contained type, for cases where we don't care which category it is
template <typename T>
using unpack_param = typename T::type;

/// @brief Check if a @c param is expected to be a linalg vector; used for filtering the big list to just vectors
template <typename T>
struct should_be_linalg_vector
    : std::integral_constant<
          bool,                                           //
          (T::expectation == category::in_vector) ||      //
              (T::expectation == category::inout_vector)  //
          > {};

/// @brief Check if a @c param is expected to be a linalg matrix; used for filtering the big list to just matrices
template <typename T>
struct should_be_linalg_matrix
    : std::integral_constant<
          bool,                                           //
          (T::expectation == category::in_matrix) ||      //
              (T::expectation == category::inout_matrix)  //
          > {};

/// @brief Check if a @c param is expected to be a linalg object; used for filtering the big list to just linalg objects
template <typename T>
using should_be_linalg_object = disjunction<should_be_linalg_vector<T>, should_be_linalg_matrix<T>>;

/// @brief A big list of all the test mdspans which are linalg objects of any kind, ignoring rank and in/out category
using linalg_vector_types = arene::base::type_lists::
    apply_each_t<arene::base::type_lists::filter_t<mdspan_types, should_be_linalg_vector>, unpack_param>;

/// @brief A big list of all the test mdspans which are linalg objects of any kind, ignoring rank and in/out category
using linalg_matrix_types = arene::base::type_lists::
    apply_each_t<arene::base::type_lists::filter_t<mdspan_types, should_be_linalg_matrix>, unpack_param>;

/// @brief A big list of all the test mdspans which are linalg objects of any kind, ignoring rank and in/out category
using linalg_object_types = arene::base::type_lists::
    apply_each_t<arene::base::type_lists::filter_t<mdspan_types, should_be_linalg_object>, unpack_param>;

}  // namespace linalg_testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_TEST_HELPERS_HPP_
