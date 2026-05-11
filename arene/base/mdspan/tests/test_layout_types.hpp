// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_TEST_LAYOUT_TYPES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_TEST_LAYOUT_TYPES_HPP_

#include <gtest/gtest.h>

#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/algorithm/find.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/functional/bind_back.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/greater.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace test {

/// @brief The default runtime size to use for any dynamic dimensions of an @c extents
template <typename IndexType>
constexpr IndexType default_dynamic_extent{4};

/// @brief Make an instance of @c Extents where all of the dynamic dimensions have a particular value
/// @tparam Extents An @c extents type
/// @tparam DynamicIndices A pack of indices, one for each dynamic dimension in @c Extents
/// @param dynamic_extent The value to use for all dynamic dimensions of @c Extents
/// @return An instance of @c Extents where all of the dynamic dimensions are set to @c dynamic_extent
template <typename Extents, std::size_t... DynamicIndices>
constexpr auto
make_extents_impl(typename Extents::index_type dynamic_extent, std::index_sequence<DynamicIndices...>) noexcept
    -> Extents {
  return Extents{(static_cast<void>(DynamicIndices), dynamic_extent)...};
}

/// @brief Get an instance of @c TypeParam::extents_type where dynamic extents are expanded to @c default_dynamic_extent
/// @tparam Extents An @c extents type
/// @return An instance of @c Extents where dynamic extents are expanded to @c default_dynamic_extent
template <typename Extents>
constexpr auto make_expanded_extents() noexcept -> Extents {
  return make_extents_impl<Extents>(
      test::default_dynamic_extent<typename Extents::index_type>,
      std::make_index_sequence<Extents::rank_dynamic()>{}
  );
}

/// @brief Get the expected runtime extent for one dimension of an extent type, used to construct index packs
/// @tparam Extents An @c extents type
/// @param rank_index An index to one of the dimensions within the rank of @c Extents
/// @return The static extent if the specified dimension is static, or @c default_dynamic_extent if it's dynamic
template <typename Extents>
constexpr auto expected_runtime_extent(typename Extents::rank_type rank_index) noexcept ->
    typename Extents::index_type {
  Extents const ext{make_expanded_extents<Extents>()};
  return ext.extent(rank_index);
}

// Helper functions *not* for integer sequences --------

/// @brief Check if any of the runtime extents in the given @c extents object are 0
template <typename Extents>
constexpr auto any_extent_is_0(Extents const& extents) noexcept -> bool {
  auto const arr = arene::base::mdspan_detail::to_array(extents);
  return arene::base::find(arr.begin(), arr.end(), typename Extents::index_type{}) != arr.end();
}

/// @brief Check if all of the runtime extents in the given @c extents object are strictly greater than 1
template <typename Extents>
constexpr auto all_extents_greater_than_1(Extents const& extents) noexcept -> bool {
  using index_type = typename Extents::index_type;
  auto const arr = arene::base::mdspan_detail::to_array(extents);
  return arene::base::all_of(arr.begin(), arr.end(), arene::base::bind_back(std::greater<index_type>{}, index_type{1}));
}

// Helper functions for integer sequences --------------

/// @brief Get a sequence of @c Size copies of the same value @c Value
/// @{
template <typename Type, Type Value, typename Sequence>
struct sequence_of_value_impl;

template <typename Type, Type Value, std::size_t... Indices>
struct sequence_of_value_impl<Type, Value, std::index_sequence<Indices...>> {
  using type = std::integer_sequence<Type, (static_cast<void>(Indices), Value)...>;
};

template <typename Type, Type Value, std::size_t Size>
using sequence_of_value = typename sequence_of_value_impl<Type, Value, std::make_index_sequence<Size>>::type;
/// @}

/// @brief Get a sequence of @c Size copies of 0
template <typename Type, std::size_t Size>
using sequence_of_zeros = sequence_of_value<Type, Type{0}, Size>;

/// @brief Get the maximum in-bounds index at runtime for the given dimension of the given @c extents
/// @tparam Extents An @c extents type
/// @param rank_index An index to one of the dimensions within the rank of @c Extents
/// @return The largest index which will be in-bounds at runtime for dimension @c rank_index of @c Extents
/// @pre <c>rank_index > 0</c>; an answer will still be returned if this is violated but it will be meaningless
template <typename Extents>
constexpr auto max_runtime_index(typename Extents::rank_type rank_index) noexcept -> typename Extents::index_type {
  using index_type = typename Extents::index_type;
  return static_cast<index_type>(test::expected_runtime_extent<Extents>(rank_index) - index_type{1});
}

