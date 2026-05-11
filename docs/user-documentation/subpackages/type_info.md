<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page typeinfo type_info: Additional Information About Types

<!-- markdownlint-enable MD041 -->

The type info sub-package provides additional information about a type beyond
those provided by [the type_traits package](\ref type_traits).

The public header is

```{.cpp}
#include "arene/base/type_info.hpp"
```

The Bazel target is

```text
//:type_info
```

## Additional Information About Types {#additional-information-about-types}

The `type_info` sub-package provides facilities for obtaining additional
information about a type.

### Getting The Name Of A Type As A String {#getting-the-name-of-a-type-as-a-string}

`arene::base::type_name_v` allows you to obtain an `arene::base::inline_string`
which holds the name of that type. For example,
`arene::base::type_name_v<arene::base::result<int,my_error>>` may yield an
`arene::base::inline_string` holding `"arene::base::result<int,my_error>"`.

This is similar to `typeid(T).name()`, except that `typeid` requires RTTI
enabled, frequently yields a mangled name rather than a human-readable one, and
cannot be used in constant expressions.

The precise form of the string is compiler dependent, especially with regard to
types in anonymous namespaces, or integer types other than `int`
(`arene::base::type_name_v<unsigned short>` might be `"unsigned short"` or
`"short unsigned int"` depending on the compiler). However, these strings should
be clearly identifiable to people, and yield a unique string for every type in a
translation unit.

`arene::base::type_name_v` is declared `constexpr`, so the strings can be used
in constant expressions.

Example usage:

\snippet docs/examples/type_name_examples.cpp print_type_name_example

Here, `print_with_type_name` prints the name of the type of the argument, as
well as the value.

\snippet docs/examples/type_name_examples.cpp ordered_pair_example

Here, `ordered_pair` uses the type name to order the template arguments to
ensure that `ordered_pair<A,B>` is the same as `ordered_pair<B,A>`, regardless
of where the types are used, and without imposing any requirements on `A` and
`B`.
