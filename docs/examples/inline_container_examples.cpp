// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>

#include "arene/base/compare.hpp"
#include "arene/base/inline_container/circular_buffer.hpp"
#include "arene/base/inline_container/deque.hpp"
#include "arene/base/inline_container/function.hpp"
#include "arene/base/inline_container/map.hpp"
#include "arene/base/inline_container/map_reference.hpp"
#include "arene/base/inline_container/set.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/inline_container/vector_reference.hpp"
#include "arene/base/inline_string.hpp"

namespace vector_overview_example {
struct my_data;
auto create_element() -> my_data;
void do_stuff(my_data&) {}

//! [vector_usage_example]
struct my_data {
  // some data
};
constexpr std::size_t max_elements = 25;

void foo() {
  arene::base::inline_vector<my_data, max_elements> vec;

  vec.emplace_back(create_element());

  for (auto& elem : vec) {
    do_stuff(elem);
  }
}
//! [vector_usage_example]

}  // namespace vector_overview_example

namespace external_vector_overview_example {
struct my_data;
auto create_element() -> my_data;
void do_stuff(my_data&) {}

//! [external_vector_usage_example]
struct my_data {
  // some data
};
constexpr std::size_t max_elements = 25;

// Externally-defined API that operates on the vector elements
void process(arene::base::external_vector<my_data>& vec);

void foo() {
  // Create backing storage for the vector, suitable for holding max_elements of type my_data
  alignas(my_data) arene::base::array<arene::base::byte, sizeof(my_data) * max_elements> storage{};
  // Create the vector using the backing storage
  arene::base::external_vector<my_data> vec{storage};

  // Add an element
  vec.emplace_back(create_element());

  // Process the vector, maybe add more elements
  process(vec);

  // Do stuff with each element
  for (auto& elem : vec) {
    do_stuff(elem);
  }
}
//! [external_vector_usage_example]

}  // namespace external_vector_overview_example

namespace deque_overview_example {
struct my_data;
auto create_element() -> my_data;
void do_stuff(my_data&) {}

//! [deque_usage_example]
struct my_data {
  // some data
};
constexpr std::size_t max_elements = 25;

void foo() {
  arene::base::inline_deque<my_data, max_elements> deque;

  deque.push_back(create_element());
  deque.push_front(create_element());
  // The back is the first element, and the front is the second

  // The second element (at the front) is processed, then the first (back) element
  while (!deque.empty()) {
    do_stuff(deque.front());
    deque.pop_front();
  }
}
//! [deque_usage_example]

}  // namespace deque_overview_example

namespace map_overview_example {
struct element_data;

auto make_fred_data() -> element_data;
auto make_barney_data() -> element_data;

void do_stuff(element_data const&) {}

//! [map_usage_example]
constexpr std::size_t max_element_name_length = 30;
using element_name_type = arene::base::inline_string<max_element_name_length>;

struct element_data {
  // whatever
};

constexpr std::size_t max_element_count = 100;

arene::base::inline_map<element_name_type, element_data, max_element_count> elements;

void foo() {
  elements["Fred"] = make_fred_data();
  elements.emplace("Barney", make_barney_data());

  auto pos = elements.find("Fred");
  if (pos != elements.end()) {
    do_stuff(pos->second);
  }
}
//! [map_usage_example]

}  // namespace map_overview_example

namespace set_overview_example {

void do_stuff(std::uint32_t) {}

//! [set_usage_example]
void foo(arene::base::span<std::uint32_t> raw_values) {
  constexpr std::size_t max_unique = 42;
  arene::base::inline_set<std::uint32_t, max_unique> unique_elements;

  for (auto i : raw_values) {
    unique_elements.insert(i);
  }

  for (auto i : unique_elements) {
    do_stuff(i);
  }
}
//! [set_usage_example]

}  // namespace set_overview_example

namespace function_overview_example {

struct my_data;

void do_stuff(my_data const&) {}

auto make_data() -> arene::base::array<my_data, 50>;

//! [function_usage_example]
struct my_data {};

class processor_wrapper {
  arene::base::inline_function<void(my_data) const> callback_;

 public:
  template <typename T>
  void register_callback(T&& cb) {
    callback_ = std::forward<T>(cb);
  }

  void data_ready(my_data data) {
    if (callback_) {
      callback_(std::move(data));
    }
  }
};

void foo(processor_wrapper& proc) {
  proc.register_callback([](my_data data) { do_stuff(data); });
}

class Processor {
  processor_wrapper proc_;

  void process(my_data data, unsigned arg);

 public:
  void bar(unsigned arg) {
    proc_.register_callback([this, arg](my_data data) { process(data, arg); });
  }
};

struct other_processor {
  arene::base::inline_function<unsigned(unsigned), 100> big_callback;
};

class my_big_callable {
  arene::base::array<my_data, 50> data_;

 public:
  explicit my_big_callable(arene::base::array<my_data, 50> data)
      : data_(std::move(data)) {}
  unsigned operator()(unsigned arg);
};

static_assert(sizeof(my_big_callable) < 100, "Must fit in callback buffer");

void wibble(other_processor& proc) { proc.big_callback = my_big_callable(make_data()); }
//! [function_usage_example]

}  // namespace function_overview_example

namespace vector_ref_example {

struct Point3D {};
struct Point2D {};

//! [vector_ref_usage_example]
// Externally-defined API, independent of vector capacity or storage type
void transform(arene::base::const_vector_reference<Point3D> from, arene::base::vector_reference<Point2D> to);

void foo() {
  arene::base::inline_vector<Point3D, 30> points3d{};
  arene::base::inline_vector<Point2D, 20> points2d{};

  // Pass references to inline_vector with different capacities
  transform(arene::base::make_const_vector_reference(points3d), arene::base::make_vector_reference(points2d));
}

void bar() {
  // array of bytes as storage for external_vector must be aligned for the target type, and have suitable size
  arene::base::inline_vector<Point3D, 50> other_points3d{};
  alignas(Point2D) arene::base::array<arene::base::byte, 50 * sizeof(Point2D)> point_storage{};
  arene::base::external_vector<Point2D> external_points{point_storage};

  // Pass reference to inline_vector with different capacity and external_vector to same function
  transform(
      arene::base::make_const_vector_reference(other_points3d),
      arene::base::make_vector_reference(external_points)
  );
}
//! [vector_ref_usage_example]

}  // namespace vector_ref_example

namespace map_ref_example {

struct SomeKey {
  friend auto operator<(SomeKey const&, SomeKey const&) -> bool;
};
struct SomeData {};

//! [map_ref_usage_example]
using api_map_type = arene::base::const_inline_map_reference<SomeKey, SomeData, arene::base::compare_three_way>;

// Externally-defined APIs, independent of map capacity
void process_data(api_map_type data);

void foo() {
  arene::base::inline_map<SomeKey, SomeData, 20> data_set1{};
  arene::base::inline_map<SomeKey, SomeData, 30> data_set2{};

  // Pass references to inline_map with different capacities
  process_data(api_map_type(data_set1));
  process_data(api_map_type(data_set2));
}

//! [map_ref_usage_example]

}  // namespace map_ref_example
