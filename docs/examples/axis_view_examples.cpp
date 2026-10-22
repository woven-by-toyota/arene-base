// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <iostream>

#include "arene/base/mdspan.hpp"

namespace axis_view_examples {

//! [axis_view_usage_example]
// Iterate the rows and columns of a rank-2 mdspan.
void print_rows_and_columns(arene::base::mdspan<int const, arene::base::dextents<std::size_t, 2>> matrix) {
  for (auto row : arene::base::axis_view<0>(matrix)) {
    // row == submdspan(matrix, i, full_extent) for successive i: the i-th row of matrix
    std::cout << "row of length " << row.extent(0) << "\n";
  }
  for (auto column : arene::base::axis_view<1>(matrix)) {
    // column == submdspan(matrix, full_extent, i) for successive i: the i-th column of matrix
    std::cout << "column of length " << column.extent(0) << "\n";
  }
}
//! [axis_view_usage_example]

}  // namespace axis_view_examples
