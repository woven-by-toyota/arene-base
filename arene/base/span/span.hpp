// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SPAN_SPAN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SPAN_SPAN_HPP_

// IWYU pragma: private, include "arene/base/span.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// AUTOSAR exceptions:
// A11-3-1: Friend declarations shall not be used
// The span::iterator type declares the outer span type to be a friend so that
// the constructor can be private, preventing user code constructing iterators
// with pointers outside the span.
// The span::iterator non-member operator overloads are declared to be friends
// to allow them to be defined within the class. This ensures that they are only
// considered for overload resolution and name lookup when at least one of the
// parameters is an iterator, and avoids the namespace-scope template overloads
// that would otherwise be required.
//
// M5-0-15: Array indexing shall be the only form of pointer arithmetic.
// span inherently refers to an array of elements. Pointer arithmetic is only
// used to adjust the pointer within that array, as covered by M5-0-16 and
// M5-0-17, unless iterators are incremented unchecked.
//
// A5-0-4: Pointer arithmetic shall not be used with pointers to non-final
// classes.
// The span constructors do not allow constructing a span<Base> from a
// Derived*, so pointer arithmetic is safe unless the user does an explicit
// cast.

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers"
#include "arene/base/byte/byte.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/detail/wrapped_iterator.hpp"  // IWYU pragma: keep
#include "arene/base/iterator/next.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_const.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/type_traits/is_array_convertible.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {

// IWYU pragma: begin_keep
template <typename T, std::size_t Extent = dynamic_extent>
class span;
// IWYU pragma: end_keep

namespace span_detail {

/// @brief Helper function for gcc8 to get @c dynamic_extent
/// @return The value of @c dynamic_extent
constexpr auto get_dynamic_extent() noexcept -> std::size_t { return dynamic_extent; }

// parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Passkey idiom permitted by A11-3-1 Permit #1"
/// @brief A passkey type to restrict constructing span's iterator to span.
class span_iter_passkey {
 private:
  /// @brief Default-ctor is explicit to prevent construction via {}.
  explicit span_iter_passkey() noexcept = default;
  template <typename T, std::size_t Extent>
  friend class ::arene::base::span;
};
// parasoft-end-suppress AUTOSAR-A11_3_1-a-2

/// @brief Validate the extent of a source span given a target span
/// @param my_extent The extent of the source span
/// @param target_extent The extent of the target span
/// @return @c true If @c my_extent==dynamic_extent or if @c my_extent==target_extent .
///         @c false Otherwise.
constexpr auto is_valid_source_extent(std::size_t const my_extent, std::size_t const target_extent) noexcept -> bool {
  return (my_extent == dynamic_extent) || (my_extent == target_extent);
}

/// @brief Validate the size of a dynamic span
/// @param my_extent The extent of the source span
/// @param target_size The target size to check.
/// @param max_size The maximum size to allow.
/// @return @c true If @c is_valid_source_extent(my_extent,target_size) and @c target_size<=max_size .
///         @c false Otherwise.
constexpr auto
is_valid_runtime_size(std::size_t const my_extent, std::size_t const target_size, std::size_t const max_size) noexcept
    -> bool {
  return is_valid_source_extent(my_extent, target_size) && (target_size <= max_size);
}

/// @brief Check if an extent is dynamic
/// @param my_extent The extent to check
/// @return @c true If @c my_extent is @c dynamic_extent
///         @c false Otherwise.
constexpr auto is_dynamic_extent(std::size_t const my_extent) noexcept -> bool { return my_extent == dynamic_extent; }

/// @brief Computes the extent for a subspan of an original span with a given extent.
/// Specialization for @c Count!=dynamic_extent .
/// @tparam Extent The extent of the original span.
/// @tparam Offset The offset into the original span to slice.
/// @tparam Count The number of elements in the subspan.
/// @return std::size_t @c Count
template <
    std::size_t Extent,
    std::size_t Offset,
    std::size_t Count,
    constraints<std::enable_if_t<!is_dynamic_extent(Count)>> = nullptr>
constexpr auto extent_for_subspan() -> std::size_t {
  return Count;
}
/// @brief Computes the extent for a subspan of an original span with a given extent.
/// Specialization for @c Count==dynamic_extent and @c Extent!=dynamic_extent.
/// @tparam Extent The extent of the original span.
/// @tparam Offset The offset into the original span to slice.
/// @tparam Count The number of elements in the subspan.
/// @return std::size_t @c Extent-Offset .
template <
    std::size_t Extent,
    std::size_t Offset,
    std::size_t Count,
    constraints<std::enable_if_t<!is_dynamic_extent(Extent)>, std::enable_if_t<is_dynamic_extent(Count)>> = nullptr>
constexpr auto extent_for_subspan() -> std::size_t {
  return Extent - Offset;
}
/// @brief Computes the extent for a subspan of an original span with a given extent.
/// Specialization for @c Count==dynamic_extent and @c Extent==dynamic_extent.
/// @tparam Extent The extent of the original span.
/// @tparam Offset The offset into the original span to slice.
/// @tparam Count The number of elements in the subspan.
/// @return std::size_t @c dynamic_extent .
template <
    std::size_t Extent,
    std::size_t Offset,
    std::size_t Count,
    constraints<std::enable_if_t<is_dynamic_extent(Extent)>, std::enable_if_t<is_dynamic_extent(Count)>> = nullptr>
constexpr auto extent_for_subspan() -> std::size_t {
  return dynamic_extent;
}

/// @brief Trait to determine if a type has a member function @c data()
/// @tparam T The type to test against
template <typename T>
using use_member_data = decltype(std::declval<T>().data());

/// @brief Trait to determine if a type has a member function @c data()
/// @tparam T The type to test against
/// @return true if @c T has a member function @c data(), false otherwise
template <typename T>
constexpr bool has_member_data_v = substitution_succeeds<use_member_data, T>;

/// @brief Trait to determine if a type has a @c std::tuple_size specialization.
/// @tparam T The type to test against
template <typename T>
using use_tuple_size = decltype(std::tuple_size<T>::value);

/// @brief Trait to determine if a type has a @c std::tuple_size specialization.
/// @tparam T The type to test against
/// @return true if @c T has a @c std::tuple_size specialization, false otherwise
template <typename T>
constexpr bool has_tuple_size_v = substitution_succeeds<use_tuple_size, T>;

/// @brief A constant that is true if and only if T is an instantiation of arene::base::span
template <typename T>
extern constexpr bool is_span = false;

/// @brief A constant that is true if and only if T is an instantiation of arene::base::Span
template <typename T, std::size_t Extent>
extern constexpr bool is_span<span<T, Extent>> = true;

/// @brief Special tag type to use for constructing Spans
struct span_construction_tag {};

/// @brief A base class for Span to prevent default construction for fixed-size non-empty Spans
template <std::size_t Extent>
class span_base_fixed_size_non_empty {
 public:
  /// @brief Construct with a given run-time size
  constexpr explicit span_base_fixed_size_non_empty(span_construction_tag, std::size_t) noexcept {}

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property"
  /// @brief Returns the number of elements in the span
  /// The size is fixed to the Extent
  /// @return The number of elements in the span
  static constexpr std::integral_constant<std::size_t, Extent> size{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

 protected:
  /// @brief protected destructor
  ~span_base_fixed_size_non_empty() = default;
  /// @brief protected copy constructor
  /// @param other The source object
  span_base_fixed_size_non_empty(span_base_fixed_size_non_empty const& other) = default;
  /// @brief protected move constructor
  /// @param other The source object
  span_base_fixed_size_non_empty(span_base_fixed_size_non_empty&& other) noexcept = default;
  /// @brief protected copy assignment
  /// @param other The source object
  auto operator=(span_base_fixed_size_non_empty const& other) & -> span_base_fixed_size_non_empty& = default;
  /// @brief protected move assignment
  /// @param other The source object
  auto operator=(span_base_fixed_size_non_empty&& other) & noexcept -> span_base_fixed_size_non_empty& = default;
};

template <std::size_t Extent>
constexpr std::integral_constant<std::size_t, Extent> span_base_fixed_size_non_empty<Extent>::size;

/// @brief A base class for Span for a fixed-sized Span with an Extent of 0
class span_base_fixed_size_empty {
 public:
  /// @brief The default constructor is trivial
  constexpr span_base_fixed_size_empty() noexcept = default;

