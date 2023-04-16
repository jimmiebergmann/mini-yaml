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

#include "learnyaml_dom_utility.hpp"

TEST(dom_read_documents, fail_unknown_file)
{
    using char_type = char;

    auto read_result = yaml::dom::read_documents_from_file<char_type>("../test/this_file_does_not_exist.some_extension");
    EXPECT_EQ(read_result.result_code, yaml::read_result_code::cannot_open_file);
    EXPECT_FALSE(read_result);
    EXPECT_TRUE(read_result.root_nodes.empty());
}

TEST(dom_read_documents, ok_learnyaml_read_documents_from_file)
{
    using char_type = char;

    const auto read_result = yaml::dom::read_documents_from_file<char_type>("../examples/learnyaml.yaml");
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_TRUE(read_result);
    ASSERT_EQ(read_result.root_nodes.size(), size_t{ 1 });

    check_dom_learnyaml_tests(read_result.root_nodes.front());
}

TEST(dom_read_documents, ok_learnyaml_read_documents_raw_input)
{
    using char_type = char;

    auto file_result = yaml::impl::read_file<char>("../examples/learnyaml.yaml");
    ASSERT_EQ(file_result.result_code, yaml::impl::read_file_result_code::success);

    auto* data_ptr = file_result.data.data();
    auto data_size = file_result.data.size();

    const auto read_result = yaml::dom::read_documents<char_type>(data_ptr, data_size);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_TRUE(read_result);
    ASSERT_EQ(read_result.root_nodes.size(), size_t{ 1 });

    check_dom_learnyaml_tests(read_result.root_nodes.front());
}

TEST(dom_read_documents, ok_learnyaml_read_documents_string)
{
    using char_type = char;

    auto file_result = yaml::impl::read_file<char>("../examples/learnyaml.yaml");
    ASSERT_EQ(file_result.result_code, yaml::impl::read_file_result_code::success);

    auto string = std::basic_string<char_type>{ file_result.data.begin(), file_result.data.end() };

    const auto read_result = yaml::dom::read_documents<char_type>(string);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_TRUE(read_result);
    ASSERT_EQ(read_result.root_nodes.size(), size_t{ 1 });

    check_dom_learnyaml_tests(read_result.root_nodes.front());
}

TEST(dom_read_documents, ok_learnyaml_read_documents_string_view)
{
    using char_type = char;

    auto file_result = yaml::impl::read_file<char>("../examples/learnyaml.yaml");
    ASSERT_EQ(file_result.result_code, yaml::impl::read_file_result_code::success);

    auto string_view = yaml::basic_string_view<char_type>{ file_result.data.data(), file_result.data.size() };

    const auto read_result = yaml::dom::read_documents<char_type>(string_view);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_TRUE(read_result);
    ASSERT_EQ(read_result.root_nodes.size(), size_t{ 1 });

    check_dom_learnyaml_tests(read_result.root_nodes.front());
}