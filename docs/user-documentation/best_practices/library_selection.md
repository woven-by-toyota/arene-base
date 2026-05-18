<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page library_selection Selecting between Arene Base and the Standard Library

<!-- markdownlint-enable MD041 -->

Arene Base is a loosely-connected collection of low-level utility features for
use in C++ programs, similar to the C++ Standard Library. It is designed to
supplement the C++ standard library at a particular version (currently C++14)
and to fill in gaps where only a limited standard library is available, or one
is not available at all. This includes new data structures and algorithms,
backports of features from later standard library versions and
re-implementations of standard library features.

Additionally, Arene Base provides its own implementation of a subset of the
C++14 standard library.

This can result in uncertainty in which implementation of a data structure or
algorithm to use. While there is no single answer to this issue, this guide
presents some guidance on what factors to consider when choosing an appropriate
implementation.

## Overall Guidance {#overall-guidance}

Under the `arene::base` namespace, Arene Base provides features in three main
categories:

1. Features not present in any C++ standard.
2. Backports from later C++ versions.
3. Re-implementations of features from the targeted C++ version.

These features are intended to be built on top of an existing C++ standard
library.

Features in category 1 are not currently available from any current C++ standard
library (although may be in the future). An example of this is the
`arene::base::pointer` package. For features in this category, using the Arene
Base implementation is the only choice.

Features in category 2 attempt to match the specification of the feature as
closely as possible given the constraints of the C++14 language. An example of
this is the `arene::base::span` class, which is provided in the standard library
in C++20. For features in this category, use the standard library version if
available in the target envrionment, otherwise use the Arene Base
implementation.

Features in category 3 re-implement C++14 standard library features in order to
make a change to the feature's specification. Typically this is to backport a
change to a feature from later C++ versions, such as making a function
`constexpr`, `noexcept`, or non-allocating. For example, `arene::base::next` is
a re-implementation of `std::next` that is made `constexpr` (where `std::next`
is not `constexpr` until C++17). For features in this category, prefer the Arene
Base provided implementation if Arene Base is already a dependency.

## Standard C++ Library {#standard-cpp-library}

Arene Base also provides an implementation of a subset of the the standard
library under the `std::` namespace. The Arene Base stdlib should only be used
if there is no other implementation available. This might be the case in a
safety-critical real-time environment where the toolchain does not provide a
built-in standard library, or where the safety manual disallows use of the
standard library at the target ASIL level.

The Arene Base standard library is not a complete re-implementation of the C++14
standard. It only provides functionality needed by Arene Base itself or from
specific customer requests. If there is already an implementation of
functionality provided under `arene::base` (e.g. `rotate`), `stdlib` might not
provide another implementation and the `arene::base` implementation should be
used.

See \ref stdlib for more details.

## Data Structures {#data-structures}

The C++ standard library provides the Containers library as a generic collection
of class templates and algorithms that allow programmers to easily implement
common data structures. There are three classes of containers: sequence
containers, associative containers and unordered associative containers, each of
which is designed to support a different set of operations.

Containers manage the storage space allocated for their elements and offer
member functions to access these elements, either directly or via iterators.
Generally, containers provided by the C++ Standard Library utilize heap
allocation for storing their elements.

Arene Base offers an alternative set of containers that are optimized for
automotive applications. These containers do not employ heap allocation and
often have specified preconditions for their use.

The choice between Standard Library containers and Arene Base containers depends
on the specific application. Typically, Standard Library containers are suitable
for non-safety-critical environments where dynamic memory allocation is
permissible. In contrast, Arene Base containers are designed for real-time,
safety-critical environments where dynamic memory allocation is prohibited.

The remainder of this section is organized by Standard Library containers and
examines the alternatives provided by Arene Base. For Standard Library
containers not discussed here, Arene Base currently does not offer an
alternative container.

### Flowchart {#flowchart}

\image html container_selection_flowchart.png

### array {#array}

Arene Base offers `arene::base::array`, which is equivalent to `std::array` but
is designed to be `constexpr`. Additionally, it is a "checked" type, meaning
that operations which would result in undefined behavior (UB) in `std::array`,
such as accessing an index out of bounds using `operator[]`, are transformed
into `ARENE_PRECONDITION` violations.

Additionally, consider [using span](use_span.md) when appropriate.

It is recommended to use `arene::base::array` instead of `std::array` in all
cases where the latter is not required by a pre-existing API.

