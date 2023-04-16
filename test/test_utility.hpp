/*
* MIT License
*
* Copyright(c) 2023 Jimmie Bergmann
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

#ifndef MINIYAML_TEST_UTILITY_HEADER_FILE
#define MINIYAML_TEST_UTILITY_HEADER_FILE

#include "gtest/gtest.h"

//#define MINIYAML_NO_STD_STRING_VIEW
//#define MINIYAML_CUSTOM_NAMESPACE yaml

#include "../yaml/yaml.hpp"
#include <iostream>

#define TEST_IGNORE_NODISCARD(statement) [&]() -> decltype(statement)& { return statement; }()
#define ASSERT_NO_THROW_IGNORE_NODISCARD(statement) ASSERT_NO_THROW(TEST_IGNORE_NODISCARD(statement))
#define EXPECT_ANY_THROW_IGNORE_NODISCARD(statement) EXPECT_ANY_THROW(TEST_IGNORE_NODISCARD(statement))

inline void print_test_info(const std::string& message) {
    std::cout << "\033[0;1;32m" << "[          ] " << "\033[0m";
    std::cout << "\033[0;36m" << message << "\033[0m" << std::endl;
}

template<typename TChar>
void run_read_style(const std::function<void(std::basic_string<TChar>)>& func, std::basic_string<TChar> input, const std::string& type) {
    print_test_info(type);
    func(input);
}

template<typename TChar>
void replace_all(std::basic_string<TChar>& source, const std::basic_string<TChar>& from, const std::basic_string<TChar>& to)
{
    std::basic_string<TChar> new_string;
    new_string.reserve(source.length());

    typename std::basic_string<TChar>::size_type last_pos = 0;
    typename std::basic_string<TChar>::size_type find_pos = 0;

    while (std::string::npos != (find_pos = source.find(from, last_pos)))
    {
        new_string.append(source, last_pos, find_pos - last_pos);
        new_string += to;
        last_pos = find_pos + from.length();
    }

    new_string += source.substr(last_pos);
    source = std::move(new_string);
}

template<typename TChar>
void run_read_unix_style(std::basic_string<TChar> input, const std::function<void(std::basic_string<TChar>)>& func) {
    run_read_style<TChar>(func, input, "linux_style");
}

template<typename TChar>
void run_read_win_style(std::basic_string<TChar> input, const std::function<void(std::basic_string<TChar>)>& func) {
#if MINIYAML_HAS_IF_CONSTEXPR
    if constexpr (std::is_same<TChar, char>::value == false) {
        replace_all<TChar>(input, u8"\n", u8"\r\n");
    }
    else
#endif
    {
        replace_all<TChar>(input, "\n", "\r\n");
    }

    run_read_style<TChar>(func, input, "windows_style");
}

template<typename TChar>
void run_read_mac_style(std::basic_string<TChar> input, const std::function<void(std::basic_string<TChar>)>& func) {

#if MINIYAML_HAS_IF_CONSTEXPR
    if constexpr (std::is_same<TChar, char>::value == false) {
        replace_all<TChar>(input, u8"\n", u8"\r");
    }
    else
#endif
    {
        replace_all<TChar>(input, "\n", "\r");
    }

    run_read_style<TChar>(func, input, "mac_style");
}

template<typename TChar>
void run_read_all_styles(const std::basic_string<TChar>& input, const std::function<void(std::basic_string<TChar>)>& func) {
    run_read_unix_style<TChar>(input, func);
    run_read_win_style<TChar>(input, func);
    run_read_mac_style<TChar>(input, func);
}


#endif