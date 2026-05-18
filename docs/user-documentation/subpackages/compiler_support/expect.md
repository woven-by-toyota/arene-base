<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page compilersupportexpect Facilities For Optimizing Expected Branches

<!-- markdownlint-enable MD041 -->

Arene Base provides a helper for marking that a boolean expression is _expected_
to be `true` most of the time. The public header is:

```cpp
#include "arene/base/compiler_support/expect.hpp
```

## Using ARENE_EXPECT to optimize conditionals {#using-arene_expect-to-optimize-conditionals}

The `ARENE_EXPECT` macro takes a boolean expression as an argument, which is
_expected_ to be `true` most of the time. This expectation is passed to the
compiler, so that it may use that information for code generation and
optimization.

For example, in the following code, the `optional_configuration` parameter to
the function `foo` is not expected to be set very often, so most calls will have
an empty optional. `ARENE_EXPECT` is used to convey that expectation to the
compiler.

\snippet docs/examples/expect_examples.cpp expect_usage_example

\note If the _expectation_ is incorrect, the compiler will potentially generate
sub-optimal code. Therefore, care must be taken to ensure that the _expected_
branch is indeed taken more often than not.
