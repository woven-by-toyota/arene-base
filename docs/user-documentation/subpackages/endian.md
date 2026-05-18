<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page endian endian: Endian-Aware Facilities For Manipulating Binary Data

<!-- markdownlint-enable MD041 -->

When reading or writing binary data that is to be shared between applications,
it is important to define the byte order for data types that occupy more than
one byte, so that the applications can read the correct value for the data type
even if they have different native byte orders.

The public header is

```{.cpp}
#include "arene/base/endian.hpp""
```

The Bazel target is

```text
//:endian
```

## Determining the endianness of the current system {#determining-the-endianness-of-the-current-system}

The `arene::base::endian` enumeration is a back-port of the C++20 `std::endian`
enumeration. The `arene::base::endian::native` value can be compared against
`arene::base::endian::little` and `arene::base::endian::big` to determine if the
current target platform is big-endian or little-endian.

The specifications for `std::endian`is available on
[cppreference](https://en.cppreference.com/w/cpp/types/endian).

## Reading and writing data with a specific endianness {#reading-and-writing-data-with-a-specific-endianness}

The various overloads of `arene::base::read_little_endian` and
`arene::base::read_big_endian` facilitate reading values that are stored in
little-endian or big-endian byte order respectively. Similarly, the overloads of
`arene::base::write_little_endian` and `arene::base::write_big_endian`
facilitate writing values in little-endian or big-endian byte order
respectively. These are provided for:

- integral types
- floating-point types except `long double`.
- enumeration types

`long double` is not supported because it requires additional runtime support on
some platforms.

Usage is simple: just pass in a suitably-sized `arene::base::span` to read, or
the value to be written and a suitably-sized `arene::base::span` to write, as in
the following code.

```{.cpp}
void write(std::span<arene::base::byte> buffer) {
  int my_val = create_value();
  arene::base::write_little_endian<int>(my_val, buffer.first<sizeof(int)>());
}

void read(std::span<const arene::base::byte> buffer) {
  int my_val = arene::base::read_little_endian<int>(
    my_val, buffer.first<sizeof(int)>());
  do_something_with(my_val);
}
```

Note that `arene::base::read_little_endian`, `arene::base::read_big_endian`,
`arene::base::write_little_endian` and `arene::base::write_big_endian` require
that the provided spans have exactly the right size. The example code above uses
`buffer.first<sizeof(int)>()` to obtain a fixed-size span for the beginning of
the specified buffer. `buffer.subspan` or `buffer.last` could also be used to
obtain a fixed-size span for other parts of the buffer, or `read` and `write`
could accept fixed-size spans directly.

## Swapping the byte order of an integer {#swapping-the-byte-order-of-an-integer}

If you're manually processing data with a specific endianness, then you may need
to swap the order of bytes in an integral value, depending on the value of
`arene::base::endian::native`. The `arene::base::byte_swap` function provides
that facility: `arene::base::byte_swap(some_integral)` yields an integral value
of the same type as `some_integral`, but with the reverse byte order. Thus
`0x1234` becomes `0x3412` and `0x12345678` becomes `0x78563412`. For signed
values this may change the sign.
