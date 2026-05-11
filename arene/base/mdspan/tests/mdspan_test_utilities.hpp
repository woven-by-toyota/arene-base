// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_MDSPAN_TEST_UTILITIES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_MDSPAN_TEST_UTILITIES_HPP_

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/make_signed.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/type_traits/conditional.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace arene {
namespace base {
namespace testing {
namespace detail {

/// @brief Given an @c extents type, get another @c extents type that can be implicitly converted to the original one
template <typename Extents>
struct implicitly_convertible_extents_impl {};

/// @brief Given an @c extents type, get another @c extents type that can be implicitly converted to the original one
template <typename IndexType, std::size_t... Extents>
struct implicitly_convertible_extents_impl<arene::base::extents<IndexType, Extents...>> {
  // extents::index_type must always be integral, so we need to find an integer type that can be implicitly converted.
  using unsigned_idx = std::make_unsigned_t<IndexType>;
  using smaller = conditional_t<
      std::is_same<unsigned_idx, std::uint64_t>::value,
      std::uint32_t,
      conditional_t<std::is_same<unsigned_idx, std::uint32_t>::value, std::uint16_t, std::uint8_t>>;
  using convertible_idx = conditional_t<std::is_signed<IndexType>::value, std::make_signed_t<smaller>, smaller>;

  using type = arene::base::extents<convertible_idx, Extents...>;
  static_assert(std::is_convertible<type, arene::base::extents<IndexType, Extents...>>::value, "");
};

/// @brief Given an @c extents type, get another @c extents type that can be explicitly converted to the original one
template <typename Extents>
struct explicitly_convertible_extents_impl {};

/// @brief Given an @c extents type, get another @c extents type that can be explicitly converted to the original one
template <typename IndexType, std::size_t... Extents>
struct explicitly_convertible_extents_impl<arene::base::extents<IndexType, Extents...>> {
  // extents::index_type must always be integral, so we need to find an integer type that can be explicitly converted.
  using unsigned_idx = std::make_unsigned_t<IndexType>;
  using larger = conditional_t<
      std::is_same<unsigned_idx, std::uint8_t>::value,
      std::uint16_t,
      conditional_t<std::is_same<unsigned_idx, std::uint16_t>::value, std::uint32_t, std::uint64_t>>;
  using new_idx = conditional_t<std::is_signed<IndexType>::value, std::make_signed_t<larger>, larger>;

  using type = arene::base::extents<new_idx, Extents...>;
  static_assert(!std::is_convertible<type, arene::base::extents<IndexType, Extents...>>::value, "");
};

}  // namespace detail

/// @brief Given an @c extents type, get another @c extents type that can be implicitly converted to the original one
template <typename Extents>
using implicitly_convertible_extents = typename detail::implicitly_convertible_extents_impl<Extents>::type;

/// @brief Given an @c extents type, get another @c extents type that can be explicitly converted to the original one
template <typename Extents>
using explicitly_convertible_extents = typename detail::explicitly_convertible_extents_impl<Extents>::type;

/// @brief Helper to extract the mdspan template parameters
/// @tparam Mdspan The mdspan type
/// @{
template <class Mdspan>
struct mdspan_parameters {};

template <class ElementType, class Extents, class LayoutPolicy, class AccessorPolicy>
struct mdspan_parameters<arene::base::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>> {
  using element_type = ElementType;
  using extents_type = Extents;
  using layout_type = LayoutPolicy;
  using accessor_type = AccessorPolicy;
};
/// @}

/// @brief Helper to rebind an mdspan to a different element_type
/// @tparam Mdspan The mdspan to rebind
/// @tparam NewElementType The new element_type to use
/// @{
template <class Mdspan, class NewElementType>
struct rebind_mdspan_element_type {};

template <class ElementType, class Extents, class LayoutPolicy, class AccessorPolicy, class NewElementType>
struct rebind_mdspan_element_type<
    arene::base::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>,
    NewElementType> {
  using type = arene::base::mdspan<NewElementType, Extents, LayoutPolicy, AccessorPolicy>;
};

template <class Mdspan, class NewElementType>
using rebind_mdspan_element_type_t = typename rebind_mdspan_element_type<Mdspan, NewElementType>::type;

/// @}
/// @brief Helper to rebind an mdspan to a different extents
/// @tparam Mdspan The mdspan to rebind
/// @tparam NewExtents The new extents to use
/// @{
template <class Mdspan, class NewExtents>
struct rebind_mdspan_extents {};

template <class ElementType, class Extents, class LayoutPolicy, class AccessorPolicy, class NewExtents>
struct rebind_mdspan_extents<arene::base::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>, NewExtents> {
  using type = arene::base::mdspan<ElementType, NewExtents, LayoutPolicy, AccessorPolicy>;
};

template <class Mdspan, class NewExtents>
using rebind_mdspan_extents_t = typename rebind_mdspan_extents<Mdspan, NewExtents>::type;
/// @}

/// @brief Helper to rebind an mdspan to a different layout
/// @tparam Mdspan The mdspan to rebind
/// @tparam NewLayout The new layout to use
/// @{
template <class Mdspan, class NewLayout>
struct rebind_mdspan_layout {};

template <class ElementType, class Extents, class LayoutPolicy, class AccessorPolicy, class NewLayout>
struct rebind_mdspan_layout<arene::base::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>, NewLayout> {
  using type = arene::base::mdspan<ElementType, Extents, NewLayout, AccessorPolicy>;
};

template <class Mdspan, class NewLayout>
using rebind_mdspan_layout_t = typename rebind_mdspan_layout<Mdspan, NewLayout>::type;
/// @}

/// @brief Helper to rebind an mdspan to a different accessor
/// @tparam Mdspan The mdspan to rebind
/// @tparam NewAccessor The new accessor to use
/// @{
template <class Mdspan, class NewAccessor>
struct rebind_mdspan_accessor {};

template <class ElementType, class Extents, class LayoutPolicy, class AccessorPolicy, class NewAccessor>
struct rebind_mdspan_accessor<arene::base::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>, NewAccessor> {
  using type = arene::base::mdspan<ElementType, Extents, LayoutPolicy, NewAccessor>;
};

template <class Mdspan, class NewAccessor>
using rebind_mdspan_accessor_t = typename rebind_mdspan_accessor<Mdspan, NewAccessor>::type;
/// @}

/// @brief Helper to apply a transformation to the element type of an AccessorPolicy
/// @tparam AccessorPolicy The accessor policy to transform
/// @tparam UnaryTrait Eager transformation to apply (e.g. `std::add_const_t`)
/// @note May not work with all Accessors
/// @{
template <class AccessorPolicy, template <class> class UnaryTrait>
struct transform_accessor_element;

template <template <class...> class AccessorPolicy, class Head, class... Tail, template <class> class UnaryTrait>
struct transform_accessor_element<AccessorPolicy<Head, Tail...>, UnaryTrait> {
  using type = AccessorPolicy<UnaryTrait<Head>, Tail...>;
};
/// @}

/// @brief Helper to apply a transformation to the mdspan element type
/// @tparam Mdspan The mdspan to rebind
/// @tparam UnaryTrait Eager transformation to apply (e.g. `std::add_const_t`)
/// @note May not work with all Accessors
/// @{
template <class Mdspan, template <class> class UnaryTrait>
struct mdspan_transform_element_type {};

template <class ElementType, class Extents, class LayoutPolicy, class AccessorPolicy, template <class> class UnaryTrait>
struct mdspan_transform_element_type<
    arene::base::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>,
    UnaryTrait> {
  using type = arene::base::mdspan<
      UnaryTrait<ElementType>,
      Extents,
      LayoutPolicy,
      typename transform_accessor_element<AccessorPolicy, UnaryTrait>::type>;
};

template <class Mdspan, template <class> class UnaryTrait>
using mdspan_transform_element_type_t = typename mdspan_transform_element_type<Mdspan, UnaryTrait>::type;
/// @}

template <class Mdspan>
constexpr auto dynamic_extents_data_v = arene::base::
    sequential_values_from<typename Mdspan::index_type, typename Mdspan::index_type{1}, Mdspan::rank_dynamic()>;

/// @brief Create an array of all extent values, with the dynamic extents sequentially increasing.
///
/// Note: This requires a specialization for the rank() = 0 case, as otherwise gcc8 will complain that the function
/// cannot be used in a constant expression due to not executing the for loop.
template <class Mdspan, arene::base::constraints<std::enable_if_t<Mdspan::rank() == 0>> = nullptr>
constexpr auto make_all_extents_data() -> arene::base::array<typename Mdspan::index_type, Mdspan::rank()> {
  return arene::base::array<typename Mdspan::index_type, Mdspan::rank()>{};
}

/// @brief Create an array of all extent values, with the dynamic extents sequentially increasing.
template <class Mdspan, arene::base::constraints<std::enable_if_t<Mdspan::rank() != 0>> = nullptr>
constexpr auto make_all_extents_data() -> arene::base::array<typename Mdspan::index_type, Mdspan::rank()> {
  auto extents = arene::base::array<typename Mdspan::index_type, Mdspan::rank()>{};
  auto next_dynamic_extent = typename Mdspan::index_type{1};

  for (auto dim : arene::base::sequential_values<typename Mdspan::rank_type, Mdspan::rank()>) {
    if (Mdspan::static_extent(dim) == arene::base::dynamic_extent) {
      extents[dim] = next_dynamic_extent;
      ++next_dynamic_extent;
    } else {
      extents[dim] = static_cast<typename Mdspan::index_type>(Mdspan::static_extent(dim));
    }
  }
  return extents;
}

template <class Mdspan>
constexpr auto all_extents_data_v = make_all_extents_data<Mdspan>();

/// @brief Construct an extents object for the given mdspan with the dynamic extents all set.
/// @{
template <class Mdspan, arene::base::constraints<std::enable_if_t<(Mdspan::rank_dynamic > 0)>> = nullptr>
constexpr auto make_extents() -> typename Mdspan::extents_type {
  return typename Mdspan::extents_type{dynamic_extents_data_v<Mdspan>};
}

template <class Mdspan, arene::base::constraints<std::enable_if_t<(Mdspan::rank_dynamic == 0)>> = nullptr>
constexpr auto make_extents() -> typename Mdspan::extents_type {
  return typename Mdspan::extents_type{};
}
/// @}

/// @brief Helper type for non-copyable elements in tests
using not_copyable = ::testing::configurable_value<
    std::int32_t,
    ::testing::throws_on::nothing,
    ::testing::disable::copy_construct | ::testing::disable::copy_assign>;

/// @brief Common mdspan types used across test suites
using mdspan_types = ::testing::Types<
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t>>,
    // Mdspans with different element types
    arene::base::mdspan<std::int8_t, arene::base::dextents<std::size_t, 2>>,
    arene::base::mdspan<std::int32_t, arene::base::dextents<std::size_t, 2>>,
    arene::base::mdspan<std::uint32_t, arene::base::dextents<std::size_t, 2>>,
    arene::base::mdspan<float, arene::base::dextents<std::size_t, 2>>,
    arene::base::mdspan<double, arene::base::dextents<std::size_t, 2>>,
    arene::base::mdspan<::testing::configurable_value<std::int32_t>, arene::base::dextents<std::size_t, 2>>,
    arene::base::mdspan<not_copyable, arene::base::dextents<std::size_t, 2>>,
    // Mdspans with different index types
    arene::base::mdspan<std::int32_t, arene::base::dextents<std::int8_t, 2>>,
    arene::base::mdspan<std::int32_t, arene::base::dextents<std::uint8_t, 2>>,
    arene::base::mdspan<std::int32_t, arene::base::dextents<std::int16_t, 2>>,
    arene::base::mdspan<std::int32_t, arene::base::dextents<std::uint16_t, 2>>,
    arene::base::mdspan<std::int32_t, arene::base::dextents<std::int32_t, 2>>,
    arene::base::mdspan<std::int32_t, arene::base::dextents<std::uint32_t, 2>>,
    // Mdspans with all static extents
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 3>>,
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 3, 4>>,
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 3, 4, 5>>,
    // Mdspans with different combinations of static and dynamic extents
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, arene::base::dynamic_extent, 4, 5>>,
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 3, 4, arene::base::dynamic_extent>>,
    arene::base::mdspan<
        std::int32_t,
        arene::base::extents<std::size_t, 3, arene::base::dynamic_extent, arene::base::dynamic_extent, 5>>,
    // Mdspans with different layouts
    arene::base::mdspan<
        std::uint32_t,
        arene::base::extents<std::size_t, arene::base::dynamic_extent, 4, 5>,
        arene::base::layout_left>,
    arene::base::mdspan<
        std::uint32_t,
        arene::base::extents<std::size_t, 3, 4, arene::base::dynamic_extent>,
        arene::base::layout_left>,
    arene::base::mdspan<std::uint32_t, arene::base::dextents<std::size_t, 5>, arene::base::layout_left>>;

/// @brief Check if all static extents are positive
template <class IndexType, std::size_t... Extents>
constexpr auto all_positive_static_extents(extents<IndexType, Extents...>) noexcept -> bool {
  return all_of_v<(Extents > 0U)...>;
}

/// @brief Construct a layout mapping type from @c extents
///
/// Overload for @c layout_stride to construct via right_strides.
template <
    typename Mapping,
    arene::base::constraints<
        std::enable_if_t<std::is_same<typename Mapping::layout_type, arene::base::layout_stride>::value>> = nullptr>
constexpr auto make_mapping_with_extents(typename Mapping::extents_type const& extents) noexcept -> Mapping {
  return {extents, arene::base::layout_detail::right_strides(extents)};
}

/// @brief Construct a layout mapping type from @c extents
/// Overload for layouts which can directly be constructed from extents
template <
    typename Mapping,
    arene::base::constraints<
        std::enable_if_t<!std::is_same<typename Mapping::layout_type, arene::base::layout_stride>::value>> = nullptr>
constexpr auto make_mapping_with_extents(typename Mapping::extents_type const& extents) noexcept -> Mapping {
  return {extents};
}

/// @brief A test struct which is convertible to @c full_extent_t
struct convertible_to_full_extent : full_extent_t {
  // need to specify an implicit constructor for C++17
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  constexpr convertible_to_full_extent()
      : full_extent_t{} {}
};

/// @brief Create an @c array of @c N full-extent-like values.
/// @c full_extent_t has an explicit default constructor (per the C++ standard), so
/// @c array<full_extent_t,N>{} fails because aggregate initialization copy-list-initializes
/// each element from @c {}, which cannot use an explicit constructor.
/// @tparam N the number of elements
/// @tparam FullExtentType the full-extent-like type (defaults to @c full_extent_t)
/// @{
template <std::size_t N, class FullExtentType = full_extent_t>
struct make_full_extents_impl {
  template <std::size_t... Is>
  constexpr auto operator()(std::index_sequence<Is...> /*unused*/) const noexcept
      -> arene::base::array<FullExtentType, N> {
    return {{(static_cast<void>(Is), FullExtentType{})...}};
  }
};

template <class FullExtentType>
struct make_full_extents_impl<0, FullExtentType> {
  constexpr auto operator()(std::index_sequence<> /*unused*/) const noexcept -> arene::base::array<FullExtentType, 0> {
    return {};
  }
};

template <std::size_t N, class FullExtentType = full_extent_t>
constexpr auto make_full_extents() noexcept -> arene::base::array<FullExtentType, N> {
  return make_full_extents_impl<N, FullExtentType>{}(std::make_index_sequence<N>{});
}
/// @}

/// @brief Functor that transforms an extent value into a full-range @c extent_slice
/// @tparam IndexType the index type of the extents
template <class IndexType>
struct make_full_range_extent_slice {
  using extent_slice_type = extent_slice<IndexType, IndexType, std::integral_constant<IndexType, 1>>;

  constexpr auto operator()(IndexType ext) const noexcept -> extent_slice_type { return {IndexType{}, ext}; }
};

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_MDSPAN_TEST_UTILITIES_HPP_
