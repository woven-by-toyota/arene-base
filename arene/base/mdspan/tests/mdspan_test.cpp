// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/mdspan.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/transform.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/integer_sequences.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/mdspan/detail/representable_cast.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_object.hpp"
#include "arene/base/stdlib_choice/is_rvalue_reference.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/tuple/apply.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_manipulation/repeat_type.hpp"
#include "arene/base/type_traits/is_implicitly_constructible.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/type_identity.hpp"
#include "arene/base/utility/safe_comparisons.hpp"
#include "testlibs/utilities/configurable_value.hpp"

// IWYU pragma: no_include "stdlib/include/type_traits"

// NOLINTBEGIN(readability-identifier-length)

namespace {

/// @brief Construct a span from an array.
/// @{
template <typename T, std::size_t N>
constexpr auto span_of(arene::base::array<T, N> const& arr) -> arene::base::span<T const, N> {
  return arr;
}

template <typename T, std::size_t N>
constexpr auto span_of(arene::base::array<T, N>& arr) -> arene::base::span<T, N> {
  return arr;
}

template <typename T, std::size_t N>
constexpr auto span_of(arene::base::array<T, N> const&&) = delete;
/// @}

/// @brief Return the maximum index of a given dimension
template <class IndexType>
constexpr auto max_index(IndexType dim) -> IndexType {
  using arene::base::mdspan_detail::representable_cast;
  return dim == IndexType{} ? IndexType{} : representable_cast<IndexType>(dim - IndexType{1});
}

template <typename Mdspan>
class MdspanTestSuite : public testing::Test {};

TYPED_TEST_SUITE(MdspanTestSuite, arene::base::testing::mdspan_types, );

template <class Mdspan>
constexpr auto mdspan_data() -> arene::base::array<typename Mdspan::value_type, 10> {
  return arene::base::array<typename Mdspan::value_type, 10>{
      typename Mdspan::value_type{1},
      typename Mdspan::value_type{2},
      typename Mdspan::value_type{3},
      typename Mdspan::value_type{4},
      typename Mdspan::value_type{5},
      typename Mdspan::value_type{6},
      typename Mdspan::value_type{7},
      typename Mdspan::value_type{8},
      typename Mdspan::value_type{9},
      typename Mdspan::value_type{10}
  };
}

/// @test The mdspan extents_type alias exists and is an alias for the extents parameter.
TYPED_TEST(MdspanTestSuite, ExtentsTypeAliasIsAliasForExtentsTemplateParameter) {
  testing::StaticAssertTypeEq<
      typename TypeParam::extents_type,
      typename arene::base::testing::mdspan_parameters<TypeParam>::extents_type>();
}

/// @test The mdspan layout_type alias exists and is an alias for the LayoutPolicy parameter.
TYPED_TEST(MdspanTestSuite, LayoutTypeAliasIsAliasForLayoutPolicyTemplateParameter) {
  testing::StaticAssertTypeEq<
      typename TypeParam::layout_type,
      typename arene::base::testing::mdspan_parameters<TypeParam>::layout_type>();
}

/// @test The mdspan accessor_type alias exists and is an alias for the AccessorPolicy parameter.
TYPED_TEST(MdspanTestSuite, AccessorTypeAliasIsAliasForAccessorPolicyTemplateParameter) {
  testing::StaticAssertTypeEq<
      typename TypeParam::accessor_type,
      typename arene::base::testing::mdspan_parameters<TypeParam>::accessor_type>();
}

/// @test The mdspan mapping_type alias exists and is an alias for @c LayoutPolicy::mapping<Extents>.
TYPED_TEST(MdspanTestSuite, MappingTypeAliasIsAliasForMappingBasedOnTemplateParameter) {
  using expected_extents = typename arene::base::testing::mdspan_parameters<TypeParam>::extents_type;
  using expected_layout = typename arene::base::testing::mdspan_parameters<TypeParam>::layout_type;

  testing::StaticAssertTypeEq<
      typename TypeParam::mapping_type,
      typename expected_layout::template mapping<expected_extents>>();
}

/// @test The mdspan element_type alias exists and is an alias for the ElementType parameter.
TYPED_TEST(MdspanTestSuite, ElementTypeAliasIsAliasForElementTypeTemplateParameter) {
  testing::StaticAssertTypeEq<
      typename TypeParam::element_type,
      typename arene::base::testing::mdspan_parameters<TypeParam>::element_type>();
}

/// @test The mdspan index_type alias exists and is an alias for the Extents index type.
TYPED_TEST(MdspanTestSuite, IndexTypeAliasIsAliasForIndexTypeTemplateParameter) {
  using expected_extents = typename arene::base::testing::mdspan_parameters<TypeParam>::extents_type;

  testing::StaticAssertTypeEq<typename TypeParam::index_type, typename expected_extents::index_type>();
}

/// @test The mdspan size_type alias exists and is an alias for the Extents size type.
TYPED_TEST(MdspanTestSuite, SizeTypeAliasIsAliasForSizeTypeTemplateParameter) {
  using expected_extents = typename arene::base::testing::mdspan_parameters<TypeParam>::extents_type;

  testing::StaticAssertTypeEq<typename TypeParam::size_type, typename expected_extents::size_type>();
}

/// @test The mdspan rank_type alias exists and is an alias for the Extents rank type.
TYPED_TEST(MdspanTestSuite, RankTypeAliasIsAliasForRankTypeTemplateParameter) {
  using expected_extents = typename arene::base::testing::mdspan_parameters<TypeParam>::extents_type;

  testing::StaticAssertTypeEq<typename TypeParam::rank_type, typename expected_extents::rank_type>();
}

/// @test The mdspan data_handle_type alias exists and is an alias for the AccessorPolicy::data_handle_type parameter.
TYPED_TEST(MdspanTestSuite, DataHandleTypeAliasIsAliasForAccessorDataHandlePolicyTemplateParameter) {
  using expected_accessor = typename arene::base::testing::mdspan_parameters<TypeParam>::accessor_type;

  testing::StaticAssertTypeEq<typename TypeParam::data_handle_type, typename expected_accessor::data_handle_type>();
}

/// @test The mdspan reference alias exists and is an alias for the AccessorPolicy::reference parameter.
TYPED_TEST(MdspanTestSuite, RankAliasIsAliasForAccessorPolicyRankTemplateParameter) {
  using expected_accessor = typename arene::base::testing::mdspan_parameters<TypeParam>::accessor_type;

  testing::StaticAssertTypeEq<typename TypeParam::reference, typename expected_accessor::reference>();
}

/// @test The mdspan value_type alias removes any const and volatile qualifiers.
TEST(NonParameterizedMdspanTestSuite, ValueTypeRemovesCvQualifiers) {
  using mdspan_type = arene::base::mdspan<int const volatile, arene::base::extents<std::size_t, 3, 4, 5>>;

  testing::StaticAssertTypeEq<typename mdspan_type::element_type, int const volatile>();
  testing::StaticAssertTypeEq<typename mdspan_type::value_type, int>();
}

/// @test The mdspan static rank functions return the same value as the extents static rank functions.
TYPED_TEST(MdspanTestSuite, StaticRankFunctionsReturnSameValueAsExtents) {
  STATIC_ASSERT_EQ(TypeParam::rank(), TypeParam::extents_type::rank());
  STATIC_ASSERT_EQ(TypeParam::rank_dynamic(), TypeParam::extents_type::rank_dynamic());
}

/// @test The mdspan static_extent function returns the same value as the extents static_extent function.
CONSTEXPR_TYPED_TEST(MdspanTestSuite, StaticExtentFunctionReturnsSameValueAsExtents) {
  for (auto dim : arene::base::sequential_values<typename TypeParam::rank_type, TypeParam::rank()>) {
    CONSTEXPR_ASSERT(TypeParam::static_extent(dim) == TypeParam::extents_type::static_extent(dim));
  }
}

/// @brief Check if the extents of @c span are equal to the extents of @c extents
template <class Mdspan, class Extents>
constexpr auto extents_are_equal(Mdspan const& span, Extents const& extents) -> bool {
  // NOLINTNEXTLINE(readability-use-anyofallof)
  for (auto dim : arene::base::sequential_values<typename Mdspan::rank_type, Mdspan::rank()>) {
    if (span.extent(dim) != extents.extent(dim)) {
      return false;
    }
  }
  return true;
}

/// @test The mdspan extent function returns the same value as the extents' extent function.
CONSTEXPR_TYPED_TEST(MdspanTestSuite, ExtentFunctionReturnsSameValueAsExtentsDoes) {
  auto data = mdspan_data<TypeParam>();
  auto extents = arene::base::testing::make_extents<TypeParam>();
  auto span = TypeParam{data.data(), extents};

  CONSTEXPR_ASSERT(extents_are_equal(span, extents));
}

/// @test The mdspan empty function returns true if the mdspan is empty.
CONDITIONAL_TYPED_TEST(
    MdspanTestSuite,
    EmptyFunctionReturnsTrueIfEmpty,
    std::is_default_constructible<TypeParam>::value
) {
  constexpr TypeParam span{};
  STATIC_ASSERT_TRUE(span.empty());
}

/// @test The mdspan empty function returns false if the mdspan is not empty.
CONSTEXPR_TYPED_TEST(MdspanTestSuite, EmptyFunctionReturnsFalseIfNotEmpty) {
  auto data = mdspan_data<TypeParam>();
  auto extents = arene::base::testing::make_extents<TypeParam>();
  auto span = TypeParam{data.data(), extents};
  CONSTEXPR_ASSERT_FALSE(span.empty());
}

/// @test The mdspan size function returns 0 if the mdspan is empty.
CONDITIONAL_TYPED_TEST(
    MdspanTestSuite,
    SizeFunctionReturnsZeroIfEmpty,
    std::is_default_constructible<TypeParam>::value
) {
  constexpr TypeParam span{};
  STATIC_ASSERT_EQ(span.size(), 0);
}

/// @brief Check if the dynamic extents of @c span are all zero; the static extents can be non-zero
template <class Mdspan>
constexpr auto all_dynamic_extents_are_zero(Mdspan const& span) -> bool {
  // NOLINTNEXTLINE(readability-use-anyofallof)
  for (auto dim : arene::base::sequential_values<typename Mdspan::rank_type, Mdspan::rank()>) {
    if (span.static_extent(dim) == arene::base::dynamic_extent && span.extent(dim) != typename Mdspan::index_type{}) {
      return false;
    }
  }
  return true;
}

/// @test A default constructed mdspan has all dynamic extents equal to zero
CONDITIONAL_TYPED_TEST(
    MdspanTestSuite,
    DefaultConstructedMdspanHasAllDynamicExtentsEqualToZero,
    std::is_default_constructible<TypeParam>::value
) {
  constexpr TypeParam span{};
  STATIC_ASSERT_TRUE(all_dynamic_extents_are_zero(span));
}

/// @test The mdspan default constructor requires at least one dynamic constraint
TYPED_TEST(MdspanTestSuite, DefaultConstructorRequiresAtLeastOneDynamicConstraint) {
  // Note: The other constraints for default constructibility are checked via the rebinding tests below.
  STATIC_ASSERT_EQ(std::is_default_constructible<TypeParam>::value, TypeParam::rank_dynamic() > 0);
}

/// @brief A non-default constructible type
struct not_default_constructible {
  not_default_constructible() = delete;
};

/// @brief An accessor whose data handle type is not default constructible
template <class T>
class accessor_with_not_default_constructbile_data_handle {
 public:
  using offset_policy = accessor_with_not_default_constructbile_data_handle;
  using element_type = T;
  using reference = T&;
  using data_handle_type = not_default_constructible;

