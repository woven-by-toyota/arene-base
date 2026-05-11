<!-- Copyright 2026, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page getting_started Getting Started

<!-- markdownlint-enable MD041 -->

## Consuming arene_base in Your Project {#consuming-arene_base-in-your-project}

`arene_base` releases are published on github as tarballs, which can be pulled
into Bazel using appropriate rules. Other build systems are not supported at
this time. Use of `arene_base` requires Bazel 7+.

### Bazel using `bzlmod` {#bazel-using-bzlmod}

The recommended way to import Arene Base is via Bazel modules. _Please note that
Bazel 6 is not supported when using `arene-base` through Bazel modules. Ensure
that you are using a compatible version of Bazel to avoid any compatibility
issues._

If your project uses the bzlmod module system, add the following to your
`MODULE.bazel` file, replacing `VERSION` with the specific version such as
`3.47.2` from our
[release tags](https://github.com/woven-by-toyota/arene-base/tags):

```python
bazel_dep(name = "arene-base", version = "VERSION")
```

As Arene Base is not currently published via a Bazel registry, you will then
need to specify the location of the archive, again replacing `VERSION` with the
specific version number:

```python
archive_override(
    module_name = "arene-base",
    urls = [
        "https://github.com/woven-by-toyota/arene-base/archive/refs/tags/VERSION.tar.gz",
    ],
)
```

If you intend to use `@arene-base//:testing/gtest` and are on Bazel 9, you will
also need to specify a version of Googletest that supports Bazel 9 (e.g.
1.17.0.bcr.2). We currently test against the last release that supports C++14.

### Bazel using `WORKSPACE` {#bazel-using-workspace}

_Please note that support for Bazel `WORKSPACE` files may be removed at any
time._

If your project is still using Bazel `WORKSPACE` files, add the following to
your `WORKSPACE`, replacing `VERSION` with the specific version such as `3.47.2`
from our [release tags](https://github.com/woven-by-toyota/arene-base/tags).

```python
http_archive(
    name = "arene-base",
    urls = [
        "https://github.com/woven-by-toyota/arene-base/archive/refs/tags/VERSION.tar.gz",
    ],
)
```

If you intend to use `@arene-base//:testing/gtest`, you will also need to
provide Googletest. We currently test against the last release that supports
C++14.

```python
http_archive(
  name = "com_google_googletest",
  integrity = "sha256-itWYxzrXluDYKAsILOvYKmMNc+c808cAV5OKZQG7pdc=",
  strip_prefix = "googletest-1.14.0",
  urls = [
      "https://mirror.bazel.build/github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz",
      "https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz",
  ],
)
```

## Feedback {#feedback}

We would love to hear from you if you have a question/concern/feature
request/bug report. Please file
[GitHub Issues](https://github.com/woven-by-toyota/arene-base/issues) with your
feedback.