  /// @brief Construct with a given run-time size
  constexpr explicit span_base_fixed_size_empty(span_construction_tag, std::size_t) noexcept {}

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property"
  /// @brief Returns the number of elements in the span
  /// The size is fixed at zero
  /// @return 0
  static constexpr std::integral_constant<std::size_t, 0> size{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

 protected:
  /// @brief protected destructor
  ~span_base_fixed_size_empty() = default;
  /// @brief protected copy constructor
  /// @param other The source object
  span_base_fixed_size_empty(span_base_fixed_size_empty const& other) = default;
  /// @brief protected move constructor
  /// @param other The source object
  span_base_fixed_size_empty(span_base_fixed_size_empty&& other) = default;
  /// @brief protected copy assignment
  /// @param other The source object
  auto operator=(span_base_fixed_size_empty const& other) & -> span_base_fixed_size_empty& = default;
  /// @brief protected move assignment
  /// @param other The source object
  auto operator=(span_base_fixed_size_empty&& other) & -> span_base_fixed_size_empty& = default;
};

/// @brief A base class for Span for a dynamically-sized span
class span_base_dynamic_size {
 protected:
  /// @brief The default constructor sets the size to 0
  constexpr span_base_dynamic_size() noexcept = default;
  /// @brief Construct with a given run-time size
  /// @param span_size The size of the span
  constexpr explicit span_base_dynamic_size(span_construction_tag, std::size_t const span_size) noexcept
      : dynamic_size_(span_size) {}

  /// @brief Returns the number of elements in the span
  /// @return The number of elements in the span
  ARENE_NODISCARD constexpr auto size() const noexcept -> std::size_t { return this->dynamic_size_; }

  /// @brief protected destructor
  ~span_base_dynamic_size() = default;
  /// @brief protected copy constructor
  /// @param other The source object
  span_base_dynamic_size(span_base_dynamic_size const& other) = default;
  /// @brief protected move constructor
  /// @param other The source object
  span_base_dynamic_size(span_base_dynamic_size&& other) = default;
  /// @brief protected copy assignment
  /// @param other The source object
  auto operator=(span_base_dynamic_size const& other) & -> span_base_dynamic_size& = default;
  /// @brief protected move assignment
  /// @param other The source object
  auto operator=(span_base_dynamic_size&& other) & -> span_base_dynamic_size& = default;

