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
void ok_array_loop_test(Titerator begin, Titerator end, bool is_const_iterator, const std::vector<yaml::dom::node_type>& node_types)
{
    auto node_count = static_cast<size_t>(std::distance(begin, end));
    ASSERT_EQ(node_count, node_types.size());

    size_t loop_count = 0;
    for (auto it = begin; it != end; ++it) {
        ASSERT_LT(loop_count, node_types.size());

        auto& value = *it;
        EXPECT_EQ(value->type(), node_types[loop_count]);
        EXPECT_EQ(std::is_const<typename std::remove_reference<decltype(value)>::type>::value, is_const_iterator);

        ++loop_count;
    }

    EXPECT_EQ(loop_count, node_types.size());
}


TEST(dom_array_node, fail_as_int32_t)
{
    using char_type = char;
    auto node = yaml::dom::node<char_type>::create_array();
    EXPECT_EQ(node.as<int32_t>(123), 123);
}

TEST(dom_array_node, fail_as_string)
{
    using char_type = char;
    auto node = yaml::dom::node<char_type>::create_array();
    EXPECT_STREQ(node.as<std::string>("fail").c_str(), "fail");
}

TEST(dom_array_node, ok_array)
{
    using char_type = char;
    using node_t = yaml::dom::node<char_type>;

    auto node = node_t::create_array();

    ASSERT_EQ(node.type(), yaml::dom::node_type::array);

    EXPECT_ANY_THROW_IGNORE_NODISCARD(node.as_object());
    EXPECT_ANY_THROW_IGNORE_NODISCARD(node.as_scalar());
    ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_array());
    EXPECT_FALSE(node.is_null());
    EXPECT_FALSE(node.is_scalar());
    EXPECT_FALSE(node.is_object());
    EXPECT_TRUE(node.is_array());

    auto& array_node = node.as_array();
    EXPECT_EQ(&array_node.overlying_node(), &node);

    // Empty tests.
    EXPECT_TRUE(array_node.empty());
    ASSERT_EQ(array_node.size(), size_t{ 0 });
    EXPECT_FALSE(array_node.contains(0));

    // Insert
    {
        {
            array_node.push_back();

            EXPECT_FALSE(array_node.empty());
            ASSERT_EQ(array_node.size(), size_t{ 1 });
            EXPECT_TRUE(array_node.contains(0));
            EXPECT_FALSE(array_node.contains(1));

            ASSERT_NO_THROW_IGNORE_NODISCARD(array_node.at(0));
            auto& item = array_node.at(0);
            EXPECT_EQ(item.type(), yaml::dom::node_type::null);
        }
        {
            array_node.push_back(node_t::create_scalar());

            ASSERT_EQ(array_node.size(), size_t{ 2 });
            EXPECT_TRUE(array_node.contains(1));
            EXPECT_FALSE(array_node.contains(2));

            ASSERT_NO_THROW_IGNORE_NODISCARD(array_node.at(1));
            auto& item = array_node.at(1);
            EXPECT_EQ(item.type(), yaml::dom::node_type::scalar);
        }
        {
            array_node.insert(array_node.begin());

            ASSERT_EQ(array_node.size(), size_t{ 3 });
            EXPECT_TRUE(array_node.contains(2));
            EXPECT_FALSE(array_node.contains(3));

            ASSERT_NO_THROW_IGNORE_NODISCARD(array_node.at(0));
            auto& item = array_node.at(0);
            EXPECT_EQ(item.type(), yaml::dom::node_type::null);
        }
        {
            array_node.insert(array_node.end(), node_t::create_object());

            ASSERT_EQ(array_node.size(), size_t{ 4 });
            EXPECT_TRUE(array_node.contains(3));
            EXPECT_FALSE(array_node.contains(4));

            ASSERT_NO_THROW_IGNORE_NODISCARD(array_node.at(3));
            auto& item = array_node.at(3);
            EXPECT_EQ(item.type(), yaml::dom::node_type::object);
        }
        {
            array_node.insert(array_node.begin() + 1, node_t::create_array());

            ASSERT_EQ(array_node.size(), size_t{ 5 });
            EXPECT_TRUE(array_node.contains(4));
            EXPECT_FALSE(array_node.contains(5));

            ASSERT_NO_THROW_IGNORE_NODISCARD(array_node.at(1));
            auto& item = array_node.at(1);
            EXPECT_EQ(item.type(), yaml::dom::node_type::array);
        }
    }

    // Loop
    {
        std::vector<yaml::dom::node_type> node_types = {
            yaml::dom::node_type::null,
            yaml::dom::node_type::array,
            yaml::dom::node_type::null,
            yaml::dom::node_type::scalar,
            yaml::dom::node_type::object
        };

        std::vector<yaml::dom::node_type> node_types_reverse = {
            yaml::dom::node_type::object,
            yaml::dom::node_type::scalar,
            yaml::dom::node_type::null,
            yaml::dom::node_type::array,
            yaml::dom::node_type::null
        };

        const auto& const_array_node = array_node;

        ok_array_loop_test(array_node.begin(), array_node.end(), false, node_types);
        ok_array_loop_test(const_array_node.begin(), const_array_node.end(), true, node_types);
        ok_array_loop_test(array_node.cbegin(), array_node.cend(), true, node_types);
        ok_array_loop_test(array_node.rbegin(), array_node.rend(), false, node_types_reverse);
        ok_array_loop_test(const_array_node.rbegin(), const_array_node.rend(), true, node_types_reverse);
        ok_array_loop_test(array_node.crbegin(), array_node.crend(), true, node_types_reverse);
    }

    // Erase
    {
        ASSERT_EQ(array_node.size(), size_t{ 5 });

        auto it1 = array_node.erase(array_node.begin());
        ASSERT_EQ(array_node.size(), size_t{ 4 });

        array_node.erase(it1, std::next(std::next(it1)));
        ASSERT_EQ(array_node.size(), size_t{ 2 });

        ASSERT_NO_THROW_IGNORE_NODISCARD(array_node.at(0));
        auto& item1 = array_node.at(0);
        EXPECT_EQ(item1.type(), yaml::dom::node_type::scalar);

        ASSERT_NO_THROW_IGNORE_NODISCARD(array_node.at(1));
        auto& item2 = array_node.at(1);
        EXPECT_EQ(item2.type(), yaml::dom::node_type::object);

        array_node.pop_back();
        ASSERT_EQ(array_node.size(), size_t{ 1 });

        auto& item1_1 = array_node.at(0);
        EXPECT_EQ(item1_1.type(), yaml::dom::node_type::scalar);
    }
}

TEST(dom_array_node, ok_read)
{
    const std::string input =
        "- test 1\n"
        "- test 2\n"
        "- test 3\n"
        "- test 4\n";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::array);
    ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_array());
    auto& array_node = node.as_array();

    ASSERT_FALSE(array_node.empty());
    ASSERT_EQ(array_node.size(), size_t{ 4 });

    EXPECT_EQ(array_node.at(0).type(), yaml::dom::node_type::scalar);
    EXPECT_EQ(array_node.at(1).type(), yaml::dom::node_type::scalar);
    EXPECT_EQ(array_node.at(2).type(), yaml::dom::node_type::scalar);
    EXPECT_EQ(array_node.at(3).type(), yaml::dom::node_type::scalar);
}