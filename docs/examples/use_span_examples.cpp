// Copyright 2024, Toyota Motor Corporation

#include <iostream>

#include "arene/base/filesystem/error_code.hpp"
#include "arene/base/filesystem/file_handle.hpp"
#include "arene/base/span.hpp"

namespace use_span {
namespace original {
//! [original_read_example]
// Print the specified number of items from the supplied buffer
void print_items(int const* items, std::size_t count) {
  for (std::size_t index = 0; index < count; ++index) {
    std::cout << items[index] << "\n";
  }
}

void some_caller() {
  constexpr std::size_t count = 3;
  int buffer[count] = {1, 2, 3};
  print_items(buffer, count);
}
//! [original_read_example]
}  // namespace original

namespace indexed_items {
//! [indexed_read_example]
// Print all the items from the supplied buffer
void print_items(arene::base::span<int const> items) {
  for (std::size_t index = 0; index < items.size(); ++index) {
    std::cout << items[index] << "\n";
  }
}

void some_caller() {
  constexpr std::size_t count = 3;
  int buffer[count] = {1, 2, 3};
  print_items(buffer);
}
//! [indexed_read_example]
}  // namespace indexed_items

namespace for_loop {
//! [for_loop_read_example]
// Print all the items from the supplied buffer
void print_items(arene::base::span<int const> items) {
  for (auto& element : items) {
    std::cout << element << "\n";
  }
}
//! [for_loop_read_example]
}  // namespace for_loop

void handle_error(arene::base::filesystem::error_code);

//! [return_span_example]
void process_read_data(arene::base::span<arene::base::byte> data);

void read_and_process_data(arene::base::filesystem::file_handle file) {
  constexpr std::size_t buffer_size = 100;
  std::array<arene::base::byte, buffer_size> buffer;

  auto read_result = file.sequential_read(buffer);
  if (!read_result) {
    handle_error(read_result.error());
  } else {
    auto read_data = *read_result;
    std::cout << "Read " << read_data.size() << " bytes\n";
    process_read_data(read_data);
  }
}
//! [return_span_example]
}  // namespace use_span