  accessor_with_not_default_constructbile_data_handle() = default;
  constexpr auto access(data_handle_type, std::size_t) const noexcept -> reference;
  constexpr auto offset(data_handle_type, std::size_t) const noexcept -> data_handle_type;
};

/// @test The mdspan default constructor requires that the data handle type is default constructible
TYPED_TEST(MdspanTestSuite, DefaultConstructorRequiresDataHandleIsDefaultConstructible) {
  using mdspan_type = arene::base::testing::rebind_mdspan_accessor_t<
      TypeParam,
      accessor_with_not_default_constructbile_data_handle<typename TypeParam::element_type>>;
  STATIC_ASSERT_FALSE(std::is_default_constructible<mdspan_type>::value);
}

/// @brief A layout whose mapping type is not default constructible
class layout_with_not_default_constructible_mapping {
 public:
  template <typename Extents>
  class mapping : public arene::base::layout_right::mapping<Extents> {
   public:
    using layout_type = layout_with_not_default_constructible_mapping;
    mapping() = delete;
  };
};

/// @test The mdspan default constructor requires that the mapping type is default constructible
TYPED_TEST(MdspanTestSuite, DefaultConstructorRequiresMappingIsDefaultConstructible) {
  using mdspan_type =
      arene::base::testing::rebind_mdspan_layout_t<TypeParam, layout_with_not_default_constructible_mapping>;
  STATIC_ASSERT_FALSE(std::is_default_constructible<mdspan_type>::value);
}

/// @brief An accessor that is not default constructible
template <class T>
class not_default_constructible_accessor {
 public:
  using offset_policy = not_default_constructible_accessor;
  using element_type = T;
  using reference = T&;
  using data_handle_type = T*;

  not_default_constructible_accessor() = delete;
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr not_default_constructible_accessor(T) {}
  constexpr auto access(data_handle_type, std::size_t) const noexcept -> reference;
  constexpr auto offset(data_handle_type, std::size_t) const noexcept -> data_handle_type;
};

/// @test The mdspan default constructor requires that the accessor type is default constructible
TYPED_TEST(MdspanTestSuite, DefaultConstructorRequiresAccessorIsDefaultConstructible) {
  using mdspan_type = arene::base::testing::
      rebind_mdspan_accessor_t<TypeParam, not_default_constructible_accessor<typename TypeParam::element_type>>;
  STATIC_ASSERT_FALSE(std::is_default_constructible<mdspan_type>::value);
}

/// @brief Check that @c span has the expected properties
/// @tparam Mdspan The mdspan type to check
/// @param span The mdspan to check
///
/// The mdspan construction tests all construct the same span in different ways. This function is used to check
/// that the constructed span has the following properties:
/// * The mdspan is not empty
/// * The mdspan size is equal to the product of its extents
/// * The mdspan's dynamic extents should be sequentially increasing
template <class Mdspan>
constexpr auto mdspan_has_expected_properties(Mdspan const& span) -> bool {
  // Dynamic extents should be sequentially increasing.
  auto expected_dynamic_extent = typename Mdspan::index_type{1};
  auto expected_size = typename Mdspan::size_type{1};

  for (auto dim : arene::base::sequential_values<typename Mdspan::rank_type, Mdspan::rank()>) {
    auto static_extent = span.static_extent(dim);
    auto extent = span.extent(dim);

    expected_size =
        static_cast<typename Mdspan::size_type>(expected_size * static_cast<typename Mdspan::size_type>(extent));

    if (static_extent == arene::base::dynamic_extent) {
      if (extent != expected_dynamic_extent) {
        return false;
      }
      expected_dynamic_extent++;
    } else {
      if (static_extent != static_cast<std::size_t>(extent)) {
        return false;
      }
    }
  }

  return expected_size == span.size() && !span.empty();
}

/// @brief An accessor that tracks how many times it has been copied or moved
template <class T>
class tracking_accessor {
 public:
  using offset_policy = tracking_accessor;
  using element_type = T;
  using reference = T&;
  using data_handle_type = T*;

  std::size_t copy_ctor_count{0};
  std::size_t copy_assign_count{0};
  std::size_t move_ctor_count{0};
  std::size_t move_assign_count{0};

  constexpr tracking_accessor() = default;
  constexpr tracking_accessor(tracking_accessor const&) noexcept { ++copy_ctor_count; }
  constexpr tracking_accessor(tracking_accessor&&) noexcept { ++move_ctor_count; }
  constexpr auto operator=(tracking_accessor const&) noexcept -> tracking_accessor<T>& {
    ++copy_assign_count;
    return *this;
  }
  constexpr auto operator=(tracking_accessor&&) noexcept -> tracking_accessor<T>& {
    ++move_assign_count;
    return *this;
  }
  ~tracking_accessor() = default;

  constexpr auto access(data_handle_type, std::size_t) const noexcept -> reference;
  constexpr auto offset(data_handle_type, std::size_t) const noexcept -> data_handle_type;
};

/// @test A mdspan can be copy constructed from another mdspan
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeCopyConstructedFromAnotherMdspan) {
  using mdspan_type =
      arene::base::testing::rebind_mdspan_accessor_t<TypeParam, tracking_accessor<typename TypeParam::element_type>>;

  auto data = mdspan_data<TypeParam>();
  auto exts = arene::base::testing::make_extents<TypeParam>();
  auto source_span = mdspan_type{data.data(), exts};
  auto copied_span = mdspan_type{source_span};

  CONSTEXPR_ASSERT(mdspan_has_expected_properties(copied_span));
  CONSTEXPR_ASSERT_EQ(copied_span.accessor().copy_ctor_count, 1);
  CONSTEXPR_ASSERT_EQ(copied_span.accessor().move_ctor_count, 0);
  CONSTEXPR_ASSERT_EQ(copied_span.accessor().copy_assign_count, 0);
  CONSTEXPR_ASSERT_EQ(copied_span.accessor().move_assign_count, 0);
}

/// @test A mdspan can be copy assigned from another mdspan
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeCopyAssignedFromAnotherMdspan) {
  using mdspan_type =
      arene::base::testing::rebind_mdspan_accessor_t<TypeParam, tracking_accessor<typename TypeParam::element_type>>;

  auto data = mdspan_data<TypeParam>();
  auto exts = arene::base::testing::make_extents<TypeParam>();
  auto source_span = mdspan_type{data.data(), exts};
  auto copied_span = mdspan_type{data.data(), exts};

  copied_span = source_span;

  CONSTEXPR_ASSERT(mdspan_has_expected_properties(copied_span));
  CONSTEXPR_ASSERT_EQ(copied_span.accessor().copy_ctor_count, 0);
  CONSTEXPR_ASSERT_EQ(copied_span.accessor().move_ctor_count, 0);
  CONSTEXPR_ASSERT_EQ(copied_span.accessor().copy_assign_count, 1);
  CONSTEXPR_ASSERT_EQ(copied_span.accessor().move_assign_count, 0);
}

/// @test A mdspan can be move constructed from another mdspan
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeMoveConstructedFromAnotherMdspan) {
  using mdspan_type =
      arene::base::testing::rebind_mdspan_accessor_t<TypeParam, tracking_accessor<typename TypeParam::element_type>>;

  auto data = mdspan_data<TypeParam>();
  auto exts = arene::base::testing::make_extents<TypeParam>();
  auto source_span = mdspan_type{data.data(), exts};
  auto moved_span = mdspan_type{std::move(source_span)};

  CONSTEXPR_ASSERT(mdspan_has_expected_properties(moved_span));
  CONSTEXPR_ASSERT_EQ(moved_span.accessor().copy_ctor_count, 0);
  CONSTEXPR_ASSERT_EQ(moved_span.accessor().move_ctor_count, 1);
  CONSTEXPR_ASSERT_EQ(moved_span.accessor().copy_assign_count, 0);
  CONSTEXPR_ASSERT_EQ(moved_span.accessor().move_assign_count, 0);
}

