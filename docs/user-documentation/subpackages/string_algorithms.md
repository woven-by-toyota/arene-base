<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page string_algorithms string_algorithms: Algorithms For Manipulating Strings

<!-- markdownlint-enable MD041 -->

Arene base provides algorithms to operate on strings, for trimming whitespace,
comparisons, and `constexpr` length calculations.

The public header is:

```{.cpp}
#include "arene/base/string_algorithms.hpp"
```

The Bazel target is

```text
//:string_algorithms
```

# Introduction {#introduction}

The `string_algorithms` package provides functions for working with strings:

- `arene::base::ltrim`, `arene::base::rtrim` and `arene::base::trim` for
  removing whitespace characters from the ends of a string,
- `arene::base::string_length`, which is a `constexpr`-compatible version of
  `strlen`, and
- `arene::base::lexicographic_string_compare`, which does a 3-way comparison of
  two strings, and is used internally in the comparison functions of
  `string_view` and `inline_string`.

## String trimming functions {#string-trimming-functions}

Arene Base provides three sets of string trimming functions:

- `arene::base::ltrim`, which removes whitespace from the beginning of the
  string,
- `arene::base::rtrim`, which removes whitespace from the end of the string, and
- `arene::base::trim`, which removes whitespace from both ends of the string.

In all cases, an overload is provided which accepts an
`arene::base::string_view`, and returns an `arene::base::string_view` to the
same underlying characters, except omitting the trimmed whitespace, along with
an overload that accepts a reference to an `arene::base::inline_string`, and
returns a **copy** of the string, with the trimmed whitespace removed.

```{.cpp}
arene::base::inline_string<20> s{"     hello world     "};
auto s1 = arene::base::ltrim(s); // copy of "hello world     "
auto s2 = arene::base::rtrim(s); // copy of "     hello world"
auto s3 = arene::base::trim(arene::base::string_view{s}); // string_view for "hello world" in s
```

## `arene::base::string_length` {#arene-base-string_length}

This function is purely a `constexpr`-compatible version of `strlen`, and is
therefore used in the implementation of the string types, to enable them to work
in `constexpr` code.

## `arene::base::lexicographic_string_compare` {#arene-base-lexicographic_string_compare}

This function does a \ref compare "three-way comparison" of two strings,
represented either as a C-style `const char*` string or a
`arene::base::span<const char>`, and is used internally in the comparison
functions of `string_view` and `inline_string`. It returns an
`arene::base::strong_ordering` value indicating whether the first string is less
than, equal to, or greater than the second string, using a lexical ordering that
ensures UTF-8 characters are ordered correctly.
