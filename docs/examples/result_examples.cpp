// Copyright 2024, Toyota Motor Corporation

/// @example examples/result_examples.cpp
/// @brief Examples for the result subpackage documentation
///

#include <cstdint>
#include <string>
#include <tuple>

#include "arene/base/byte.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/filesystem/directory_handle.hpp"
#include "arene/base/filesystem/error_code.hpp"
#include "arene/base/filesystem/path_string.hpp"
#include "arene/base/result.hpp"
#include "arene/base/span.hpp"

namespace result_examples {

namespace basic_examples {
struct error {};
struct io_error {};

//! [basic_example]
// An API which either computes an @c int value, or produces @c error
arene::base::result<int, error> compute_foo(int);

// An API which either successfully writes all of @c bytes to some I/O sink, or returns an @c io_error .
arene::base::result<void, io_error> do_io(arene::base::span<arene::base::byte> bytes);
//! [basic_example]
}  // namespace basic_examples

ARENE_IGNORE_START();
ARENE_IGNORE_CLANG("-Wunused-private-field", "Private fields not used for examples");
void in_place_value_example() {
  struct geometry_error {};

  //! [in_place_value]
  class point {
    int x_val;
    int y_val;

   public:
    point(int x, int y)
        : x_val(x),
          y_val(y) {}
  };

  // position will have a value-channel populated equivalent to point{1,2}
  arene::base::result<point, geometry_error> position(arene::base::in_place_value, 1, 2);
  //! [in_place_value]

  std::ignore = position;
}

void in_place_error_example() {
  struct point {};

  //! [in_place_error]
  class geometry_error {
    int x_out_of_bounds_by;
    int y_out_of_bounds_by;

   public:
    geometry_error(int x, int y)
        : x_out_of_bounds_by(x),
          y_out_of_bounds_by(y) {}
  };

  // position will have a error-channel populated equivalent to geometry_error{1,2}
  arene::base::result<point, geometry_error> position(arene::base::in_place_error, 1, 2);
  //! [in_place_error]

  std::ignore = position;
}

void in_place_void_value_example() {
  struct geometry_error {};

  //! [in_place_value_void]
  // position will have a value-channel populated
  arene::base::result<void, geometry_error> success(arene::base::in_place_value);
  //! [in_place_value_void]

  std::ignore = success;
}

void in_place_void_error_example() {
  struct point {};

  //! [in_place_error_void]
  class geometry_error {
    int x_out_of_bounds_by;
    int y_out_of_bounds_by;

   public:
    geometry_error(int x, int y)
        : x_out_of_bounds_by(x),
          y_out_of_bounds_by(y) {}
  };

  // error will have a error-channel populated equivalent to geometry_error{1,2}
  arene::base::result<void, geometry_error> error(arene::base::in_place_error, 1, 2);
  //! [in_place_error_void]

  std::ignore = error;
}
ARENE_IGNORE_END();

class Foo {
 public:
  Foo(std::int32_t, std::int32_t) {}
};

class Bar {
 public:
  Bar(std::int32_t, std::int32_t) {}
};

void value_result_example() {
  //! [value_result]
  // Make "result" contain a "Foo" value object, initialized as "Foo(1, 2)"
  arene::base::result<Foo, Bar> res = arene::base::value_result(Foo(1, 2));
  //! [value_result]
  std::ignore = res;
}

void value_result_void_example() {
  //! [value_result_void]
  // Make "result" contain a "value", even though there are no objects of type "void"
  arene::base::result<void, Bar> res = arene::base::value_result();
  //! [value_result_void]
  std::ignore = res;
}

void error_result_example() {
  //! [error_result]
  // Make "result" contain a "Bar" error object, initialized as "Bar(1, 2)"
  arene::base::result<Foo, Bar> res = arene::base::error_result(Bar(1, 2));
  //! [error_result]
  std::ignore = res;
}

arene::base::result<int, std::string> function_that_may_fail() { return arene::base::value_result(42); }

template <typename Arg>
void do_something_with(Arg&&) {}

void expand_result_example() {
  //! [expand_result]
  arene::base::result<int, std::string> const result = function_that_may_fail();
  arene::base::expand_result(
      result,
      [](int v) {
        // what to do when in the value state
        do_something_with(v);
      },
      [](std::string const& e) {
        // what to do when in the error state
        do_something_with(e);
      }
  );
  //! [expand_result]
}

void transfor_error_example() {
  class dummy_log {
   public:
    void log(arene::base::filesystem::error_code) {}
  };

  dummy_log logger;

  arene::base::filesystem::path_string dir_path{};

  //! [transform_error]
  auto maybe_dir = arene::base::filesystem::directory_handle::open(dir_path).transform_error(
      [&logger](arene::base::filesystem::error_code err) {
        logger.log(err);
        return err;
      }
  );
  //! [transform_error]

  std::ignore = maybe_dir;
}

void and_then_example() {
  arene::base::filesystem::path_string dir_path{};
  arene::base::filesystem::path_string filename{};

  //! [and_then]
  auto maybe_file = arene::base::filesystem::directory_handle::open(dir_path).and_then(
      [&filename](arene::base::filesystem::directory_handle dir) { return dir.open_file(filename); }
  );
  //! [and_then]

  std::ignore = maybe_file;
}

void or_else_example() {
  arene::base::filesystem::path_string dir_path{};

  class some_logger {
   public:
    void log(arene::base::filesystem::error_code) {}
  };

  some_logger logger;

  //! [or_else]
  auto maybe_dir = arene::base::filesystem::directory_handle::open(dir_path).or_else(
      [&logger](arene::base::filesystem::error_code err
      ) -> arene::base::result<arene::base::filesystem::directory_handle, arene::base::filesystem::error_code> {
        logger.log(err);
        return arene::base::error_result(err);
      }
  );
  //! [or_else]

  std::ignore = maybe_dir;
}

}  // namespace result_examples