/// @test A mdspan can be move assigned from another mdspan
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeMoveAssignedFromAnotherMdspan) {
  using mdspan_type =
      arene::base::testing::rebind_mdspan_accessor_t<TypeParam, tracking_accessor<typename TypeParam::element_type>>;

  auto data = mdspan_data<TypeParam>();
  auto exts = arene::base::testing::make_extents<TypeParam>();
  auto source_span = mdspan_type{data.data(), exts};
  auto copied_span = mdspan_type{data.data(), exts};

  copied_span = std::move(source_span);

  CONSTEXPR_ASSERT(mdspan_has_expected_properties(copied_span));
  CONSTEXPR_ASSERT_EQ(copied_span.accessor().copy_ctor_count, 0);
  CONSTEXPR_ASSERT_EQ(copied_span.accessor().move_ctor_count, 0);
  CONSTEXPR_ASSERT_EQ(copied_span.accessor().copy_assign_count, 0);
  CONSTEXPR_ASSERT_EQ(copied_span.accessor().move_assign_count, 1);
}

/// @brief Helper to construct an mdspan from a variadic list of extents specified via an index sequence.
template <class Mdspan, std::size_t... Extents>
constexpr auto construct_mdspan_from_list(typename Mdspan::data_handle_type handle, std::index_sequence<Extents...>)
    -> Mdspan {
  return Mdspan{std::move(handle), Extents...};
}

/// @test A mdspan can be constructed by specifying a data handle and a variadic list of dynamic extents
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeConstructedBySpecifyingHandleAndVariadicListOfDynamicExtents) {
  auto data = mdspan_data<TypeParam>();
  auto span = construct_mdspan_from_list<TypeParam>(
      data.data(),
      arene::base::make_index_sequence_from<1U, TypeParam::rank_dynamic()>()
  );

  CONSTEXPR_ASSERT(mdspan_has_expected_properties(span));
}

/// @brief Helper to extract the extents as an index sequence from an mdspan type
template <typename Mdspan>
struct extract_extents_as_sequence;

/// @brief Helper to extract the extents as an index sequence from an mdspan type
template <
    typename ElementType,
    typename IndexType,
    std::size_t... Extents,
    typename LayoutPolicy,
    typename AccessorPolicy>
struct extract_extents_as_sequence<
    arene::base::mdspan<ElementType, arene::base::extents<IndexType, Extents...>, LayoutPolicy, AccessorPolicy>> {
  using type = std::index_sequence<Extents...>;
};

/// @brief Helper to extract the extents as an index sequence from an mdspan type
template <typename Mdspan>
using extract_extents_as_sequence_t = typename extract_extents_as_sequence<Mdspan>::type;

/// @brief Helper to transform extents into an index sequence with dynamic extents replaced by sequential values
template <std::size_t Counter, typename Extents>
struct make_extents_sequence_impl;

/// @brief Base case: no more extents to process
template <std::size_t Counter>
struct make_extents_sequence_impl<Counter, std::index_sequence<>> {
  using type = std::index_sequence<>;
};

/// @brief Recursive case: process the first extent and recurse on the rest
template <std::size_t Counter, std::size_t First, std::size_t... Rest>
struct make_extents_sequence_impl<Counter, std::index_sequence<First, Rest...>> {
 private:
  static constexpr bool is_dynamic = (First == arene::base::dynamic_extent);
  static constexpr std::size_t value = is_dynamic ? Counter : First;
  static constexpr std::size_t next_counter = is_dynamic ? (Counter + 1) : Counter;

  using rest_sequence = typename make_extents_sequence_impl<next_counter, std::index_sequence<Rest...>>::type;

 public:
  using type = arene::base::integer_sequence_cat<std::index_sequence<value>, rest_sequence>;
};

/// @brief Generate an index sequence of extents for the given Mdspan, sequentially increasing any dynamic extents.
template <typename Mdspan>
using make_extents_sequence = typename make_extents_sequence_impl<1, extract_extents_as_sequence_t<Mdspan>>::type;

static_assert(
    std::is_same<
        make_extents_sequence<arene::base::mdspan<
            int,
            arene::base::extents<
                std::size_t,
                99,
                arene::base::dynamic_extent,
                12,
                arene::base::dynamic_extent,
                arene::base::dynamic_extent,
                42>>>,
        std::index_sequence<99, 1, 12, 2, 3, 42>>::value,
    ""
);

/// @test A mdspan can be constructed by specifying a data handle and a variadic list of all extents
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeConstructedBySpecifyingHandleAndVariadicListOfAllExtents) {
  auto data = mdspan_data<TypeParam>();
  auto span = construct_mdspan_from_list<TypeParam>(data.data(), make_extents_sequence<TypeParam>());

  CONSTEXPR_ASSERT(mdspan_has_expected_properties(span));
}

/// @brief Check if an mdspan is constructible by specifying @c NumberOfExtents @c Mdspan::index_type parameters to the
/// variadic list constructor.
/// @{
template <class Mdspan, class ExtentsList>
class is_constructible_from_n_extents_impl : public std::false_type {};

template <class Mdspan, class... Extents>
class is_constructible_from_n_extents_impl<Mdspan, arene::base::type_list<Extents...>>
    : public std::is_constructible<Mdspan, typename Mdspan::data_handle_type, Extents...> {};

template <class Mdspan, std::size_t NumberOfExtents>
extern constexpr auto is_constructible_from_n_extents_v = is_constructible_from_n_extents_impl<
    Mdspan,
    typename arene::base::repeat_type<NumberOfExtents, arene::base::type_list, typename Mdspan::index_type>::type>::
    value;
/// @}

/// @test A mdspan cannot be constructed by specifying a data handle and a variadic list of extents if the accessor is
/// not default constructible
TYPED_TEST(
    MdspanTestSuite,
    MdspanConstructedBySpecifyingHandleAndVariadicListExtentsRequiresAccessorIsDefaultConstructible
) {
  STATIC_ASSERT_TRUE(is_constructible_from_n_extents_v<TypeParam, TypeParam::rank()>);

  using not_constructible_mdspan_type = arene::base::testing::
      rebind_mdspan_accessor_t<TypeParam, not_default_constructible_accessor<typename TypeParam::element_type>>;
  STATIC_ASSERT_FALSE(is_constructible_from_n_extents_v<not_constructible_mdspan_type, TypeParam::rank()>);
}

/// @test A mdspan cannot be constructed by specifying a data handle and a variadic list extents if the mapping is not
/// constructible from the extents
TYPED_TEST(
    MdspanTestSuite,
    MdspanConstructedBySpecifyingHandleAndVariadicListOfExtentsRequiresMappingConstructibleFromExtents
) {
  STATIC_ASSERT_TRUE(is_constructible_from_n_extents_v<TypeParam, TypeParam::rank()>);

  // layout_stride does not have a constructor from just an extents object
  using not_constructible_mdspan_type =
      arene::base::testing::rebind_mdspan_layout_t<TypeParam, arene::base::layout_stride>;
  STATIC_ASSERT_FALSE(is_constructible_from_n_extents_v<not_constructible_mdspan_type, TypeParam::rank()>);
}

/// @test A mdspan cannot be constructed by specifying a data handle and a variadic list of extents if the list size
/// is not equal to mdspan::rank() or mdspan::rank_dynamic()
TYPED_TEST(MdspanTestSuite, MdspanConstructedBySpecifyingHandleAndVariadicListOfExtentsRequiresCorrectListSize) {
  constexpr auto rank = TypeParam::rank();
  constexpr auto rank_dynamic = TypeParam::rank_dynamic();

  STATIC_ASSERT_TRUE(is_constructible_from_n_extents_v<TypeParam, TypeParam::rank()>);
  STATIC_ASSERT_TRUE(is_constructible_from_n_extents_v<TypeParam, TypeParam::rank_dynamic()>);

  STATIC_ASSERT_FALSE(is_constructible_from_n_extents_v<TypeParam, rank + 1>);
  STATIC_ASSERT_FALSE(is_constructible_from_n_extents_v<TypeParam, rank_dynamic + 5>);
}

/// @test A mdspan can be constructed by specifying a data handle and a span of all the dynamic extents
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeConstructedBySpecifyingHandleAndSpanOfDynamicExtents) {
  auto data = mdspan_data<TypeParam>();
  auto exts_span = arene::base::span<
      typename TypeParam::index_type const,
      TypeParam::rank_dynamic()>{arene::base::testing::dynamic_extents_data_v<TypeParam>};
  auto span = TypeParam{data.data(), exts_span};

  CONSTEXPR_ASSERT(mdspan_has_expected_properties(span));
}

/// @test A mdspan can be constructed by specifying a data handle and a span of all the extents
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeConstructedBySpecifyingHandleAndSpanOfAllExtents) {
  auto data = mdspan_data<TypeParam>();
  auto exts_span =
      arene::base::span<typename TypeParam::index_type const, TypeParam::rank()>{arene::base::testing::
                                                                                     all_extents_data_v<TypeParam>};

  auto span = TypeParam{data.data(), exts_span};
  CONSTEXPR_ASSERT(mdspan_has_expected_properties(span));
}

/// @test A mdspan cannot be constructed by specifying a data handle and a span of extents if the accessor is not
/// default constructible
TYPED_TEST(MdspanTestSuite, MdspanConstructedBySpecifyingHandleAndSpanOfExtentsRequiresAccessorIsDefaultConstructible) {
  using extents_span = arene::base::span<typename TypeParam::index_type, TypeParam::rank()>;
  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, typename TypeParam::data_handle_type, extents_span const&>::value
  );

  using not_constructible_mdspan_type = arene::base::testing::
      rebind_mdspan_accessor_t<TypeParam, not_default_constructible_accessor<typename TypeParam::element_type>>;
  STATIC_ASSERT_FALSE(std::is_constructible<
                      not_constructible_mdspan_type,
                      typename not_constructible_mdspan_type ::data_handle_type,
                      extents_span const&>::value);
}

