<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page string_view string_view: Non-Owning Views Onto Character Data

<!-- markdownlint-enable MD041 -->

This sub-package provides two "view" classes that refer to strings allocated
elsewhere (either as `arene::base::inline_string` objects, or string literals,
or something else): `arene::base::string_view`, and
`arene::base::null_terminated_string_view`

The public header is:

```{.cpp}
#include "arene/base/string_view.hpp"
```

The Bazel target is

```text
//:string_view
```

# Introduction {#introduction}

The `string_view` sub-package provides:

- the _view_ types `arene::base::string_view` and
  `arene::base::null_terminated_string_view`, which are intended for use as
  function parameters, where you wish to consume a string, but don't care where
  it is stored, and
- literal suffixes for creating instances of `arene::base::string_view` and
  `arene::base::null_terminated_string_view` directly from string literals.

## The view types {#the-view-types}

The `string_view` sub-package provides two view types:
`arene::base::string_view` and `arene::base::null_terminated_string_view`.
`arene::base::string_view` is a backport of `std::string_view`, and can
represent _any_ string: `std::string`, `arene::base::inline_string`, string
literals, `char` arrays, and C-style strings with a terminating `NUL` character,
whereas `arene::base::null_terminated_string_view` can only represent C-style
strings with a terminating `NUL` character.

Both types are **view** types, and as such do not control the lifetime of the
referenced string. They are cheap to copy, which makes them ideal for use as
function arguments, but care should be taken when using them in other contexts,
to ensure that the string referenced by the view object outlives the view
object. They are also both `const` view types: they do not allow modifying the
referenced string in any way, either as a whole, or by modifying individual
characters.

In addition, `arene::base::string_view` provides a `three_way_compare` member
function, to facilitate \ref compare "three-way comparisons" similar to the
C++20 `<=>` operator.

### `arene::base::string_view` {#arene-base-string_view}

`arene::base::string_view` is a backport of
[`std::string_view`](https://en.cppreference.com/w/cpp/header/string_view), and
is a wrapper for a `const char*` and a length. It can thus refer to any string
that is represented as a contiguous sequence of characters, including strings
that are sub-strings of longer strings. Because the length is explicitly stored,
it does not rely on a marker such as a `NUL` character to indicate the end of
the string.

The API of `arene::base::string_view` is as similar to that of
`const std::string` as possible. The key difference is that `substr` returns a
`arene::base::string_view` rather than a new string holding a copy of the
relevant characters. This means that such sub-strings are _still_ views on the
original, with the same lifetime constraints.

Functions provided:

- Implicit conversion from `std::string`, string literals, and C-style strings
  represented via `const char*`, as well as from `arene::base::inline_string`
  and `arene::base::null_terminated_string_view`
- Iteration, with random-access iterators, including reverse iteration
- `length` and `size` functions to return the string length
- Comparisons with other `arene::base::string_view` instances, and other string
  types, via `compare` and `three_way_compare` member functions, comparison
  operators, and `starts_with` and `ends_with` member functions.
- `substr` for obtaining sub-strings of any length, at any offset
- Indexing with `operator[]`
- Access to first and last characters via `front` and `back`
- Copying portions of the string into a destination buffer
- Removing a prefix or suffix via `remove_prefix` and `remove_suffix`
- Searching for a character or string via `find`, `rfind`, `find_first_of`,
  `find_last_of`, `find_first_not_of`, and `find_last_not_of` member functions.

### `arene::base::null_terminated_string_view` {#arene-base-null_terminated_string_view}

`arene::base::null_terminated_string_view` is a wrapper for a C-style string,
terminated with a `NUL` character. If you have a function that accepts a string,
which you are intending to pass unchanged to an underlying API such as `fopen`
or `getaddrinfo` that expects a C-style string terminated with a `NUL`
character, then you can use `arene::base::null_terminated_string_view` as the
type of the function parameter rather than `arene::base::string_view`, in order
to guarantee that property, and avoid unnecessary copying.

`arene::base::null_terminated_string_view` can be implicitly constructed from
`std::string` and `arene::base::inline_string`, both of which guarantee that the
stored string is `NUL` terminated. It can also be explicitly constructed from a
`const char*`, in which case the code that invokes the constructor is taking
responsibility to ensure that the string is indeed `NUL` terminated.

The API of `arene::base::null_terminated_string_view` is deliberately minimal,
since it does not store the length. There is an implicit conversion to
`arene::base::string_view` for those cases where a more comprehensive API is
required, at the expense of computing the length. The operations provided are:

- _Iteration_. `begin()` returns a random-access iterator, whereas `end()`
  returns a sentinel value to represent the end of the string,
- `empty()` for checking if the string is empty.
- `length()` to _compute_ the length: this is not cached, so is calculated on
  every call by traversing the string.
- `c_str()` and `data()` for accessing the stored string.
- `substr(offset)` which returns the `arene::base::null_terminated_string_view`
  starting at the specified offset, or an empty string if that is beyond the end
  of the string.
- `substr(offset,length)` which returns a `arene::base::string_view` for the
  sub-string starting at the specified offset, up to the specified length, or an
  empty `arene::base::string_view` if the offset is beyond the end of the
  string.
- Comparisons against other instances of
  `arene::base::null_terminated_string_view`, instances of
  `arene::base::string_view`, instances of `std::string`, and instances of
  C-style strings represented via `const char*`, using operators,
  `three_way_compare` and `compare`.

### Examples {#examples}

#### `string_view` function arguments {#string_view-function-arguments}

Multiple string types can be seamlessly passed to the same function if it
accepts an `arene::base::string_view` as an argument.

```{.cpp}
void foo(arene::base::string_view s);

void bar() {
  foo("hello");
  arene::base::inline_string<20> hello{"hello world"};
  foo(hello);
  std::string goodbye("goodbye");
  foo(goodbye);
  foo(arene::base::string_view{hello}.substr(3, 5)); // pass "lo wo"
}
```

#### `null_terminated_string_view` for passing to C APIs {#null_terminated_string_view-for-passing-to-c-apis}

If you intend to pass a string argument to an API that expects a C-style string,
use `arene::base::null_terminated_string_view`. This will allow passing any
string type that guarantees there is a `NUL` terminator for the string.

```{.cpp}
void foo(arene::base::null_terminated_string_view s) {
  auto file = fopen(s.c_str(), "r");
  // ...
  fclose(file);
}

void bar() {
  foo(arene::base::null_terminated_string_view{"file1.txt"});
  arene::base::inline_string<20> file2{"file2.txt"};
  foo(file2);
  std::string file3("file3.txt");
  foo(file3);
  using namespace arene::base::literals;
  foo("file4"_ntsv);
}
```

## Literal suffixes {#literal-suffixes}

The `string_view` sub-package provides literal suffixes for the view types in
the `arene::base::literals` namespace. You can enable this by writing

```{.cpp}
using namespace arene::base::literals;
```

at block scope, or at namespace-scope in a `.cpp` file.

You can then use the `_asv` suffix to create an `arene::base::string_view`, or
the `_ntsv` suffix to create an `arene::base::null_terminated_string_view`. e.g.
`"some string"_asv` yields an `arene::base::string_view` instance referencing
the string literal `"some string"`, and `"hello world"_ntsv` yields an
`arene::base::null_terminated_string_view` instancing referencing the string
literal `"hello world"`.
