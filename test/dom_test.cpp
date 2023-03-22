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

// =====================================================================
// Tests

TEST(dom_read, ok_quickstart)
{
    const std::string input =
        "scalar: foo bar\n"
        "list:\n"
        " - hello world\n"
        " - integer : 123\n"
        "   boolean : true";
    
    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    auto root = std::move(read_result.root_node);
    ASSERT_EQ(root.type(), yaml::dom::node_type::object);

    {
        auto str1 = root.as_object().at("scalar").as_scalar().as_string();
        auto str2 = root.as_object().at("list").as_array().at(0).as_scalar().as_string();
        auto str3 = root.as_object().at("list").as_array().at(1).as_object().at("integer").as_scalar().as_string();
        auto str4 = root.as_object().at("list").as_array().at(1).as_object().at("boolean").as_scalar().as_string();

        EXPECT_STREQ(str1.c_str(), "foo bar");
        EXPECT_STREQ(str2.c_str(), "hello world");
        EXPECT_STREQ(str3.c_str(), "123");
        EXPECT_STREQ(str4.c_str(), "true");
    }
    {
        auto str1 = root["scalar"].as_string();
        auto str2 = root["list"][0].as_string();
        auto str3 = root["list"][1]["integer"].as_string();
        auto str4 = root["list"][1]["boolean"].as_string();

        EXPECT_STREQ(str1.c_str(), "foo bar");
        EXPECT_STREQ(str2.c_str(), "hello world");
        EXPECT_STREQ(str3.c_str(), "123");
        EXPECT_STREQ(str4.c_str(), "true");
    }
}

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

TEST(dom_create_node, ok_array)
{
    using char_type = char;
    using node_t = yaml::dom::node<char_type>;

    auto node = node_t::create_array();

    ASSERT_EQ(node.type(), yaml::dom::node_type::array);
    EXPECT_ANY_THROW(node.as_object());
    EXPECT_ANY_THROW(node.as_scalar());
    ASSERT_NO_THROW(node.as_array());
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

            ASSERT_NO_THROW(array_node.at(0));
            auto& item = array_node.at(0);
            EXPECT_EQ(item.type(), yaml::dom::node_type::null);
        }
        {
            array_node.push_back(node_t::create_scalar());

            ASSERT_EQ(array_node.size(), size_t{ 2 });
            EXPECT_TRUE(array_node.contains(1));
            EXPECT_FALSE(array_node.contains(2));

            ASSERT_NO_THROW(array_node.at(1));
            auto& item = array_node.at(1);
            EXPECT_EQ(item.type(), yaml::dom::node_type::scalar);
        }
        {
            array_node.insert(array_node.begin());

            ASSERT_EQ(array_node.size(), size_t{ 3 });
            EXPECT_TRUE(array_node.contains(2));
            EXPECT_FALSE(array_node.contains(3));

            ASSERT_NO_THROW(array_node.at(0));
            auto& item = array_node.at(0);
            EXPECT_EQ(item.type(), yaml::dom::node_type::null);
        }
        {
            array_node.insert(array_node.end(), node_t::create_object());

            ASSERT_EQ(array_node.size(), size_t{ 4 });
            EXPECT_TRUE(array_node.contains(3));
            EXPECT_FALSE(array_node.contains(4));

            ASSERT_NO_THROW(array_node.at(3));
            auto& item = array_node.at(3);
            EXPECT_EQ(item.type(), yaml::dom::node_type::object);
        }
        {
            array_node.insert(array_node.begin() + 1, node_t::create_array());

            ASSERT_EQ(array_node.size(), size_t{ 5 });
            EXPECT_TRUE(array_node.contains(4));
            EXPECT_FALSE(array_node.contains(5));

            ASSERT_NO_THROW(array_node.at(1));
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

        auto it2 = array_node.erase(it1, std::next(std::next(it1)));
        ASSERT_EQ(array_node.size(), size_t{ 2 });

        ASSERT_NO_THROW(array_node.at(0));
        auto& item1 = array_node.at(0);
        EXPECT_EQ(item1.type(), yaml::dom::node_type::scalar);

        ASSERT_NO_THROW(array_node.at(1));
        auto& item2 = array_node.at(1);
        EXPECT_EQ(item2.type(), yaml::dom::node_type::object);

        array_node.pop_back();
        ASSERT_EQ(array_node.size(), size_t{ 1 });

        auto& item1_1 = array_node.at(0);
        EXPECT_EQ(item1_1.type(), yaml::dom::node_type::scalar);
    }
}

