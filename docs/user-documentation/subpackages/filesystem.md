<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page filesystem filesystem: Traversal Oriented Filesystem Access

<!-- markdownlint-enable MD041 -->

The `//:filesystem` package contains classes and functions for accessing files
and directories in a portable fashion using a "traversal oriented" approach. It
focuses on the filesystem interactions themselves; path manipulation facilities
are not currently provided.

Support for the `filesystem` subpackage may be disabled by specifying
`--no@arene-base//configs:use_filesystem` either on the Bazel command line, or
in `.bazelrc`

## Quickstart Summary {#quickstart-summary}

The two primary APIs for interaction with the filesystem in the package are the
`arene::base::filesystem::directory_handle` and
`arene::base::filesystem::file_handle` classes.

`arene::base::filesystem::directory_handle` is a handle for a directory, which
can be used to access and manipulate the files and directories in the filesystem
subtree rooted in that directory.

`arene::base::filesystem::file_handle` is a handle to an open file, which can be
used to read data from and write data to that file.

Most filesystem operations can fail, so they return an `arene::base::result`
holding either the desired result type or an
`arene::base::filesystem::error_code` holding details of the error.

## Overall philosophy {#overall-philosophy}

The Arene filesystem library is intended to provide access to the filesystem in
a way that avoids certain filesystem race conditions: access to files and
directories is done via "handles", which maintain their association to a
particular file or directory even if that file or directory is renamed or
deleted. For example, if a `arene::base::filesystem::directory_handle` is opened
on `/foo/bar`, which is then renamed to `/baz/wibble`, then the handle still
refers to the same directory in its new location.

Once a "root" directory has been opened with
`arene::base::filesystem::directory_handle::open`, subsequent filesystem access
is done based on the returned directory handle. Relative paths are constrained
to be within the directory subtree below this "root" directory; attempting to
use `..` path segments to access the parent directory will result in an error.
The only way to access files or directories outside this subtree is to call
`arene::base::filesystem::directory_handle::open` again with the desired path,
or via symbolic links within the subtree. This reduces accidental accesses to
files outside the subtree. It also reduces the chance of errors due to
concurrent access to the filesystem from another process: once a directory has
been opened, then subsequent accesses from that
`arene::base::filesystem::directory_handle` object will refer to that specific
directory, even if it is renamed. Opening files via the directory handle thus
ensures that the files are in that specific directory, rather than another,
should the original be renamed.

Files can only be opened via the `open_file` and `create_file` member functions
of a `directory_handle` object. Thus to open a file with an absolute path you
must first open a `directory_handle` for the parent directory, and then open the
file from there.

The API has also been designed to avoid dynamic memory allocation: strings are
passed via `arene::base::null_terminated_string_view` or
`arene::base::inline_string`.

## Error Handling {#error-handling}

Most filesystem operations can fail, so they return an `arene::base::result`
holding either the desired result type or an
`arene::base::filesystem::error_code` holding details of the error. For
operations where there is no data to be returned on success, the result type is
`void`.

If you wish failures to be translated into exceptions, then the `throw_error()`
member function of the error code will throw a `std::system_error` representing
the failure.

Alternatively, the POSIX `errno` value associated with the error code can be
queried using the `value()` member function, and the `message()` member function
can be used to obtain a string describing the error.

## Directory Access {#directory-access}

A "root" directory for access is opened by passing the path to
`arene::base::filesystem::directory_handle::open`, which returns an
`arene::base::result` holding a `arene::base::filesystem::directory_handle` on
success.

### Reading directory contents {#reading-directory-contents}

This directory handle can then be used to read the contents of the directory
using the `arene::base::filesystem::directory_handle::next_entry` member
function. This returns an `arene::base::null_terminated_string_view` referring
to the name of the next file or subdirectory on success, and an error code
otherwise. If there are no more entries, then an error is returned with the
value `ERANGE`. The string view is only valid until the next call to
`next_entry` or `rewind`; it is up to the user to copy the string content into
persistent storage if needed.

The handle can be reset to the first entry via the
`arene::base::filesystem::directory_handle::rewind` member function.

### Creating and opening subdirectories {#creating-and-opening-subdirectories}

An existing subdirectory can be opened using the
`arene::base::filesystem::directory_handle::open_subdirectory` member function.
Alternatively, a new subdirectory can be created using the
`arene::base::filesystem::directory_handle::create_subdirectory` member
function. Both of these return an `arene::base::filesystem::directory_handle`
referring to the specified subdirectory on success, and an error code on
failure. Only one subdirectory is created per `create_subdirectory` call, so if
you pass a path with intervening subdirectories that do not exist, then the call
will fail.

### Creating and opening files {#creating-and-opening-files}

Files are opened or created via the `open_file` and `create_file` member
functions of `arene::base::filesystem::directory_handle`. The specified path
must be a relative path, both of which return `arene::base::result` objects with
an `arene::base::filesystem::file_handle` as the value type.

