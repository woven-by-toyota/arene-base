<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page non_null_ptr Non Null Pointer

<!-- markdownlint-enable MD041 -->

## Introduction {#introduction}

A common source of defects in code that deals with pointers, both raw and fancy,
is the accidental violation of assumptions made in a system that a pointer is
not null. At best, the requirement that a pointer not be null is documented in
its API documentation. However this requirement is not validated by any static
analysis. It may be validated by runtime checks, however this is not robust:
developers often forget them, or may assume that other logic has already checked
the invariant when this is not the case.

A more robust solution to this problem is to encode the requirement that a
pointer not be null into the type system. With such a type:

1. APIs self-document their invariants/preconditions.
1. Developers do not have to make assumptions on if an invariant/precondition
   has been met. It is always safe to assume the pointer is not null.
1. An attempt to pass the literal `nullptr` can be hoisted to a compile-time
   error.
1. An attempt to pass a pointer that is nullptr can be uniformly guarded against
   at runtime without developer action.

The name of this type is `non_null<P>`, where `P` is any "pointer-like" type for
which a comparison to `nullptr` would be meaningful.

## Design {#design}

### Template Parameter {#template-parameter}

The type consumes a single template parameter, `P`. This represents the
_pointer-like_ type to which the `non_null` invariant is to be enforced. To
qualify as _pointer-like_, the type must at minimum:

1. Define `operator->()` and `operator*()`.
1. Define `operator!=(const P&, std::nullptr_t)` or have that operation be valid
   through implicit conversion.

### Ownership of the Held Pointer {#ownership-of-the-held-pointer}

`non_null<P>` models `P`, and thus owns the pointer it holds.

### Construction {#construction}

As there is no reasonable default for a pointer which cannot be null,
`non_null<P>` is not default-constructible and is thus a _non-regular-type_.

`non_null<P>` can be constructed/assigned from any type `U` which is convertible
to `P`, as well as any `non_null<U>` where `U` is convertible to `P`. If `P` is
copyable, then `non_null<P>` is copyable. Construction must be explicit as `P`
may be `nullptr` but `non_null<P>` may not and thus the conversion is not
inherently valid.

Construction from the literal `nullptr` is `delete`'d. In addition, attempting
to construct a `non_null<P>` from any value which compares equal to `nullptr` is
an `ARENE_PRECONDITION` violation.

Examples of constructing `non_null`:

```cpp
T some_t;
auto from_raw_ptr = non_null<T*>{&some_t};

auto from_sptr = non_null<std::shared_ptr<T>>{std::make_shared<T>()};

auto from_uptr = non_null<std::unique_ptr<T>>{std::make_unique<T>()};

// this is a compiler error
auto from_nullptr_literal = non_null<T*>{nullptr};

// this is a runtime abort
T* a_null_value = nullptr;
auto from_nullptr_value = non_null<T*>{a_null_value};
```

