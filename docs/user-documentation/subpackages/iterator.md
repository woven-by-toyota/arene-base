<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page iterator iterator: Backports of Iterator Facilities From Newer C++ Versions

<!-- markdownlint-enable MD041 -->

Arene base provides backports of various C++ iterator utilities, either because
they are not natively available in C++14, or because later versions introduced
improvements, such as being marked `constexpr`.

The public header is

```{.cpp}
#include "arene/base/iterator.hpp""
```

The Bazel target is

```text
//:iterator
```

# Standard Library Backports {#subpackages-iterator-standard-library-backports}

Backports from newer versions of the standard library, generally to provide
`constexpr` support.

## Iterator Operations {#subpackages-iterator-iterator-operations}

|             utility              |                                                       description                                                       |                                    stdlib equivalent                                    |
| :------------------------------: | :---------------------------------------------------------------------------------------------------------------------: | :-------------------------------------------------------------------------------------: |
|      `arene::base::advance`      |                              Advances the given iterator by the specified number of steps                               |          [`std::advance`](https://en.cppreference.com/w/cpp/iterator/advance)           |
|     `arene::base::distance`      |                                    Returns the number of steps between two iterators                                    |         [`std::distance`](https://en.cppreference.com/w/cpp/iterator/distance)          |
|       `arene::base::next`        | Takes an initial iterator and a number of steps to move forward within a container, and returns the resulting iterator  |             [`std::next`](https://en.cppreference.com/w/cpp/iterator/next)              |
|       `arene::base::prev`        | Takes an initial iterator and a number of steps to move backward within a container, and returns the resulting iterator |             [`std::prev`](https://en.cppreference.com/w/cpp/iterator/prev)              |
| `arene::base::ranges::iter_move` |       Moves the element pointed to by the given iterator, typically used to implement move semantics for ranges.        | [`std::ranges::iter_move`](https://en.cppreference.com/w/cpp/iterator/ranges/iter_move) |

## Iterator Traits {#subpackages-iterator-iterator-traits}

|                  utility                  |                                                       description                                                       |                                             stdlib equivalent                                              |
| :---------------------------------------: | :---------------------------------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------------------------------------------: |
|    `arene::base::incrementable_traits`    |              Computes the difference type of an incrementable type (generally a type used as an iterator).              |       [`std::incrementable_traits`](https://en.cppreference.com/w/cpp/iterator/incrementable_traits)       |
| `arene::base::indirectly_readable_traits` | Specialization template to define the properties of an indirectly readable type (generally a type used as an iterator). | [`std::indirectly_readable_traits`](https://en.cppreference.com/w/cpp/iterator/indirectly_readable_traits) |
|     `arene::base::indirect_result_t`      |      Computes the result type of invoking a callable with references obtained from indirectly readable iterators.       |          [`std::indirect_result_t`](https://en.cppreference.com/w/cpp/iterator/indirect_result_t)          |
|     `arene::base::iter_difference_t`      |                                    Computes the difference type of an iterator type.                                    |          [`std::iter_difference_t`](https://en.cppreference.com/w/cpp/iterator/iter_difference_t)          |
|        `arene::base::iter_value_t`        |             Computes the value type of an indirectly readable type (generally a type used as an iterator).              |                   [`std::iter_value_t`](https://en.cppreference.com/cpp/iterator/iter_t)                   |
|      `arene::base::iter_reference_t`      |           Computes the reference type of an indirectly readable type (generally a type used as an iterator).            |                 [`std::iter_reference_t`](https://en.cppreference.com/cpp/iterator/iter_t)                 |
|  `arene::base::iter_rvalue_reference_t`   |        Computes the rvalue-reference type of an indirectly readable type (generally a type used as an iterator).        |             [`std::iter_rvalue_reference_t`](https://en.cppreference.com/cpp/iterator/iter_t)              |
|   `arene::base::iter_const_reference_t`   |        Computes the const-reference type of an indirectly readable type (generally a type used as an iterator).         |              [`std::iter_const_reference_t`](https://en.cppreference.com/cpp/iterator/iter_t)              |

## Iterator Adaptors {#subpackages-iterator-iterator-adaptors}

|               utility                |                    description                    |                                        stdlib equivalent                                         |
| :----------------------------------: | :-----------------------------------------------: | :----------------------------------------------------------------------------------------------: |
|   `arene::base::reverse_iterator`    |   Converts an iterator into a reverse iterator.   |      [`std::reverse_iterator`](https://en.cppreference.com/w/cpp/iterator/reverse_iterator)      |
| `arene::base::make_reverse_iterator` | Template-deduction factory for `reverse_iterator` | [`std::make_reverse_iterator`](https://en.cppreference.com/w/cpp/iterator/make_reverse_iterator) |

## Concept Backports {#subpackages-iterator-concept-backports}

Backports of concepts from newer versions of the standard library, implemented
as C++14-compatible type traits.

|                 utility                  |                                                                                           description                                                                                           |                                       stdlib equivalent                                        |
| :--------------------------------------: | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------------------------------: |
| `arene::base::is_indirectly_readable_v`  |            Checks if type is indirectly readable, meaning the return from its `operator*` satisfies the required relationships for its value, reference, and rvalue reference types.            |  [`std::indirectly_readable`](https://en.cppreference.com/w/cpp/iterator/indirectly_readable)  |
| `arene::base::is_weakly_incrementable_v` | Checks if a type supports weakly incrementable operations. _Weakly_ means that `a == b` semantically does not imply that `++a == ++b` semantically, and does not require `equality_comparable`. | [`std::weakly_incrementable`](https://en.cppreference.com/w/cpp/iterator/weakly_incrementable) |