The `create_temporary_file` member function can also be used to create a
temporary file which is deleted when the handle is closed or destroyed. Where
the filesystem supports it, this is created with no name. Otherwise, it is
created with an ephemeral random name and immediately removed from the directory
so it cannot be subsequently reopened.

### Renaming and deleting files and directories {#renaming-and-deleting-files-and-directories}

Files and directories can be renamed with the
`arene::base::filesystem::directory_handle`'s `rename` member function, which
takes the old name and the new name, both of which must be relative to the
directory referred to be the handle. Though a file can be moved from one
subdirectory to another via `rename`, the target subdirectory must exist;
otherwise the call will fail.

Files can be removed with the `unlink_entry` member function. This takes the
name of the file or directory to be removed, and an
`arene::base::filesystem::directory_handle::unlink_type` which specifies whether
it is a directory to be removed, or something else (e.g. a file or symbolic
link). Directories can only be removed if they are empty.

### Information about directory entries {#information-about-directory-entries}

Information about a directory entry can be obtained with the `get_file_stats`
member function. This returns a
[`struct stat`](https://pubs.opengroup.org/onlinepubs/009696799/basedefs/sys/stat.h.html)
holding information about the entry.

### Temporary directories {#temporary-directories}

In addition to the
`arene::base::filesystem::directory_handle::create_temporary_file` function for
creating an individual temporary file, the
`arene::base::filesystem::temporary_directory` class can be used to create a
temporary directory, which is deleted along with its contents when the object is
destroyed.

The location of the temporary directory can be specified via the `TMPDIR`
environment variable. If that environment variable is not specified, then `/tmp`
is used. If the current process does not have write permissions for the
specified directory, or it does not exist, the operation will fail. The
temporary directory is created inside the specified directory.

The temporary directory will have a name containing random characters to ensure
uniqueness. If multiple threads or processes attempt to create a temporary
directory simultaneously, they will each create their own directory with a
distinct name: if the chosen name already exists, the code will retry with an
alternate name until it encounters an error that forces the operation to fail,
or it successfully creates a directory with a unique name.

## File Access {#file-access}

Files are opened or created via the `open_file` and `create_file` member
functions of
[`arene::base::filesystem::directory_handle`](#creating-and-opening-files). As
with the directory handles, an `arene::base::filesystem::file_handle` refers to
a specific file once opened, even if that file is renamed.

The `arene::base::filesystem::file_handle` object can then be used to read from
the file, write to the file, and obtain the current size of the file.

The `arene::base::filesystem::file_handle` class provides two sets of functions
for reading and writing data to and from a file: the **Sequential Access**
functions, and the **Random Access** functions.

### Sequential access {#sequential-access}

There is a single sequential access position for each file handle, which is used
and updated by all sequential reads and writes via that file handle. Initially
it is at the start of the file, and is moved automatically with every sequential
read or write, but it can be moved explicitly with the
`arene::base::filesystem::file_handle::sequential_seek` member function.

The `arene::base::filesystem::file_handle::sequential_read` member function
reads data from the file into the specified buffer from the current sequential
access position. If any data is read successfully, an `arene::base::span`
covering the portion of the buffer that was populated is returned; this may be
less than the provided buffer size. If no data could be read, then an
`arene::base::filesystem::error_code` is returned.

The `arene::base::filesystem::file_handle::sequential_write` member function
writes data to the file from the specified buffer at the current sequential
access position. If any data is written successfully, an `arene::base::span`
covering the portion of the buffer holding any unwritten data is returned; if
this is not empty then it could be used in a subsequent call to write more data.
If no data could be written, then an `arene::base::filesystem::error_code` is
returned.

### Random access {#random-access}

Rather than reading or writing a file sequentially, you can instead write at
specific positions using the random access API. In this case, you must
explicitly specify the position for every read and write: the position is
specified as an offset from the start of the file. These functions do not alter
the sequential access position.

The `arene::base::filesystem::file_handle::read_at` member function reads data
from the file **starting at the specified position** into the specified buffer.
If any data is read successfully, an `arene::base::span` covering the portion of
the buffer that was populated is returned; this may be less than the provided
buffer size. Attempts to read data at or beyond the end of the file will return
an empty `span`. If data could not be read, then an
`arene::base::filesystem::error_code` is returned.

The `arene::base::filesystem::file_handle::write_at` member function writes data
to the file from the specified buffer **at the specified position**. If any data
is written successfully, an `arene::base::span` covering the portion of the
buffer holding any unwritten data is returned; if this is not empty then it
could be used in a subsequent call to write more data. If no data could be
written, then an `arene::base::filesystem::error_code` is returned. If the
specified offset is beyond the end of the file, if the write succeeds then the
file will be zero-padded between the old size and the specified offset.

## Examples {#examples}

### Opening files and directories {#opening-files-and-directories}

You can obtain a directory handle with
`arene::base::filesystem::directory_handle::open(path)`. This handle can then be
used to access files or subdirectories relative to that root path. Existing
files can be opened with `dir_handle.open_file(relative_path)`, and new files
created with `dir_handle.create_file(relative_path)`.

```{.cpp}
void foo() {
  auto dir = arene::base::filesystem::directory_handle::open("/some/path");
  if(!dir) {
    // handle case where directory cannot be opened
  }

  auto file = dir->open_file("myfile.dat");
  if(!file) {
    file = dir->create_file("myfile.dat");
  }
  if(!file) {
      // handle case where file cannot be opened or created
  }
  write_content(*file);
}
```

### Reading the contents of a directory {#reading-the-contents-of-a-directory}

You can also read the contents of the directory via the handle. The `next_entry`
member function returns an `arene::base::null_terminated_string_view`
referencing the name of the next directory entry, if there is one, or an error
code otherwise. Once you've reached the end of the directory, you can restart
iteration with the `rewind` member function.

```{.cpp}
using dir_listing = arene::base::inline_vector<arene::base::filesystem::path_string, 100>;

dir_listing list_directory_contents(arene::base::filesystem::directory_handle& dir) {
  dir_listing content;
  auto rewind_result = dir.rewind();
  if(!rewind_result) {
    rewind_result.error().throw_error();
  }
  while(true) {
    auto entry = dir.next_entry();
    if(!entry) {
      if(entry.error().value() == ERANGE) {
        return content;
      }
      entry.error().throw_error();
    }
    content.emplace_back(entry.value());
  }
}
```

### Reading from a file sequentially {#reading-from-a-file-sequentially}

Reading a file into memory can be implemented using the **sequential access**
functions:

```{.cpp}
std::vector<arene::base::byte> read_file(
    arene::base::filesystem::directory_handle& directory,
    arene::base::null_terminated_string_view filename) {
  auto file = directory.open_file(filename);
  if (!file) {
    file.error().throw_error();
  }
  std::vector<arene::base::byte> buffer(file->size().value());
  arene::base::span<arene::base::byte> target(buffer);
  while (!target.empty()) {
    auto read_result = file->sequential_read(target);
    if (!read_result) {
      read_result.error().throw_error();
    }
    if(read_result->empty()) {
      buffer.resize(buffer.size() - target.size());
      break;
    }
    target = target.subspan(read_result->size());
  }
  return buffer;
}
```

This throws if there is an error; otherwise it keeps reading chunks until the
entire file has been read.

### Random Access Writing Example {#random-access-writing-example}

Writing data to a specific position a file into memory can be implemented using
the **random access** functions:

```{.cpp}
void write_to_file(
    arene::base::filesystem::directory_handle& directory,
    arene::base::null_terminated_string_view filename,
    arene::base::span<const byte> data,
    std::uint64_t file_position) {
  auto file = directory.open_file(filename);
  if (!file) {
    file.error().throw_error();
  }
  while(!data.empty()) {
    auto write_result = file->write_at(data, file_position);
    if (!write_result) {
      write_result.error().throw_error();
    }
    file_position += (data->size() - write_result->size());
    data = write_result.value();
  }
}
```

This throws if there is an error; otherwise it keeps writing chunks until the
entire block of data has been written.

### Temporary Directories {#temporary-directories}

During the execution of an application, you may wish to store temporary files
that need to be deleted when no longer needed. By using the
`arene::base::filesystem::temporary_directory` class, this can be handled
automatically: the constructor creates a uniquely named temporary directory, and
the destructor removes that directory and any files and directories contained
within it. Any temporary files used by your application can then be created
within this new directory, and they will be removed when the
`temporary_directory` object is destroyed.

```{.cpp}
void foo() {
  arene::base::filesystem::temporary_directory tempdir;

  auto dir = arene::base::directory_handle::open(tempdir.path());

  auto file = dir->create_file("myfile.tmp");

  // Main part of foo
} // temporary directory and file deleted automatically

```

### Temporarily Changing The Current Directory {#temporarily-changing-the-current-directory}

Some operations read and write files in the "current directory" of the process.
If a particular section of code needs to change the "current directory" prior to
performing such an operation, then it is often prudent to restore it to its
previous value afterwards, in order to avoid causing problems for other parts of
the application. In order to make it easy to ensure that the current directory
is changed back to its previous value when a given section code exits, Arene
Base provides the `arene::base::filesystem::scoped_change_directory` class.

This simple RAII class changes the current directory to the specified path on
construction, saving the old value in the process. The current directory is then
changed back to the old value when the object is destroyed.

```{.cpp}
void foo(){
  arene::base::filesystem::scoped_change_directory chdir("/some/path");

  // perform operations that access /some/path as the "current directory"
} // previous value of current directory restored
```