/// @test A mdspan cannot be constructed by specifying a data handle and a span of extents if the mapping is not
/// constructible from the extents
TYPED_TEST(
    MdspanTestSuite,
    MdspanConstructedBySpecifyingHandleAndSpanOfExtentsRequiresMappingConstructibleFromExtents
) {
  using extents_span = arene::base::span<typename TypeParam::index_type, TypeParam::rank()>;
  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, typename TypeParam::data_handle_type, extents_span const&>::value
  );

  // layout_stride does not have a constructor from just an extents object
  using not_constructible_mdspan_type =
      arene::base::testing::rebind_mdspan_layout_t<TypeParam, arene::base::layout_stride>;
  STATIC_ASSERT_FALSE(std::is_constructible<
                      not_constructible_mdspan_type,
                      typename not_constructible_mdspan_type ::data_handle_type,
                      extents_span const&>::value);
}

/// @test A mdspan cannot be constructed by specifying a data handle and a span of extents if the span size is not
/// equal to mdspan::rank() or mdspan::rank_dynamic()
TYPED_TEST(MdspanTestSuite, MdspanConstructedBySpecifyingHandleAndSpanOfExtentsRequiresCorrectSpanSize) {
  using handle_type = typename TypeParam::data_handle_type;
  using index_type = typename TypeParam::index_type;

  constexpr auto rank = TypeParam::rank();
  constexpr auto rank_dynamic = TypeParam::rank_dynamic();

  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, handle_type, arene::base::span<index_type, rank>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, handle_type, arene::base::span<index_type, rank_dynamic>>::value);

  STATIC_ASSERT_FALSE(std::is_constructible<TypeParam, handle_type, arene::base::span<index_type, rank + 1>>::value);
  STATIC_ASSERT_FALSE(
      std::is_constructible<TypeParam, handle_type, arene::base::span<index_type, rank_dynamic - 1>>::value
  );
}

/// @test A mdspan can be implicitly constructed by specifying a data handle and a span of extents only if the span
/// size is equal to mdspan::rank_dynamic()
CONDITIONAL_TYPED_TEST(
    MdspanTestSuite,
    MdspanCanBeImplicitlyConstructedBySpecifyingHandleAndSpanOfExtentsIfSpanSizeEqualsDynamicRank,
    (TypeParam::rank() != TypeParam::rank_dynamic())
) {
  using handle_type = typename TypeParam::data_handle_type;
  using index_type = typename TypeParam::index_type;
  constexpr auto rank = TypeParam::rank();
  constexpr auto rank_dynamic = TypeParam::rank_dynamic();

  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, handle_type, arene::base::span<index_type, rank>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, handle_type, arene::base::span<index_type, rank_dynamic>>::value);

  STATIC_ASSERT_TRUE(arene::base::is_implicitly_constructible_v<
                     TypeParam,
                     handle_type,
                     arene::base::span<index_type, rank_dynamic>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_implicitly_constructible_v<TypeParam, handle_type, arene::base::span<index_type, rank>>);
}

/// @test A mdspan can be constructed by specifying a data handle and an array of all the dynamic extents
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeConstructedBySpecifyingHandleAndArrayOfDynamicExtents) {
  auto data = mdspan_data<TypeParam>();
  auto span = TypeParam{data.data(), arene::base::testing::dynamic_extents_data_v<TypeParam>};

  CONSTEXPR_ASSERT(mdspan_has_expected_properties(span));
}

/// @test A mdspan can be constructed by specifying a data handle and an array of all the extents
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeConstructedBySpecifyingHandleAndArrayOfAllExtents) {
  auto data = mdspan_data<TypeParam>();
  auto span = TypeParam{data.data(), arene::base::testing::all_extents_data_v<TypeParam>};
  CONSTEXPR_ASSERT(mdspan_has_expected_properties(span));
}

/// @test A mdspan cannot be constructed by specifying a data handle and an array of extents if the accessor is not
/// default constructible
TYPED_TEST(
    MdspanTestSuite,
    MdspanConstructedBySpecifyingHandleAndArrayOfExtentsRequiresAccessorIsDefaultConstructible
) {
  using extents_array = arene::base::array<typename TypeParam::index_type, TypeParam::rank()>;
  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, typename TypeParam::data_handle_type, extents_array const&>::value
  );

  using not_constructible_mdspan_type = arene::base::testing::
      rebind_mdspan_accessor_t<TypeParam, not_default_constructible_accessor<typename TypeParam::element_type>>;
  STATIC_ASSERT_FALSE(std::is_constructible<
                      not_constructible_mdspan_type,
                      typename not_constructible_mdspan_type ::data_handle_type,
                      extents_array const&>::value);
}

/// @test A mdspan cannot be constructed by specifying a data handle and an array of extents if the mapping is not
/// constructible from the extents
TYPED_TEST(
    MdspanTestSuite,
    MdspanConstructedBySpecifyingHandleAndArrayOfExtentsRequiresMappingConstructibleFromExtents
) {
  using extents_array = arene::base::array<typename TypeParam::index_type, TypeParam::rank()>;
  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, typename TypeParam::data_handle_type, extents_array const&>::value
  );

  // layout_stride does not have a constructor from just an extents object
  using not_constructible_mdspan_type =
      arene::base::testing::rebind_mdspan_layout_t<TypeParam, arene::base::layout_stride>;
  STATIC_ASSERT_FALSE(std::is_constructible<
                      not_constructible_mdspan_type,
                      typename not_constructible_mdspan_type ::data_handle_type,
                      extents_array const&>::value);
}

/// @test A mdspan cannot be constructed by specifying a data handle and an array of extents if the span size is not
/// equal to mdspan::rank() or mdspan::rank_dynamic()
TYPED_TEST(MdspanTestSuite, MdspanConstructedBySpecifyingHandleAndArrayOfExtentsRequiresCorrectArraySize) {
  using handle_type = typename TypeParam::data_handle_type;
  using index_type = typename TypeParam::index_type;

  constexpr auto rank = TypeParam::rank();
  constexpr auto rank_dynamic = TypeParam::rank_dynamic();

  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, handle_type, arene::base::array<index_type, rank>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, handle_type, arene::base::array<index_type, rank_dynamic>>::value
  );

  STATIC_ASSERT_FALSE(std::is_constructible<TypeParam, handle_type, arene::base::array<index_type, rank + 1>>::value);
  STATIC_ASSERT_FALSE(
      std::is_constructible<TypeParam, handle_type, arene::base::array<index_type, rank_dynamic + 5>>::value
  );
}

/// @test A mdspan can be implicitly constructed by specifying a data handle and an array of extents only if the span
/// size is equal to mdspan::rank_dynamic()
CONDITIONAL_TYPED_TEST(
    MdspanTestSuite,
    MdspanCanBeImplicitlyConstructedBySpecifyingHandleAndArrayOfExtentsIfArraySizeEqualsDynamicRank,
    (TypeParam::rank() != TypeParam::rank_dynamic())
) {
  using handle_type = typename TypeParam::data_handle_type;
  using index_type = typename TypeParam::index_type;
  constexpr auto rank = TypeParam::rank();
  constexpr auto rank_dynamic = TypeParam::rank_dynamic();

  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, handle_type, arene::base::array<index_type, rank>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, handle_type, arene::base::array<index_type, rank_dynamic>>::value
  );

  STATIC_ASSERT_TRUE(arene::base::is_implicitly_constructible_v<
                     TypeParam,
                     handle_type,
                     arene::base::array<index_type, rank_dynamic>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_implicitly_constructible_v<TypeParam, handle_type, arene::base::array<index_type, rank>>);
}

/// @test A mdspan can be constructed by specifying a data handle and an extents object
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeConstructedBySpecifyingHandleAndExtents) {
  auto data = mdspan_data<TypeParam>();
  auto exts = arene::base::testing::make_extents<TypeParam>();
  auto span = TypeParam{data.data(), exts};

  CONSTEXPR_ASSERT(mdspan_has_expected_properties(span));
}

/// @test A mdspan cannot be constructed by specifying a data handle and an extents object if the accessor is not
/// default constructible
TYPED_TEST(MdspanTestSuite, MdspanConstructedBySpecifyingHandleAndExtentsRequiresAccessorIsDefaultConstructible) {
  STATIC_ASSERT_TRUE(
      std::is_constructible<TypeParam, typename TypeParam::data_handle_type, typename TypeParam::extents_type const&>::
          value
  );

  using not_constructible_mdspan_type = arene::base::testing::
      rebind_mdspan_accessor_t<TypeParam, not_default_constructible_accessor<typename TypeParam::element_type>>;
  STATIC_ASSERT_FALSE(std::is_constructible<
                      not_constructible_mdspan_type,
                      typename not_constructible_mdspan_type ::data_handle_type,
                      typename not_constructible_mdspan_type ::extents_type const&>::value);
}

/// @test A mdspan cannot be constructed by specifying a data handle and an extents object if the mapping is not
/// constructible from the extents
TYPED_TEST(MdspanTestSuite, MdspanConstructedBySpecifyingHandleAndExtentsRequiresMappingConstructibleFromExtents) {
  STATIC_ASSERT_TRUE(
      std::is_constructible<TypeParam, typename TypeParam::data_handle_type, typename TypeParam::extents_type const&>::
          value
  );

  // layout_stride does not have a constructor from just an extents object
  using not_constructible_mdspan_type =
      arene::base::testing::rebind_mdspan_layout_t<TypeParam, arene::base::layout_stride>;
  STATIC_ASSERT_FALSE(std::is_constructible<
                      not_constructible_mdspan_type,
                      typename not_constructible_mdspan_type ::data_handle_type,
                      typename not_constructible_mdspan_type ::extents_type const&>::value);
}