/// @brief An @c integer_sequence where each member is the maximum in-bounds index of its respective dimension
/// @tparam Extents An @c extents type for which we're generating an index pack
/// @{
template <typename Extents, typename RankIndexSequence = std::make_index_sequence<Extents::rank()>>
struct sequence_of_max_values_impl;

template <typename Extents, std::size_t... RankIndices>
struct sequence_of_max_values_impl<Extents, std::index_sequence<RankIndices...>> {
  using type = std::integer_sequence<typename Extents::index_type, max_runtime_index<Extents>(RankIndices)...>;
};

template <typename Extents>
using sequence_of_max_values = typename sequence_of_max_values_impl<Extents>::type;
/// @}

/// @brief Get an @c arene::base::type_list consisting of N copies of a given type
/// @tparam Type The type to put in the @c arene::base::type_list
/// @tparam Count The number ("N") of copies of @c Type to put in the @c arene::base::type_list
/// @{
template <typename Type, std::size_t Count, typename = std::make_index_sequence<Count>>
struct list_of_n_type_copies_impl {};

template <typename Type, std::size_t Count, std::size_t... IndexPack>
struct list_of_n_type_copies_impl<Type, Count, std::index_sequence<IndexPack...>> {
  using type = arene::base::type_list<decltype(static_cast<void>(IndexPack), std::declval<Type>())...>;
};

template <typename Type, std::size_t Count>
using list_of_n_type_copies = typename list_of_n_type_copies_impl<Type, Count>::type;
/// @}

// --- //

/// @brief Given a set of extents, make some left-handed strides for them and return them as an array
template <
    typename ExtentsType,
    typename IndexType = typename ExtentsType::index_type,
    arene::base::constraints<std::enable_if_t<ExtentsType::rank() != 0UL>> = nullptr>
constexpr auto make_left_strides(ExtentsType const& extents) noexcept
    -> arene::base::array<IndexType, ExtentsType::rank()> {
  arene::base::array<IndexType, ExtentsType::rank()> strides{};
  IndexType stride_so_far{1UL};
  for (std::size_t dim = 0UL; dim < ExtentsType::rank(); ++dim) {
    strides[dim] = stride_so_far;
    stride_so_far = static_cast<IndexType>(stride_so_far * static_cast<IndexType>(extents.extent(dim)));
    if (extents.extent(dim) == typename ExtentsType::index_type{}) {
      // In our stride-setting algorithm, encountering a 0 extent resets the stride_so_far to 1.
      stride_so_far = IndexType{1};
    }
  }

  return strides;
}

/// @brief Given a set of extents, make some left-handed strides for them and return them as an array
/// @note This special case for rank() == 0 is because GCC8 doesn't see functions as being usable in constant
/// expressions if they contain for loops that never actually iterate.
template <
    typename ExtentsType,
    typename IndexType = typename ExtentsType::index_type,
    arene::base::constraints<std::enable_if_t<ExtentsType::rank() == 0UL>> = nullptr>
constexpr auto make_left_strides(ExtentsType const&) noexcept -> arene::base::array<IndexType, ExtentsType::rank()> {
  return {};
}

// --- extents mutators --- //

/// @brief Given an @c extents type, get another @c extents type with one dimension added or removed so the rank changes
template <typename Extents>
struct extents_with_different_rank_impl {};

/// @brief Given an @c extents type, get another @c extents type with one dimension added or removed so the rank changes
template <typename IndexType, std::size_t FirstExtent, std::size_t... LastExtents>
struct extents_with_different_rank_impl<arene::base::extents<IndexType, FirstExtent, LastExtents...>> {
  using type = arene::base::extents<IndexType, LastExtents...>;
};

/// @brief Given an @c extents type, get another @c extents type with one dimension added or removed so the rank changes
template <typename IndexType>
struct extents_with_different_rank_impl<arene::base::extents<IndexType>> {
  using type = arene::base::extents<IndexType, arene::base::dynamic_extent>;
};

/// @brief Given an @c extents type, get another @c extents type with one dimension added or removed so the rank changes
template <typename Extents>
using extents_with_different_rank = typename extents_with_different_rank_impl<Extents>::type;

// --- //

/// @brief Given an @c extents type, get another @c extents type with all of the static dimensions changed
template <typename Extents>
struct extents_with_different_static_part_impl {};

/// @brief Given an @c extents type, get another @c extents type with all of the static dimensions changed
template <typename IndexType, std::size_t... Extents>
struct extents_with_different_static_part_impl<arene::base::extents<IndexType, Extents...>> {
  using type = arene::base::extents<IndexType, (Extents != arene::base::dynamic_extent ? Extents + 1U : Extents)...>;
};