 private:
  /// @brief The size of the span
  std::size_t dynamic_size_{0U};
};

/// @brief Select instantiation of span_base with/without default construction
template <std::size_t Extent>
using span_base = typename std::conditional<
    is_dynamic_extent(Extent),
    span_base_dynamic_size,
    typename std::conditional<Extent == 0, span_base_fixed_size_empty, span_base_fixed_size_non_empty<Extent>>::type>::
    type;

}  // namespace span_detail

// parasoft-begin-suppress AUTOSAR-A13_5_1-a-2 "False positive: no non-const overload"
/// @brief A backport of @c std::span for C++14. Provides a view over a contiguous range of elements of type T.
///
/// @tparam T The type of the element the span views
/// @tparam Extent The size of the span. If set to @c arene::base::dynamic_extent , then the Size of the range is
///         determined at runtime. Otherwise the range of elements is exactly @c Extent elements.
template <typename T, std::size_t Extent>
class span : span_detail::span_base<Extent> {
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: These identifiers do not hide anything"
  /// @brief The base class for default construction
  using base = span_detail::span_base<Extent>;

  // parasoft-begin-suppress AUTOSAR-M0_1_3-c "False positive: data_ptr_ and max_extent are both used"
  /// @brief The start of the data
  T* data_ptr_{nullptr};

  /// @brief The max extent of the span
  static constexpr std::size_t max_extent{
      static_cast<std::size_t>(static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()) / sizeof(T))
  };
  // parasoft-end-suppress AUTOSAR-M0_1_3-c

  /// @brief Check if Span is dynamic size or Extent is valid for fixed size span
  static_assert((Extent == dynamic_extent) || Extent <= max_extent, "span extent exceeds its max value");

  /// @brief Validate the extent of a source span given the target span
  /// @tparam target_extent The extent of the source span
  /// @return bool Equivalent to @c span_detail::is_valid_source_extent(Extent,TargetExtent) .
  template <std::size_t TargetExtent>
  static constexpr bool is_valid_source_extent_v{span_detail::is_valid_source_extent(Extent, TargetExtent)};

  /// @brief Validate the size of a dynamic span
  /// @param target_extent The size of the dynamic span
  /// @return @c true The size is valid
  ///         @c false The size is invalid
  static constexpr auto is_valid_runtime_size(std::size_t target_extent) noexcept -> bool {
    return span_detail::is_valid_runtime_size(Extent, target_extent, max_extent);
  }

  /// @brief Tag type for a the pre-checked size constructor
  class pre_checked_size {
   public:
    /// @brief Default construction requires naming the type
    constexpr explicit pre_checked_size() noexcept = default;
  };
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief Construct with the specified data and size.
  ///
  /// @param data_pointer The data from which to construct the span.
  /// @param data_size The size of the data span to construct.
  /// @post @c size()==data_size .
  /// @post @c data()=data_pointer .
  /// @pre @c data_pointer+data_size must be in the valid range of @c data_pointer , else behavior is undefined.
  /// @pre @c data_size must satisfy @c is_valid_runtime_size else @c ARENE_PRECONDITION violation.
  // NOLINTNEXTLINE(readability-non-const-parameter)
  constexpr span(pre_checked_size, T* data_pointer, std::size_t data_size) noexcept
      : base(span_detail::span_construction_tag{}, data_size),
        data_ptr_(data_pointer) {}

 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: these type aliases do not hide anything"
  // alias size from the base class into the public space.
  using base::size;

