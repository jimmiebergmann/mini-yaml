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

template<typename Titerator>
void ok_object_loop_test(Titerator begin, Titerator end, bool is_const_iterator, const std::vector<std::pair<std::string, yaml::dom::node_type>>& node_types)
{
    auto node_count = static_cast<size_t>(std::distance(begin, end));
    ASSERT_EQ(node_count, node_types.size());

    size_t loop_count = 0;
    for (auto it = begin; it != end; ++it) {
        ASSERT_LT(loop_count, node_types.size());

        auto& key = it->first;
        auto& value = (*it).second;
        EXPECT_EQ(key, node_types[loop_count].first);
        EXPECT_EQ(value->type(), node_types[loop_count].second);
        EXPECT_EQ(std::is_const<typename std::remove_reference<decltype(value)>::type>::value, is_const_iterator);

        ++loop_count;
    }

    EXPECT_EQ(loop_count, node_types.size());
}

TEST(dom_object_node, fail_as_int32_t)
{
    using char_type = char;
    auto node = yaml::dom::node<char_type>::create_object();
    EXPECT_EQ(node.as<int32_t>(123), 123);
}

TEST(dom_object_node, fail_as_string)
{
    using char_type = char;
    auto node = yaml::dom::node<char_type>::create_object();
    EXPECT_STREQ(node.as<std::string>("fail").c_str(), "fail");
}

TEST(dom_object_node, ok_object)
{
    using char_type = char;

    auto node = yaml::dom::node<char_type>::create_object();

    ASSERT_EQ(node.type(), yaml::dom::node_type::object);
    ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_object());
    EXPECT_ANY_THROW_IGNORE_NODISCARD(node.as_scalar());
    EXPECT_ANY_THROW_IGNORE_NODISCARD(node.as_array());
    EXPECT_FALSE(node.is_null());
    EXPECT_FALSE(node.is_scalar());
    EXPECT_TRUE(node.is_object());
    EXPECT_FALSE(node.is_array());

    auto& object_node = node.as_object();
    EXPECT_EQ(&object_node.overlying_node(), &node);

    // Empty tests.
    EXPECT_TRUE(object_node.empty());
    ASSERT_EQ(object_node.size(), size_t{ 0 });

    // Insert
    {
        EXPECT_FALSE(object_node.contains("key 1"));

        auto result = object_node.insert("key 1");
        ASSERT_TRUE(result.second);
        EXPECT_FALSE(object_node.empty());
        EXPECT_EQ(object_node.size(), size_t{ 1 });

        EXPECT_TRUE(object_node.contains("key 1"));
    }
    {
        EXPECT_TRUE(object_node.contains("key 1"));
        auto result = object_node.insert("key 1");
        ASSERT_FALSE(result.second);
        EXPECT_TRUE(object_node.contains("key 1"));
    }
    {
        EXPECT_FALSE(object_node.contains("key 2"));

        auto result = object_node.insert("key 2", yaml::dom::node<char_type>::create_scalar());
        EXPECT_TRUE(result.second);
        EXPECT_FALSE(object_node.empty());
        EXPECT_EQ(object_node.size(), size_t{ 2 });

        EXPECT_TRUE(object_node.contains("key 2"));
    }
    {
        EXPECT_FALSE(object_node.contains("key 3"));

        auto result = object_node.insert("key 3", yaml::dom::node<char_type>::create_object());
        EXPECT_TRUE(result.second);
        EXPECT_FALSE(object_node.empty());
        EXPECT_EQ(object_node.size(), size_t{ 3 });

        EXPECT_TRUE(object_node.contains("key 3"));
    }

    // Loop
    {
        std::vector<std::pair<std::string, yaml::dom::node_type>> node_types = {
            { "key 1", yaml::dom::node_type::null },
            { "key 2", yaml::dom::node_type::scalar },
            { "key 3", yaml::dom::node_type::object },
        };

        std::vector<std::pair<std::string, yaml::dom::node_type>> node_types_reverse = {
            { "key 3", yaml::dom::node_type::object },
            { "key 2", yaml::dom::node_type::scalar },
            { "key 1", yaml::dom::node_type::null },
        };

        const auto& const_object_node = object_node;

        ok_object_loop_test(object_node.begin(), object_node.end(), false, node_types);
        ok_object_loop_test(const_object_node.begin(), const_object_node.end(), true, node_types);
        ok_object_loop_test(object_node.cbegin(), object_node.cend(), true, node_types);
        ok_object_loop_test(object_node.rbegin(), object_node.rend(), false, node_types_reverse);
        ok_object_loop_test(const_object_node.rbegin(), const_object_node.rend(), true, node_types_reverse);
        ok_object_loop_test(object_node.crbegin(), object_node.crend(), true, node_types_reverse);
    }

    // Erase
    {
        auto it = object_node.find("key 2");
        ASSERT_NE(it, object_node.end());

        auto next_it = object_node.erase(it);
        EXPECT_EQ(object_node.size(), size_t{ 2 });

        ASSERT_NE(next_it, object_node.end());
        EXPECT_STREQ(next_it->first.c_str(), "key 3");

        auto erase_ret = object_node.erase("key 1");
        EXPECT_EQ(object_node.size(), size_t{ 1 });
        EXPECT_EQ(erase_ret, size_t{ 1 });

        next_it = object_node.erase(object_node.begin());
        EXPECT_EQ(object_node.size(), size_t{ 0 });
        EXPECT_TRUE(object_node.empty());
        ASSERT_EQ(next_it, object_node.end());
    }
}

TEST(dom_object_node, ok_read)
{
    const std::string input =
        "key 1: test 1\n"
        "key 2: test 2\n"
        "key 3: test 3\n"
        "key 4: test 4\n";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::object);
    ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_object());
    auto& object_node = node.as_object();

    ASSERT_FALSE(object_node.empty());
    ASSERT_EQ(object_node.size(), size_t{ 4 });

    EXPECT_TRUE(object_node.contains("key 1"));
    EXPECT_TRUE(object_node.contains("key 2"));
    EXPECT_TRUE(object_node.contains("key 3"));
    EXPECT_TRUE(object_node.contains("key 4"));
}