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

#ifndef SRC_CPP_MT_QUICKSORT_HPP_
#define SRC_CPP_MT_QUICKSORT_HPP_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "util.hpp"

/**
 * This class will need a thread-pool.  However, after some research, I didn't find a
 * very good thread pool that can be used for this recursive calculation.
 *
 * For example, the most popular implementation https://github.com/vit-vit/ctpl 's stop
 * function shuts down threads too early, which is not a good practice for quicksort where
 * new tasks will be created by the main thread.
 */
namespace MultiThreadedQuickSort {

}

#endif  // SRC_CPP_MT_QUICKSORT_HPP_
