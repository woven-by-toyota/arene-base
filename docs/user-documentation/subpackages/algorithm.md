<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page algorithm algorithm: Standard Algorithms

<!-- markdownlint-enable MD041 -->

The algorithm library contains backports of C++ standard algorithms from newer
versions of C++, along with additional useful algorithms with similar
iterator-based design philosophies.

The public export header is provided by:

```{.cpp}
#include "arene/base/algorithm.hpp"
```

and the stable bazel target is:

```text
//:algorithm
```

## Standard Library Backports {#standard-library-backports}

The following algorithms are backported from newer versions of C++ either
because they did not yet exist in C++14, or because improvements were introduced
in later versions, such as being marked `constexpr`. The backport of an
algorithm is from the most recent ratified `stdlib` version.

| function                                         | description                                                                                                                                   | stdlib equivalent                                                                                                         |
| ------------------------------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------- |
| `arene::base::all_of`                            | Checks if a unary predicate returns true for all elements in a specified range                                                                | [`std::all_of`](https://en.cppreference.com/w/cpp/algorithm/all_any_none_of)                                              |
| `arene::base::any_of`                            | Checks if a unary predicate returns true for at least one element in a specified range                                                        | [`std::any_of`](https://en.cppreference.com/w/cpp/algorithm/all_any_none_of)                                              |
| `arene::base::clamp`                             | Clamps a given value between lower and upper bounds of the same type                                                                          | [`std::clamp`](https://en.cppreference.com/w/cpp/algorithm/clamp)                                                         |
| `arene::base::copy`                              | Copies the elements in the specified range to another location                                                                                | [`std::copy`](https://en.cppreference.com/w/cpp/algorithm/all_any_none_of)                                                |
| `arene::base::copy_if`                           | Only copies the elements for which the predicate returns true                                                                                 | [`std::copy_if`](https://en.cppreference.com/w/cpp/algorithm/copy)                                                        |
| `arene::base::equal`                             | Determines if two sequences are equal                                                                                                         | [`std::equal`](https://en.cppreference.com/w/cpp/algorithm/equal)                                                         |
| `arene::base::fill`                              | Fills a sequence with a given value by repeated assignment.                                                                                   | [`std::fill`](https://en.cppreference.com/w/cpp/algorithm/fill)                                                           |
| `arene::base::find`                              | Return the first iterator in the specified range with the value specified, or `last` if there is no such iterator                             | [`std::find`](https://en.cppreference.com/w/cpp/algorithm/find)                                                           |
| `arene::base::find_if`                           | Return the first iterator in the specified range whose value satisfies the specified predicate, or `last` if there is no such iterator        | [`std::find_if`](https://en.cppreference.com/w/cpp/algorithm/find)                                                        |
| `arene::base::inner_product`                     | Computes the inner product of two ranges of elements                                                                                          | [`std::inner_product`](https://en.cppreference.com/w/cpp/algorithm/inner_product)                                         |
| `arene::base::iota`                              | Fills a range with successive increments of the starting value                                                                                | [`std::iota`](https://en.cppreference.com/w/cpp/algorithm/iota)                                                           |
| `arene::base::iter_swap`                         | Swaps the values of the elements the given iterators are pointing to.                                                                         | [`std::iter_swap`](https://en.cppreference.com/w/cpp/algorithm/iter_swap)                                                 |
| `arene::base::lexicographical_compare`           | Imparts a lexicographical ordering of two sequences                                                                                           | [`std::lexicographical_compare`](https://en.cppreference.com/w/cpp/algorithm/lexicographical_compare)                     |
| `arene::base::lexicographical_compare_three_way` | Determines the three-way (less, equal, greater) lexicographical ordering of two sequences                                                     | [`std::lexicographical_compare_three_way`](https://en.cppreference.com/w/cpp/algorithm/lexicographical_compare_three_way) |
| `arene::base::move`                              | Indicate that an object's resources may be transferred to another object                                                                      | [`std::move`](https://en.cppreference.com/w/cpp/utility/move)                                                             |
| `arene::base::move_backward`                     | Moves the elements from a range to another range in reverse order                                                                             | [`std::move_backward`](https://en.cppreference.com/w/cpp/algorithm/move_backward)                                         |
| `arene::base::rotate`                            | Rotates the order of the elements in the specified range in such a way that the element specified as the middle becomes the new first element | [`std::rotate`](https://en.cppreference.com/w/cpp/algorithm/rotate)                                                       |
| `arene::base::sort`                              | Sorts a range into ascending order.                                                                                                           | [`std::sort`](https://en.cppreference.com/w/cpp/algorithm/sort)                                                           |
| `arene::base::swap_ranges`                       | Exchange the elements in one range with the elements of another                                                                               | [`std::swap_ranges`](https://en.cppreference.com/w/cpp/algorithm/swap_ranges)                                             |
| `arene::base::transform`                         | Applies the given function to a range of elements and stores the results in another range                                                     | [`std::transform`](https://en.cppreference.com/w/cpp/algorithm/transform)                                                 |
| `arene::base::transform_reduce`                  | Applies an invocable, then reduces out of order                                                                                               | [`std::transform_reduce`](https://en.cppreference.com/w/cpp/algorithm/transform_reduce)                                   |