/// @test A mdspan can be constructed by specifying a data handle and a mapping
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeConstructedBySpecifyingHandleAndMapping) {
  using mapping_type = typename TypeParam::mapping_type;

  auto data = mdspan_data<TypeParam>();
  auto exts = arene::base::testing::make_extents<TypeParam>();
  auto span = TypeParam{data.data(), mapping_type{exts}};

  CONSTEXPR_ASSERT(mdspan_has_expected_properties(span));
}

/// @test A mdspan cannot be constructed by specifying a data handle and a mapping if the accessor is not default
/// constructible
TYPED_TEST(MdspanTestSuite, MdspanConstructedBySpecifyingHandleAndMappingRequiresAccessorIsDefaultConstructible) {
  STATIC_ASSERT_TRUE(
      std::is_constructible<TypeParam, typename TypeParam::data_handle_type, typename TypeParam::mapping_type const&>::
          value
  );

  using not_constructible_mdspan_type = arene::base::testing::
      rebind_mdspan_accessor_t<TypeParam, not_default_constructible_accessor<typename TypeParam::element_type>>;
  STATIC_ASSERT_FALSE(std::is_constructible<
                      not_constructible_mdspan_type,
                      typename not_constructible_mdspan_type ::data_handle_type,
                      typename not_constructible_mdspan_type ::mapping_type const&>::value);
}

/// @test A mdspan can be constructed by specifying a data handle, a mapping and an accessor
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeConstructedBySpecifyingHandleMappingAndAccessor) {
  using mapping_type = typename TypeParam::mapping_type;

  auto data = mdspan_data<TypeParam>();
  auto exts = arene::base::testing::make_extents<TypeParam>();
  auto span = TypeParam{data.data(), mapping_type{exts}, typename TypeParam::accessor_type{}};

  CONSTEXPR_ASSERT(mdspan_has_expected_properties(span));
}

/// @brief An accessor that can be converted from a default_accessor to @c FromElement to an accessor to @c ToElement
template <class ToElement, class FromElement>
class converting_accessor {
 public:
  using offset_policy = converting_accessor;
  using element_type = ToElement;
  using reference = ToElement&;
  using data_handle_type = FromElement*;

  converting_accessor() = default;

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr converting_accessor(arene::base::default_accessor<FromElement>) {}

  constexpr auto access(data_handle_type, std::size_t) const noexcept -> reference;
  constexpr auto offset(data_handle_type, std::size_t) const noexcept -> data_handle_type;
};

/// @test A mdspan can be constructed from a mdspan with a different element type
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeConstructedFromAnMdspanWithADifferentElementType) {
  using source_element_type = typename TypeParam::element_type;
  using target_element_type = ::testing::configurable_value<source_element_type>;

  using mdspan_with_rebound_element_type =
      arene::base::testing::rebind_mdspan_element_type_t<TypeParam, target_element_type>;
  using target_mdspan_type = arene::base::testing::rebind_mdspan_accessor_t<
      mdspan_with_rebound_element_type,
      converting_accessor<target_element_type, source_element_type>>;

  auto data = mdspan_data<TypeParam>();
  auto exts = arene::base::testing::make_extents<TypeParam>();
  auto source_span = TypeParam{data.data(), exts};
  auto copied_span = target_mdspan_type{source_span};

  CONSTEXPR_ASSERT(mdspan_has_expected_properties(copied_span));
}

/// @test A mdspan cannot be constructed from a mdspan with a different element type if the data handle cannot be
/// constructed
TYPED_TEST(
    MdspanTestSuite,
    MdspanCannotBeConstructedFromAnMdspanWithADifferentElementTypeIfDataHandleCannotBeConstructed
) {
  using source_element_type = typename TypeParam::element_type;
  using target_element_type = ::testing::configurable_value<source_element_type>;

  using mdspan_with_rebound_element_type =
      arene::base::testing::rebind_mdspan_element_type_t<TypeParam, target_element_type>;

  // The data handle is `target_element_type*`, which cannot be constructed from `source_element_type*`.
  using target_mdspan_type = arene::base::testing::
      rebind_mdspan_accessor_t<mdspan_with_rebound_element_type, arene::base::default_accessor<target_element_type>>;

  STATIC_ASSERT_FALSE(std::is_constructible<target_mdspan_type, TypeParam const&>::value);
}

/// @test A mdspan with all dynamic extents can be constructed from a mdspan with different extents
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeConstructedFromAnMdspanWithADifferentExtents) {
  using all_dynamic_extents = arene::base::dextents<std::size_t, TypeParam::rank()>;
  using target_mdspan_type = arene::base::testing::rebind_mdspan_extents_t<TypeParam, all_dynamic_extents>;

  auto data = mdspan_data<TypeParam>();
  auto exts = arene::base::testing::make_extents<TypeParam>();
  auto source_span = TypeParam{data.data(), exts};
  auto copied_span = target_mdspan_type{source_span};

  // This can't use `mdspan_has_expected_properties` since it assumes the dynamic extents are sequentially increasing,
  // but the dynamic extent values are copied from the static extents.
  for (auto dim : arene::base::sequential_values<typename TypeParam::rank_type, TypeParam::rank()>) {
    CONSTEXPR_ASSERT(arene::base::cmp_equal(source_span.extent(dim), copied_span.extent(dim)));
  }
}

/// @test A mdspan can be constructed from a mdspan with all dynamic extents
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeConstructedFromAllDynamicExtentsToTargetExtents) {
  using all_dynamic_extents = arene::base::dextents<typename TypeParam::index_type, TypeParam::rank()>;
  using source_mdspan_type = arene::base::testing::rebind_mdspan_extents_t<TypeParam, all_dynamic_extents>;
  using target_mdspan_type = TypeParam;

  auto data = mdspan_data<TypeParam>();
  auto exts = arene::base::testing::make_extents<TypeParam>();
  auto source_span = source_mdspan_type{data.data(), exts};
  auto target_span = target_mdspan_type{source_span};

  for (auto dim : arene::base::sequential_values<typename TypeParam::rank_type, TypeParam::rank()>) {
    CONSTEXPR_ASSERT(arene::base::cmp_equal(source_span.extent(dim), target_span.extent(dim)));
  }
}

/// @test A mdspan cannot be constructed from a mdspan with a different extents if the extents cannot be constructed
TYPED_TEST(MdspanTestSuite, MdspanCannotBeConstructedFromAnMdspanWithADifferentExtentsIfExtentsCannotBeConstructed) {
  using different_static_extents = arene::base::extents<std::size_t, 99, 98, 97, 96>;
  using target_mdspan_type = arene::base::testing::rebind_mdspan_extents_t<TypeParam, different_static_extents>;

  STATIC_ASSERT_FALSE(std::is_constructible<target_mdspan_type, TypeParam const&>::value);
}

/// @brief A layout whose mapping type is convertible from layout left and right
template <class Base>
class layout_with_convertible_mapping {
 public:
  template <typename Extents>
  class mapping : public Base::template mapping<Extents> {
    using base_mapping = typename Base::template mapping<Extents>;

   public:
    using layout_type = layout_with_convertible_mapping;

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr mapping(base_mapping const& mapping_in)
        : base_mapping{mapping_in} {}
  };
};

/// @test A mdspan can be constructed from a mdspan with a different layout
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeConstructedFromAnMdspanWithADifferentLayout) {
  using target_mdspan_type = arene::base::testing::
      rebind_mdspan_layout_t<TypeParam, layout_with_convertible_mapping<typename TypeParam::layout_type>>;

  auto data = mdspan_data<TypeParam>();
  auto exts = arene::base::testing::make_extents<TypeParam>();
  auto source_span = TypeParam{data.data(), exts};
  auto copied_span = target_mdspan_type{source_span};

  CONSTEXPR_ASSERT(mdspan_has_expected_properties(copied_span));
}

/// @test A mdspan cannot be constructed from a mdspan with a different layout if the layout is not constructible
TYPED_TEST(MdspanTestSuite, MdspanCannotBeConstructedFromAnMdspanWithADifferentLayoutIfTheLayoutIsNotConstructible) {
  using not_constructible_mdspan_type =
      arene::base::testing::rebind_mdspan_layout_t<TypeParam, layout_with_not_default_constructible_mapping>;

  STATIC_ASSERT_FALSE(std::is_constructible<not_constructible_mdspan_type, TypeParam const&>::value);
}

/// @brief An accessor that is implicitly convertible from the default accessor
template <class T>
class convertible_from_default_accessor {
 public:
  using offset_policy = convertible_from_default_accessor;
  using element_type = T;
  using reference = T&;
  using data_handle_type = T*;

  convertible_from_default_accessor() = default;
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr convertible_from_default_accessor(arene::base::default_accessor<T>) {}
  constexpr auto access(data_handle_type, std::size_t) const noexcept -> reference;
  constexpr auto offset(data_handle_type, std::size_t) const noexcept -> data_handle_type;
};

/// @test A mdspan can be constructed from a mdspan with a different accessor
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanCanBeConstructedFromAnMdspanWithADifferentAccessor) {
  using target_mdspan_type = arene::base::testing::
      rebind_mdspan_accessor_t<TypeParam, convertible_from_default_accessor<typename TypeParam::element_type>>;

  auto data = mdspan_data<TypeParam>();
  auto exts = arene::base::testing::make_extents<TypeParam>();
  auto source_span = TypeParam{data.data(), exts};
  auto copied_span = target_mdspan_type{source_span};

  CONSTEXPR_ASSERT(mdspan_has_expected_properties(copied_span));
}

