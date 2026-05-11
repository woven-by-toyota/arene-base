<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page source_location source_location: A Backport of std::source_location

<!-- markdownlint-enable MD041 -->

The `source_location` sub-package provides a facility akin to that provided by
the C++20
[`std::source_location`](https://en.cppreference.com/w/cpp/utility/source_location)
class.

The public header is

```{.cpp}
#include "arene/base/source_location.hpp"
```

The Bazel target is

```text
//:source_location
```

## Source Locations {#source-locations}

`arene::base::src_line_info` stores information about a line of code: the name
of the source file where it is located, the name of the function where it is
located, and the line number. This is similar to
[`std::source_location`](https://en.cppreference.com/w/cpp/utility/source_location),
except that no column number is provided, as it is not available without the
compiler intrinsics used for `std::source_location`.

The main use case is in association with the `ARENE_GET_SRC_CODE_LOCATION_INFO`
macro, which captures the information about the source file where the macro is
used:

```{.cpp}
void foo(const arene::base::src_line_info& info);

void bar() {
  foo(ARENE_GET_SRC_CODE_LOCATION_INFO());
}
```

Sadly, this cannot meaningfully be used as a default argument in a function
definition (like `std::source_location::current()`), since it would yield the
location of the function **definition**, and not the location of the function
**call**.
