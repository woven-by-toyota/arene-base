<!--
Copyright 2026, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

# Arene Base

## What is `arene_base`?

`arene_base` provides low level C++ facilities to help development of safety
critical software in compliance with Woven by Toyota's C++ development best practices.

## Feedback

We would love to hear from you if you have a question, concern, feature request,
or bug report.

For general inquiries, you can contact us at: arene-base-public@woven.toyota

### Feature Requests and Bug Reports

We welcome feature requests and improvement suggestions. Please
[open a GitHub issue](https://github.com/woven-by-toyota/arene-base/issues) to
submit feature requests, bug reports, or other feedback. Issues will be
addressed on a best-effort basis by the Arene Base developers.

## Build and Test

The following are methods for building and testing the project.

### Prerequisites

Currently, Arene Base is supported for use with the following platforms and
toolchains. Supported toolchains and platforms may be added in the future.

- Ubuntu (22.04+)
- Bazel (version 7 is well tested; higher versions may or may not work) or
  Bazelisk installed
- C++14 with Clang

### How to build and test

To build all targets using the local toolchain:

```shell
bazelisk build  --config=local_toolchain --  //arene/base/... //stdlib/...
```

To run all tests:

```shell
bazelisk test --config=local_toolchain --  //arene/base/... //stdlib/...
```

To run specific tests:

```shell
# Run tests for a specific component
bazelisk test --config=local_toolchain -- //arene/base/array/tests:all

# Run tests matching a pattern
bazelisk test --config=local_toolchain -- //arene/base/math/tests:all
```

If you are using Bazel without Bazelisk, replace `bazelisk` with your `bazel`
binary.

## Consuming Arene Base in Your Project

It is currently easiest to use Arene Base with Bazel. When using Arene Base in a
project, you can do so in the following ways.

### Using Bzlmod (module.bazel)

If you're using Bzlmod, add `arene-base` to your `MODULE.bazel` file:

```starlark
bazel_dep(name = "arene-base")

archive_override(
    module_name = "arene-base",
    urls = ["https://github.com/woven-by-toyota/arene-base/archive/main.zip"],
    strip_prefix = "arene-base-main",
)
```

Then include the desired targets in your `BUILD` files:

```starlark
cc_binary(
    name = "arene_base_example",
    srcs = ["main.cpp"],
    deps = [
        "@arene-base//:array",
    ],
)
```

### Configuration Notes

#### Using the stdlib Target

The `stdlib` target is a cut-down implementation of the C++ Standard Library,
providing only those features required for Arene Base itself or explicitly
requested. It only includes Standard Library features from C++14. Library
features from later versions of the C++ Standard may be added to the
`arene::base` namespace, provided they can be implemented using C++14 language
features.

When using the `//stdlib` target in your dependencies, you need to pass an
additional flag to Bazel:

```shell
bazel build --@arene-base//configs:use_toolchain_stdlib=false //your:target
```

This flag is only required when using `//stdlib`. Note that some targets may be
incompatible with this flag (e.g., the `//:filesystem` and `//:synchronization`
targets).