TEST(dom_create_node, ok_null)
{
    using char_type = char;

    auto node = yaml::dom::node<char_type>{};

    ASSERT_EQ(node.type(), yaml::dom::node_type::null);
    EXPECT_ANY_THROW(node.as_scalar());
    EXPECT_ANY_THROW(node.as_object());
    EXPECT_ANY_THROW(node.as_array());
    EXPECT_TRUE(node.is_null());
    EXPECT_FALSE(node.is_scalar());
    EXPECT_FALSE(node.is_object());
    EXPECT_FALSE(node.is_array());
}

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

TEST(dom_create_node, ok_object)
{
    using char_type = char;

    auto node = yaml::dom::node<char_type>::create_object();

    ASSERT_EQ(node.type(), yaml::dom::node_type::object);
    ASSERT_NO_THROW(node.as_object());
    EXPECT_ANY_THROW(node.as_scalar());
    EXPECT_ANY_THROW(node.as_array());
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
        EXPECT_EQ(object_node.size(), size_t{2});

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

template<typename Titerator>
void ok_scalar_loop_test(Titerator begin, Titerator end, bool is_const_iterator, const std::vector<std::string>& lines)
{
    auto node_count = static_cast<size_t>(std::distance(begin, end));
    ASSERT_EQ(node_count, lines.size());

    size_t loop_count = 0;
    for (auto it = begin; it != end; ++it) {
        ASSERT_LT(loop_count, lines.size());

        auto& value = *it;
        EXPECT_EQ(std::is_const<typename std::remove_reference<decltype(value)>::type>::value, is_const_iterator);
        EXPECT_STREQ(std::string(value.data(), value.size()).c_str(), lines[loop_count].c_str());

        ++loop_count;
    }

    EXPECT_EQ(loop_count, lines.size());
}

TEST(dom_create_node, ok_scalar)
{
    using char_type = char;

    auto node = yaml::dom::node<char_type>::create_scalar();

    ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
    ASSERT_NO_THROW(node.as_scalar());
    EXPECT_ANY_THROW(node.as_object());
    EXPECT_ANY_THROW(node.as_array());
    EXPECT_FALSE(node.is_null());
    EXPECT_TRUE(node.is_scalar());
    EXPECT_FALSE(node.is_object());
    EXPECT_FALSE(node.is_array());

    auto& scalar_node = node.as_scalar();
    EXPECT_EQ(&scalar_node.overlying_node(), &node);

    // Block style tests.
    EXPECT_EQ(scalar_node.block_style(), yaml::block_style_type::none);
    EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::strip);

    scalar_node.block_style(yaml::block_style_type::literal);
    EXPECT_EQ(scalar_node.block_style(), yaml::block_style_type::literal);

    scalar_node.chomping(yaml::chomping_type::keep);
    EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::keep);

    // Empty tests.
    ASSERT_TRUE(scalar_node.empty());
    ASSERT_EQ(scalar_node.size(), size_t{ 0 });

    EXPECT_EQ(scalar_node.begin(), scalar_node.end());
    EXPECT_EQ(scalar_node.cbegin(), scalar_node.cend());
    EXPECT_EQ(scalar_node.crbegin(), scalar_node.crend());

    // Insert tests.
    {
        scalar_node.push_back("First line of scalar.");

        ASSERT_FALSE(scalar_node.empty());
        ASSERT_EQ(scalar_node.size(), size_t{ 1 });

        ASSERT_NE(scalar_node.begin(), scalar_node.end());
        ASSERT_NE(scalar_node.cbegin(), scalar_node.cend());
        ASSERT_NE(scalar_node.crbegin(), scalar_node.crend());

        EXPECT_STREQ(scalar_node.begin()->c_str(), "First line of scalar.");

        scalar_node.push_back("Second line of scalar.");

        ASSERT_EQ(scalar_node.size(), size_t{ 2 });
    }
    // Loop
    {
        std::vector<std::string> lines = {
            "First line of scalar.",
            "Second line of scalar."
        };

        std::vector<std::string> lines_reverse = {
            "Second line of scalar.",
            "First line of scalar."           
        };

        const auto& const_scalar_node = scalar_node;

        ok_scalar_loop_test(scalar_node.begin(), scalar_node.end(), false, lines);
        ok_scalar_loop_test(const_scalar_node.begin(), const_scalar_node.end(), true, lines);
        ok_scalar_loop_test(scalar_node.cbegin(), scalar_node.cend(), true, lines);
        ok_scalar_loop_test(scalar_node.rbegin(), scalar_node.rend(), false, lines_reverse);
        ok_scalar_loop_test(const_scalar_node.rbegin(), const_scalar_node.rend(), true, lines_reverse);
        ok_scalar_loop_test(scalar_node.crbegin(), scalar_node.crend(), true, lines_reverse);
    }

    // as_string() tests.
    {
        scalar_node.block_style(yaml::block_style_type::none);
        scalar_node.chomping(yaml::chomping_type::strip);
        EXPECT_STREQ(scalar_node.as_string().c_str(), "First line of scalar. Second line of scalar.");
    }
}