### string {#string}

Arene Base offers `arene::base::inline_string`, a replacement for `std::string`
that does not use dynamic memory allocation. Additionally, it provides the view
types `arene::base::string_view` and `arene::base::null_terminated_string_view`,
which are intended for use as function parameters when you need to consume a
string but do not care about its storage location.

It is recommended to use `arene::base::inline_string` when dynamic memory is not
available.

### vector {#vector}

Arene Base provides `arene::base::inline_vector`, similar to `std::vector`.
Elements are stored "inline" in the vector itself rather than being
heap-allocated. This design makes the class suitable for situations where
dynamic allocation is undesirable and the size of the container may change. If
the container will always contain a fixed number of elements, use
`arene::base::array` instead. The fixed maximum capacity ensures that there is
sufficient space for a specified number of elements, preventing unpredictable
runtime errors.

It is recommended to use `arene::base::inline_vector` when dynamic memory is not
available.

### map {#map}

Arene Base offers `arene::base::inline_map`, similar to `std::map`. Elements are
stored "inline" in the map itself rather than being heap-allocated. This design
makes the class suitable for situations where dynamic allocation is undesirable.
The fixed maximum capacity ensures that there is sufficient space for a
specified number of elements, preventing unpredictable runtime errors.

It is recommended to use `arene::base::inline_map` when dynamic memory is not
available.

### set {#set}

Arene Base provides `arene::base::inline_set`, similar to `std::set`. Elements
are stored "inline" in the set itself rather than being heap-allocated. This
design makes the class suitable for situations where dynamic allocation is
undesirable. The fixed maximum capacity ensures that there is sufficient space
for a specified number of elements, preventing unpredictable runtime errors.

It is recommended to use `arene::base::inline_set` when dynamic memory is not
available.

### function {#function}

Arene Base offers `arene::base::inline_function`, an implementation of a
type-erased invocable similar to `std::function`. `inline_function` has the
distinction that it never performs dynamic allocation. Instead, it uses an
internal buffer for storage, and attempts to store invocables that exceed the
internal buffer's size will result in a compilation error.

The decision on which function type to use is highly dependent on the
cirucumstance. Some guidelines for the possible options are:

- For maximum usability and performance, make your API a template, and accept
  any invocable.
- Using a function pointer means you cannot pass stateful lambdas, or classes
  with a function call operator. Also, adds an indirection vs a template arg.
- Using `arene::base::function_ref` is ideal if you are not intending to store
  the invocable, as nothing is copied. Storing it for later will lead to
  undefined behaviour in many cases (e.g. with lambdas) as the referenced
  invocable will have been destroyed. Still adds an indirection, but only copies
  a pointer.
- Using `arene::base::inline_function` allows storing any movable invocable with
  the right signature. Copies the invocable into storage, which must be big
  enough. No dynamic memory allocation. Adds an indirection vs a template arg.
- Using `std::function` allows storing any movable invocable with the right
  signature. Copies the invocable into dynamically-allocated storage. Adds an
  indirection vs a template arg.

### Intrusive Containers {#intrusive-containers}

Intrusive containers serve as alternatives to standard library containers by not
storing the values of the elements provided by the user. Instead, the linking
structure of the container is stored in the elements themselves, and the
container merely provides a standardized way to traverse it.

Intrusive containers offer benefits such as improved performance and memory
efficiency but require careful memory management from developers. The containers
are also unbounded: there is no maximum capacity, and the container itself does
not need to hold storage for the elements.

See more information in the
[intrusive containers documentation](../subpackages/intrusive.md).

#### queue {#queue}

Arene Base offers `arene::base::intrusive::queue`, a singly-linked list-based
intrusive queue data structure. It forms a logical FIFO queue, allowing elements
to be added at the end and removed (popped) from the front.

This is similar to a `std::queue` with `std::forward_list` as the `Container`
template parameter.

It is recommended to use `arene::base::intrusive::queue` when dynamic memory is
not available.

#### priority queue {#priority-queue}

Arene Base provides `arene::base::intrusive::priority_queue`, a
max-heap-tree-based intrusive priority queue data structure. It forms a logical
priority queue, allowing elements to be added and removed while providing access
to the element with the highest priority, as determined by the comparator.

It is recommended to use `arene::base::intrusive::priority_queue` when dynamic
memory is not available.

## Algorithms {#algorithms}