  /// @brief The type of the elements of the span
  using element_type = T;
  /// @brief The type of the elements of the span without @c const or @c volatile qualification
  using value_type = arene::base::remove_cvref_t<T>;
  /// @brief The type of the size of the span
  using size_type = std::size_t;
  /// @brief The type of pointers to elements of the span
  using pointer = T*;
  /// @brief The type of pointers to @c const elements of the span
  using const_pointer = T const*;
  /// @brief The type of references to elements of the span
  using reference = T&;
  /// @brief The type of references to @c const elements of the span
  using const_reference = T const&;
  /// @brief The type of an iterator into the data viewed by the span.
  using iterator = detail::wrapped_iterator<pointer, span_detail::span_iter_passkey>;
  /// @brief The type of an iterator into a const view of the data viewed by the span.
  using const_iterator = detail::wrapped_iterator<const_pointer, span_detail::span_iter_passkey>;
  /// @brief The type of an iterator into a reversed view of the data viewed by the span.
  using reverse_iterator = ::arene::base::reverse_iterator<iterator>;
  /// @brief The type of an iterator into a const and reversed view of the data viewed by the span.
  using const_reverse_iterator = ::arene::base::reverse_iterator<const_iterator>;
  /// @brief The type used for obtaining differences between iterators of the span.
  using difference_type = typename iterator::difference_type;

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property"
  /// @brief The Extent of the span --- either dynamic_extent, or the fixed size
  static constexpr std::integral_constant<std::size_t, Extent> extent{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief The size of the span as a signed integer
  /// @return The size
  ARENE_NODISCARD constexpr auto ssize() const noexcept -> difference_type {
    return static_cast<difference_type>(this->size());
  }

  /// @brief Default-constructs an empty span. Only exists if @c Extent==dynamic_extent or @c Extent==0
  /// @post @c size()==0 .
  constexpr span() noexcept = default;

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  /// @brief default copy constructor
  /// @param other The span being copied
  constexpr span(span const& other) = default;
  /// @brief default move constructor
  /// @param other The span being moved
  constexpr span(span&& other) = default;
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  /// @brief default copy assignment operator
  /// @param other The span being copied
  constexpr auto operator=(span const& other) -> span& = default;
  /// @brief default move assignment operator
  /// @param other The span being moved
  constexpr auto operator=(span&& other) -> span& = default;

  /// @brief default destructor
  ~span() = default;

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Construct with the specified data and size. Prevents implicit derived-to-base pointer conversions on the
  /// supplied data argument.
  ///
  /// @tparam U The type of the data to convert from.
  /// @param data_pointer The data from which to construct the span.
  /// @param data_size The size of the data span to construct.
  /// @post @c size()==data_size .
  /// @post @c data()=data_pointer .
  /// @pre @c data_pointer+data_size must be in the valid range of @c data_pointer , else behavior is undefined.
  /// @pre @c data_size must satisfy @c is_valid_runtime_size else @c ARENE_PRECONDITION violation.
  template <typename U, constraints<std::enable_if_t<arene::base::is_array_convertible_v<U, T>>> = nullptr>
  // NOLINTNEXTLINE(readability-non-const-parameter)
  constexpr span(U* data_pointer, std::size_t data_size) noexcept
      : span(pre_checked_size{}, data_pointer, data_size) {
    ARENE_PRECONDITION(is_valid_runtime_size(data_size));
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Constructor from a c-array.
  ///
  /// @tparam N The size of the c-array. Must satisfy @c is_valid_source_extent .
  /// @param array The array to construct the span from
  /// @post @c size()==N .
  /// @post @c data()==&array[0] .
  template <std::size_t N, constraints<std::enable_if_t<is_valid_source_extent_v<N>>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions,cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  constexpr span(T (&array)[N]) noexcept
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
      : span(pre_checked_size{}, array, N) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Constructor from a @c std::array.
  ///
  /// @tparam ArrayLikeT The template-template type of the array-like container. A container is array-like if it has a
  ///         member function @c data() and @c std::tuple_size<ArrayLikeType<U,N>> is defined.
  /// @tparam U The @c value_type of the array. Must satisfy @c is_array_convertible_v<U,T>
  /// @tparam N The size of the array, Must satisfy @c is_valid_source_extent
  /// @param array The array to construct the span from
  /// @post @c size()==N .
  /// @post @c data()==array.data() .
  template <
      template <typename U, std::size_t N>
      class ArrayLikeT,
      typename U,
      std::size_t N,
      constraints<
          std::enable_if_t<span_detail::has_member_data_v<ArrayLikeT<U, N>>>,
          std::enable_if_t<span_detail::has_tuple_size_v<ArrayLikeT<U, N>>>,
          std::enable_if_t<std::tuple_size<ArrayLikeT<U, N>>::value == N>,
          std::enable_if_t<arene::base::is_array_convertible_v<U, T>>,
          std::enable_if_t<is_valid_source_extent_v<N>>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr span(ArrayLikeT<U, N>& array) noexcept
      : span(pre_checked_size{}, array.data(), N) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Constructor from a const @c std::array -like type.
  ///
  /// @tparam ArrayLikeT The template-template type of the array-like container. A container is array-like if it has a
  ///         member function @c data() and @c std::tuple_size<ArrayLikeType<U,N>> is defined.
  /// @tparam U The @c value_type of the array. Must satisfy <c>is_array_convertible_v<const U, T> </c>
  /// @tparam N The size of the array, Must satisfy @c is_valid_source_extent
  /// @param array The array to construct the span from
  /// @post @c size()==N .
  /// @post @c data()==array.data() .
  template <
      template <typename U, std::size_t N>
      class ArrayLikeT,
      typename U,
      std::size_t N,
      constraints<
          std::enable_if_t<span_detail::has_member_data_v<ArrayLikeT<U, N>>>,
          std::enable_if_t<span_detail::has_tuple_size_v<ArrayLikeT<U, N>>>,
          std::enable_if_t<std::tuple_size<ArrayLikeT<U, N>>::value == N>,
          std::enable_if_t<std::is_const<T>::value && arene::base::is_array_convertible_v<U, T>>,
          std::enable_if_t<is_valid_source_extent_v<N>>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr span(ArrayLikeT<U, N> const& array) noexcept
      : span(pre_checked_size{}, array.data(), N) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "Span has reference semantics: forwarding not appropriate"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "Span has reference semantics: forwarding not appropriate"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "Span has reference semantics: forwarding not appropriate"
  /// @brief Constructor from an rvalue @c std::array -like type.
  ///
  /// @tparam ArrayLikeT The template-template type of the array-like container. A container is array-like if it has a
  ///         member function @c data() and @c std::tuple_size<ArrayLikeType<U,N>> is defined.
  /// @tparam U The @c value_type of the array. Must satisfy <c>is_array_convertible_v<const U, T> </c>
  /// @tparam N The size of the array, Must satisfy @c is_valid_source_extent
  /// @param array The array to construct the span from
  /// @post @c size()==N .
  /// @post @c data()==array.data() .
  template <
      template <typename U, std::size_t N>
      class ArrayLikeT,
      typename U,
      std::size_t N,
      constraints<
          std::enable_if_t<span_detail::has_member_data_v<ArrayLikeT<U, N>>>,
          std::enable_if_t<span_detail::has_tuple_size_v<ArrayLikeT<U, N>>>,
          std::enable_if_t<std::tuple_size<ArrayLikeT<U, N>>::value == N>,
          std::enable_if_t<arene::base::is_array_convertible_v<U, T>>,
          std::enable_if_t<is_valid_source_extent_v<N>>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr span(ArrayLikeT<U, N>&& array) noexcept
      : span(pre_checked_size{}, array.data(), N) {}
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Constructor from an const rvalue @c std::array -like type.
  ///
  /// @tparam ArrayLikeT The template-template type of the array-like container. A container is array-like if it has a
  ///         member function @c data() and @c std::tuple_size<ArrayLikeType<U,N>> is defined.
  /// @tparam U The @c value_type of the array. Must satisfy <c>is_array_convertible_v<const U, T> </c>
  /// @tparam N The size of the array, Must satisfy @c is_valid_source_extent
  /// @param array The array to construct the span from
  /// @post @c size()==N .
  /// @post @c data()==array.data() .
  template <
      template <typename U, std::size_t N>
      class ArrayLikeT,
      typename U,
      std::size_t N,
      constraints<
          std::enable_if_t<span_detail::has_member_data_v<ArrayLikeT<U, N>>>,
          std::enable_if_t<span_detail::has_tuple_size_v<ArrayLikeT<U, N>>>,
          std::enable_if_t<std::tuple_size<ArrayLikeT<U, N>>::value == N>,
          std::enable_if_t<std::is_const<T>::value && arene::base::is_array_convertible_v<U, T>>,
          std::enable_if_t<is_valid_source_extent_v<N>>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr span(ArrayLikeT<U, N> const&& array) noexcept
      : span(pre_checked_size{}, array.data(), N) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  /// @brief Disable construction of fixed size spans from other spans of mismatched size.
  /// @param other The source object
  template <
      typename U,
      std::size_t OtherExtent,
      constraints<std::enable_if_t<
          !span_detail::is_dynamic_extent(Extent) && !span_detail::is_dynamic_extent(OtherExtent) &&
          Extent != OtherExtent>> = nullptr>
  constexpr span(span<U, OtherExtent>&& other) noexcept = delete;

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "Span has reference semantics: forwarding not appropriate"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "Span has reference semantics: forwarding not appropriate"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "Span has reference semantics: forwarding not appropriate"
  /// @brief Constructor from a container of T with contiguous elements.
  /// @tparam Container the type of the container to construct from. A "container" is a type which has @c data() and
  ///         @c size() member functions, where @c data() returns a pointer (or equivalent) to contiguous memory
  ///         containing elements pointer-convertible to @c T .
  /// @param source The container to construct the span from
  /// @pre If @c Extent is not @c dynamic_extent , then @c source.size()==Extent must be true, else it is an
  ///      @c ARENE_PRECONDITION violation.
  template <
      typename Container,
      constraints<
          std::enable_if_t<
              !span_detail::has_tuple_size_v<arene::base::remove_cvref_t<Container>> &&
              span_detail::is_dynamic_extent(Extent)>,
          std::enable_if_t<arene::base::is_array_convertible_v<
              arene::base::remove_reference_t<decltype(*std::declval<Container>().data())>,
              T>>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions,bugprone-forwarding-reference-overload)
  constexpr span(Container&& source) noexcept
      : span(source.data(), source.size()) {}
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "Span has reference semantics: forwarding not appropriate"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "Span has reference semantics: forwarding not appropriate"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "Span has reference semantics: forwarding not appropriate"
  /// @brief Constructor from a container of T with contiguous elements.
  /// @tparam Container the type of the container to construct from. A "container" is a type which has @c data() and
  ///         @c size() member functions, where @c data() returns a pointer (or equivalent) to contiguous memory
  ///         containing elements pointer-convertible to @c T .
  /// @param source The container to construct the span from
  /// @pre If @c Extent is not @c dynamic_extent , then @c source.size()==Extent must be true, else it is an
  ///      @c ARENE_PRECONDITION violation.
  template <
      typename Container,
      constraints<
          std::enable_if_t<
              !span_detail::has_tuple_size_v<arene::base::remove_cvref_t<Container>> &&
              !span_detail::is_dynamic_extent(Extent)>,
          std::enable_if_t<arene::base::is_array_convertible_v<
              arene::base::remove_reference_t<decltype(*std::declval<Container>().data())>,
              T>>> = nullptr>
  // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
  explicit constexpr span(Container&& source) noexcept
      : span(source.data(), source.size()) {
    ARENE_PRECONDITION(source.size() == Extent);
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Constructor from another @c span<U,OtherExtent> when @c Extent is "no more restrictive" then @c OtherExtent
  /// @tparam U The type of the element in the other span. Must be pointer-convertible to @c T
  /// @tparam OtherExten If @c Extent is not @c dynamic_extent , then must be equal to @c Extent .
  /// @param other The @c span to construct from.
  template <
      typename U,
      std::size_t OtherExtent,
      constraints<
          std::enable_if_t<arene::base::is_array_convertible_v<U, T>>,
          std::enable_if_t<is_valid_source_extent_v<OtherExtent>>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr span(span<U, OtherExtent> const& other) noexcept
      // parasoft-begin-suppress AUTOSAR-A5_16_1-2 "False positive: used as a constructor argument not a subexpression"
      : span(other.data(), (!span_detail::is_dynamic_extent(Extent)) ? Extent : other.size()) {}
  // parasoft-end-suppress AUTOSAR-A5_16_1-2
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  /// @brief Constructor from @c span<U,dynamic_extent> when @c Extent is not @c dynamic_extent .
  /// @tparam U The type of the element in the other span. Must be pointer-convertible to @c T
  /// @param other The @c span to construct from.
  /// @pre @c span.size()==Extent , else @c ARENE_PRECONDITION violation.
  template <
      typename U,
      std::size_t OtherExtent,
      constraints<
          std::enable_if_t<arene::base::is_array_convertible_v<U, T>>,
          std::enable_if_t<(!span_detail::is_dynamic_extent(Extent)) && span_detail::is_dynamic_extent(OtherExtent)>> =
          nullptr>
  explicit constexpr span(span<U, OtherExtent> const& other) noexcept
      : span(other.data(), Extent) {}
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  /// @brief Predicate to test if the span views onto non-zero elements.
  ///
  /// @return true if <c>size() == 0</c>.
  /// @return false otherwise.
  ARENE_NODISCARD constexpr auto empty() const noexcept -> bool { return this->size() == 0U; }

  /// @brief Returns the total size in bytes of the elements in the span.
  ///
  /// @return The total size, in bytes, of the elements in the span.
  ARENE_NODISCARD constexpr auto size_bytes() const noexcept -> std::size_t { return this->size() * sizeof(T); }

  /// @brief Accessor to the underlying data as a raw pointer.
  ///
  /// @return A pointer to the start of the elements covered by the span.
  ARENE_NODISCARD constexpr auto data() const noexcept -> T* { return data_ptr_; }

  /// @brief Retrieve the element at the specified index into the span. index must be less than size()
  ///
  /// @param index The index of the element to access
  /// @pre <c>index < size()</c> else @c ARENE_PRECONDITION violation.
  /// @return A reference to the element at the specified index into the span.

  constexpr auto operator[](std::size_t index) const noexcept -> T& {
    ARENE_PRECONDITION(index < this->size());
    return begin()[static_cast<difference_type>(index)];
  }

  /// @brief Access the first element in the span.
  ///
  /// @pre @c empty() must be false, else @c ARENE_PRECONDITION violation.
  /// @return A reference to the first element in the span.
  ARENE_NODISCARD constexpr auto front() const noexcept -> T& {
    ARENE_PRECONDITION(!empty());
    return *data_ptr_;
  }

  /// @brief Access the last element in the span.
  ///
  /// @pre @c empty() must be false, else @c ARENE_PRECONDITION violation.
  /// @return A reference to the last element in the span.
  ARENE_NODISCARD constexpr auto back() const noexcept -> T& {
    ARENE_PRECONDITION(!empty());
    return this->operator[](this->size() - 1U);
  }

  /// @brief Create a new fixed-size span from the beginning of the span.
  ///
  /// @tparam Count The number of elements to include.
  /// @tparam UE dummy parameter for SFINAE.
  /// @pre @c Count<=size() . else @c ARENE_PRECONDITION violation.
  /// @return span<T, Count> containing the first @c Count elements from the span.
  template <
      std::size_t Count,
      std::size_t UE = Extent,
      constraints<std::enable_if_t<span_detail::is_dynamic_extent(UE)>> = nullptr>
  ARENE_NODISCARD constexpr auto first() const noexcept -> span<T, Count> {
    ARENE_PRECONDITION(Count <= this->size());
    return {data_ptr_, Count};
  }
  /// @brief Create a new fixed-size span from the beginning of the span.
  ///
  /// @tparam Count The number of elements to include. Must be @c <=Extent .
  /// @return span<T, Count> containing the first @c Count elements from the span.
  template <
      std::size_t Count,
      std::size_t UE = Extent,
      constraints<std::enable_if_t<!span_detail::is_dynamic_extent(UE)>, std::enable_if_t<Count <= Extent>> = nullptr>
  ARENE_NODISCARD constexpr auto first() const noexcept -> span<T, Count> {
    return {data_ptr_, Count};
  }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False positive: 'count' does not hide anything"

  /// @brief Create a new dynamic-size span from the beginning of the span.
  ///
  /// @param count The number of elements to include.
  /// @pre @c count must be <= @c size() . else @c ARENE_PRECONDITION violation.
  /// @return span<T, Count> containing the first @c count elements from the span.
  ARENE_NODISCARD constexpr auto first(std::size_t count) const noexcept -> span<T> {
    ARENE_PRECONDITION(count <= this->size());
    return {data_ptr_, count};
  }

  // parasoft-end-suppress AUTOSAR-A2_10_1-a

  /// @brief Create a new fixed-size span for the last @c Count elements.
  ///
  /// @tparam Count The number of elements to include.
  /// @pre @c Count<=size() , else @c ARENE_PRECONDITION violation.
  /// @return span<T, Count> containing the last @c Count elements from the span.
  template <
      std::size_t Count,
      std::size_t UE = Extent,
      constraints<std::enable_if_t<span_detail::is_dynamic_extent(UE)>> = nullptr>
  ARENE_NODISCARD constexpr auto last() const noexcept -> span<T, Count> {
    ARENE_PRECONDITION(Count <= this->size());
    std::size_t const offset{this->size() - Count};
    return span<T, Count>{arene::base::next(data_ptr_, static_cast<std::ptrdiff_t>(offset)), Count};
  }
  /// @brief Create a new fixed-size span for the last @c Count elements.
  ///
  /// @tparam Count The number of elements to include. Must be @c <=Extent .
  /// @return span<T, Count> containing the last @c Count elements from the span.
  template <
      std::size_t Count,
      std::size_t UE = Extent,
      constraints<std::enable_if_t<!span_detail::is_dynamic_extent(UE)>, std::enable_if_t<Count <= Extent>> = nullptr>
  ARENE_NODISCARD constexpr auto last() const noexcept -> span<T, Count> {
    std::size_t const offset{this->size() - Count};
    return span<T, Count>{arene::base::next(data_ptr_, static_cast<std::ptrdiff_t>(offset)), Count};
  }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False positive: 'count' does not hide anything"

  /// @brief Create a new dynamic-size span for the last @c count elements.
  ///
  /// @param count The number of elements to include.
  /// @pre @c count must be <= @c size() . else @c ARENE_PRECONDITION violation.
  /// @return span<T, Count> containing the last @c count elements from the span.
  ARENE_NODISCARD constexpr auto last(std::size_t count) const noexcept -> span<T> {
    ARENE_PRECONDITION(count <= this->size());
    std::size_t const offset{this->size() - count};
    return span<T>{arene::base::next(data_ptr_, static_cast<std::ptrdiff_t>(offset)), count};
  }

  // parasoft-end-suppress AUTOSAR-A2_10_1-a

  /// @brief Creates a new fixed-size span which is a slice of this span
  ///
  /// @tparam Offset The index of the first element to contain in the subspan. Must be @c <=Extent
  /// @tparam Count The number of elements to include in the subspan. Must satisfy @c Offset+Count<=Extent , or be
  ///               @c dynamic_extent .
  /// @return span<T,Extent-Offset> Equivalent to @c last<Extent-Offset>() .
  template <
      std::size_t Offset,
      std::size_t Count = span_detail::get_dynamic_extent(),
      constraints<
          std::enable_if_t<Offset <= Extent>,
          std::enable_if_t<span_detail::is_dynamic_extent(Count) && !span_detail::is_dynamic_extent(Extent)>> = nullptr>
  ARENE_NODISCARD constexpr auto subspan() const noexcept
      -> span<T, span_detail::extent_for_subspan<Extent, Offset, Count>()> {
    return last<Extent - Offset>();
  }
  /// @brief Creates a new fixed-size span which is a slice of this span
  ///
  /// @tparam Offset The index of the first element to contain in the subspan. Must be @c <=Extent
  /// @tparam Count The number of elements to include in the subspan. Must satisfy @c Offset+Count<=Extent , or be
  ///               @c dynamic_extent .
  /// @return span<T,Count> Equivalent to @c last<Extent-Offset>().first<Count>() .
  template <
      std::size_t Offset,
      std::size_t Count = span_detail::get_dynamic_extent(),
      constraints<
          std::enable_if_t<Offset <= Extent>,
          std::enable_if_t<!span_detail::is_dynamic_extent(Count) && !span_detail::is_dynamic_extent(Extent)>> =
          nullptr>
  ARENE_NODISCARD constexpr auto subspan() const noexcept
      -> span<T, span_detail::extent_for_subspan<Extent, Offset, Count>()> {
    return last<Extent - Offset>().template first<Count>();
  }
  /// @brief Creates a new dynamic-sized span which is a slice of this span
  ///
  /// @tparam Offset The index of the first element to contain in the subspan. Must be <= @c size()
  /// @tparam Count The number of elements to include in the subspan. <c> offset + count </c> must be <= @c size()
  /// @return span<T> equivalent to @c last(size()-Offset) .
  /// @pre <c> Count<= size() && (Count+Offset <= size()) </c>, else @c ARENE_PRECONDITION violation.
  template <
      std::size_t Offset,
      std::size_t Count = span_detail::get_dynamic_extent(),
      constraints<
          std::enable_if_t<Offset <= Extent>,
          std::enable_if_t<span_detail::is_dynamic_extent(Count) && span_detail::is_dynamic_extent(Extent)>> = nullptr>
  ARENE_NODISCARD constexpr auto subspan() const noexcept
      -> span<T, span_detail::extent_for_subspan<Extent, Offset, Count>()> {
    ARENE_PRECONDITION(Offset <= this->size());
    return last(this->size() - Offset);
  }
  /// @brief Creates a new fixed-size span which is a slice of this span
  ///
  /// @tparam Offset The index of the first element to contain in the subspan. Must be <= @c size()
  /// @tparam Count The number of elements to include in the subspan. <c> offset + count </c> must be <= @c size()
  /// @return span<T,Count> Equivalent to @c last(size()-Offset).first<Count>() .
  /// @pre If @c Extent is @c dynamic_extent , then <c> Count<= size() && (Count+Offset <= size()) </c>, else
  ///      @c ARENE_PRECONDITION violation.
  template <
      std::size_t Offset,
      std::size_t Count = span_detail::get_dynamic_extent(),
      constraints<
          std::enable_if_t<Offset <= Extent>,
          std::enable_if_t<!span_detail::is_dynamic_extent(Count) && span_detail::is_dynamic_extent(Extent)>> = nullptr>
  ARENE_NODISCARD constexpr auto subspan() const noexcept
      -> span<T, span_detail::extent_for_subspan<Extent, Offset, Count>()> {
    ARENE_PRECONDITION(Offset <= this->size());
    return last(this->size() - Offset).template first<Count>();
  }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False positive: 'count' does not hide anything"

  /// @brief Creates a new dynamic-size span which is a slice of this span
  ///
  /// @param offset The index of the first element to contain in the subspan. Must be <= @c size()
  /// @param count The number of elements to include in the subspan. <c> offset + count </c> must be <= @c size()
  /// @return span<T> If @c count==dynamic_extent , equivalent to @c last(size()-offset) , else
  ///         @c last(size()-offset).first(count) .
  /// @pre <c> count<= size() && (count+offset <= size()) </c>, else @c ARENE_PRECONDITION violation.
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters) this API is required by span's stdlib compatibility.
  ARENE_NODISCARD constexpr auto subspan(std::size_t offset, std::size_t count = dynamic_extent) const noexcept
      -> span<T> {
    ARENE_PRECONDITION(offset <= this->size());
    auto const last_start = this->size() - offset;
    if (span_detail ::is_dynamic_extent(count)) {
      return last(last_start);
    }
    return last(last_start).first(count);
  }

  // parasoft-end-suppress AUTOSAR-A2_10_1-a

  /// @brief Obtain an iterator to the beginning of the data viewed by the span.
  ///
  /// @return iterator An iterator pointing to the first position in the span.
  ARENE_NODISCARD constexpr auto begin() const noexcept -> iterator {
    return iterator{span_detail::span_iter_passkey{}, data_ptr_};
  }

  /// @brief Obtain a const_iterator to the beginning of the data viewed by the span.
  ///
  /// @return const_iterator A const iterator pointing to the first position in the span.
  ARENE_NODISCARD constexpr auto cbegin() const noexcept -> const_iterator {
    return const_iterator{span_detail::span_iter_passkey{}, data_ptr_};
  }

  /// @brief Obtain an iterator to the one-past-the-end of the data viewed by the span
  ///
  /// @return iterator An iterator equivalent to @c begin()+size() .
  ARENE_NODISCARD constexpr auto end() const noexcept -> iterator {
    return iterator{span_detail::span_iter_passkey{}, arene::base::next(data_ptr_, ssize())};
  }

  /// @brief Obtain a const_iterator to the one-past-the-end of the data viewed by the span
  ///
  /// @return const_iterator A const iterator equivalent to @c cbegin()+size() .
  ARENE_NODISCARD constexpr auto cend() const noexcept -> const_iterator {
    return const_iterator{span_detail::span_iter_passkey{}, arene::base::next(data_ptr_, ssize())};
  }

  /// @brief Obtain an iterator to the beginning of a reversed view of the data in the span.
  ///
  /// @return iterator An iterator equivalent to @c reverse_iterator(end())
  ARENE_NODISCARD constexpr auto rbegin() const noexcept -> reverse_iterator { return reverse_iterator(end()); }

  /// @brief Obtain a const_iterator to the beginning of a reversed view of the data in the span.
  ///
  /// @return const_reverse_iterator An iterator equivalent to @c const_reverse_iterator(cend())
  ARENE_NODISCARD constexpr auto crbegin() const noexcept -> const_reverse_iterator {
    return const_reverse_iterator(cend());
  }

  /// @brief Obtain an iterator to one-past-the-end of a reversed view of the data in the span.
  ///
  /// @return iterator An iterator equivalent to @c reverse_iterator(begin())
  ARENE_NODISCARD constexpr auto rend() const noexcept -> reverse_iterator { return reverse_iterator(begin()); }

  /// @brief Obtain a const iterator to one-past-the-end of a reversed view of the data in the span.
  ///
  /// @return const_reverse_iterator A const iterator equivalent to @c const_reverse_iterator(cbegin())
  ARENE_NODISCARD constexpr auto crend() const noexcept -> const_reverse_iterator {
    return const_reverse_iterator(cbegin());
  }
};
// parasoft-end-suppress AUTOSAR-A13_5_1-a-2

/// @brief Obtain a const view to the object representation of the elements of the span @c spn.
/// @param spn The span of elements to inspect.
/// @return A <c>span<const Byte, Size></c> of the object representations of the elements of @c spn.
template <typename T, std::size_t N>
auto as_bytes(span<T, N> spn) noexcept
    -> span<byte const, span_detail::is_dynamic_extent(N) ? dynamic_extent : (N * sizeof(T))> {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return {reinterpret_cast<byte const*>(spn.data()), spn.size_bytes()};
}

/// @brief Obtain a non-const view to the object representation of the elements of the span @c spn.
/// @param spn The span of elements to inspect.
/// @return A <c>span<byte, Size></c> of the object representations of the elements of @c spn.
template <typename T, std::size_t N, constraints<std::enable_if_t<!std::is_const<T>::value>> = nullptr>
auto as_writable_bytes(span<T, N> spn) noexcept
    -> span<byte, span_detail::is_dynamic_extent(N) ? dynamic_extent : (N * sizeof(T))> {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return {reinterpret_cast<byte*>(spn.data()), spn.size_bytes()};
}

template <typename T, std::size_t Extent>
constexpr std::integral_constant<std::size_t, Extent> span<T, Extent>::extent;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SPAN_SPAN_HPP_