/// @brief Given an @c extents type, get another @c extents type with all of the static dimensions changed
template <typename Extents>
using extents_with_different_static_part = typename extents_with_different_static_part_impl<Extents>::type;

// --- //

// these need to be defined in a separate header to satisfy IWYU
// https://github.com/include-what-you-use/include-what-you-use/issues/1707

/// @brief A layout which changes whether or not it reports being strided/exhaustive/unique based on template params
template <bool Strided, bool Exhaustive, bool Unique>
class user_defined_layout {
 public:
  /// @brief A user-defined type with the basic interface of a layout mapping but no constructors or data
  template <typename Extents>
  class mapping : private arene::base::layout_right::mapping<Extents> {
    using base_type = arene::base::layout_right::mapping<Extents>;

   public:
    using extents_type = Extents;
    using typename base_type::index_type;
    using typename base_type::rank_type;
    using layout_type = user_defined_layout;

    static constexpr auto is_always_strided() noexcept -> bool { return Strided; }
    static constexpr auto is_always_exhaustive() noexcept -> bool { return Exhaustive; }
    static constexpr auto is_always_unique() noexcept -> bool { return Unique; }

    static constexpr auto is_strided() noexcept -> bool { return Strided; }
    static constexpr auto is_exhaustive() noexcept -> bool { return Exhaustive; }
    static constexpr auto is_unique() noexcept -> bool { return Unique; }

    // These extra members are not part of the test, but are needed so that this class satisfies is_layout_mapping_v.
    using base_type::extents;
    using base_type::required_span_size;
    using base_type::stride;
    using base_type::operator();

    // Equality operator is needed so that this mapping will satisfy is_layout_mapping_v; it's not used in these tests
    friend constexpr auto operator==(mapping const&, mapping const&) noexcept -> bool { return true; }
  };
};

// A class with a base form that's comparable to TypeParam but can be configured to fail any of the constraints.
template <bool IsLayoutMappingAlike, bool SameRank, bool IsStrided>
class equality_constraints_configurable {
 public:
  template <typename Extents>
  class mapping {
   public:
    using extents_type = std::
        conditional_t<SameRank, Extents, arene::base::dextents<typename Extents::index_type, Extents::rank() + 1U>>;

    using index_type = typename extents_type::index_type;
    using rank_type = typename extents_type::rank_type;
    using layout_type = equality_constraints_configurable;

    static constexpr auto is_always_strided() noexcept -> bool { return IsStrided; }
    static constexpr auto is_always_exhaustive() noexcept -> bool { return true; }
    template <bool Enable = IsLayoutMappingAlike, arene::base::constraints<std::enable_if_t<Enable>> = nullptr>
    static constexpr auto is_always_unique() noexcept -> bool {
      return true;
    }

    static constexpr auto is_strided() noexcept -> bool { return IsStrided; }
    static constexpr auto is_exhaustive() noexcept -> bool { return true; }
    template <bool Enable = IsLayoutMappingAlike, arene::base::constraints<std::enable_if_t<Enable>> = nullptr>
    static constexpr auto is_unique() noexcept -> bool {
      return true;
    }

    // The definitions below this are not checked in the constraints, but are needed to instantiate the function body.

    constexpr auto extents() const noexcept -> extents_type const& { return extents_; }

    constexpr auto stride(rank_type) const noexcept -> index_type { return {}; }

    template <typename... T>
    constexpr auto operator()(T&&...) const noexcept -> index_type {
      return {};
    }

    constexpr auto required_span_size() const noexcept -> index_type { return {}; }

    // Equality operator is needed so that this mapping will satisfy is_layout_mapping_v; it's not used in these tests
    friend constexpr auto operator==(mapping const&, mapping const&) noexcept -> bool { return true; }

   private:
    extents_type extents_{};  // This instance variable is needed so a reference to it can be returned from extents()
  };
};

// --- //

// Extents types to use for the layouts in the type-parameterized layout tests
using base_extents_types = ::testing::Types<
    arene::base::extents<std::uint32_t>,
    arene::base::extents<std::uint32_t, 0>,
    arene::base::extents<std::uint16_t, 3>,
    arene::base::extents<std::uint32_t, 99, arene::base::dynamic_extent>,
    arene::base::extents<std::uint32_t, 0, arene::base::dynamic_extent>,
    arene::base::extents<std::uint16_t, arene::base::dynamic_extent, arene::base::dynamic_extent>,
    arene::base::extents<std::int16_t, 6, 1, 8>,
    arene::base::extents<std::int32_t, 0, arene::base::dynamic_extent, 0>,
    arene::base::extents<std::uint16_t, 9, 28, 0, 22>,
    arene::base::extents<std::int16_t, arene::base::dynamic_extent, 6, 1, 8>,
    arene::base::extents<std::int32_t, 14, 4, 2, 87, 3>,
    arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 980, 1, arene::base::dynamic_extent, 4>,
    arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 980, 0, arene::base::dynamic_extent, 4>>;

