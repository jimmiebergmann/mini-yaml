/*
* MIT License
*
* Copyright(c) 2022 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/


#include "gtest/gtest.h"

//#define MINIYAML_NO_STD_STRING_VIEW
//#define MINIYAML_CUSTOM_NAMESPACE yaml2
#include "../yaml/yaml.hpp"

#include <array>
#include <algorithm>
#include <iostream>

#define TEST_IGNORE_NODISCARD(statement) [&]() -> decltype(statement)& { return statement; }()
#define ASSERT_NO_THROW_IGNORE_NODISCARD(statement) ASSERT_NO_THROW(TEST_IGNORE_NODISCARD(statement))
#define EXPECT_ANY_THROW_IGNORE_NODISCARD(statement) EXPECT_ANY_THROW(TEST_IGNORE_NODISCARD(statement))

namespace mini_yaml_test {

#if defined(__cpp_char8_t)
    using u8_string_type = std::u8string;
#else
    using u8_string_type = std::string;
#endif

    inline void print_info(const std::string& message) {
        std::cout << "\033[0;1;32m" << "[          ] " << "\033[0m";
        std::cout << "\033[0;36m" << message << "\033[0m" << std::endl;
    }

}