TEST(dom_read, ok_object)
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
    ASSERT_NO_THROW(node.as_object());
    auto& object_node = node.as_object();

    ASSERT_FALSE(object_node.empty());
    ASSERT_EQ(object_node.size(), size_t{ 4 });

    EXPECT_TRUE(object_node.contains("key 1"));
    EXPECT_TRUE(object_node.contains("key 2"));
    EXPECT_TRUE(object_node.contains("key 3"));
    EXPECT_TRUE(object_node.contains("key 4"));
}

TEST(dom_read, ok_null)
{
    const std::string input =
        "key 1: ~\n"
        "key 2: null\n"
        "key 3: Null\n"
        "key 4: NULL\n";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::object);
    ASSERT_NO_THROW(node.as_object());
    auto& object_node = node.as_object();

    ASSERT_FALSE(object_node.empty());
    ASSERT_EQ(object_node.size(), size_t{ 4 });

    ASSERT_TRUE(object_node.contains("key 1"));
    ASSERT_TRUE(object_node.contains("key 2"));
    ASSERT_TRUE(object_node.contains("key 3"));
    ASSERT_TRUE(object_node.contains("key 4"));

    auto& node_1 = object_node.at("key 1");
    EXPECT_EQ(node_1.type(), yaml::dom::node_type::null);
    auto& node_2 = object_node.at("key 2");
    EXPECT_EQ(node_2.type(), yaml::dom::node_type::null);
    auto& node_3 = object_node.at("key 3");
    EXPECT_EQ(node_3.type(), yaml::dom::node_type::null);
    auto& node_4 = object_node.at("key 4");
    EXPECT_EQ(node_4.type(), yaml::dom::node_type::null);

}

TEST(dom_read, ok_scalar)
{
    const std::string input =
        "This is a scalar\n"
        "with multiple lines.";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
    ASSERT_NO_THROW(node.as_scalar());
    auto& scalar_node = node.as_scalar();

    EXPECT_EQ(scalar_node.block_style(), yaml::block_style_type::none);
    EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::strip);

    EXPECT_STREQ(scalar_node.as_string().c_str(), "This is a scalar with multiple lines.");
}

TEST(dom_read, ok_scalar_with_gaps)
{
    const std::string input =
        "first\n"
        "second\n"
        "\n"
        "third\n"
        "\n"
        "\n"
        "fourth\n"
        "\n"
        "\n";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
    ASSERT_NO_THROW(node.as_scalar());
    auto& scalar_node = node.as_scalar();

    EXPECT_EQ(scalar_node.block_style(), yaml::block_style_type::none);
    EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::strip);

    EXPECT_EQ(scalar_node.size(), size_t{ 7 });

    EXPECT_STREQ(scalar_node.as_string().c_str(), "first second\nthird\n\nfourth");

    // Add removed trailing empty lines + front empty line.
    scalar_node.insert(scalar_node.begin(), "");
    scalar_node.push_back("");
    scalar_node.push_back("");
    EXPECT_EQ(scalar_node.size(), size_t{ 10 });

    EXPECT_STREQ(scalar_node.as_string().c_str(), "first second\nthird\n\nfourth");
}

