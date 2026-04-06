// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_MDSPAN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_MDSPAN_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/mdspan/default_accessor.hpp"  // IWYU pragma: export
#include "arene/base/mdspan/detail/representable_cast.hpp"
#include "arene/base/mdspan/detail/tuple_span.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/is_accessor_policy.hpp"
#include "arene/base/mdspan/is_layout_mapping.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_abstract.hpp"
#include "arene/base/stdlib_choice/is_array.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_object.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/tuple/apply.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/utility/safe_comparisons.hpp"
#include "arene/base/utility/swap.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

namespace mdspan_detail {

/// @brief Helper variable to indicate if all the extents are dynamic
/// @tparam Extents The extents
template <typename Extents>
extern constexpr bool all_extents_are_dynamic_v = Extents::rank() == Extents::rank_dynamic();

/// @brief Helper variable to indicate if all the extents are static
/// @tparam Extents The extents
template <typename Extents>
extern constexpr bool all_extents_are_static_v = Extents::rank_dynamic() == 0;

/// @brief Validate that @c SourceExtents can be used to construct @c TargetExtents at runtime
/// @tparam TargetExtents The extents type to construct
/// @tparam SourceExtents The extents type to construct from
///
/// @pre For each rank index @c r of @c extents_type, <c> static_extent(r) == dynamic_extent || static_extent(r) ==
/// other.extent(r) </c> is true, otherwise this is an @c ARENE_PRECONDITION violation.
///
/// Note: this overload is a no-op as the extent values can be verified by the converting constructor constraints. This
/// prevents instantiating a branch that cannot actually be covered by tests.
// parasoft-begin-suppress AUTOSAR-M0_1_8-b "This overload avoids checking when the extents are statically known to be
// compatible"
template <
    class TargetExtents,
    class SourceExtents,
    arene::base::constraints<std::enable_if_t<
        TargetExtents::rank() == 0 || all_extents_are_dynamic_v<TargetExtents> ||
        all_extents_are_static_v<SourceExtents>>> = nullptr>
constexpr auto validate_extents_compatibility(SourceExtents const&) -> void {}
// parasoft-end-suppress AUTOSAR-M0_1_8-b

/// @brief Validate that @c SourceExtents can be used to construct @c TargetExtents at runtime
/// @tparam TargetExtents The extents type to construct
/// @tparam SourceExtents The extents type to construct from
/// @param source The extents to construct from
///
/// @pre For each rank index @c r of @c extents_type, <c> static_extent(r) == dynamic_extent || static_extent(r) ==
/// other.extent(r) </c> is true, otherwise this is an @c ARENE_PRECONDITION violation.
///
/// Note: this overload does not perform the dynamic extent check as all the @c TargetExtents are static. This prevents
/// instantiating a branch that cannot actually be covered by tests.
template <
    class TargetExtents,
    class SourceExtents,
    arene::base::constraints<
        std::enable_if_t<TargetExtents::rank() != 0>,
        std::enable_if_t<!all_extents_are_static_v<SourceExtents>>,
        std::enable_if_t<all_extents_are_static_v<TargetExtents>>> = nullptr>
constexpr auto validate_extents_compatibility(SourceExtents const& source) -> void {
  // parasoft-begin-suppress AUTOSAR-M14_6_1-a "False positive: No unqualified size_t"
  for (auto dim : arene::base::sequential_values<typename TargetExtents::rank_type, TargetExtents::rank()>) {
    // parasoft-end-suppress AUTOSAR-M14_6_1-a
    // Note that static_extent() always returns std::size_t, while extent() returns index_type.
    ARENE_PRECONDITION(cmp_equal(TargetExtents::static_extent(dim), source.extent(dim)));
  }
}

/// @brief Validate that @c SourceExtents can be used to construct @c TargetExtents at runtime
/// @tparam TargetExtents The extents type to construct
/// @tparam SourceExtents The extents type to construct from
/// @param source The extents to construct from
///
/// @pre For each rank index @c r of @c extents_type, <c> static_extent(r) == dynamic_extent || static_extent(r) ==
/// other.extent(r) </c> is true, otherwise this is an @c ARENE_PRECONDITION violation.
template <
    class TargetExtents,
    class SourceExtents,
    arene::base::constraints<
        std::enable_if_t<TargetExtents::rank() != 0>,
        std::enable_if_t<!all_extents_are_static_v<SourceExtents>>,
        std::enable_if_t<!all_extents_are_static_v<TargetExtents>>,
        std::enable_if_t<!all_extents_are_dynamic_v<TargetExtents>>> = nullptr>
constexpr auto validate_extents_compatibility(SourceExtents const& source) -> void {
  // parasoft-begin-suppress AUTOSAR-M14_6_1-a "False positive: No unqualified size_t"
  for (auto dim : arene::base::sequential_values<typename TargetExtents::rank_type, TargetExtents::rank()>) {
    // parasoft-end-suppress AUTOSAR-M14_6_1-a
    // Note that static_extent() always returns std::size_t, while extent() returns index_type.
    ARENE_PRECONDITION(
        (TargetExtents::static_extent(dim) == arene::base::dynamic_extent) ||
        cmp_equal(TargetExtents::static_extent(dim), source.extent(dim))
    );
  }
}

}  // namespace mdspan_detail

