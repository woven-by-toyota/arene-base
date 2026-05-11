<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page stdlib Partial Implementation of the C++ Standard Library

<!-- markdownlint-enable MD041 -->

Some of the target platforms for Arene Base do not have a C++ Standard Library
implementation provided with the toolchain. In order to allow Arene Base to be
used on such platforms, we provide a partial implementation of the C++ Standard
Library. The features provided comply with the C++14 standard except where noted
below.

The Arene Base standard library is not a complete re-implementation of the C++14
standard. It only provides functionality needed by Arene Base itself or from
specific customer requests. Please check the [API documentation](\ref std) for
the list of provided facilities.

It is expected that if the Arene Base stdlib is being used, it is the _only_
standard library used. Compiling or linking the Arene Base stdlib against other
standard library implementations is not supported, nor is linking code built
with the Arene Base stdlib to code built with other implementations.

The Arene Base implementation of the C++ Standard Library is only available for
specific targets; on other platforms users are expected to use the
toolchain-provided Standard Library implementation.

## Usage {#usage}

To use the Arene Base C++ Standard Library implementation, add a dependency to
`@arene-base//stdlib` to your Bazel target. `@arene-base//stdlib` _cannot_ be
used along with a toolchain provided C++ standard library.

Some platforms are configured to use the toolchain C++ standard library by
default (e.g. Linux x86). Arene Base can be explicitly configured to disable use
of the toolchain provided C++ standard library with

```starlark
# //:.bazelrc
common --@arene-base//configs:use_toolchain_stdlib=False
```

If you _need_ to conditionally depend on `@arene-base//stdlib`, you can use the
config settings `@arene-base//configs:toolchain_stdlib_enabled` and
`@arene-base//configs:toolchain_stdlib_disabled`.

```starlark
    deps = select({
        "@arene-base//configs:toolchain_stdlib_enabled": [],
        "@arene-base//configs:toolchain_stdlib_disabled": [
            "@arene-base//stdlib",
        ],
    }),
```

## Deviations from the Standard {#deviations-from-the-standard}

The Arene Base stdlib generally follows the published C++14 standard closely,
but there are a few exceptions:

1. `std::chrono::time_point` is specified to require that its `Clock` parameter
   model a `Clock` in the sense of `[time.clock]`. However, none of the major
   standard library implementations actually enforce this, and it makes the
   class less useful; furthermore, this requirement was dropped in C++23. We
   therefore follow the other implementations in not enforcing it. As long as
   the `Clock` parameter has a typename called `Clock::duration`, it will work.
2. Certain algorithms which were not `constexpr` in C++14 have been implemented
   as `constexpr`. This is unintentional and the `constexpr` designation will be
   removed in a future update; applications should not rely on this `constexpr`
   behaviour.

Additionally, the standard permits implementations to make certain extensions.
The following changes to the strict standard wording are allowed by the
standard, but relying on them may reduce portability:

1. Most functions have stricter `noexcept` specifiers than what is required by
   the standard. This means that most functions in the `std` namespace will
   correctly report whether or not they can throw an exception as long as their
   type parameters do so as well.
2. Many type traits are implemented in terms of the `{trait_name}_v`
   freestanding values which were added in C++17. These are accessible to user
   code as well, but it is more portable to use the `{trait_name}::value` static
   members instead.