TEST(dom_read, ok_file_learnyaml)
{
    using char_type = char;

    auto read_result = yaml::dom::read_document_from_file<char_type>("../test/learnyaml.yaml");
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    auto root_node = std::move(read_result.root_node);
    ASSERT_EQ(root_node.type(), yaml::dom::node_type::object);
    ASSERT_NO_THROW(root_node.as_object());
    auto& root_object_node = root_node.as_object();

    ASSERT_EQ(root_object_node.size(), size_t{ 26 });

    {
        auto it = root_object_node.find("key");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "value");
    }
    {
        auto it = root_object_node.find("another_key");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "Another value goes here.");
    }
    {
        auto it = root_object_node.find("a_number_value");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "100");
    }
    {
        auto it = root_object_node.find("scientific_notation");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "1e+12");
    }
    {
        auto it = root_object_node.find("hex_notation");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "0x123");
    }
    {
        auto it = root_object_node.find("octal_notation");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "0123");
    }

    {
        auto it = root_object_node.find("boolean");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "true");
    }
    {
        auto it = root_object_node.find("null_value");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        EXPECT_EQ(node.type(), yaml::dom::node_type::null);
    }
    {
        auto it = root_object_node.find("another_null_value");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        EXPECT_EQ(node.type(), yaml::dom::node_type::null);
    }
    {
        auto it = root_object_node.find("key with spaces");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "value");
    }
    
    {
        auto it = root_object_node.find("empty_value");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::null);
    }

    
    {
        auto it = root_object_node.find("no");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "no");
    }
    {
        auto it = root_object_node.find("yes");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "No");
    }
    {
        auto it = root_object_node.find("not_enclosed");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "yes");
    }
    {
        auto it = root_object_node.find("enclosed");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "\"yes\"");
    }

    {
        auto it = root_object_node.find("Superscript two");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "\\u00B2");
    }

    {
        auto it = root_object_node.find("literal_block");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_EQ(scalar_node.block_style(), yaml::block_style_type::literal);
        EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::clip);

        auto string = scalar_node.as_string();

        static const std::string compare_str = 
            "This entire block of text will be the value of the 'literal_block' key,\n"
            "with line breaks being preserved.\n"
            "\n"
            "The literal continues until de-dented, and the leading indentation is\n"
            "stripped.\n"
            "\n"
            "    Any lines that are 'more-indented' keep the rest of their indentation -\n"
            "    these lines will be indented by 4 spaces.\n";

        EXPECT_STREQ(string.c_str(), compare_str.c_str());
    }
    {
        auto it = root_object_node.find("folded_style");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_EQ(scalar_node.block_style(), yaml::block_style_type::folded);
        EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::clip);

        auto string = scalar_node.as_string();

        static const std::string compare_str =
            "This entire block of text will be the value of 'folded_style', but this time, all newlines will be replaced with a single space.\n"
            "Blank lines, like above, are converted to a newline character.\n"
            "\n"
            "    'More-indented' lines keep their newlines, too -\n"
            "    this text will appear over two lines.\n";

        EXPECT_STREQ(string.c_str(), compare_str.c_str());
    }
    {
        auto it = root_object_node.find("literal_strip");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_EQ(scalar_node.block_style(), yaml::block_style_type::literal);
        EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::strip);

        auto string = scalar_node.as_string();

        static const std::string compare_str =
            "This entire block of text will be the value of the 'literal_block' key,\n"
            "with trailing blank line being stripped.";

        EXPECT_STREQ(string.c_str(), compare_str.c_str());
    }
    {
        auto it = root_object_node.find("block_strip");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_EQ(scalar_node.block_style(), yaml::block_style_type::folded);
        EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::strip);

        auto string = scalar_node.as_string();

        static const std::string compare_str =
            "This entire block of text will be the value of 'folded_style', but this time, all newlines will be replaced with a single space and  trailing blank line being stripped.";

        EXPECT_STREQ(string.c_str(), compare_str.c_str());
    }
    {
        auto it = root_object_node.find("literal_keep");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_EQ(scalar_node.block_style(), yaml::block_style_type::literal);
        EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::keep);

        auto string = scalar_node.as_string();

        static const std::string compare_str =
            "This entire block of text will be the value of the 'literal_block' key,\n"
            "with trailing blank line being kept.\n\n";

        EXPECT_STREQ(string.c_str(), compare_str.c_str());
    }
    {
        auto it = root_object_node.find("block_keep");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_EQ(scalar_node.block_style(), yaml::block_style_type::folded);
        EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::keep);

        auto string = scalar_node.as_string();

        static const std::string compare_str =
            "This entire block of text will be the value of 'folded_style', but this time, all newlines will be replaced with a single space and  trailing blank line being kept.\n"
            "\n";

        EXPECT_STREQ(string.c_str(), compare_str.c_str());
    }

    {
        auto it = root_object_node.find("a_nested_map");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::object);
        ASSERT_NO_THROW(node.as_object());
        auto& object_node = node.as_object();

        EXPECT_EQ(object_node.size(), size_t{ 3 });
        {
            auto it2 = object_node.find("key");
            ASSERT_NE(it2, object_node.end());

            EXPECT_STREQ(it2->first.c_str(), "key");

            auto& node2 = *it2->second;
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);
            ASSERT_NO_THROW(node2.as_scalar());
            auto& scalar_node = node2.as_scalar();

            auto string = scalar_node.as_string();
            EXPECT_STREQ(string.c_str(), "value");
        }
        {
            auto it2 = object_node.find("another_key");
            ASSERT_NE(it2, object_node.end());

            EXPECT_STREQ(it2->first.c_str(), "another_key");

            auto& node2 = *it2->second;
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);
            ASSERT_NO_THROW(node2.as_scalar());
            auto& scalar_node = node2.as_scalar();

            auto string = scalar_node.as_string();
            EXPECT_STREQ(string.c_str(), "Another Value");
        }
        {
            auto it2 = object_node.find("another_nested_map");
            ASSERT_NE(it2, object_node.end());

            EXPECT_STREQ(it2->first.c_str(), "another_nested_map");

            auto& node2 = *it2->second;
            ASSERT_EQ(node2.type(), yaml::dom::node_type::object);
            ASSERT_NO_THROW(node2.as_object());
            auto& object_node2 = node2.as_object();

            EXPECT_EQ(object_node2.size(), size_t{ 1 });
            {
                auto it3 = object_node2.find("hello");
                ASSERT_NE(it3, object_node2.end());

                EXPECT_STREQ(it3->first.c_str(), "hello");

                auto& node3 = *it3->second;
                ASSERT_EQ(node3.type(), yaml::dom::node_type::scalar);
                ASSERT_NO_THROW(node3.as_scalar());
                auto& scalar_node2 = node3.as_scalar();

                auto string2 = scalar_node2.as_string();
                EXPECT_STREQ(string2.c_str(), "hello");
            }
        }

    }

    {
        auto it = root_object_node.find("0.25");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "a float key");
    }

    {
        auto it = root_object_node.find("a_sequence");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::array);
        ASSERT_NO_THROW(node.as_array());
        auto& array_node = node.as_array();

        ASSERT_EQ(array_node.size(), size_t{ 7 });
        {
            auto& node2 = array_node.at(0);
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);

            ASSERT_NO_THROW(node2.as_scalar());
            auto& scalar_node2 = node2.as_scalar();

            auto string = scalar_node2.as_string();
            EXPECT_STREQ(string.c_str(), "Item 1");
        }
        {
            auto& node2 = array_node.at(1);
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);

            ASSERT_NO_THROW(node2.as_scalar());
            auto& scalar_node2 = node2.as_scalar();

            auto string = scalar_node2.as_string();
            EXPECT_STREQ(string.c_str(), "Item 2");
        }
        {
            auto& node2 = array_node.at(2);
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);

            ASSERT_NO_THROW(node2.as_scalar());
            auto& scalar_node2 = node2.as_scalar();

            auto string = scalar_node2.as_string();
            EXPECT_STREQ(string.c_str(), "0.5");
        }
        {
            auto& node2 = array_node.at(3);
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);

            ASSERT_NO_THROW(node2.as_scalar());
            auto& scalar_node2 = node2.as_scalar();

            auto string = scalar_node2.as_string();
            EXPECT_STREQ(string.c_str(), "Item 4");
        }
        {
            auto& node2 = array_node.at(4);
            ASSERT_EQ(node2.type(), yaml::dom::node_type::object);

            ASSERT_NO_THROW(node2.as_object());
            auto& object_node2 = node2.as_object();

            ASSERT_EQ(object_node2.size(), size_t{ 2 });
            {
                auto& node3 = object_node2.at("key");
                ASSERT_EQ(node3.type(), yaml::dom::node_type::scalar);

                ASSERT_NO_THROW(node3.as_scalar());
                auto& scalar_node3 = node3.as_scalar();

                auto string = scalar_node3.as_string();
                EXPECT_STREQ(string.c_str(), "value");
            }
            {
                auto& node3 = object_node2.at("another_key");
                ASSERT_EQ(node3.type(), yaml::dom::node_type::scalar);

                ASSERT_NO_THROW(node3.as_scalar());
                auto& scalar_node3 = node3.as_scalar();

                auto string = scalar_node3.as_string();
                EXPECT_STREQ(string.c_str(), "another_value");
            }
        }
        {
            auto& node2 = array_node.at(5);
            ASSERT_EQ(node2.type(), yaml::dom::node_type::array);

            ASSERT_NO_THROW(node2.as_array());
            auto& array_node2 = node2.as_array();
            
            ASSERT_EQ(array_node2.size(), size_t{ 2 });
            {
                auto& node3 = array_node2.at(0);
                ASSERT_EQ(node3.type(), yaml::dom::node_type::scalar);

                ASSERT_NO_THROW(node3.as_scalar());
                auto& scalar_node3 = node3.as_scalar();

                auto string = scalar_node3.as_string();
                EXPECT_STREQ(string.c_str(), "This is a sequence");
            }
            {
                auto& node3 = array_node2.at(1);
                ASSERT_EQ(node3.type(), yaml::dom::node_type::scalar);

                ASSERT_NO_THROW(node3.as_scalar());
                auto& scalar_node3 = node3.as_scalar();

                auto string = scalar_node3.as_string();
                EXPECT_STREQ(string.c_str(), "inside another sequence");
            }
        }
        {
            auto& node2 = array_node.at(6);
            ASSERT_EQ(node2.type(), yaml::dom::node_type::array);

            ASSERT_NO_THROW(node2.as_array());
            auto& array_node2 = node2.as_array();

            ASSERT_EQ(array_node2.size(), size_t{ 1 });
            {
                auto& node3 = array_node2.at(0);
                ASSERT_EQ(node3.type(), yaml::dom::node_type::array);

                ASSERT_NO_THROW(node3.as_array());
                auto& array_node3 = node3.as_array();

                ASSERT_EQ(array_node3.size(), size_t{ 2 });
                {
                    auto& node4 = array_node3.at(0);
                    ASSERT_EQ(node4.type(), yaml::dom::node_type::scalar);

                    ASSERT_NO_THROW(node4.as_scalar());
                    auto& scalar_node4 = node4.as_scalar();

                    auto string = scalar_node4.as_string();
                    EXPECT_STREQ(string.c_str(), "Nested sequence indicators");
                }
                {
                    auto& node4 = array_node3.at(1);
                    ASSERT_EQ(node4.type(), yaml::dom::node_type::scalar);

                    ASSERT_NO_THROW(node4.as_scalar());
                    auto& scalar_node4 = node4.as_scalar();

                    auto string = scalar_node4.as_string();
                    EXPECT_STREQ(string.c_str(), "can be collapsed");
                }
            }
        }

    }

    {
        auto it = root_object_node.find("set2");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::object);
        ASSERT_NO_THROW(node.as_object());
        auto& object_node = node.as_object();

        EXPECT_EQ(object_node.size(), size_t{ 3 });
        {
            auto it2 = object_node.find("item1");
            ASSERT_NE(it2, object_node.end());

            EXPECT_STREQ(it2->first.c_str(), "item1");

            auto& node2 = *it2->second;
            EXPECT_EQ(node2.type(), yaml::dom::node_type::null);
        }
        {
            auto it2 = object_node.find("item2");
            ASSERT_NE(it2, object_node.end());

            EXPECT_STREQ(it2->first.c_str(), "item2");

            auto& node2 = *it2->second;
            EXPECT_EQ(node2.type(), yaml::dom::node_type::null);
        }
        {
            auto it2 = object_node.find("item3");
            ASSERT_NE(it2, object_node.end());

            EXPECT_STREQ(it2->first.c_str(), "item3");

            auto& node2 = *it2->second;
            EXPECT_EQ(node2.type(), yaml::dom::node_type::null);
        }
    }

}

TEST(dom_read, fail_unknown_file)
{
    using char_type = char;

    auto read_result = yaml::dom::read_document_from_file<char_type>("../test/this_file_does_not_exist.some_extension");
    EXPECT_EQ(read_result.result_code, yaml::read_result_code::cannot_open_file);
    EXPECT_FALSE(read_result);
}