// parasoft-begin-suppress AUTOSAR-A12_1_6-a "False positive: Constructors for mdspan have different constraints"
// parasoft-begin-suppress AUTOSAR-A10_1_1-a "Multiple inheritance is private and used to allow for
// empty-base-optimization"
// parasoft-begin-suppress AUTOSAR-A12_1_5-a "False positive: Delegating constructors are used where they can be"
/// @brief A multidimensional view of elements over a contiguous sequence of elements
/// @tparam ElementType the element type
/// @tparam Extents an instantiation of @c extents representing the shape of the multidimensional index space
/// @tparam LayoutPolicy a layout policy (defaults to @c layout_right)
/// @tparam AccessorPolicy an accessor policy (defaults to @c default_accessor<ElementType>)
///
/// Backport of @c std::mdspan from C++23. The @c mdspan class template is a multidimensional array view that maps a
/// multidimensional index to an element of the array. The mapping and element access policies are configurable, and the
/// underlying array need not be contiguous or even exist in memory at all.
template <
    typename ElementType,
    typename Extents,
    typename LayoutPolicy = layout_right,
    typename AccessorPolicy = default_accessor<ElementType>>
// NOLINTNEXTLINE(hicpp-special-member-functions) Defines all constructors specified by the standard.
class mdspan
    : private AccessorPolicy
    , private LayoutPolicy::template mapping<Extents> {
  static_assert(std::is_object<ElementType>::value, "ElementType must be a complete object type");
  static_assert(!std::is_abstract<ElementType>::value, "ElementType must not be an abstract class");
  static_assert(!std::is_array<ElementType>::value, "ElementType must not be an array");
  static_assert(is_extents_v<Extents>, "Extents type parameter must be a specialization of arene::base::extents");
  static_assert(is_accessor_policy_v<AccessorPolicy>, "AccessorPolicy must meet the accessor policy requirements");
  static_assert(
      std::is_same<ElementType, typename AccessorPolicy::element_type>::value,
      "ElementType must match AccessorPolicy::element_type"
  );
  static_assert(
      is_layout_mapping_policy_v<LayoutPolicy>,
      "LayoutPolicy must meet the layout mapping policy requirements"
  );

 public:
  /// @brief The extents type
  using extents_type = Extents;
  /// @brief The layout policy type
  using layout_type = LayoutPolicy;
  /// @brief The accessor policy type
  using accessor_type = AccessorPolicy;
  /// @brief The mapping type
  using mapping_type = typename layout_type::template mapping<extents_type>;
  /// @brief The element type
  using element_type = ElementType;
  /// @brief The value type
  using value_type = std::remove_cv_t<element_type>;

  /// @brief The index type
  using index_type = typename extents_type::index_type;
  /// @brief The size type
  using size_type = typename extents_type::size_type;
  /// @brief The rank type
  using rank_type = typename extents_type::rank_type;
  /// @brief The data handle type
  using data_handle_type = typename accessor_type::data_handle_type;
  /// @brief The reference type
  using reference = typename accessor_type::reference;

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: these are not 'member data', they are a public
  // property"
  /// @brief Get the number of dimensions
  /// @return The rank of the mdspan
  static constexpr std::integral_constant<rank_type, Extents::rank()> rank{};

  /// @brief Get the number of dimensions with dynamic extent
  /// @return The dynamic rank of the mdspan
  static constexpr std::integral_constant<rank_type, Extents::rank_dynamic()> rank_dynamic{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  // parasoft-begin-suppress AUTOSAR-A10_2_1-a "False positive: Extents is inherited privately by the mapping_type, so
  // it is not redefined here."
  /// @brief Get the static extent for the given dimension
  /// @param dimension The index of the dimension for which to get the static extent
  /// @return The value from @c Extents for the specified dimension
  static constexpr auto static_extent(rank_type dimension) noexcept -> std::size_t {
    return extents_type::static_extent(dimension);
  }

  /// @brief Get the actual extent for the given dimension, returning the extent supplied to the constructor for dynamic
  /// extents
  /// @param dimension The index of the dimension for which to get the extent
  /// @return The extent of the specified dimension
  constexpr auto extent(rank_type dimension) const noexcept -> index_type { return this->extents().extent(dimension); }
  // parasoft-end-suppress AUTOSAR-A10_2_1-a

  /// @brief Default constructor
  ///
  /// @pre <c>[0, mapping().required_span_size())</c> is an accessible range of @c data_handle() and @c accessor() for
  /// the values of @c mapping() and @c accessor() after the invocation of this constructor. This precondition is not
  /// checked, but violating it is undefined behavior.
  ///
  /// @note This constructor has no visible effect and creates an mdspan with all dynamic
  /// extents equal to zero. This implies that a default constructed span covers no elements.
  template <
      typename Ext = Extents,
      typename Map = mapping_type,
      typename Acc = AccessorPolicy,
      constraints<
          std::enable_if_t<(Ext::rank_dynamic() > 0)>,
          std::enable_if_t<std::is_default_constructible<typename Acc::data_handle_type>::value>,
          std::enable_if_t<std::is_default_constructible<Map>::value>,
          std::enable_if_t<std::is_default_constructible<Acc>::value>> = nullptr>
  constexpr mdspan() noexcept
      : accessor_type{},
        mapping_type{},
        ptr_{} {}

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: Delegates to constructor which does initialize the base
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: All parameters *are* named."

  /// @brief Construct mdspan with a data handle and extents
  /// @tparam OtherIndexTypes The types of the extents used to initialize the layout mapping
  /// @param handle A user supplied data handle
  /// @param extents_in User supplied extents
  ///
  /// Construct an @c mdspan initialized with a user supplied @c data_handle_type, a variadic list of extents which are
  /// converted to @c extents_type and used to initialize the layout mapping, and a default constructed @c
  /// accessor_type.
  ///
  /// @pre <c> [0, mapping().required_span_size()) </c> is an accessible range of @c handle and @c accessor() for the
  /// values of @c mapping() and @c accessor() after the invocation of this constructor. This precondition is not
  /// checked, but violating it is undefined behavior.
  template <
      class... OtherIndexTypes,
      typename Ext = Extents,
      typename Map = mapping_type,
      typename Acc = AccessorPolicy,
      constraints<
          std::enable_if_t<all_of_v<std::is_convertible<OtherIndexTypes, index_type>::value...>>,
          std::enable_if_t<all_of_v<std::is_nothrow_constructible<index_type, OtherIndexTypes>::value...>>,
          std::enable_if_t<sizeof...(OtherIndexTypes) == rank() || sizeof...(OtherIndexTypes) == rank_dynamic()>,
          std::enable_if_t<std::is_constructible<Map, Ext>::value>,
          std::enable_if_t<std::is_default_constructible<Acc>::value>> = nullptr>
  constexpr explicit mdspan(data_handle_type handle, OtherIndexTypes... extents_in) noexcept
      : mdspan{std::move(handle), extents_type{static_cast<index_type>(std::move(extents_in))...}} {}
  // parasoft-end-suppress CERT_C-EXP37-a

  // classes"
  /// @brief Construct mdspan with a data handle and extents
  /// @tparam OtherIndexType The type of the extent values used to initialize the layout mapping
  /// @param handle A user supplied data handle
  /// @param extents_in User supplied extents
  ///
  /// Construct an @c mdspan initialized with a user supplied @c data_handle_type, a span of extents which are
  /// converted to @c extents_type and used to initialize the layout mapping, and a default constructed  @c
  /// accessor_type.
  ///
  /// @pre <c> [0, mapping().required_span_size()) </c> is an accessible range of @c handle and @c accessor() for the
  /// values of @c mapping() and @c accessor() after the invocation of this constructor. This precondition is not
  /// checked, but violating it is undefined behavior.
  ///
  /// @note This constructor is @c explicit only if <c> N != rank_dynamic() </c>.
  template <
      class OtherIndexType,
      std::size_t Size,
      typename Ext = Extents,
      typename Map = mapping_type,
      typename Acc = AccessorPolicy,
      constraints<
          std::enable_if_t<std::is_convertible<OtherIndexType const&, index_type>::value>,
          std::enable_if_t<std::is_nothrow_constructible<index_type, OtherIndexType const&>::value>,
          std::enable_if_t<Size == rank()>,
          std::enable_if_t<Size != rank_dynamic()>,
          std::enable_if_t<std::is_constructible<Map, Ext>::value>,
          std::enable_if_t<std::is_default_constructible<Acc>::value>> = nullptr>
  constexpr explicit mdspan(data_handle_type handle, span<OtherIndexType, Size> extents_in) noexcept
      : mdspan{std::move(handle), extents_type{extents_in}} {}

  /// @brief Construct mdspan with a data handle and extents
  /// @tparam OtherIndexType The type of the extent values used to initialize the layout mapping
  /// @param handle A user supplied data handle
  /// @param extents_in User supplied extents
  ///
  /// Construct an @c mdspan initialized with a user supplied @c data_handle_type, a span of extents which are
  /// converted to @c extents_type and used to initialize the layout mapping, and a default constructed  @c
  /// accessor_type.
  ///
  /// @pre <c> [0, mapping().required_span_size()) </c> is an accessible range of @c handle and @c accessor() for the
  /// values of @c mapping() and @c accessor after the invocation of this constructor. This precondition is not checked,
  /// but violating it is undefined behavior.
  ///
  /// @note This constructor is @c explicit only if <c> N != rank_dynamic() </c>.
  template <
      class OtherIndexType,
      typename Ext = Extents,
      typename Map = mapping_type,
      typename Acc = AccessorPolicy,
      constraints<
          std::enable_if_t<std::is_convertible<OtherIndexType const&, index_type>::value>,
          std::enable_if_t<std::is_nothrow_constructible<index_type, OtherIndexType const&>::value>,
          std::enable_if_t<std::is_constructible<Map, Ext>::value>,
          std::enable_if_t<std::is_default_constructible<Acc>::value>> = nullptr>
  constexpr mdspan(data_handle_type handle, span<OtherIndexType, rank_dynamic()> extents_in) noexcept
      : mdspan{std::move(handle), extents_type{extents_in}} {}

  /// @brief Construct mdspan with a data handle and extents
  /// @tparam OtherIndexType The type of the extent values used to initialize the layout mapping
  /// @param handle A user supplied data handle
  /// @param extents_in User supplied extents
  ///
  /// Construct an @c mdspan initialized with a user supplied @c data_handle_type, an array of extents which are
  /// converted to @c extents_type and used to initialize the layout mapping, and a default constructed @c
  /// accessor_type.
  ///
  /// @pre <c> [0, mapping().required_span_size()) </c> is an accessible range of @c handle and @c accessor() for the
  /// values of @c mapping() and @c accessor after the invocation of this constructor. This precondition is not checked,
  /// but violating it is undefined behavior.
  ///
  /// @note This constructor is @c explicit only if <c> N != rank_dynamic() </c>.
  template <
      class OtherIndexType,
      std::size_t Size,
      typename Ext = Extents,
      typename Map = mapping_type,
      typename Acc = AccessorPolicy,
      constraints<
          std::enable_if_t<std::is_convertible<OtherIndexType const&, index_type>::value>,
          std::enable_if_t<std::is_nothrow_constructible<index_type, OtherIndexType const&>::value>,
          std::enable_if_t<Size == rank()>,
          std::enable_if_t<Size != rank_dynamic()>,
          std::enable_if_t<std::is_constructible<Map, Ext>::value>,
          std::enable_if_t<std::is_default_constructible<Acc>::value>> = nullptr>
  constexpr explicit mdspan(data_handle_type handle, array<OtherIndexType, Size> const& extents_in) noexcept
      : mdspan{std::move(handle), arene::base::span<OtherIndexType const, Size>{extents_in}} {}

  /// @brief Construct mdspan with a data handle and extents
  /// @tparam OtherIndexType The type of the extent values used to initialize the layout mapping
  /// @param handle A user supplied data handle
  /// @param extents_in User supplied extents
  ///
  /// Construct an @c mdspan initialized with a user supplied @c data_handle_type, an array of extents which are
  /// converted to @c extents_type and used to initialize the layout mapping, and a default constructed @c
  /// accessor_type.
  ///
  /// @pre <c> [0, mapping().required_span_size()) </c> is an accessible range of @c handle and @c accessor() for the
  /// values of @c mapping() and @c accessor after the invocation of this constructor. This precondition is not checked,
  /// but violating it is undefined behavior.
  ///
  /// @note This constructor is @c explicit only if <c> N != rank_dynamic() </c>.
  template <
      class OtherIndexType,
      typename Ext = Extents,
      typename Map = mapping_type,
      typename Acc = AccessorPolicy,
      constraints<
          std::enable_if_t<std::is_convertible<OtherIndexType const&, index_type>::value>,
          std::enable_if_t<std::is_nothrow_constructible<index_type, OtherIndexType const&>::value>,
          std::enable_if_t<std::is_constructible<Map, Ext>::value>,
          std::enable_if_t<std::is_default_constructible<Acc>::value>> = nullptr>
  constexpr mdspan(data_handle_type handle, array<OtherIndexType, rank_dynamic()> const& extents_in) noexcept
      : mdspan{std::move(handle), arene::base::span<OtherIndexType const, rank_dynamic()>{extents_in}} {}

  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  /// @brief Construct mdspan with a data handle and extents
  /// @param handle A user supplied data handle
  /// @param extents_in User supplied extents
  ///
  /// @pre <c>[0, mapping().required_span_size())</c> is an accessible range of @c handle and @c accessor() for
  /// the values of @c mapping() and @c accessor() after the invocation of this constructor. This precondition is not
  /// checked, but violating it is undefined behavior.
  ///
  /// Construct an @c mdspan initialized with a user supplied @c data_handle_type, a user supplied @c extents_type used
  /// to initialize the layout mapping, and a default constructed @c accessor_type.
  template <
      typename Ext = Extents,
      typename Map = mapping_type,
      typename Acc = AccessorPolicy,
      constraints<
          std::enable_if_t<std::is_constructible<Map, Ext const&>::value>,
          std::enable_if_t<std::is_default_constructible<Acc>::value>> = nullptr>
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: Parameter 'extents_in' is used"
  constexpr mdspan(data_handle_type handle, extents_type const& extents_in) noexcept
      : accessor_type{},
        mapping_type{extents_in},
        ptr_{std::move(handle)} {}
  // parasoft-end-suppress AUTOSAR-A0_1_4-a

  /// @brief Construct mdspan with a data handle and mapping
  /// @param handle A user supplied data handle
  /// @param mapping_in User supplied mapping
  ///
  /// @pre <c>[0, mapping_in.required_span_size())</c> is an accessible range of @c handle and @c accessor() for
  /// the value of @c accessor() after the invocation of this constructor. This precondition is not checked, but
  /// violating it is undefined behavior.
  ///
  /// Construct an @c mdspan initialized with a user supplied @c data_handle_type, a user supplied @c mapping, and a
  /// default constructed @c accessor_type.
  template <
      typename Acc = AccessorPolicy,
      constraints<std::enable_if_t<std::is_default_constructible<Acc>::value>> = nullptr>
  constexpr mdspan(data_handle_type handle, mapping_type const& mapping_in) noexcept
      : accessor_type{},
        mapping_type{mapping_in},
        ptr_{std::move(handle)} {}

  /// @brief Construct mdspan with a data handle, mapping, and accessor
  /// @param handle A user supplied data handle
  /// @param mapping_in User supplied mapping
  /// @param accessor_in User supplied accessor
  ///
  /// @pre <c>[0, mapping_in.required_span_size())</c> is an accessible range of @c handle and @c accessor_in. This
  /// precondition is not checked, but violating it is undefined behavior.
  ///
  /// Construct an @c mdspan initialized with a user supplied @c data_handle_type, a user supplied @c mapping, and a
  /// user supplied @c accessor_type.
  constexpr mdspan(data_handle_type handle, mapping_type const& mapping_in, accessor_type const& accessor_in) noexcept
      : accessor_type{accessor_in},
        mapping_type{mapping_in},
        ptr_{std::move(handle)} {}

 private:
  /// @brief Check if the converting constructor should be explicit
  /// @tparam OtherExtents The extents type of the other @c mdspan
  /// @tparam OtherLayoutPolicy The layout policy of the other @c mdspan
  /// @tparam OtherAccessor The accessor policy of the other @c mdspan
  template <class OtherExtents, class OtherLayoutPolicy, class OtherAccessor>
  static constexpr bool converting_constructor_is_explicit_v{
      !std::is_convertible<typename OtherLayoutPolicy::template mapping<OtherExtents> const&, mapping_type>::value ||
      !std::is_convertible<OtherAccessor const&, accessor_type>::value
  };

 public:
  /// @brief Converting constructor from another @c mdspan
  /// @tparam OtherElementType The element type of the other @c mdspan
  /// @tparam OtherExtents The extents type of the other @c mdspan
  /// @tparam OtherLayoutPolicy The layout policy of the other @c mdspan
  /// @tparam OtherAccessor The accessor policy of the other @c mdspan
  /// @param other The other mdspan to construct from
  ///
  /// @pre For each rank index @c r of @c extents_type, <c> static_extent(r) == dynamic_extent || static_extent(r) ==
  /// other.extent(r) </c> is true, otherwise this is an @c ARENE_PRECONDITION violation.
  /// @pre <c>[0, mapping().required_span_size())</c> is an accessible range of @c handle() and @c accessor() for values
  /// of handle(), mapping(), and accessor() after the invocation of this constructor. This precondition is not checked,
  /// but violating it is undefined behavior.
  ///
  /// @note This constructor is @c explicit only if <c> !std::is_convertible<typename OtherLayoutPolicy::template
  /// mapping<OtherExtents> const&, mapping_type>::value || !std::is_convertible<OtherAccessor const&,
  /// accessor_type>::value; </c>
  template <
      class OtherElementType,
      class OtherExtents,
      class OtherLayoutPolicy,
      class OtherAccessor,
      constraints<
          std::enable_if_t<converting_constructor_is_explicit_v<OtherExtents, OtherLayoutPolicy, OtherAccessor>>,
          std::enable_if_t<std::is_constructible<
              mapping_type,
              typename OtherLayoutPolicy::template mapping<OtherExtents> const&>::value>,
          std::enable_if_t<std::is_constructible<accessor_type, OtherAccessor const&>::value>,
          std::enable_if_t<
              std::is_constructible<data_handle_type, typename OtherAccessor::data_handle_type const&>::value>,
          std::enable_if_t<std::is_constructible<extents_type, OtherExtents const&>::value>> = nullptr>
  explicit constexpr mdspan(mdspan<OtherElementType, OtherExtents, OtherLayoutPolicy, OtherAccessor> const& other)
      : accessor_type{other.accessor()},
        mapping_type{other.mapping()},
        ptr_{other.data_handle()} {
    mdspan_detail::validate_extents_compatibility<extents_type>(other.extents());
  }

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: Delegates to constructor which does construct base
  // classes"
  /// @brief Converting constructor from another @c mdspan
  /// @tparam OtherElementType The element type of the other @c mdspan
  /// @tparam OtherExtents The extents type of the other @c mdspan
  /// @tparam OtherLayoutPolicy The layout policy of the other @c mdspan
  /// @tparam OtherAccessor The accessor policy of the other @c mdspan
  /// @param other The other mdspan to construct from
  ///
  /// @pre For each rank index @c r of @c extents_type, <c> static_extent(r) == dynamic_extent || static_extent(r) ==
  /// other.extent(r) </c> is true, otherwise this is an @c ARENE_PRECONDITION violation.
  /// @pre <c>[0, mapping().required_span_size())</c> is an accessible range of @c handle() and @c accessor() for
  /// values of handle(), mapping(), and accessor() after the invocation of this constructor. This precondition is not
  /// checked, but violating it is undefined behavior.
  ///
  /// @note This constructor is @c explicit only if <c> !std::is_convertible<typename OtherLayoutPolicy::template
  /// mapping<OtherExtents> const&, mapping_type>::value || !std::is_convertible<OtherAccessor const&,
  /// accessor_type>::value; </c>
  template <
      class OtherElementType,
      class OtherExtents,
      class OtherLayoutPolicy,
      class OtherAccessor,
      constraints<
          std::enable_if_t<!converting_constructor_is_explicit_v<OtherExtents, OtherLayoutPolicy, OtherAccessor>>,
          std::enable_if_t<std::is_constructible<
              mapping_type,
              typename OtherLayoutPolicy::template mapping<OtherExtents> const&>::value>,
          std::enable_if_t<std::is_constructible<accessor_type, OtherAccessor const&>::value>,
          std::enable_if_t<
              std::is_constructible<data_handle_type, typename OtherAccessor::data_handle_type const&>::value>,
          std::enable_if_t<std::is_constructible<extents_type, OtherExtents const&>::value>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr mdspan(mdspan<OtherElementType, OtherExtents, OtherLayoutPolicy, OtherAccessor> const& other)
      : accessor_type{other.accessor()},
        mapping_type{other.mapping()},
        ptr_{other.data_handle()} {
    mdspan_detail::validate_extents_compatibility<extents_type>(other.extents());
  }

  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  /// @brief Get the size of the multidimensional index space
  /// @return The size of the multidimensional index space
  constexpr auto size() const noexcept -> size_type {
    return static_cast<size_type>(
        layout_detail::extent_product(this->extents(), {static_cast<rank_type>(0), rank()}).value
    );
  }

  /// @brief Check if the size is zero
  /// @return @c true if the size is zero, @c false otherwise
  constexpr auto empty() const noexcept -> bool { return this->size() == static_cast<size_type>(0); }

  /// @brief Get the extents object
  /// @return A reference to the extents object
  using mapping_type::extents;

  // parasoft-begin-suppress AUTOSAR-M9_3_1-a "False positive: Returns a const reference to member data."
  // parasoft-begin-suppress AUTOSAR-A9_3_1-a "False positive: Returns a const reference to member data."
  /// @brief Get the data handle
  /// @return The data handle
  constexpr auto data_handle() const noexcept -> data_handle_type const& { return ptr_; }
  // parasoft-end-suppress AUTOSAR-M9_3_1-a
  // parasoft-end-suppress AUTOSAR-A9_3_1-a

  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: No identifiers are hidden by this function"
  /// @brief Get the mapping
  /// @return A reference to the mapping object
  constexpr auto mapping() const noexcept -> mapping_type const& { return *this; }
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  /// @brief Get the accessor
  /// @return A reference to the accessor object
  constexpr auto accessor() const noexcept -> accessor_type const& { return *this; }

  /// @brief Access an element at the specified multi-dimensional index
  /// @tparam OtherIndexTypes Non-type template parameter pack of indices
  /// @param indices The multi-dimensional index as a variadic
  /// @return reference to the indices-th element of the mdspan
  template <
      class... OtherIndexTypes,
      constraints<
          std::enable_if_t<std::is_convertible<OtherIndexTypes, index_type>::value>...,
          std::enable_if_t<std::is_nothrow_constructible<index_type, OtherIndexTypes>::value>...,
          std::enable_if_t<sizeof...(OtherIndexTypes) == extents_type::rank>> = nullptr>
  constexpr auto operator()(OtherIndexTypes... indices) const  //
      noexcept(arene::base::is_nothrow_invocable_v<mapping_type const&, OtherIndexTypes&&...>&& noexcept(
          std::declval<accessor_type const&>().access(  //
              std::declval<data_handle_type const&>(),  //
              std::declval<std::size_t>()
          )
      ))            //
      -> reference  //
  {
    return accessor().access(
        data_handle(),
        mdspan_detail::representable_cast<std::size_t>(mapping()(std::move(indices)...))
    );
  }

  /// @brief Access an element with an @c array of indices
  /// @tparam OtherIndexType Index type contained by the array
  /// @param indices The multi-dimensional index as an @c array
  /// @return reference to the indices-th element of the mdspan
  template <
      class OtherIndexType,
      constraints<
          std::enable_if_t<std::is_convertible<OtherIndexType const&, index_type>::value>,
          std::enable_if_t<std::is_nothrow_constructible<index_type, OtherIndexType const&>::value>> = nullptr>
  constexpr auto operator()(array<OtherIndexType, extents_type::rank> const& indices) const  //
      noexcept(arene::base::is_nothrow_invocable_v<
               decltype(apply),
               mdspan const&,
               array<OtherIndexType, extents_type::rank()> const&>)  //
      -> reference {
    return apply(*this, indices);
  }

  /// @brief Access an mdspan element with a @c span of indices
  /// @tparam OtherIndexType Index type contained by the span
  /// @param indices The multi-dimensional index as an @c span
  /// @return reference to the indices-th element of the mdspan
  template <
      class OtherIndexType,
      constraints<
          std::enable_if_t<std::is_convertible<OtherIndexType const&, index_type>::value>,
          std::enable_if_t<std::is_nothrow_constructible<index_type, OtherIndexType const&>::value>> = nullptr>
  constexpr auto operator()(span<OtherIndexType, extents_type::rank> const indices) const  //
      noexcept(arene::base::is_nothrow_invocable_v<
               decltype(apply),
               mdspan const&,
               mdspan_detail::tuple_span<OtherIndexType, extents_type::rank> const&>)  //
      -> reference {
    return apply(*this, mdspan_detail::tuple_span<OtherIndexType, extents_type::rank>(indices));
  }

  /// @brief Return whether or not this mdspan's mapping is always unique, i.e. distinct indices always map to distinct
  /// elements
  /// @return @c true because this is enforced by the constructors as a precondition
  /// @note In mathematical terms this asks if the mapping is always injective/one-to-one.
  using mapping_type::is_always_unique;

  /// @brief Return whether or not this mdspan's mapping is always exhaustive, i.e. every element is reachable using
  /// some indices
  /// @return @c false because it's possible to construct non-exhaustive mappings using this class
  /// @note In mathematical terms this asks if the mapping is always surjective/onto.
  using mapping_type::is_always_exhaustive;

  /// @brief Return whether or not this mdspan's mapping is always strided, i.e. every dimension has a constant stride
  /// @return @c true because this class directly uses striding to compute its mapped indices
  /// @note In mathematical terms this asks if the mapping is always affine (isomorphic to a dot product plus offset).
  using mapping_type::is_always_strided;

  /// @brief Return whether or not this instance is unique, i.e. distinct indices always map to distinct elements
  /// @return Always @c true because this is enforced by the constructors as a precondition
  /// @note In mathematical terms this asks if the mapping is injective/one-to-one.
  using mapping_type::is_unique;

  /// @brief Return whether or not this instance is exhaustive, i.e. every element is reachable using some indices
  /// @return @c true if every provided stride is the product of the extents corresponding to some subset of the other
  /// strides, otherwise @c false
  /// @note In mathematical terms this asks if the mapping is surjective/onto.
  using mapping_type::is_exhaustive;

  /// @brief Return whether or not this instance is strided, i.e. every dimension has a constant stride
  /// @return Always @c true because this class directly uses striding to compute its mapped indices
  /// @note In mathematical terms this asks if the mapping is affine (isomorphic to a dot product plus an offset).
  using mapping_type::is_strided;

  /// @brief Get the stride of a particular rank in this @c mapping
  /// @param dimension The index of the dimension to return the stride of
  /// @return The stride of the selected rank
  /// @pre <c>dimension < extents_type::rank()</c>
  using mapping_type::stride;

  // parasoft-begin-suppress AUTOSAR-A7_1_1-a "Declaring 'lhs' or 'rhs' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-M7_1_2-c "Declaring 'lhs' or 'rhs' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-A8_4_9-a "Declaring 'lhs' or 'rhs' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Friend swap operators permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: This is a public function, and doesn't need to be used
  // in the translation unit"

  /// @brief swaps the contents
  /// @param lhs left @c mdspan value to swap
  /// @param rhs right @c mdspan value to swap
  friend constexpr auto swap(mdspan& lhs, mdspan& rhs) noexcept -> void {
    arene::base::swap(lhs.ptr_, rhs.ptr_);
    arene::base::swap(static_cast<mapping_type&>(lhs), static_cast<mapping_type&>(rhs));
    arene::base::swap(static_cast<accessor_type&>(lhs), static_cast<accessor_type&>(rhs));
  }

  // parasoft-end-suppress AUTOSAR-A7_1_1-a
  // parasoft-end-suppress AUTOSAR-M7_1_2-c
  // parasoft-end-suppress AUTOSAR-A8_4_9-a
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
  // parasoft-end-suppress AUTOSAR-A0_1_3-a

 private:
  // parasoft-begin-suppress AUTOSAR-M14_6_1-a "False positive: The identifier data_handle_type is brought into this
  // class scope"
  /// @brief The data handle
  data_handle_type ptr_;
  // parasoft-end-suppress AUTOSAR-M14_6_1-a
};

// parasoft-end-suppress AUTOSAR-A12_1_5-a
// parasoft-end-suppress AUTOSAR-A10_1_1-a
// parasoft-end-suppress AUTOSAR-A12_1_6-a

/// @brief Get the number of dimensions in the extents
/// @tparam ElementType the element type
/// @tparam Extents an instantiation of @c extents representing the shape of the multidimensional index space
/// @tparam LayoutPolicy a layout policy (defaults to @c layout_right)
/// @tparam AccessorPolicy an accessor policy (defaults to @c default_accessor<ElementType>)
template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
constexpr std::
    integral_constant<typename mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>::rank_type, Extents::rank()>
        mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>::rank;

/// @brief Get the number of dimensions with dynamic extent
/// @tparam ElementType the element type
/// @tparam Extents an instantiation of @c extents representing the shape of the multidimensional index space
/// @tparam LayoutPolicy a layout policy (defaults to @c layout_right)
/// @tparam AccessorPolicy an accessor policy (defaults to @c default_accessor<ElementType>)
template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
constexpr std::integral_constant<
    typename mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>::rank_type,
    Extents::rank_dynamic()>
    mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>::rank_dynamic;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_MDSPAN_HPP_
