// Copyright 2021 Zheng Shao, zshao9@gmail.com

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//    http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "quicksort.hpp"

// Usage:
// 1. build: g++ -g -O2 -std=c++2a -o quicksort_main quicksort_main.cpp
// 2. run: time ./quicksort_main [threads=1] [million_records=10]
//
// This program has 2 goals:
// 1. verify the correctness of the multi-threaded quicksort
// 2. measure the speed of the multi-threaded execution vs a single-threaded one

int main(int argc, char* argv[])
{
    int threads = (argc < 2 ? 1 : atoi(argv[1]));
    int LEN = (argc < 3 ? 10 : atoi(argv[2])) * 1000 * 1000;

    int THREADS = 8;
    int* tmp = new int[LEN];
    for (int i = 0; i < LEN; i++) {
        tmp[i] = (int)((uint64_t(i) * i) % (10 * 1000 * 1000));
    }
    
    QuickSort::SortNThreads((uint8_t *)tmp, 4, LEN, 0, threads);
    
    bool ok = true;
    for (int i = 1; i < LEN; i++) {
        if (tmp[i - 1] > tmp[i]) {
            std::cout << "Wrong order: " << tmp[i - 1] << " > "
                      << "tmp[i]" << std::endl;
            ok = false;
            break;
        }
    }
    if (ok) {
        std::cout << "Sort succeeded" << std::endl;
    }
    delete[] tmp;
    return 0;
}