/// @test A mdspan cannot be constructed from a mdspan with a different accessor if the accessor is not constructible
TYPED_TEST(MdspanTestSuite, MdspanCannotBeConstructedFromAnMdspanWithADifferentAccessorIfAccessorIsNotConstructible) {
  using target_mdspan_type = arene::base::testing::
      rebind_mdspan_accessor_t<TypeParam, not_default_constructible_accessor<typename TypeParam::element_type>>;
  STATIC_ASSERT_FALSE(std::is_constructible<target_mdspan_type, TypeParam const&>::value);
}

/// @test A mdspan can be implicitly constructed from an mdspan of another type if the mapping and accessor can be
/// implicitly converted.
TYPED_TEST(
    MdspanTestSuite,
    MdspanCanBeImplicitlyConstructedFromAnotherMdspanIfMappingAndAccessorAreImplicitlyConvertible
) {
  using target_mdspan_type = arene::base::testing::
      rebind_mdspan_accessor_t<TypeParam, convertible_from_default_accessor<typename TypeParam::element_type>>;

  STATIC_ASSERT_TRUE(arene::base::is_implicitly_constructible_v<target_mdspan_type, TypeParam>);
}

/// @test A mdspan cannot be implicitly constructed from an mdspan of another type if the mapping cannot be implicitly
/// converted.
///
/// Note: The `explicitly_convertible_extents` relies on finding a larger index type to be explicitly convertible, so
/// this test is limited to index types that are less than the max integer size.
CONDITIONAL_TYPED_TEST(
    MdspanTestSuite,
    MdspanCannotBeImplicitlyConstructedFromAnotherMdspanIfMappingIsNotImplicitlyConvertible,
    sizeof(typename TypeParam::index_type) < sizeof(std::int64_t)
) {
  using non_convertible_extents_type =
      arene::base::testing::explicitly_convertible_extents<typename TypeParam::extents_type>;

  using source_mdspan_type = arene::base::mdspan<
      typename TypeParam::element_type,
      non_convertible_extents_type,
      typename TypeParam::layout_type,
      typename TypeParam::accessor_type>;
  using target_mdspan_type = TypeParam;

  STATIC_ASSERT_TRUE(std::is_constructible<target_mdspan_type, source_mdspan_type>::value);
  STATIC_ASSERT_FALSE(arene::base::is_implicitly_constructible_v<target_mdspan_type, source_mdspan_type>);
  STATIC_ASSERT_FALSE(std::is_convertible<source_mdspan_type, target_mdspan_type>::value);
}

/// @brief An accessor that is explicitly constructible from the default accessor
template <class T>
class explicitly_constructible_from_default_accessor {
 public:
  using offset_policy = explicitly_constructible_from_default_accessor;
  using element_type = T;
  using reference = T&;
  using data_handle_type = T*;

  explicitly_constructible_from_default_accessor() = default;
  explicit constexpr explicitly_constructible_from_default_accessor(arene::base::default_accessor<T>) {}
  constexpr auto access(data_handle_type, std::size_t) const noexcept -> reference;
  constexpr auto offset(data_handle_type, std::size_t) const noexcept -> data_handle_type;
};

/// @test A mdspan cannot be implicitly constructed from an mdspan of another type if the accessor cannot be
/// implicitly converted.
TYPED_TEST(MdspanTestSuite, MdspanCannotBeImplicitlyConstructedFromAnotherMdspanIfAccessorIsNotImplicitlyConvertible) {
  using target_mdspan_type = arene::base::testing::rebind_mdspan_accessor_t<
      TypeParam,
      explicitly_constructible_from_default_accessor<typename TypeParam::element_type>>;

  STATIC_ASSERT_TRUE(std::is_constructible<target_mdspan_type, TypeParam>::value);
  STATIC_ASSERT_FALSE(arene::base::is_implicitly_constructible_v<target_mdspan_type, TypeParam>);
}

/// @test The data member observers return const references to their respective members.
CONSTEXPR_TYPED_TEST(MdspanTestSuite, DataMemberObserversReturnConstRefToTheDataMember) {
  auto data = mdspan_data<TypeParam>();
  auto extents = arene::base::testing::make_extents<TypeParam>();
  auto span = TypeParam{data.data(), extents};

  CONSTEXPR_ASSERT(std::is_same<decltype(span.extents()), typename TypeParam::extents_type const&>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(span.data_handle()), typename TypeParam::data_handle_type const&>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(span.mapping()), typename TypeParam::mapping_type const&>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(span.accessor()), typename TypeParam::accessor_type const&>::value);

  CONSTEXPR_ASSERT_EQ(span.data_handle(), typename TypeParam::data_handle_type{data.data()});
  CONSTEXPR_ASSERT_EQ(span.extents(), extents);
  CONSTEXPR_ASSERT_EQ(span.mapping(), typename TypeParam::mapping_type{extents});

  // The accessor policy does not specify any comparison operators.
  std::ignore = span.accessor();
}

/// @brief Helper layout where all properties return false
struct always_false_layout {
  template <typename Extents>
  class mapping : public arene::base::layout_right::mapping<Extents> {
   public:
    using layout_type = always_false_layout;

    static constexpr auto is_always_unique() noexcept -> bool { return false; }
    static constexpr auto is_always_exhaustive() noexcept -> bool { return false; }
    static constexpr auto is_always_strided() noexcept -> bool { return false; }

    constexpr auto is_unique() const noexcept -> bool { return false; }
    constexpr auto is_exhaustive() const noexcept -> bool { return false; }
    constexpr auto is_strided() const noexcept -> bool { return false; }
  };
};

/// @test Test mdspan layout properties accessor defer to the given layout
TEST(NonParameterizedMdspanTestSuite, MdspanLayoutPropertiesAreDeterminedByTheLayout) {
  using true_mdspan_type = arene::base::mdspan<int, arene::base::dextents<std::size_t, 5>, arene::base::layout_right>;
  using false_mdspan_type = arene::base::mdspan<int, arene::base::dextents<std::size_t, 5>, always_false_layout>;

  STATIC_ASSERT_TRUE(true_mdspan_type::is_always_unique());
  STATIC_ASSERT_TRUE(true_mdspan_type::is_always_exhaustive());
  STATIC_ASSERT_TRUE(true_mdspan_type::is_always_strided());

  STATIC_ASSERT_FALSE(false_mdspan_type::is_always_unique());
  STATIC_ASSERT_FALSE(false_mdspan_type::is_always_exhaustive());
  STATIC_ASSERT_FALSE(false_mdspan_type::is_always_strided());

  constexpr true_mdspan_type true_span{};
  constexpr false_mdspan_type false_span{};

  STATIC_ASSERT_TRUE(true_span.is_unique());
  STATIC_ASSERT_TRUE(true_span.is_exhaustive());
  STATIC_ASSERT_TRUE(true_span.is_strided());

  STATIC_ASSERT_FALSE(false_span.is_unique());
  STATIC_ASSERT_FALSE(false_span.is_exhaustive());
  STATIC_ASSERT_FALSE(false_span.is_strided());
}

/// @brief Check that the strides returned by a mdspan match those of its mapping
template <class Mdspan>
constexpr auto strides_equal_to_mapping_strides(Mdspan const& span) -> bool {
  auto mapping = typename Mdspan::mapping_type{span.extents()};

  // NOLINTNEXTLINE(readability-use-anyofallof)
  for (auto dim : arene::base::sequential_values<typename Mdspan::rank_type, Mdspan::rank()>) {
    if (span.stride(dim) != mapping.stride(dim)) {
      return false;
    }
  }
  return true;
}

/// @test The mdspan stride function returns the correct stride from the underlying layout when it is a default
/// constructed layout right.
CONSTEXPR_TYPED_TEST(MdspanTestSuite, MdspanStrideReturnsSameValueAsMappingStride) {
  auto data = mdspan_data<TypeParam>();
  auto extents = arene::base::testing::make_extents<TypeParam>();
  auto span = TypeParam{data.data(), extents};

  CONSTEXPR_ASSERT(strides_equal_to_mapping_strides(span));

  // The right_strides detail function is called by the default construction above, but is not being marked covered
  // unless explicitly called.
  std::ignore = arene::base::layout_detail::right_strides(typename TypeParam::extents_type{});
}
namespace test {
// declaration so we can make an unqualified call to swap
template <class>
auto swap() -> void = delete;

}  // namespace test

/// @test The static assertion that the bounds of a layout's Extents don't overflow works correctly
TYPED_TEST(MdspanTestSuite, CheckIndicesForOverflow) {
  // This function is only ever run inside of a static assert so we don't get coverage for it unless we run it
  // manually on all the extents that are instantiated.
  STATIC_ASSERT_FALSE(arene::base::layout_detail::mapped_indices_overflow<typename TypeParam::extents_type>());
}

/// @test mdspan::operator() returns expected element from all zero indices.
CONSTEXPR_TYPED_TEST(MdspanTestSuite, CheckAllZeroIndices) {
  using value_type = typename TypeParam::value_type;

  auto data = mdspan_data<TypeParam>();
  auto extents = arene::base::testing::make_extents<TypeParam>();
  auto mdspan = TypeParam{data.data(), extents};
  CONSTEXPR_ASSERT(mdspan.size() != 0);

  auto const all_zeros = arene::base::array<typename TypeParam::index_type, TypeParam::rank()>{};
  auto& r1 = arene::base::apply(mdspan, all_zeros);
  auto& r2 = mdspan(all_zeros);
  auto& r3 = mdspan(span_of(all_zeros));

  CONSTEXPR_ASSERT_EQ(&r1, data.data());
  CONSTEXPR_ASSERT_EQ(&r2, data.data());
  CONSTEXPR_ASSERT_EQ(&r3, data.data());

  CONSTEXPR_ASSERT(std::is_same<decltype(&r1), value_type*>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(&r2), value_type*>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(&r3), value_type*>::value);
}

