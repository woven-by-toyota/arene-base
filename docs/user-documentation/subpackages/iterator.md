<!-- Copyright 2024, Toyota Motor Corporation -->

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

## Standard Library Backports {#standard-library-backports}

Backports from newer versions of the standard library, generally to provide
`constexpr` support.

### Iterator Operations {#iterator-operations}

|         utility         |                                                       description                                                       |                           stdlib equivalent                            |
| :---------------------: | :---------------------------------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------: |
| `arene::base::advance`  |                              Advances the given iterator by the specified number of steps                               |  [`std::advance`](https://en.cppreference.com/w/cpp/iterator/advance)  |
| `arene::base::distance` |                                    Returns the number of steps between two iterators                                    | [`std::distance`](https://en.cppreference.com/w/cpp/iterator/distance) |
|   `arene::base::next`   | Takes an initial iterator and a number of steps to move forward within a container, and returns the resulting iterator  |     [`std::next`](https://en.cppreference.com/w/cpp/iterator/next)     |
|   `arene::base::prev`   | Takes an initial iterator and a number of steps to move backward within a container, and returns the resulting iterator |     [`std::prev`](https://en.cppreference.com/w/cpp/iterator/prev)     |

### Iterator Adaptors {#iterator-adaptors}

|               utility                |                    description                    |                                        stdlib equivalent                                         |
| :----------------------------------: | :-----------------------------------------------: | :----------------------------------------------------------------------------------------------: |
|   `arene::base::reverse_iterator`    |   Converts an iterator into a reverse iterator.   |      [`std::reverse_iterator`](https://en.cppreference.com/w/cpp/iterator/reverse_iterator)      |
| `arene::base::make_reverse_iterator` | Template-deduction factory for `reverse_iterator` | [`std::make_reverse_iterator`](https://en.cppreference.com/w/cpp/iterator/make_reverse_iterator) |