Note that in practice, you rarely need to construct `non_null` directly.
Instead, there are various factory helpers, discussed in detail
[below](#factory-functions). In addition, there are
[convenience aliases](#convenience-aliases) for specifying the type in a less
verbose manner when the type must be explicitly specified.

### Member Types {#member-types}

`non_null<P>` provides the following member types, for compatibility with
various standard pointer-trait facilities:

- `non_null<P>::element_type`, which is `std::pointer_traits<P>::element_type`.

In addition, it defines the following:

- `non_null<P>::held_pointer`, which is `P`.
- `non_null<P>::pointer`, which is the type returned by `non_null<P>::get()`.

### API Uniformity with stdlib Smart Pointers {#api-uniformity-with-stdlib-smart-pointers}

`non_owning_ptr` provides general API uniformity with existing stdlib smart
pointer types:

- `auto non_null<P>::get()->pointer`: returns a raw pointer to the pointed-to
  element.
- `void non_null<P>::reset(U ptr)`: replaces the held pointer with a new pointer
  of any type `U` that is convertible to `held_pointer` and that is not
  `nullptr`.

### Access to the Held Pointer {#access-to-the-held-pointer}

As `non_null<P>::get()` models `P::get()`, a separate interface is needed to
obtain a reference to the underlying pointer. This is provided by the
`non_null<P>::unwrap()` method. For fancy pointers, `unwrap()` provides `const`
and `rvalue` overloads that return `const P&` and `P&&` respectively. For raw
pointers, `unwrap()` returns `P`.

Examples:

```cpp
auto raw_ptr_non_null = arene::base::make_non_null(some_t_ptr);

// The type of unwrapped_raw will be T*
decltype(auto) unwrapped_raw = raw_ptr_non_null.unwrap();

auto fancy_ptr_non_null = arene::base::make_shared_ptr<T>(arg1, arg2);

// the type of unwrapped_fancy will be const std::shared_ptr<T>&
decltype(auto) unwrapped_fancy = fancy_ptr_non_null.unwrap();

// the type passed to foo() will be std::shared_ptr<T>&&
foo(std::move(fancy_ptr_non_null).unwrap());
```

Note that it is not possible to obtain `P&`; doing so would allow users to
accidentally violate the invariant of `non_null` through direct assignment to
the underlying pointer.

### Conversion to Other Types {#conversion-to-other-types}

As `non_null<P>` models `P`, explicit conversion to `bool` is supported, as well
as the `!` operator, and has meaning equivalent to `P`.

In addition, for ergonomic convenience `non_null<T>` supports:

- _implicit_ conversion to `const P&` and `P&&` for fancy pointers, and `P` for
  raw pointers. This allows it to be "dropped in" to existing codebases without
  creating high water marks for converting all downstream users to
  `non_null<P>`.
- _explicit_ conversion from `const non_null<P>&` to `U` if
  `std::is_constructible<U, const P&>`.
- _explicit_ conversion from `non_null<P>&&` to `U` if
  `std::is_constructible<U, P&&>` and `P` is a fancy pointer.

Here's a simple example showing how this functionality allows incremental
changes:

```cpp

// Our structure that has been updated to use non_null
struct configuration {
    non_null_unique_ptr<T> resource; // this used to be std::unique_ptr<T>
};

// A legacy API that consumes a nullable unique pointer
void legacy_api(std::unique_ptr<T> dependency);

// A legacy API that was written when configuration contained a nullable unique_ptr
bool legacy_entrypoint(configuration config) {
    // this branch will never be taken, but is still valid
    if(!config.resource) {
        return false;
    }
    // the implicit conversion allows this call to continue to work without changes
    legacy_api(std::move(config.resource));
    return true;
}
```

### Comparison {#comparison}

As `non_null<P>` models `P`, it supports 6-way comparison with semantics
equivalent to comparing `P`.

For the comparisons to other types, the `non_null` type is accepted as a
template parameter `NN`, which is constrained to be exactly the current
instantiation of `non_null`. This prevents implicit conversions, and thus
requires that one of the operands is a `non_null` type, avoiding ambiguous
overload resolution and potential infinite recursion evaluating constraints like
`is_equality_comparable`.

### Dereference Operators {#dereference-operators}

`non_null<P>::operator->()` and `non_null<P>::operator*()` are provided and
simply pass-through to the held `P`.

In order to provide improved safety, `operator->` and `operator*` are "checked"
operations when `P` is not a raw pointer, to ensure the invariant of non-null
has been maintained. See [below](#how-can-the-invariant-be-violated) for
additional details on why this is needed.

### How Can The Invariant Be Violated? {#how-can-the-invariant-be-violated?}

Ordinarily, `non_null<P>` cannot become `nullptr`; it cannot be constructed from
`nullptr`, `reset()` to `nullptr`, and there is no way to get a non-const lvalue
reference to the held pointer.

However, if `P` is a fancy pointer, in order to be _movable_, it is not
practical to prevent the held pointer from becoming `nullptr`. Otherwise, moving
a `non_null<shared_ptr<T>>` would actually result in a copy, and
`non_null<unique_ptr<T>>` would be neither copyable nor movable, making it
non-functional in real programs. The relaxing of this constraint for fancy
pointers means that if a `non_null` of such a type is dereferenced or similar
after being moved from, the pointer may be `nullptr`.

However, most safety/coding standards ban use-after-move. `clang-tidy` also has
a check to detect use-after-move statically,
[bugprone-use-after-move](https://clang.llvm.org/extra/clang-tidy/checks/bugprone/use-after-move.html)
which is very reliable. Given this, allowing the invariant to be violated for
fancy pointers in a moved-from `non_null<P>` is considered an acceptable
tradeoff. For additional safety, for any API which returns the held pointer or
dereferences it, an `ARENE_INVARIANT` validates the held pointer is not
`nullptr`. Violation of the invariant results in process abandonment.

This check does incur a small performance penalty; the impact of this check is
mitigated in many cases with optimizations enabled as the compiler will remove
redundant `nullptr` checks.

## Convenience Aliases {#convenience-aliases}

In order to provide consistent ergonomics with existing smart pointers, the
following convenience alias templates are provided:

```cpp
// A non-null raw pointer to T
template<typename T>
using non_null_ptr = non_null<T*>;

// A non-null std::shared_ptr<T>
template<typename T>
using non_null_shared_ptr = non_null<std::shared_ptr<T>>;

// A non-null std::unique_ptr<T>
template<typename T>
using non_null_unique_ptr = non_null<std::unique_ptr<T>>;
```

## Factory Functions {#factory-functions}

In order to facilitate easier adoption of `non_null_shared_ptr` and
`non_null_unique_ptr`, equivalents of `std::make_shared<T>` and
`std::make_unique<T>` are provided which return `non_null_shared_ptr<T>` and
`non_null_unique_ptr<T>` respectively. They have all the same advantages as
their stdlib counterparts, and are intended to be "drop in replacements." As
there are no situations where `std::make_[shared|unique]` can ever return
`nullptr`, all usage in a codebase can and should be replaced with these
helpers.

Examples:

```cpp
auto non_null_sptr_to_t = arene::base::make_shared<T>(arg1, arg2);

auto non_null_uptr_to_t = arene::base::make_unique<T>(arg1, arg2);
```

There is also a factory to create `non_null_ptr<T>` with argument deduction from
the input pointer for C++14 contexts:

```cpp
T some_t;
// non_null_ptr_to_t will have type non_null_ptr<T>
auto non_null_ptr_to_t = arene::base::make_non_null(&some_t);
```

## Requirements {#requirements}

For Jama requirements, please see
[Arene_Eco_System-L5SW-1227](https://stargate.jamacloud.com/perspective.req#/items/4254312?projectId=67)
