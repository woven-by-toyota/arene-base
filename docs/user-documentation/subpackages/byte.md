<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page byte byte: Byte Handling Facilities

<!-- markdownlint-enable MD041 -->

`arene::base::byte` is a backport of `std::byte` from C++17. It has the same
aliasing properties as `std::byte`, `char` and `unsigned char`, so is ideal for
use as backing storage for pre-allocated buffers for objects constructed at
runtime, and for buffers for data read from a file or sent across the network.

Just as for `std::byte`, `arene::base::byte` does not provide arithmetic
operations, but does provide bitwise operators.

The specifications for `std::byte` is available on
[cppreference](https://en.cppreference.com/w/cpp/types/byte).

The public header is

```{.cpp}
#include "arene/base/byte.hpp"
```

The Bazel target is

```text
//:byte
```

## Converting Between Bytes and Integers {#converting-between-bytes-and-integers}

Conversion to `arene::base::byte` from an integer type can be accomplished via
`arene::base::to_byte`. This API does **not** do any form of bounds checking, it
is equivalent to `static_cast`'ing the value to an `unsigned char`.

Conversion from `arene::base::byte` to an integer type can be accomplished via
`arene::base::to_integer`. This is a backport of `std::to_integer`, whose
specification is available on
[cppreference](https://en.cppreference.com/w/cpp/types/byte).

## The `_byte` Literal {#the-byte-literal}

To assist with constructing `arene::base::byte` values from literals in a C++14
context, a _User Defined Literal_ is provided, `arene::base::literals::_byte`.
This allows direct construction of a `byte`, like so:

```cpp
using arene::base::literals::_byte;

auto some_byte = 0b00001010_byte; // the type of some_byte will be arene::base::byte
```

This can be particularly helpful in unit tests when predefined `byte` values are
needed. The `_byte` literal does **not** perform any bounds checking on the
literal value; it is equivalent to calling `arene::base::to_byte` on the literal
value.