Both the C++ Standard Library and Arene Base provide a variety of components for
performing algorithmic operations on containers and other sequences.

Just as with the data structures, the Arene Base components provide features not
present in the C++14 Standard Library:

- Algorithms not present in any version of the C++ Standard Library.
- Backports of algorithms not present in C++14, but present in later versions.
- `constexpr` versions of C++14 algorithms, where the C++14 implementation is
  not `constexpr`.
- Preconditions are checked using `ARENE_PRECONDITION` to check the
  preconditions are met, where this is possible in a reasonable fashion. For
  example, indices are always checked for being in-range, but a precondition
  that a sequence is in sorted order would not be checked.
- The Arene Base implementation of algorithms does not use dynamic memory
  allocation, even in cases where the C++ Standard Library implementation is
  permitted to do so.

Where there is an Arene Base implementation of a given algorithm, it is
recommended to use that in preference to the equivalent version from the C++
Standard Library in all cases. For this reason, the Arene Base implementation of
the C++ Standard Library often does not contain implementations of algorithms
where there is a version of that algorithm present in the `arene::base`
namespace.

### Flowchart {#flowchart}

This flowchart will help you choose the appropriate algorithm for a given
purpose.

\image{anchor:algorithm-selection} html algorithm-selection-flowchart.png
"Selecting an algorithm"

The following sections describe the Arene Base algorithms, and how they differ
from the equivalent C++ Standard Library versions.

### all_of {#all_of}

The `all_of` algorithm checks if all of the supplied values are `true`, or all
the results of applying the given predicate to the each of the supplied values
are `true`.

The Arene Base implementation of `arene::base::all_of` is usable in `constexpr`
contexts in C++14, whereas the C++ Standard Library implementation is not usable
in `constexpr` contexts prior to C++20.

In addition, the Arene Base implementation provides an overload that takes any
number of `bool` parameters, and an overload that takes a
`std::initializer_list<bool>` holding the values to check. These overloads are
also usable in a `constexpr` context.

### any_of {#any_of}

The `any_of` algorithm checks if any of the supplied values are `true`.

The Arene Base implementation of `arene::base::any_of` is usable in `constexpr`
contexts in C++14, whereas the C++ Standard Library implementation is not usable
in `constexpr` contexts prior to C++20.

In addition, the Arene Base implementation provides an overload that takes any
number of `bool` parameters, and an overload that takes a
`std::initializer_list<bool>` holding the values to check. These overloads are
also usable in a `constexpr` context.

Arene Base does not provide an overload that takes a predicate.

### clamp {#clamp}

The `clamp` algorithm ensures that a value is within a specified range. The
return value is equal to the source if it is in range, the lower limit if the
source is below the lower limit, and the upper limit if the source is above the
upper limit.

The Arene Base implementation of `arene::base::clamp` is usable in `constexpr`
contexts, and available even in C++14, whereas the C++ Standard Library
implementation is not available at all prior to C++17.

### copy {#copy}

The `copy` algorithm copies a series of elements from a source range to a
destination range.

The Arene Base implementation of `arene::base::copy` is usable in `constexpr`
contexts in C++14, whereas the C++ Standard Library implementation is not usable
in `constexpr` contexts prior to C++20.

### copy_if {#copy_if}

The `copy_if` algorithm copies a series of elements that satisfy a predicate
from a source range to a destination range.

The Arene Base implementation of `arene::base::copy_if` is usable in `constexpr`
contexts in C++14, whereas the C++ Standard Library implementation is not usable
in `constexpr` contexts prior to C++20.

### equal {#equal}

The `equal` algorithm compares two ranges to see if they have the same size and
the corresponding elements from each range are equal.

The Arene Base implementation of `arene::base::equal` is usable in `constexpr`
contexts in C++14, whereas the C++ Standard Library implementation is not usable
in `constexpr` contexts prior to C++20.

### fill {#fill}

The `fill` algorithm assigns the same value to every element in a sequence.

The Arene Base implementation of `arene::base::fill` is usable in `constexpr`
contexts in C++14, whereas the C++ Standard Library implementation is not usable
in `constexpr` contexts prior to C++20.

### find {#find}

The `find` algorithm returns the iterator referencing the first element in the
supplied range that is equal to the given element, or the end of the range if
there is no equal element.

The Arene Base implementation of `arene::base::find` is usable in `constexpr`
contexts in C++14, whereas the C++ Standard Library implementation is not usable
in `constexpr` contexts prior to C++20.

