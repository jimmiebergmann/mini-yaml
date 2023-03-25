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


#include "test_utility.hpp"

TEST(impl_test, is_empty_or_whitespace) 
{
    EXPECT_TRUE(yaml::impl::is_empty_or_whitespace(std::string("")));
    EXPECT_FALSE(yaml::impl::is_empty_or_whitespace(std::string("Nope")));
    
    EXPECT_TRUE(yaml::impl::is_empty_or_whitespace(std::string(" ")));
    EXPECT_TRUE(yaml::impl::is_empty_or_whitespace(std::string("  ")));
    EXPECT_TRUE(yaml::impl::is_empty_or_whitespace(std::string("\t")));
    
    EXPECT_TRUE(yaml::impl::is_empty_or_whitespace(std::string("\t\t")));

    EXPECT_TRUE(yaml::impl::is_empty_or_whitespace(std::string(" \t")));
    EXPECT_TRUE(yaml::impl::is_empty_or_whitespace(std::string("\t ")));
    EXPECT_TRUE(yaml::impl::is_empty_or_whitespace(std::string("\t \t \t \t")));
    EXPECT_TRUE(yaml::impl::is_empty_or_whitespace(std::string(" \t \t \t \t")));

    EXPECT_FALSE(yaml::impl::is_empty_or_whitespace(std::string(" Nope")));
    EXPECT_FALSE(yaml::impl::is_empty_or_whitespace(std::string("     Nope")));
    EXPECT_FALSE(yaml::impl::is_empty_or_whitespace(std::string("Nope ")));
    EXPECT_FALSE(yaml::impl::is_empty_or_whitespace(std::string("Nope    ")));

    EXPECT_FALSE(yaml::impl::is_empty_or_whitespace(std::string("\tNope")));
    EXPECT_FALSE(yaml::impl::is_empty_or_whitespace(std::string("\t\t\t\tNope")));
    EXPECT_FALSE(yaml::impl::is_empty_or_whitespace(std::string("Nope\t")));
    EXPECT_FALSE(yaml::impl::is_empty_or_whitespace(std::string("Nope\t\t\t\t")));

    EXPECT_FALSE(yaml::impl::is_empty_or_whitespace(std::string("\t Nope")));
    EXPECT_FALSE(yaml::impl::is_empty_or_whitespace(std::string("\t \t \t \t Nope")));
    EXPECT_FALSE(yaml::impl::is_empty_or_whitespace(std::string(" \tNope")));
    EXPECT_FALSE(yaml::impl::is_empty_or_whitespace(std::string(" \t \t \t \tNope")));
}