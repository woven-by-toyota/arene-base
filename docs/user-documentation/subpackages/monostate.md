<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page monostate monostate: Backport of std::monostate

<!-- markdownlint-enable MD041 -->

The `monostate` sub-package provides a facility akin to that provided by the
C++17
[`std::monostate`](https://en.cppreference.com/w/cpp/utility/variant/monostate)
class.

The public header is

```{.cpp}
#include "arene/base/monostate.hpp"
```

The Bazel target is

```text
//:monostate
```

## A type with only one valid value {#a-type-with-only-one-valid-value}

`arene::base::monostate` is a type with only one valid value. It has no
behaviour, and all instances compare equal.

It can be used as a replacement for `void` to indicate "no value": whereas
`void` cannot be stored in a container, and cannot be passed as a function
argument, `arene::base::monostate` can. When this substitution needs to happen
automatically based on an input type, `arene::base::monostate_identity<T>` can
be used: it is the same as `T`, unless `T` is `void` in which case it is
`arene::base::monostate`.

`arene::base::monostate_identity` is used in `arene::base::result` to allow
`void` as a value type: `arene::base::result<void, E>` stores
`arene::base::monostate` as the value type.

`arene::base::monostate` can also be used with `arene::base::variant` when it is
desirable to model "no value";
`arene::base::variant<arene::base::monostate, std::string, int>` can either be a
string, an integer, or "nothing". This is the purpose for which
[`std::monostate`](https://en.cppreference.com/w/cpp/utility/variant/monostate)
was introduced in C++17, and provides a better alternative to
`arene::base::optional<arene::base::variant<std::string, int>>`, since it can be
queried with the same API as `std::string` and `int`, rather than having to
check the status of the `arene::base::optional` wrapper separately.
