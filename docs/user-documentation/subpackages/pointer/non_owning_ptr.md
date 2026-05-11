<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page non_owning_ptr Non Owning Pointer

<!-- markdownlint-enable MD041 -->

## Introduction {#introduction}

Raw pointers are useful for conveying nullable reference semantics without
claims on object lifetime. However, they have a number of properties that
introduce potential safety hazards:

- Dereferencing a null pointer is undefined behavior.
- Pointer arithmetic allows the possibility of accessing invalid memory regions.

In addition, when mixed with C APIs, it can be unclear if a raw pointer is or is
not referring to an object whose lifetime needs management. In safety critical
embedded systems, mixed C/C++ codebases are common.

A non-owning nullable pointer type with precondition checks on dereference
removes these hazards. It turns nullptr dereference into defined behavior, and
disallows pointer arithmetic. In addition by virtue of being analogous to other
"smart pointer" types, its typename can convey the fact that object lifetime is
not managed.

The name for this type shall be `non_owning_ptr`. This matches the convention of
the existing smart pointer types from the stdlib, which all follow a
`<description>_ptr` convention.

## Design {#design}

### Size/Triviality {#sizetriviality}

`non_owning_ptr` is no larger than an equivalent pointer to the underlying type,
and is a _trivial_ type. This ensures there is no runtime penalty for storing or
copying the type relative to a raw pointer.

### Template Parameter {#template-parameter}

The type consumes a single template parameter, `T`. Similar to other smart
pointers, this type represents the _type that is pointed to_. So
`non_owning_ptr<Foo>` is analogous to `Foo*`, and `non_owning_ptr<const Foo>` is
`const Foo*`.

### Member Types {#member-types}

`non_owning_ptr<T>` provides the following member types, for compatibility with
various standard pointer-trait facilities:

- `non_owning_ptr<T>::pointer`, which is be `T*`.
- `non_owning_ptr<T>::reference`, which is be `T&`.
- `non_owning_ptr<T>::element_type`, which is be `T`.

### API Uniformity with stdlib Smart Pointers {#api-uniformity-with-stdlib-smart-pointers}

`non_owning_ptr` provides general API uniformity with existing stdlib smart
pointer types:

- `auto non_owning_ptr<T>::get()->pointer`: accesses the underlying pointer.
- `void non_owning_ptr<T>::reset(pointer = nullptr)`: replaces the underlying
  pointer with a new pointer, defaulted to `nullptr`.

### Construction/Assignment {#constructionassignment}

As `non_owning_ptr<T>` models `T*` semantically, users are able to
construct/assign `non_owning_ptr<T>` from types which `T*` could be
constructed/assigned. This means:

- Default construction as a trivial type equivalent to `T*`.
- Implicit construction from:
  - `nullptr`
  - `T*`
  - `non_owning_ptr<T>`
  - `U*` where `std::is_convertible<U*, T*>`
  - `non_owning_ptr<U>` where `std::is_convertible<U*, T*>`
- Assignment from same.

### Conversion to Other Types {#conversion-to-other-types}

As `non_owning_ptr<T>` models `T*`, explicit conversion to `bool` is supported,
as well as the `!` operator, and has meaning equivalent to `T*`.

In addition, for ergonomic convenience, `non_owning_ptr<T>` supports explicit
conversion to:

- `U*` where `std::is_convertable<T*, U*>`

### Comparison {#comparison}

As `non_owning_ptr<T>` models `T*`, it supports 6-way comparison with semantics
equivalent to comparing `T*`.

### Checked Dereference Operators {#checked-dereference-operators}

In order to provide improved safety, `operator->` and `operator*` are "checked"
operations. In the event that the pointer is `nullptr`, an `ARENE_PRECONDITION`
violation occurs, which results in program abandonment.

This check does incur a small performance penalty; this is mitigated with
optimizations enabled as the compiler will often be able to remove redundant
`nullptr` checks. As it is required by the \woven coding standard to validate a
nullable pointer is not null before dereferencing it, this means that in most
real world situations there should be no runtime performance penalty over a raw
pointer.

## Requirements {#requirements}

For detailed Jama requirements, please see
[Arene_Eco_System-L5SW-1299](https://stargate.jamacloud.com/perspective.req?docId=4277752&projectId=67)
