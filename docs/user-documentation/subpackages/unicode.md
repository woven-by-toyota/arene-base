<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page unicode unicode: Facilities For Manipulating Unicode Strings

<!-- markdownlint-enable MD041 -->

Arene base provides facilities for working with Unicode strings.

The public header is:

```{.cpp}
#include "arene/base/unicode.hpp"
```

The Bazel target is

```text
//:unicode
```

# Introduction {#introduction}

The `unicode` sub-package provides facilities to support the use of UTF-8
strings. Currently this is just `arene::base::is_valid_utf8`, which checks if a
string is a valid sequence of characters encoded in UTF-8.

## `arene::base::is_valid_utf8` {#arene-base-is_valid_utf8}

This function checks if a string is a valid sequence of characters encoded in
UTF-8. It accepts an `arene::base::string_view`, so just about any string value
can be passed as an argument. It returns `true` if and only if the supplied
string is valid UTF-8. It detects invalid UTF-8 character sequences, truncated
UTF-8 sequences, and sequences that map to invalid Unicode characters.

```{.cpp}
if(!arene::base::is_valid_utf8(some_string)) {
  report_error();
}
```