### find_if {#find_if}

The `find_if` algorithm returns the iterator referencing the first element in
the supplied range for which invoking the supplied predicate returns `true`, or
the end of the range if there is no such element.

The Arene Base implementation of `arene::base::find_if` is usable in `constexpr`
contexts in C++14, whereas the C++ Standard Library implementation is not usable
in `constexpr` contexts prior to C++20.

### iter_swap {#iter_swap}

The `iter_swap` algorithm swaps the elements referred to by two iterators.

The Arene Base implementation of `arene::base::iter_swap` is usable in
`constexpr` contexts in C++14, whereas the C++ Standard Library implementation
is not usable in `constexpr` contexts prior to C++20.

### lexicographical_compare {#lexicographical_compare}

The `lexicographical_compare` algorithm compares two ranges, returning `true` if
the first is before the second in a lexicographical ordering, `false` otherwise.
The ordering is checked using the supplied comparator, or `std::less<>` if none
is supplied.

The Arene Base implementation of `arene::base::lexicographical_compare` is
usable in `constexpr` contexts in C++14, whereas the C++ Standard Library
implementation is not usable in `constexpr` contexts prior to C++20.

### lexicographical_compare_three_way {#lexicographical_compare_three_way}

The `lexicographical_compare_three_way` algorithm compares two ranges, returning
`arene::base::string_ordering` to indicate if the first is less than the second,
equal to the second, or greater than the second, in a lexicographical ordering.
The ordering is checked using the supplied three-way comparator, or
`arene::base::compare_three_way` if none is supplied.

The Arene Base implementation of
`arene::base::lexicographical_compare_three_way` is usable in `constexpr`
contexts in C++14, whereas the C++ Standard Library implementation is not
available at all prior to C++20.

The Arene Base implementation always returns `arene::base::strong_ordering`,
since Arene Base doesn't currently provide a weak ordering comparison result
type, whereas the C++20 standard library implementation returns
`std::strong_ordering` or `std::weak_ordering` as appropriate for the
comparison.

### move {#move}

The `move` algorithm moves a series of elements from a source range to a
destination range.

The Arene Base implementation of `arene::base::move` is usable in `constexpr`
contexts in C++14, whereas the C++ Standard Library implementation is not usable
in `constexpr` contexts prior to C++20.

### move_backward {#move_backward}

The `move_backward` algorithm moves a series of elements from a source range to
a destination range. The ranges are iterated in reverse, so the last element is
moved first.

The Arene Base implementation of `arene::base::move_backward` is usable in
`constexpr` contexts in C++14, whereas the C++ Standard Library implementation
is not usable in `constexpr` contexts prior to C++20.

### rotate {#rotate}

The `rotate` algorithm rotates a range so that the specified "middle" element is
now the first element, and the elements that were originally prior to the
"middle" element have been moved to the end.

The Arene Base implementation of `arene::base::rotate` is usable in `constexpr`
contexts in C++14, whereas the C++ Standard Library implementation is not usable
in `constexpr` contexts prior to C++20.

### sort {#sort}

The `sort` algorithm sorts a range of elements into order, according to the
supplied comparator, or `std::less<>` if no comparator is provided.

The Arene Base implementation of `arene::base::sort` is usable in `constexpr`
contexts in C++14, whereas the C++ Standard Library implementation is not usable
in `constexpr` contexts prior to C++20.

### swap_ranges {#swap_ranges}

The `swap_ranges` algorithm swaps the elements from one range with the
corresponding elements of a second range.

The Arene Base implementation of `arene::base::swap_ranges` is usable in
`constexpr` contexts in C++14, whereas the C++ Standard Library implementation
is not usable in `constexpr` contexts prior to C++20.

### transform {#transform}

The `transform` algorithm applies an invocable to each element in a range, and
stores the result of that invocation in the corresponding element of the
destination range.

The Arene Base implementation of `arene::base::transform` is usable in
`constexpr` contexts in C++14, whereas the C++ Standard Library implementation
is not usable in `constexpr` contexts prior to C++20.

The Arene Base implementation provides additional overloads which applies the
invocable to the elements of a `std::tuple`, `std::pair` or `std::array` rather
than an iterator range, returning a new `std::tuple`, `std::pair` or
`std::array` as appropriate, holding the results of those invocations.