// Left, right, and strided layout types to use in the type-parameterized layout tests
using layout_left_types = arene::base::type_lists::apply_each_t<base_extents_types, arene::base::layout_left::mapping>;
using layout_right_types =
    arene::base::type_lists::apply_each_t<base_extents_types, arene::base::layout_right::mapping>;
using layout_stride_types =
    arene::base::type_lists::apply_each_t<base_extents_types, arene::base::layout_stride::mapping>;

using layout_types = arene::base::type_lists::concat_t<layout_left_types, layout_right_types, layout_stride_types>;

// --- //

// A base test fixture for type-parameterized layout tests
template <typename TypeParam>
class layout_test_base : public testing::Test {
  /// @brief An instance of @c TypeParam::extents_type where dynamic extents are expanded to @c default_dynamic_extent
  static constexpr auto expanded_extents{test::make_expanded_extents<typename TypeParam::extents_type>()};

  /// @brief Use the given mapping's function call operator with the given index pack and return the result
  /// @tparam Indices A pack of indices to use as arguments for the function call operator
  /// @param mapping A layout mapping object which maps packs of indices to one-dimensional indices
  /// @return The one-dimensional index mapped to by the multi-dimensional @c Indices
  template <typename Type, Type... Indices>
  constexpr auto map_from_index_pack(TypeParam const& mapping, std::integer_sequence<Type, Indices...>) noexcept
      -> decltype(mapping(Indices...)) {
    static_assert(noexcept(mapping(Indices...)), "Function calls on a mapping should always be noexcept");
    return mapping(Indices...);
  }

  /// @brief Check if the function call operator of @c TypeParam{} works on the given pack of index types
  /// @tparam IndexTypes A pack of index types to try calling @c TypeParam{} on
  /// @return @c true if @c TypeParam{}(IndexTypes{}...) is well-formed, @c false if not
  template <typename... IndexTypes>
  constexpr auto can_map_from_indices(arene::base::type_list<IndexTypes...>) noexcept -> bool {
    return arene::base::is_invocable_v<TypeParam, IndexTypes...>;
  }

 protected:
  /// @brief Map a pack of 0s with the right size for this @c mapping to a one-dimensional output index
  /// @return The one-dimensional output index corresponding to an input of all 0s
  template <typename IndexType = std::size_t>
  constexpr auto map_from_zero_indices() noexcept -> typename TypeParam::index_type {
    return map_from_index_pack(TypeParam{}, sequence_of_zeros<IndexType, TypeParam::extents_type::rank()>{});
  }

  /// @brief Map a pack of maximum values for each dimension of this @c mapping to a one-dimensional output index
  /// @return The one-dimensional output index corresponding to an input of all maximum indices
  constexpr auto map_from_max_indices() noexcept -> typename TypeParam::index_type {
    return map_from_index_pack(TypeParam{}, sequence_of_max_values<typename TypeParam::extents_type>{});
  }

  /// @brief Check if the function call operator of @c TypeParam{} works on a pack of N <c>size_t</c>s
  /// @tparam IndexCount The number ("N") of <c>size_t</c>s to try calling @c TypeParam{} on
  /// @return @c true if @c TypeParam{} can be called on a pack of @c IndexCount <c>size_t</c>s, @c false if not
  template <std::size_t IndexCount>
  constexpr auto can_map_from_n_size_ts() noexcept -> bool {
    return can_map_from_indices(list_of_n_type_copies<std::size_t, IndexCount>{});
  }

  /// @brief Check if the function call operator of @c TypeParam{} works on a pack of the given type
  /// @tparam IndexType The index type to try calling @c TypeParam{} on
  /// @return @c true if @c TypeParam{} can be called on a pack of @c rank() <c>IndexType</c>s, @c false if not
  template <typename IndexType>
  constexpr auto can_map_from_indices_with_type() noexcept -> bool {
    return can_map_from_indices(list_of_n_type_copies<IndexType, TypeParam::extents_type::rank()>{});
  }
};

}  // namespace test

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_TEST_LAYOUT_TYPES_HPP_
