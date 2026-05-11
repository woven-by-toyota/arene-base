<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page span span: A Non-Owning View Onto Contiguous Data

<!-- markdownlint-enable MD041 -->

`arene::base::span` is a backport of
[`std::span`](https://en.cppreference.com/w/cpp/container/span) with additional
bounds-checking to avoid undefined behaviour.

The public header is

```{.cpp}
#include "arene/base/span.hpp"
```

The Bazel target is

```text
//:span
```

## Views Over Contiguous Data {#views-over-contiguous-data}

`arene::base::span` provides a view over a contiguous sequence of objects. It
takes 2 template parameters; the first is the type of the elements, and the
second is the _extent_ of the sequence, either `arene::base::dynamic_extent` or
the number of elements in the sequence. If the _extent_ is omitted, then it
defaults to `arene::base::dynamic_extent`. If the _extent_ is the number of
elements, then any objects of that span type **always** refer to a sequence with
that number of elements. If the _extent_ is `arene::base::dynamic_extent`, then
the number of elements is a dynamic property, and may vary between instances of
the span type.

Thus `arene::base::span<int, 5>` is a fixed-size span of 5 elements of type
`int`, whereas `arene::base::span<int>` is shorthand for
`arene::base::span<int, arene::base::dynamic_extent>`, which is a dynamic-sized
span that covers any number of elements of type `int`.

The number of elements in a given `arene::base::span` object is always passed to
the constructor, either explicitly, or implicitly (such as via the bounds of an
array, or the size of a container). For fixed-size spans, this is checked, and
it is a [precondition violation](\ref contracts) if it does not match the fixed
size. The number of elements referenced by a span can be obtained with the
`size` member function. For a fixed-size span, this always returns the fixed
size. For a dynamic-sized span, this returns the actual number of elements
referenced by this particular span object.

Elements of the sequence referenced by a span can be accessed by the indexing
operator (\ref arene::base::span::operator[]). Attempting to access an element
beyond the end of the span is a [precondition violation](\ref contracts).

\note The indexing operator returns a reference to the element type of the span,
independent of the `const`-ness of the `span` itself: indexing into
`const arene::base::span<int>` returns an `int&`, while indexing into
`arene::base::span<const int>` returns a `const int&`.

`arene::base::span` is also a valid range, with iterators, for use with
range-based `for` and standard library algorithms.

`arene::base::span` is a backport of
[`std::span`](https://en.cppreference.com/w/cpp/container/span)
