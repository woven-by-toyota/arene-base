<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page inline_string inline_string: Non-Allocating, Static Capacity String

<!-- markdownlint-enable MD041 -->

Arene base provides a facility for representing character strings in C++ without
dynamic memory allocation, in the form of the `arene::base::inline_string` class
template that holds the storage for the string inside the object.

The public header is:

```cpp
#include "arene/base/inline_string.hpp"
```

The Bazel target is

```text
//:inline_string
```

# Introduction {#introduction}

The `inline_string` package provides the `arene::base::inline_string` type which
is a replacement for `std::string` that doesn't use dynamic memory allocation,
along with `arene::base::to_inline_string`, which creates an `inline_string`
representation of an integral or boolean value.

## The `arene::base::inline_string` class template {#the-arene-base-inline_string-class-template}

The `arene::base::inline_string` class template provides a replacement for
`std::string` that does not perform dynamic memory allocation; instead, the
string is always stored within the object itself. This in turn requires that the
internal storage is large enough for the strings that need to be stored. Rather
than the size of that storage being specified by the library, which will always
lead to the buffer being too small for some use cases and too large for others,
it is instead exposed as a template parameter. Every use of
`arene::base::inline_string` must therefore specify the maximum length of the
stored string. Thus `arene::base::inline_string<50>` can store strings up to 50
characters in length, whereas `arene::base::inline_string<1000>` can store
strings up to 1000 characters in length.

The API is similar to `std::string`; The only function not supported is
`reserve`, since the capacity is fixed. However, there are a number of
differences in behavior due to the requirement of non-throwing in
safety-critical systems:

- The constructors which consume a string-like source without a compile-time
  bounded capacity are `ARENE_PRECONDITION` violations if the source string
  cannot fit into the capacity. [`try_construct`](#try_construct) is provided to
  allow construction to fail in a recoverable manner.
- The `substr` and `erase` APIs clamp to the end of the string if the offset is
  outside the range of the string instead of throwing `std::out_of_range`.
- The `insert` and `replace` APIs have `ARENE_PRECONDITION` violations instead
  of throwing `std::out_of_range` if the offset to insert/replace is out of
  range. For the overloads which consume offsets into the insert/replace string,
  it is a no-op if these are out of range of the insert/replacement string.
- Any API in `std::string` which requires resizing the string, and would result
  in a new size above the maximum capacity, result in an `ARENE_PRECONDITION`
  violation instead of throwing `std::length_error`. This includes `assign`,
  `append`, `insert`, `replace`, and `operator=`.
- The `operator+` overloads for `string_view`, `const char*` and `char` have
  `ARENE_PRECONDITION` violations if the resultant `inline_string` would exceed
  capacity of the `inline_string` operand. When `operator+` is invoked with two
  `inline_string` operands, the resulting `inline_string`'s capacity is the
  _sum_ of the input capacities, and thus cannot fail.
- Any API which in `std::string` that consumes an index and has undefined
  behavior if this index is out of range will instead result in an
  `ARENE_PRECONDITION` violation. Ex: `inline_string::operator[]`.

In addition, `arene::base::inline_string` provides a `three_way_compare` member
function, to facilitate \ref compare "three-way comparisons" similar to the
C++20 `<=>` operator.

### Example usage {#example-usage}

If we have some form of `data_record` which contains a _name_, and those names
have a maximum length, then we can use `inline_string<max_name_length>` as the
type of the data member that holds that name.

```{.cpp}
constexpr std::size_t max_name_length = 100;
using name_string = arene::base::inline_string<max_name_length>;

class data_record {
private:
  name_string name_;
  other_data data_;

public:
  /// Construct a data record with a given name and data
  data_record(arene::base::string_view name, const other_data& data):
    name_(name), data_(data) {}

  /// Get the entire name
  const name_string& get_name() const {
    return name_;
  }

  /// Get the initial part of the name, up to the first space, if there is one,
  /// or the whole name otherwise
  name_string get_initial_name_segment() const {
    const auto first_space = name_.find(' ');
    return name_.substr(0, first_space);
  }

  // ...
};
```

Note that we use `arene::base::string_view` as the constructor argument, to
avoid excessive copying: whereas `std::string` can be cheaply moved,
`inline_string` is just as expensive to move as to copy, since the data is held
internally. The use of `string_view` for the parameter also means that _any_
string type can be used when constructing a `data_record`, delaying any type
conversion and resultant copying until the initialization of the member.

### `try_construct` {#try_construct}

As `inline_string` has a maximum capacity, several of its copy/conversion
constructors have preconditions that the input string fit into the capacity of
the `inline_string`:

- Copy-construction from an `inline_string` whose `max_size` is larger than the
  target's `max_size`.
- Construction from a `string_view`
- Construction from a pair of iterators.

To facilitate more ergonomic construction from such inputs where it is not
practical/desireable to ensure the size bounds before hand, the
`inline_string<MaxSize>::try_construct` overload set is provided. These helpers
return `optional<inline_string<MaxSize>>`, where the `optional` is null if the
input string would not fit into `MaxSize`.

### Size-erased reference wrapper {#size-erased-reference-wrapper}

`arene::base::inline_string_reference` and
`arene::base::const_inline_string_reference` wrapper classes are provided to
pass the inline string objects to functions not templated to the maximum size of
the string.

```{.cpp}
void merge(arene::base::inline_string_reference merged,
           arene::base::const_inline_string_reference id,
           arene::base::const_inline_string_reference name);

arene::base::inline_string<36> merged;
arene::base::inline_string<12> id;
arene::base::inline_string<24> name;
// ...
merge(
  arene::base::inline_string_reference(merged),
  arene::base::const_inline_string_reference(id),
  arene::base::const_inline_string_reference(name)
);
```

## `arene::base::to_inline_string` {#arene-base-to_inline_string}

This overloaded function is an equivalent of `std::to_string`, except that it
creates an `arene::base::inline_string` value rather than a `std::string` value.
It is provided for integral and boolean types, and produces a string
representation of the supplied value. The maximum length of the returned string
is set to the maximum length of the string representation of the type: it is
thus `5` for `bool`, since `false` has 5 letters, 10 for `std::uint32_t`
arguments, since the maximum value has 10 digits, and 11 for `std::int32_t`
arguments, to add space for the minus sign for the maximum negative value.

```{.cpp}
auto s = arene::base::to_inline_string(42);
```