/// @test mdspan::operator() returns expected element from all zero indices with a const element type.
CONSTEXPR_TYPED_TEST(MdspanTestSuite, CheckAllZeroIndicesConstData) {
  using value_type = typename TypeParam::value_type;

  using mdspan_type = arene::base::testing::mdspan_transform_element_type_t<TypeParam, std::add_const_t>;

  auto const data = mdspan_data<mdspan_type>();
  auto const extents = arene::base::testing::make_extents<mdspan_type>();
  auto const mdspan = mdspan_type{data.data(), extents};
  CONSTEXPR_ASSERT(mdspan.size() != 0);

  auto const all_zeros = arene::base::array<typename mdspan_type::index_type, mdspan_type::rank()>{};
  auto& r1 = arene::base::apply(mdspan, all_zeros);
  auto& r2 = mdspan(all_zeros);
  auto& r3 = mdspan(span_of(all_zeros));

  CONSTEXPR_ASSERT_EQ(&r1, data.data());
  CONSTEXPR_ASSERT_EQ(&r2, data.data());
  CONSTEXPR_ASSERT_EQ(&r3, data.data());

  CONSTEXPR_ASSERT(std::is_same<decltype(&r1), value_type const*>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(&r2), value_type const*>::value);
  CONSTEXPR_ASSERT(std::is_same<decltype(&r3), value_type const*>::value);
}

/// @test mdspan::operator() returns expected element from all last indices.
CONSTEXPR_TYPED_TEST(MdspanTestSuite, CheckLastIndices) {
  using arene::base::mdspan_detail::representable_cast;

  using index_type = typename TypeParam::index_type;

  constexpr auto extents = arene::base::testing::make_extents<TypeParam>();
  auto last_index = arene::base::mdspan_detail::to_array(extents);
  arene::base::transform(last_index.begin(), last_index.end(), last_index.begin(), max_index<index_type>);

  auto data = arene::base::array<
      typename TypeParam::element_type,
      representable_cast<std::size_t>(typename TypeParam::mapping_type{extents}.required_span_size())  //
      >{};
  auto mdspan = TypeParam{data.data(), extents};
  CONSTEXPR_ASSERT_FALSE(mdspan.empty());

  auto& r1 = arene::base::apply(mdspan, last_index);
  auto& r2 = mdspan(last_index);
  auto& r3 = mdspan(span_of(last_index));
  auto& acc = mdspan.accessor();
  auto& map = mdspan.mapping();
  CONSTEXPR_ASSERT(data.data() <= &r1);
  CONSTEXPR_ASSERT(data.data() <= &r2);
  CONSTEXPR_ASSERT(data.data() <= &r3);
  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  CONSTEXPR_ASSERT(&r1 < data.data() + map.required_span_size());
  CONSTEXPR_ASSERT(&r2 < data.data() + map.required_span_size());
  CONSTEXPR_ASSERT(&r3 < data.data() + map.required_span_size());
  // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  if (map.is_unique() && map.is_exhaustive()) {
    CONSTEXPR_ASSERT_EQ(&r1, acc.offset(data.data(), mdspan.size() - std::size_t{1}));
    CONSTEXPR_ASSERT_EQ(&r2, acc.offset(data.data(), mdspan.size() - std::size_t{1}));
    CONSTEXPR_ASSERT_EQ(&r3, acc.offset(data.data(), mdspan.size() - std::size_t{1}));
  }
}

/// @test All three mdspan::operator() overloads are no-throw invocable
CONSTEXPR_TYPED_TEST(MdspanTestSuite, NoThrowInvocableTests) {
  using index_type = typename TypeParam::index_type;

  // variadic overload
  using index_args = arene::base::repeat_type_t<TypeParam::rank(), arene::base::type_list, index_type>;
  using nothrow_invocable_with_index_type =
      arene::base::type_lists::concat_t<arene::base::is_nothrow_invocable<TypeParam const&>, index_args>;
  CONSTEXPR_ASSERT(nothrow_invocable_with_index_type::value);
  using mdspan_call_result =
      arene::base::type_lists::concat_t<arene::base::invoke_result<TypeParam const&>, index_args>;
  CONSTEXPR_ASSERT(std::is_same<typename TypeParam::reference, typename mdspan_call_result::type>::value);

  constexpr auto extents = arene::base::testing::make_extents<TypeParam>();
  auto last_index = arene::base::mdspan_detail::to_array(extents);

  // array overload
  CONSTEXPR_ASSERT(arene::base::is_nothrow_invocable_v<TypeParam const&, decltype(last_index)>);
  CONSTEXPR_ASSERT(
      std::is_same<typename TypeParam::reference, arene::base::invoke_result_t<TypeParam, decltype(last_index)>>::value
  );

  // span overload
  CONSTEXPR_ASSERT(arene::base::is_nothrow_invocable_v<TypeParam const&, decltype(span_of(last_index))>);
  CONSTEXPR_ASSERT(std::is_same<
                   typename TypeParam::reference,
                   arene::base::invoke_result_t<TypeParam, decltype(span_of(last_index))>>::value);
}

/// @test mdspan::operator() is not invocable with an extra index
CONSTEXPR_TYPED_TEST(MdspanTestSuite, CheckInvocabilityWithExtraIndex) {
  constexpr auto rank = TypeParam::rank();
  using index_type = typename TypeParam::index_type;

  using index_args = arene::base::repeat_type_t<rank + 1, arene::base::type_list, index_type>;
  using invocable_with_extra_index =
      arene::base::type_lists::concat_t<arene::base::is_invocable<TypeParam>, index_args>;
  CONSTEXPR_ASSERT_FALSE(invocable_with_extra_index::value);

  CONSTEXPR_ASSERT_FALSE(arene::base::is_invocable_v<TypeParam, arene::base::array<index_type, rank + 1>>);
  CONSTEXPR_ASSERT_FALSE(arene::base::is_invocable_v<TypeParam, arene::base::span<index_type, rank + 1>>);
}

/// @test mdspan::operator() is not invocable with indices which do not convert to index_type
CONSTEXPR_TYPED_TEST(MdspanTestSuite, NotInvocableWithNonconvertibleIndexType) {
  struct nonconvertible_index_type {};
  CONSTEXPR_ASSERT_FALSE(std::is_convertible<nonconvertible_index_type, typename TypeParam::index_type>::value);
  using index_args = arene::base::repeat_type_t<TypeParam::rank(), arene::base::type_list, nonconvertible_index_type>;
  using invocable_with_nonconvertible_indices =
      arene::base::type_lists::concat_t<arene::base::is_invocable<TypeParam>, index_args>;
  CONSTEXPR_ASSERT_EQ(invocable_with_nonconvertible_indices::value, TypeParam::rank() == 0);
}

/// @test mdspan::operator() is not invocable when index_type is not nothrow constructible from the supplied indices
CONSTEXPR_TYPED_TEST(MdspanTestSuite, NotInvocableWithConvertibleButPotentiallyThrowingIndexType) {
  struct convertible_index_type {
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    operator typename TypeParam::index_type() const noexcept(false) { return {}; }
  };

  CONSTEXPR_ASSERT(std::is_convertible<convertible_index_type, typename TypeParam::index_type>::value);
  CONSTEXPR_ASSERT_FALSE(std::is_nothrow_constructible<typename TypeParam::index_type, convertible_index_type>::value);
  using index_args = arene::base::repeat_type_t<TypeParam::rank(), arene::base::type_list, convertible_index_type>;
  using invocable_with_convertible_index_type =
      arene::base::type_lists::concat_t<arene::base::is_invocable<TypeParam>, index_args>;
  CONSTEXPR_ASSERT_EQ(invocable_with_convertible_index_type::value, TypeParam::rank() == 0);
}

/// @brief An accessor that applies a UnaryTrait transform to a base accessor
template <template <class> class UnaryTrait, class Accessor, bool AccessNoexcept = true>
struct transformed_accessor : Accessor {
  using offset_policy = transformed_accessor;
  using reference = typename UnaryTrait<typename Accessor::element_type>::type;

  constexpr auto access(typename Accessor::data_handle_type p, std::size_t i) const  //
      noexcept(AccessNoexcept)                                                       //
      -> reference                                                                   //
  {
    auto const& acc = static_cast<Accessor const&>(*this);
    return static_cast<reference>(acc.access(std::move(p), i));
  }
};

/// @test mdspan::operator() returns an rvalue reference if the accessor is
/// designed to return an rvalue reference.
CONSTEXPR_TYPED_TEST(MdspanTestSuite, WithRvalueReferenceAccessor) {
  using mdspan_type = arene::base::testing::rebind_mdspan_accessor_t<
      TypeParam,
      transformed_accessor<std::add_rvalue_reference, typename TypeParam::accessor_type>>;

  auto data = mdspan_data<mdspan_type>();
  auto extents = arene::base::testing::make_extents<mdspan_type>();
  auto mdspan = mdspan_type{data.data(), extents};
  CONSTEXPR_ASSERT(mdspan.size() != 0);

  auto const all_zeros = arene::base::array<typename mdspan_type::index_type, mdspan_type::rank()>{};
  auto&& r1 = arene::base::apply(mdspan, all_zeros);
  auto&& r2 = mdspan(all_zeros);
  auto&& r3 = mdspan(span_of(all_zeros));

  static_assert(
      std::is_rvalue_reference<arene::base::invoke_result_t<decltype(mdspan), decltype(all_zeros)>>::value,
      "accessor must return an rvalue reference"
  );

  CONSTEXPR_ASSERT_EQ(&r1, data.data());
  CONSTEXPR_ASSERT_EQ(&r2, data.data());
  CONSTEXPR_ASSERT_EQ(&r3, data.data());
}

