<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page variant variant: A Discriminated Union Type

<!-- markdownlint-enable MD041 -->

`arene::base::variant` is a backport of
[`std::variant`](https://en.cppreference.com/w/cpp/container/variant), with some
modifications to be in line with the general error handling policies of
`arene_base`.

The public header is

\snippet docs/examples/subpackages/variant_examples.cpp include_header

The Bazel target is

```text
//:variant
```

## A Discriminated Union Type {#a-discriminated-union-type}

`arene::base::variant` allows you to store an object which is one of a specified
list of types. This is similar to a `union`, but with stronger type safety: the
type of the stored object can always be queried, and attempting to access the
stored object via the wrong type is a runtime error rather than undefined
behaviour; variants are therefore also known as _discriminated unions_. Beyond
type safety, `variant` allows for types with non-trivial constructors,
destructors and assignment operators to be stored directly. This is in contrast
to a plain `union` which would require the manual invocation of constructors,
assignment operators, and destructors to properly manage object lifetimes.

## Declaring a Variant {#declaring-a-variant}

`variant`s are declared with a list of types that represent the possible active
alternatives. Note that the types do not need to be unique. However, it is
unspecified which alternative of a duplicate type is interacted with when using
type-based access APIs, and the visitor pattern will not distinguish between
them.

\snippet docs/examples/subpackages/variant_examples.cpp declaring_variant

## Interacting with a Variant {#interacting-with-a-variant}

There are two primary ways to interact with a `variant`: procedurally, and with
the visitor pattern.

### Procedural Access {#procedural-access}

With procedural access, the active alternative is queried, and the appropriate
accessor method is used to fetch the active alternative. The active alternative
can be queried by index or type, and similarly can be accessed by index or type.
It is invalid to access an alternative which is not currently active. Doing so
will throw `arene::base::bad_variant_access`.

A simple example of procedural access for printing the active alternative can be
seen below:

\snippet docs/examples/subpackages/variant_examples.cpp procedural_variant_usage

### Visitor Pattern {#visitor-pattern}

The visitor pattern allows for a more structured way to interact with the
`variant`. A visitor is a callable object that can be invoked with all of the
possible alternatives of the `variant`. The visitor can be a function, a lambda,
or any invocable object that can accept the types of the alternatives in the
`variant`. The visitor is invoked with the active alternative, and can perform
operations based on the type of the alternative. The return types of the
alternative handlers in the visitor can be any type, including `void`, and this
will be the return type of `visit`. The cref qualification of both the `visitor`
and `variant` are maintained.

@note If the return type for `visit` is not explicitly specified, then the
return type from every alternative's handler must be _exactly_ the same type.
Alternatively, by invoking `visit<RetT>(visitor, variant)`, the requirement is
relaxed to require only that the return type from every alternative's handler is
implicitly convertible to `RetT`.

The printing example above can be rewritten using the visitor pattern as
follows:

\snippet docs/examples/subpackages/variant_examples.cpp visit_variant_usage

The visitor pattern is exactly analogous to overload resolution which has been
deferred to runtime. In fact, a free function overload set can be exposed for
runtime overload resolution across a constrained type set like so:

\snippet docs/examples/subpackages/variant_examples.cpp binding_an_overload_set

This pattern is useful for building up a DSL around a constrained type set as a
form of dynamic polymorphism with different tradeoffs. Typical inheritance with
virtual functions makes adding new _type-dependent behavior_ for existing
operations easy: you simply implement a new type which inherits from the base
and overload the methods. However adding new _operations_ is difficult, because
it involves altering all existing derived classes. In contrast, using a variant
and the visitor pattern makes adding new _operations_ easy: you simply implement
a new visitor. However adding new _types_ to existing operations is difficult,
as it involves altering all existing visitors. Which is more appropriate for a
given application will depend on system design.

### bind_overloads {#bind_overloads}

The `arene::base::bind_overloads` utility function, provided by the
`//:functional` subpackage, can be used to simplify the creation of visitors for
use with `arene::base::variant`, allowing the easy use of inline lambdas to
create visitors, like so:

\snippet docs/examples/subpackages/variant_examples.cpp bind_overloads_usage
