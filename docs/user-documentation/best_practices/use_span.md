<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page use_span Use arene::base::span instead of a pointer and length

<!-- markdownlint-enable MD041 -->

It is a common pattern in code avoiding dynamic memory allocation is to have the
allocation of a region of storage separate from the code that reads or writes to
it. The address of this storage buffer, and the number of elements to read or
write, is then passed into or returned from APIs that access it.

If the pointer to the buffer and length are passed around separately, then this
is error-prone:

- Length arguments can be confused with other integral arguments, especially if
  more than one buffer is being passed,
- Buffer over-run checks have to be manually written for each access, which are
  easily forgotten,
- Iterating over the elements in the buffer requires pointer arithmetic, which
  is against our coding standards,
- It is easy for the supplied size to get out-of-sync with the real size of the
  storage, either due to a typo, or due to an incomplete change,
- In particular, the pointer could be `nullptr`, even if the size is non-zero,
  leading to Undefined Behaviour from dereferencing a null pointer.

`arene::base::span` provides a safer alternative: it combines the pointer and
length into a single object, so they are tied together. Access to the elements
via the `arene::base::span` then does automatic range-checking for the indexing
operator, preventing buffer over-runs. In addition, `arene::base::span` is a C++
_range_, so can be used with range-based `for` loops to iterate over the
elements, or `begin` and `end` iterators can be obtained, to use with standard
library algorithms. Finally, `arene::base::span::subspan` provides a
bounds-checked way to obtain an `arene::base::span` that refers to a portion of
the original range of elements.

> **Guideline: Replace uses of pointer+length with `arene::base::span`**

## Examples {#examples}

### Reading From a Buffer {#reading-from-a-buffer}

You can use `arene::base::span` with a `const` element type as a replacement for
read-only input buffers.

Before:

\snippet docs/examples/use_span_examples.cpp original_read_example

This loops over the items in the buffer in turn, but there is no
bounds-checking. If the loop condition was mis-typed as `index <= count`, then
the code will read off the end of the buffer.

After:

\snippet docs/examples/use_span_examples.cpp indexed_read_example

With `arene::base::span`, the size is directly associated with the pointer, so
the accesses are bounds-checked. Now, if the loop condition was mis-typed as
`index <= items.size()`, then the buffer over-run will be detected, and the
program will abort with a [precondition violation](\ref contracts).

Note also that `some_caller` no longer needs to specify the count, because the
entire array is being processed, and the `arene::base::span` can deduce the size
of the span from the array bounds.

Given that `print_items` doesn't actually use the `index` other than for
indexing into the span, the code can be improved further, and use the
range-based `for` loop instead, thus eliminating the indexing entirely:

\snippet docs/examples/use_span_examples.cpp for_loop_read_example

### Returning A Portion of a Buffer {#returning-a-portion-of-a-buffer}

Functions that fill a buffer, like `read` and `snprintf`, need some mechanism
for communicating how much of the buffer was filled. Such functions can end up
using a combination of the return value and "out" parameters to communicate the
desired data. This can lead to complications in the API, and a separation of the
size of the valid data from the pointer to the valid data.

`arene::base::span` can also be used to simplify this use case. For example, the
`arene::base::filesystem::file_handle::sequential_read` function reads data from
a file into a supplied buffer, and returns an `arene::base::result` holding an
`arene::base::span` on success. The returned `arene::base::span` covers the
region of the supplied buffer that was populated by the call.

\snippet docs/examples/use_span_examples.cpp return_span_example

Here, the only reference to `buffer` is in the call to `sequential_read`. The
code that handles the result does not need to know where the buffer is, since it
is encapsulated inside the returned `arene::base::span`. This reduces the
potential for errors, and simplifies the code, compared to equivalent code using
`read`.

The implementation of `sequential_read` also demonstrates how
`arene::base::span::subspan` can be used to obtain a span referencing the
populated data:

<!-- prettier-ignore-start -->
\dontinclude arene/base/filesystem/file_handle.cpp
\skip file_handle::sequential_read
\until }
\until }
<!-- prettier-ignore-end -->
