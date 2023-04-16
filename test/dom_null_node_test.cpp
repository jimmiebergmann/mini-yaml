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

TEST(dom_null_node, fail_as_int32_t)
{
    using char_type = char;

    auto node = yaml::dom::node<char_type>{};
    EXPECT_EQ(node.as<int32_t>(1337), 1337);
}

TEST(dom_null_node, ok_as_string)
{
    using char_type = char;

    auto node = yaml::dom::node<char_type>{};
    EXPECT_STREQ(node.as<std::string>().c_str(), "null");
}

TEST(dom_null_node, ok_null)
{
    using char_type = char;

    auto node = yaml::dom::node<char_type>{};

    ASSERT_EQ(node.type(), yaml::dom::node_type::null);
    EXPECT_ANY_THROW_IGNORE_NODISCARD(node.as_scalar());
    EXPECT_ANY_THROW_IGNORE_NODISCARD(node.as_object());
    EXPECT_ANY_THROW_IGNORE_NODISCARD(node.as_array());
    EXPECT_TRUE(node.is_null());
    EXPECT_FALSE(node.is_scalar());
    EXPECT_FALSE(node.is_object());
    EXPECT_FALSE(node.is_array());
}

TEST(dom_null_node, ok_read_null_empty)
{
    const std::string input = "";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_EQ(read_result.current_line, 0);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::null);
}

TEST(dom_null_node, ok_read_null_null)
{
    const std::string input =  "null";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_EQ(read_result.current_line, 0);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::null);
}

TEST(dom_null_node, ok_read_null_Null)
{
    const std::string input = "Null";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_EQ(read_result.current_line, 0);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::null);
}

TEST(dom_null_node, ok_read_null_NULL)
{
    const std::string input = "NULL";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_EQ(read_result.current_line, 0);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::null);
}

TEST(dom_null_node, ok_read_null_tilde)
{
    const std::string input = "~";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_EQ(read_result.current_line, 0);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::null);
}

TEST(dom_null_node, ok_read_null_whitespace_and_newlines)
{
    const std::string input =
        "  \n"
        "    \n";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_EQ(read_result.current_line, 2);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::null);
}

TEST(dom_null_node, ok_read_object_with_null_values)
{
    const std::string input =
        "key 1:    \n"
        "key 2: ~\n"
        "key 3: null\n"
        "key 4: Null\n"
        "key 5: NULL\n";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::object);
    ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_object());
    auto& object_node = node.as_object();

    ASSERT_FALSE(object_node.empty());
    ASSERT_EQ(object_node.size(), size_t{ 5 });

    ASSERT_TRUE(object_node.contains("key 1"));
    ASSERT_TRUE(object_node.contains("key 2"));
    ASSERT_TRUE(object_node.contains("key 3"));
    ASSERT_TRUE(object_node.contains("key 4"));
    ASSERT_TRUE(object_node.contains("key 5"));

    auto& node_1 = object_node.at("key 1");
    EXPECT_EQ(node_1.type(), yaml::dom::node_type::null);
    auto& node_2 = object_node.at("key 2");
    EXPECT_EQ(node_2.type(), yaml::dom::node_type::null);
    auto& node_3 = object_node.at("key 3");
    EXPECT_EQ(node_3.type(), yaml::dom::node_type::null);
    auto& node_4 = object_node.at("key 4");
    EXPECT_EQ(node_4.type(), yaml::dom::node_type::null);
    auto& node_5 = object_node.at("key 5");
    EXPECT_EQ(node_5.type(), yaml::dom::node_type::null);
}