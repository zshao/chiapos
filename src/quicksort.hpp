// Copyright 2018 Chia Network Inc

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//    http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SRC_CPP_QUICKSORT_HPP_
#define SRC_CPP_QUICKSORT_HPP_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>

#include "util.hpp"

namespace QuickSort {

    inline static void SortInner(
        uint8_t *memory,
        uint64_t memory_len,
        uint32_t L,
        uint32_t bits_begin,
        uint64_t begin,
        uint64_t end,
        uint8_t *pivot_space)
    {
        if (end - begin <= 5) {
            for (uint64_t i = begin + 1; i < end; i++) {
                uint64_t j = i;
                memcpy(pivot_space, memory + i * L, L);
                while (j > begin &&
                       Util::MemCmpBits(memory + (j - 1) * L, pivot_space, L, bits_begin) > 0) {
                    memcpy(memory + j * L, memory + (j - 1) * L, L);
                    j--;
                }
                memcpy(memory + j * L, pivot_space, L);
            }
            return;
        }

        uint64_t lo = begin;
        uint64_t hi = end - 1;

        memcpy(pivot_space, memory + (hi * L), L);
        bool left_side = true;

        while (lo < hi) {
            if (left_side) {
                if (Util::MemCmpBits(memory + lo * L, pivot_space, L, bits_begin) < 0) {
                    ++lo;
                } else {
                    memcpy(memory + hi * L, memory + lo * L, L);
                    --hi;
                    left_side = false;
                }
            } else {
                if (Util::MemCmpBits(memory + hi * L, pivot_space, L, bits_begin) > 0) {
                    --hi;
                } else {
                    memcpy(memory + lo * L, memory + hi * L, L);
                    ++lo;
                    left_side = true;
                }
            }
        }
        memcpy(memory + lo * L, pivot_space, L);
        if (lo - begin <= end - lo) {
            SortInner(memory, memory_len, L, bits_begin, begin, lo, pivot_space);
            SortInner(memory, memory_len, L, bits_begin, lo + 1, end, pivot_space);
        } else {
            SortInner(memory, memory_len, L, bits_begin, lo + 1, end, pivot_space);
            SortInner(memory, memory_len, L, bits_begin, begin, lo, pivot_space);
        }
    }

    inline static void SortInnerNThreads(
        uint8_t *memory,
        uint64_t memory_len,
        uint32_t L,
        uint32_t bits_begin,
        uint64_t begin,
        uint64_t end,
        uint8_t *pivot_space,
        uint32_t num_threads
        )
    {
        // Switch to single-threaded sort when there are less than 1024 elements, or only 1 thread left
        if (end - begin <= 1024 || num_threads <= 1) {
            SortInner(memory, memory_len, L, bits_begin, begin, end, pivot_space);
            return;
        }

        // Same as the single-threaded sort except the recursion part
        uint64_t lo = begin;
        uint64_t hi = end - 1;

        memcpy(pivot_space, memory + (hi * L), L);
        bool left_side = true;

        while (lo < hi) {
            if (left_side) {
                if (Util::MemCmpBits(memory + lo * L, pivot_space, L, bits_begin) < 0) {
                    ++lo;
                } else {
                    memcpy(memory + hi * L, memory + lo * L, L);
                    --hi;
                    left_side = false;
                }
            } else {
                if (Util::MemCmpBits(memory + hi * L, pivot_space, L, bits_begin) > 0) {
                    --hi;
                } else {
                    memcpy(memory + lo * L, memory + hi * L, L);
                    ++lo;
                    left_side = true;
                }
            }
        }
        memcpy(memory + lo * L, pivot_space, L);

        // Split the threads into 2 groups
        uint32_t num_threads_bigger = num_threads / 2;
        uint32_t num_threads_smaller = num_threads - num_threads_bigger;

        auto const new_thread_pivot_space = std::make_unique<uint8_t[]>(L);
        if (lo - begin <= end - lo) {
            // Let the new thread group take the smaller piece with smaller number of threads
            std::thread new_thread(SortInnerNThreads, memory, memory_len, L, bits_begin, begin, lo,
                                   new_thread_pivot_space.get(), num_threads_smaller);
            SortInnerNThreads(memory, memory_len, L, bits_begin, lo + 1, end,
                              pivot_space, num_threads_bigger);
            new_thread.join();
        } else {
            // Let the new thread group take the smaller piece with smaller number of threads
            std::thread new_thread(SortInnerNThreads, memory, memory_len, L, bits_begin, lo + 1, end,
                                   new_thread_pivot_space.get(), num_threads_smaller);
            SortInnerNThreads(memory, memory_len, L, bits_begin, begin, lo,
                              pivot_space, num_threads_bigger);
            new_thread.join();
        }
    }

    /**
     * Sorting with N thread
     */
    inline void SortNThreads(
        uint8_t *const memory,
        uint32_t const entry_len,
        uint64_t const num_entries,
        uint32_t const bits_begin,
        uint32_t const num_threads
        )
    {
        uint64_t const memory_len = (uint64_t)entry_len * num_entries;
        auto const pivot_space = std::make_unique<uint8_t[]>(entry_len);
        SortInnerNThreads(memory, memory_len, entry_len, bits_begin, 0, num_entries, pivot_space.get(), num_threads);
    }

    /**
     * Sorting with 1 thread
     */
    inline void Sort(
        uint8_t *const memory,
        uint32_t const entry_len,
        uint64_t const num_entries,
        uint32_t const bits_begin
    ) {
        SortNThreads(memory, entry_len, num_entries, bits_begin, 1);
    }

}

#endif  // SRC_CPP_QUICKSORT_HPP_