/// @test mdspan::operator() returns a proxy object if the accessor is
/// designed to return a proxy object.
CONDITIONAL_TYPED_TEST(
    MdspanTestSuite,
    WithProxyObjectAccessor,
    std::is_copy_constructible<typename TypeParam::element_type>::value
) {
  using mdspan_type = arene::base::testing::rebind_mdspan_accessor_t<
      TypeParam,
      transformed_accessor<arene::base::type_identity, typename TypeParam::accessor_type>>;

  auto data = mdspan_data<mdspan_type>();
  auto extents = arene::base::testing::make_extents<mdspan_type>();
  auto mdspan = mdspan_type{data.data(), extents};
  ASSERT_NE(mdspan.size(), 0);

  auto const all_zeros = arene::base::array<typename mdspan_type::index_type, mdspan_type::rank()>{};
  auto&& r1 = arene::base::apply(mdspan, all_zeros);
  auto&& r2 = mdspan(all_zeros);
  auto&& r3 = mdspan(span_of(all_zeros));

  static_assert(
      std::is_object<arene::base::invoke_result_t<decltype(mdspan), decltype(all_zeros)>>::value,
      "accessor must not return a reference"
  );

  ASSERT_NE(&r1, data.data());
  ASSERT_NE(&r2, data.data());
  ASSERT_NE(&r3, data.data());

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "These tests don't perform arithmetic, so equality is OK even for floating point");
  ASSERT_EQ(r1, *data.data());
  ASSERT_EQ(r2, *data.data());
  ASSERT_EQ(r3, *data.data());
  ARENE_IGNORE_END();
}

/// @test mdspan::operator() is noexcept(false) if the Accessor's access() function is noexcept(false)
CONSTEXPR_TYPED_TEST(MdspanTestSuite, ThrowingAccessor) {
  using throwing_accessor_type =
      transformed_accessor<std::add_lvalue_reference, typename TypeParam::accessor_type, false>;

  using mdspan_type = arene::base::testing::rebind_mdspan_accessor_t<TypeParam, throwing_accessor_type>;

  using index_type = typename mdspan_type::index_type;
  using index_args = arene::base::repeat_type_t<mdspan_type::rank(), arene::base::type_list, index_type>;

  CONSTEXPR_ASSERT(  //
      arene::base::type_lists::concat_t<arene::base::is_invocable<mdspan_type>, index_args>::value
  );
  CONSTEXPR_ASSERT(//
      arene::base::is_invocable_v<mdspan_type, arene::base::array<index_type, mdspan_type::rank()>>);
  CONSTEXPR_ASSERT(//
      arene::base::is_invocable_v<mdspan_type, arene::base::span<index_type, mdspan_type::rank()>>);

  CONSTEXPR_ASSERT(  //
      !arene::base::type_lists::concat_t<arene::base::is_nothrow_invocable<mdspan_type>, index_args>::value
  );
  CONSTEXPR_ASSERT(//
      !arene::base::is_nothrow_invocable_v<mdspan_type, arene::base::array<index_type, mdspan_type::rank()>>);
  CONSTEXPR_ASSERT(//
      !arene::base::is_nothrow_invocable_v<mdspan_type, arene::base::span<index_type, mdspan_type::rank()>>);
}

struct throwing_layout {
  template <typename Extents>
  struct mapping : public arene::base::layout_right::mapping<Extents> {
    using layout_type = throwing_layout;

    template <class... OtherIndexTypes>
    constexpr auto operator()(OtherIndexTypes&&...) const noexcept(false) -> typename Extents::index_type {
      // this value doesn't need to be correct since we are just testing noexcept
      return {};
    }
  };
};

/// @test mdspan::operator() is noexcept(false) if the Mapping's operator() function is noexcept(false)
CONSTEXPR_TYPED_TEST(MdspanTestSuite, ThrowingMapping) {
  using mdspan_type = arene::base::testing::rebind_mdspan_layout_t<TypeParam, throwing_layout>;

  using index_type = typename mdspan_type::index_type;
  using index_args = arene::base::repeat_type_t<mdspan_type::rank(), arene::base::type_list, index_type>;

  CONSTEXPR_ASSERT(  //
      arene::base::type_lists::concat_t<arene::base::is_invocable<mdspan_type>, index_args>::value
  );
  CONSTEXPR_ASSERT(//
      arene::base::is_invocable_v<mdspan_type, arene::base::array<index_type, mdspan_type::rank()>>);
  CONSTEXPR_ASSERT(//
      arene::base::is_invocable_v<mdspan_type, arene::base::span<index_type, mdspan_type::rank()>>);

  CONSTEXPR_ASSERT(  //
      !arene::base::type_lists::concat_t<arene::base::is_nothrow_invocable<mdspan_type>, index_args>::value
  );
  CONSTEXPR_ASSERT(//
      !arene::base::is_nothrow_invocable_v<mdspan_type, arene::base::array<index_type, mdspan_type::rank()>>);
  CONSTEXPR_ASSERT(//
      !arene::base::is_nothrow_invocable_v<mdspan_type, arene::base::span<index_type, mdspan_type::rank()>>);
}

/// @test mdspan values can be swapped with ADL swap and it is always noexcept
CONSTEXPR_TYPED_TEST(MdspanTestSuite, Swappable) {
  using mdspan_type = TypeParam;

  auto const extents = arene::base::testing::make_extents<mdspan_type>();

  auto data1 = mdspan_data<mdspan_type>();
  auto data2 = mdspan_data<mdspan_type>();

  auto span1 = mdspan_type{data1.data(), extents};
  auto span2 = mdspan_type{data2.data(), extents};

  using test::swap;
  swap(span1, span2);

  CONSTEXPR_ASSERT(span1.data_handle() == data2.data());
  CONSTEXPR_ASSERT(span2.data_handle() == data1.data());

  CONSTEXPR_ASSERT(noexcept(swap(std::declval<mdspan_type&>(), std::declval<mdspan_type&>())));
}

/// @test mdspan values with const element types can be swapped with ADL swap
/// and it is always noexcept
CONSTEXPR_TYPED_TEST(MdspanTestSuite, SwappableConstElementType) {
  using element_type = typename TypeParam::element_type;

  using mdspan_type = arene::base::testing::rebind_mdspan_element_type_t<  //
      arene::base::testing::rebind_mdspan_accessor_t<                      //
          TypeParam,                                                       //
          arene::base::default_accessor<element_type const>                //
          >,                                                               //
      element_type const                                                   //
      >;

  auto const extents = arene::base::testing::make_extents<mdspan_type>();

  auto const data1 = mdspan_data<mdspan_type>();
  auto const data2 = mdspan_data<mdspan_type>();

  auto span1 = mdspan_type{data1.data(), extents};
  auto span2 = mdspan_type{data2.data(), extents};

  using test::swap;
  swap(span1, span2);

  CONSTEXPR_ASSERT(span1.data_handle() == data2.data());
  CONSTEXPR_ASSERT(span2.data_handle() == data1.data());

  CONSTEXPR_ASSERT(noexcept(swap(std::declval<mdspan_type&>(), std::declval<mdspan_type&>())));
}

namespace test {

template <class ElementType>
struct stateful_accessor                          //
    : arene::base::default_accessor<ElementType>  //
{
  using offset_policy = stateful_accessor;

  int value;

  constexpr explicit stateful_accessor(int arg) noexcept
      : value{arg} {}
};

struct layout_with_stateful_mapping  //
{
  template <class Extents>
  struct mapping
      : arene::base::layout_right::mapping<Extents>  //
  {
    using layout_type = layout_with_stateful_mapping;

    int value;

    constexpr mapping(int arg, Extents ext)
        : arene::base::layout_right::mapping<Extents>{std::move(ext)},
          value{arg}  //
    {}
  };
};

}  // namespace test

/// @test mdspan values with stateful mappings and accessors can be swapped with
/// ADL swap and it is always noexcept
CONSTEXPR_TYPED_TEST(MdspanTestSuite, SwappableWithStatefulMappingAndAccessor) {
  using mdspan_type = arene::base::testing::rebind_mdspan_layout_t<
      arene::base::testing::rebind_mdspan_accessor_t<                //
          TypeParam,                                                 //
          test::stateful_accessor<typename TypeParam::element_type>  //
          >,                                                         //
      test::layout_with_stateful_mapping                             //
      >;

  using mapping_type = typename mdspan_type::mapping_type;
  using accessor_type = typename mdspan_type::accessor_type;

  auto const extents = arene::base::testing::make_extents<mdspan_type>();

  auto data1 = mdspan_data<mdspan_type>();
  auto data2 = mdspan_data<mdspan_type>();

  auto span1 = mdspan_type{data1.data(), mapping_type{1, extents}, accessor_type{3}};
  auto span2 = mdspan_type{data2.data(), mapping_type{2, extents}, accessor_type{4}};

  using test::swap;
  swap(span1, span2);

  CONSTEXPR_ASSERT(span1.data_handle() == data2.data());
  CONSTEXPR_ASSERT(span2.data_handle() == data1.data());

  CONSTEXPR_ASSERT(span1.mapping().value == 2);
  CONSTEXPR_ASSERT(span2.mapping().value == 1);

  CONSTEXPR_ASSERT(span1.accessor().value == 4);
  CONSTEXPR_ASSERT(span2.accessor().value == 3);

  CONSTEXPR_ASSERT(noexcept(swap(std::declval<mdspan_type&>(), std::declval<mdspan_type&>())));
}

}  // namespace

// NOLINTEND(readability-identifier-length)
