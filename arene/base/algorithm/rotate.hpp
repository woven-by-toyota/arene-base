// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::rotate"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_ROTATE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_ROTATE_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/algorithm/iter_swap.hpp"
#include "arene/base/algorithm/move.hpp"
#include "arene/base/algorithm/move_backward.hpp"
#include "arene/base/algorithm/swap_ranges.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/iterator/advance.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/iterator/prev.hpp"
#include "arene/base/math/gcd.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/denotes_range.hpp"
#include "arene/base/type_traits/is_movable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// IWYU pragma: no_include "arene/base/stdlib_choice/iterator_traits.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_cv.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_reference.hpp"

namespace arene {
namespace base {
namespace algorithm_detail {
namespace rotate_detail {

/// @brief function object implementing the rotate algorithm
///
class rotate_impl_fn {
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Helper function for the Rotate algorithm implementation if iterator
  /// allows forward access. Should not be called directly.
  /// @tparam Iterator The type of the Iterator
  /// @param first The beginning of the original range
  /// @param last The end of the original range
  /// @return The Iterator to the new location of the element pointed by first.
  template <class Iterator, constraints<std::enable_if_t<base::is_forward_iterator_v<Iterator>>> = nullptr>
  static constexpr auto rotate_left(Iterator first, Iterator last) -> Iterator {
    auto tmp = std::move(*first);                                             // CODEQLFP(A7-1-2)
    auto lm1 = ::arene::base::move(::arene::base::next(first), last, first);  // CODEQLFP(A7-1-1) CODEQLFP(A7-1-2)
    *lm1 = std::move(tmp);
    return lm1;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  /// @brief Helper function for the Rotate algorithm implementation if iterator
  /// allows bidirectional access. Should not be called directly.
  /// @tparam Iterator The type of the Iterator
  /// @param first The beginning of the original range
  /// @param last The end of the original range
  /// @return The Iterator to the new location of the element pointed by first.
  template <class Iterator, constraints<std::enable_if_t<base::is_forward_iterator_v<Iterator>>> = nullptr>
  static constexpr auto rotate_right(Iterator first, Iterator last) -> Iterator {
    Iterator lm1{arene::base::prev(last)};                      // CODEQLFP(A7-1-2)
    auto tmp = std::move(*lm1);                                 // CODEQLFP(A7-1-2)
    auto fp1 = ::arene::base::move_backward(first, lm1, last);  // CODEQLFP(A7-1-1) CODEQLFP(A7-1-2)
    *first = std::move(tmp);
    return fp1;
  }

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Helper function for the Rotate algorithm implementation if iterator
  /// allows forward access. Should not be called directly.
  /// @tparam Iterator The type of the Iterator
  /// @param first The beginning of the original range
  /// @param middle The element that should appear at the beginning of the rotated
  /// range
  /// @param last The end of the original range
  /// @return The Iterator to the new location of the element pointed by
  /// first.
  template <class Iterator, constraints<std::enable_if_t<base::is_forward_iterator_v<Iterator>>> = nullptr>
  static constexpr auto rotate_forward(Iterator first, Iterator middle, Iterator last) -> Iterator {
    Iterator iter{middle};
    while (true) {
      ::arene::base::iter_swap(first, iter);
      arene::base::advance(first, 1);
      arene::base::advance(iter, 1);
      if (iter == last) {
        break;
      }
      if (first == middle) {
        middle = iter;
      }
    }
    Iterator const result{first};  // parasoft-suppress AUTOSAR-A7_1_3-a-2 "False positive: const *is* on the rhs"
    if (first != middle) {
      iter = middle;
      while (true) {
        ::arene::base::iter_swap(first, iter);
        arene::base::advance(first, 1);
        arene::base::advance(iter, 1);
        if (iter == last) {
          if (first == middle) {
            break;
          }
          iter = middle;
        } else if (first == middle) {
          middle = iter;
        } else {
          // nothing to do
        }
      }
    }
    return result;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  /// @brief Helper function for the Rotate algorithm implementation if iterator
  /// allows random access. Should not be called directly.
  /// @tparam Iterator The type of the Iterator
  /// @param first The beginning of the original range
  /// @param middle The element that should appear at the beginning of the rotated
  /// range
  /// @param last The end of the original range
  /// @return The Iterator to the new location of the element pointed by first.
  template <typename Iterator, constraints<std::enable_if_t<base::is_random_access_iterator_v<Iterator>>> = nullptr>
  static constexpr auto rotate_random_access(Iterator first, Iterator middle, Iterator last)
      -> Iterator {  // CODEQLFP(A7-1-1)
    auto const distance_to_middle = arene::base::distance(first, middle);
    auto const distance_from_middle = arene::base::distance(middle, last);
    if (distance_to_middle == distance_from_middle) {
      std::ignore = ::arene::base::swap_ranges(first, middle, middle);
      return middle;
    }
    auto const gcd_distance = ::arene::base::gcd(distance_to_middle, distance_from_middle);
    auto pos = ::arene::base::next(first, gcd_distance);
    while (pos != first) {
      arene::base::advance(pos, -1);
      auto temp(std::move(*pos));
      Iterator pos1{pos};
      Iterator pos2{arene::base::next(pos1, distance_to_middle)};
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-do-while)
      do {
        *pos1 = std::move(*pos2);
        pos1 = pos2;
        auto const distance_to_end = arene::base::distance(pos2, last);
        if (distance_to_middle < distance_to_end) {
          arene::base::advance(pos2, distance_to_middle);
        } else {
          pos2 = arene::base::next(first, distance_to_middle - distance_to_end);
        }
      } while (pos2 != pos);  // CODEQLFP(A5-0-2)
      *pos1 = std::move(temp);
    }  // CODEQLFP(A5-0-2)
    return arene::base::next(first, distance_from_middle);
  }

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Overloading function for the particular Rotate algorithm
  /// implementation if iterator allows forward access. Should not be called
  /// directly.
  /// @tparam Iterator The type of the Iterator
  /// @param first The beginning of the original range
  /// @param middle The element that should appear at the beginning of the rotated
  /// range
  /// @param last The end of the original range
  /// @return The Iterator to the new location of the element pointed by
  /// first.
  template <
      class Iterator,
      constraints<
          std::enable_if_t<base::is_forward_iterator_v<Iterator>>,
          std::enable_if_t<!base::is_bidirectional_iterator_v<Iterator>>> = nullptr>
  static constexpr auto impl(Iterator first, Iterator middle, Iterator last) -> Iterator {  // CODEQLFP(DCL51-CPP)
    if (::arene::base::next(first) == middle) {
      return rotate_left(first, last);
    }
    return rotate_forward(first, middle, last);
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  /// @brief Overloading function for the particular Rotate algorithm
  /// implementation if iterator allows bidirectional access. Should not be called
  /// directly.
  /// @tparam Iterator The type of the Iterator
  /// @param first The beginning of the original range
  /// @param middle The element that should appear at the beginning of the rotated
  /// range
  /// @param last The end of the original range
  /// @return The Iterator to the new location of the element
  /// pointed by first.
  template <
      class Iterator,
      constraints<
          std::enable_if_t<base::is_bidirectional_iterator_v<Iterator>>,
          std::enable_if_t<!base::is_random_access_iterator_v<Iterator>>> = nullptr>
  static constexpr auto impl(Iterator first, Iterator middle, Iterator last) -> Iterator {  // CODEQLFP(DCL51-CPP)
    if (::arene::base::next(first) == middle) {
      return rotate_left(first, last);
    }
    if (::arene::base::next(middle) == last) {
      return rotate_right(first, last);
    }
    return rotate_forward(first, middle, last);
  }

  /// @brief Overloading function for the particular Rotate algorithm
  /// implementation if iterator allows random access. Should not be called
  /// directly.
  /// @tparam Iterator The type of the Iterator
  /// @param first The beginning of the original range
  /// @param middle The element that should appear at the beginning of the rotated
  /// range
  /// @param last The end of the original range
  /// @return The Iterator to the new location of the element pointed
  /// by first.
  template <class Iterator, constraints<std::enable_if_t<base::is_random_access_iterator_v<Iterator>>> = nullptr>
  static constexpr auto impl(Iterator first, Iterator middle, Iterator last) -> Iterator {  // CODEQLFP(DCL51-CPP)
    ARENE_PRECONDITION((::arene::base::distance(first, middle) >= 0) && (::arene::base::distance(middle, last) >= 0));
    if (::arene::base::next(first) == middle) {
      return rotate_left(first, last);
    }
    if (::arene::base::next(middle) == last) {
      return rotate_right(first, last);
    }
    return rotate_random_access(first, middle, last);
  }

 public:
  /// @brief Performs a left rotation on a range of elements.
  /// Specifically, @c rotate swaps the elements in the range <c>[first,
  /// last)</c> in such a way that the element @c middle becomes the first
  /// element of the new range and <c>middle - 1</c> becomes the last element.
  /// @tparam Iterator The type of the Iterator for the range to rotate
  /// @param first The beginning of the original range
  /// @param middle The element that should appear at the beginning of the rotated
  /// range
  /// @param last The end of the original range
  /// @return The Iterator to the new location of the element pointed by first.
  /// Equal to <c>first + (last - middle)</c>
  ///
  /// @pre <c>[first, middle)</c> is a valid range
  /// @pre <c>[middle, last)</c> is a valid range
  /// @pre <c>[first, last)</c> is a valid range
  ///
  /// @note Constraints <br>
  ///   * @c Iterator must satisfy forward iterator requirements
  ///   * @c Iterator must be ValueSwappable
  ///   * the type of <c>*first</c> must be move constructible
  ///   * the type of <c>*first</c> must be move assignable
  ///
  template <
      class Iterator,
      constraints<
          std::enable_if_t<is_forward_iterator_v<Iterator>>,
          std::enable_if_t<is_swappable_v<iter_reference_t<Iterator>>>,
          std::enable_if_t<is_movable_v<remove_cvref_t<iter_reference_t<Iterator>>>>> = nullptr>
  constexpr auto operator()(Iterator first, Iterator middle, Iterator last) const noexcept(     //
      denotes_nothrow_iterable_range_v<Iterator> &&                                             //
      is_nothrow_equality_comparable_v<Iterator> &&                                             //
      std::is_nothrow_copy_constructible<Iterator>::value &&                                    //
      std::is_nothrow_copy_assignable<Iterator>::value&&                                        //
      noexcept(arene::base::advance(                                                            //
          std::declval<Iterator&>(),                                                            //
          std::declval<iter_difference_t<Iterator>>()                                           //
      )) &&                                                                                     //
      std::is_nothrow_move_constructible<remove_cvref_t<iter_reference_t<Iterator>>>::value &&  //
      std::is_nothrow_move_assignable<remove_cvref_t<iter_reference_t<Iterator>>>::value &&     //
      (is_random_access_iterator_v<Iterator> ||                                                 //
       is_nothrow_swappable_v<remove_cvref_t<iter_reference_t<Iterator>>>)                      //
  ) -> Iterator {
    if (first == middle) {
      return last;
    }
    if (middle == last) {
      return first;
    }
    return impl(first, middle, last);
  }
};

}  // namespace rotate_detail
}  // namespace algorithm_detail

/// @def arene::base::rotate
/// @copydoc arene::base::algorithm_detail::rotate_detail::rotate_impl_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(algorithm_detail::rotate_detail::rotate_impl_fn, rotate);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_ROTATE_HPP_
