// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::mdspan_detail::count"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_COUNT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_COUNT_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief function object implementing count
class count_fn  //
{
 public:
  /// @brief count the number of elements in a range equal to a value
  /// @tparam InputIterator type of the iterators denoting the input range
  /// @tparam Type type to compare against
  /// @param first iterator to the beginning of the range
  /// @param last iterator one past the end of the range
  /// @param value value to compare with elements of the range
  /// @return number of elements in the range equal to @c value
  /// @pre @c InputIterator must satisfy the input iterator requirements.
  /// @pre @c [begin,end) must be a valid range.
  ///
  template <class InputIterator, class Type>
  constexpr auto operator()(InputIterator first, InputIterator last, Type const& value) const  //
      noexcept(                                                                                //
          noexcept(std::declval<InputIterator&>() == std::declval<InputIterator&>()) &&        //
          noexcept(++std::declval<InputIterator&>()) &&                                        //
          noexcept(*std::declval<InputIterator&>() == std::declval<Type const&>())             //
      )                                                                                        //
      -> typename std::iterator_traits<InputIterator>::difference_type                         //
  {
    typename std::iterator_traits<InputIterator>::difference_type num{};

    while (first != last) {
      // parasoft-begin-suppress AUTOSAR-M5_2_10-a "idiomatic iterator operations permitted by M5-2-10 Permit #1"
      // parasoft-begin-suppress AUTOSAR-M5_0_15-a "False positive: iterators
      // expected to refer to a valid range so incrementing is OK"
      if (*first++ == value) {
        ++num;
      }
      // parasoft-end-suppress AUTOSAR-M5_0_15-a
      // parasoft-end-suppress AUTOSAR-M5_2_10-a
    }

    return num;
  }
};

/// @def arene::base::mdspan_detail::count
/// @copydoc arene::base::mdspan_detail::count_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(mdspan_detail::count_fn, count);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_COUNT_HPP_
