<!--
Copyright 2026, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page bit bit: Bit Manipulation Facilities

<!-- markdownlint-enable MD041 -->

The `bit` subpackage provides backports of the C++20 `<bit>` header, along with
other useful bit manipulation utilities.

The public header is

\snippet docs/examples/subpackages/bit_examples.cpp include_header

The Bazel target is

```text
//:bit
```

## `bit_cast` {#bit_cast}

`arene::base::bit_cast<To>(from)` returns a value of type `To` whose object
representation is bit-identical to that of `from`.

The specification mirrors `std::bit_cast`, and the call participates in overload
resolution only when:

- `sizeof(To) == sizeof(From)`
- `std::is_trivially_copyable<To>::value` is `true`
- `std::is_trivially_copyable<From>::value` is `true`

\snippet docs/examples/subpackages/bit_examples.cpp basic_usage

### `constexpr` Availability {#constexpr-availability}

`bit_cast` is usable in a constant expression only when the compiler provides
`__builtin_bit_cast` (Clang 9+, GCC 11+). On toolchains without it (e.g. GCC 8),
the implementation falls back to `std::memcpy` and therefore cannot appear in a
`constexpr` context. Query this at compile time via the
`ARENE_HAS_CONSTEXPR_BIT_CAST` platform support macro:

\snippet docs/examples/subpackages/bit_examples.cpp constexpr_availability
