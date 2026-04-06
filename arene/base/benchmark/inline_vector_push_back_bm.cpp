// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>

#include "arene/base/inline_container/vector.hpp"

namespace {

/// @brief Benchmark for push_back
template <std::size_t Capacity, typename ValueType>
void inline_vector_push_back(benchmark::State& state) {
  arene::base::inline_vector<ValueType, Capacity> vec{};
  // NOLINTNEXTLINE(readability-identifier-length)
  for (auto _ : state) {
    state.PauseTiming();
    vec.clear();
    auto const count_to_insert = static_cast<std::size_t>(state.range(0));
    state.ResumeTiming();
    benchmark::DoNotOptimize(vec.data());

    for (std::size_t i = 0; i < count_to_insert; ++i) {
      vec.push_back(ValueType{});
    }

    benchmark::ClobberMemory();
  }
}

BENCHMARK(inline_vector_push_back<10, int>)->Range(1, 10);
BENCHMARK(inline_vector_push_back<1000, int>)->Range(1, 1000);

}  // namespace
