<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page array array: A constexpr Compatible Checked Array

<!-- markdownlint-enable MD041 -->

`arene::base::array` is a backport of `std::array` from C++17 made necessary by
the fact that the C++14 version of `std::array` is not compatible with
`constexpr`.The specification for `std::array` is available at
[`cppreference.com`](https://en.cppreference.com/w/cpp/container/array). It also
is a "checked" type, meaning that operators which in `std::array` would have UB,
such as indexing `operator[]` past the end of the array, are instead converted
into `ARENE_PRECONDITION` violations.

The public header is

```{.cpp}
#include "arene/base/array.hpp"
```

The Bazel target is

```text
//:array
```
