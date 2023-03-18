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

TEST(dom_create_node, ok_scalar)
{
    using char_type = char;

    auto node = yaml::dom::node<char_type>::create_scalar();

    ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
    ASSERT_NO_THROW(node.as_scalar());
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
    scalar_node.push_back("First line of scalar.");

    ASSERT_FALSE(scalar_node.empty());
    ASSERT_EQ(scalar_node.size(), size_t{ 1 });

    ASSERT_NE(scalar_node.begin(), scalar_node.end());
    ASSERT_NE(scalar_node.cbegin(), scalar_node.cend());
    ASSERT_NE(scalar_node.crbegin(), scalar_node.crend());

    EXPECT_STREQ(scalar_node.begin()->c_str(), "First line of scalar.");

    scalar_node.push_back("Second line of scalar.");

    ASSERT_EQ(scalar_node.size(), size_t{ 2 });

    // as_string() tests.
    scalar_node.block_style(yaml::block_style_type::none);
    scalar_node.chomping(yaml::chomping_type::strip);
    EXPECT_STREQ(scalar_node.as_string().c_str(), "First line of scalar.Second line of scalar.");
}

TEST(dom_create_node, ok_object)
{
    using char_type = char;

    auto node = yaml::dom::node<char_type>::create_object();

    ASSERT_EQ(node.type(), yaml::dom::node_type::object);
    ASSERT_NO_THROW(node.as_object());
    auto& object_node = node.as_object();
    EXPECT_EQ(&object_node.overlying_node(), &node);

    // Empty tests.
    ASSERT_TRUE(object_node.empty());
    ASSERT_EQ(object_node.size(), size_t{ 0 });

    // Create
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
        size_t loop_count = 0;
        static std::array<std::string, 3> keys = { "key 1", "key 2", "key 3" };
        static std::array<yaml::dom::node_type, 3> node_types = { yaml::dom::node_type::null, yaml::dom::node_type::scalar, yaml::dom::node_type::object };
        for (auto it = object_node.begin(); it != object_node.end(); ++it) {
            ASSERT_LT(loop_count, 3);
        
            auto& key = it->first;
            auto& value = (*it).second;

            EXPECT_EQ(key, keys[loop_count]);
            EXPECT_EQ(value->type(), node_types[loop_count]);

            ++loop_count;
        }

        EXPECT_EQ(loop_count, size_t{ 3 });
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

TEST(dom_read, ok_object_root)
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

TEST(dom_read, ok_scalar_root)
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

    EXPECT_STREQ(scalar_node.as_string().c_str(), "This is a scalarwith multiple lines."); // TODO: Currently invalid formatting.
}


TEST(dom_read, ok_file_learnyaml)
{
    using char_type = char;

    auto read_result = yaml::dom::read_document_from_file<char_type>("../test/learnyaml.yaml");
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::object);
    ASSERT_NO_THROW(node.as_object());
    auto& object_node = node.as_object();

    ASSERT_EQ(object_node.size(), size_t{ 24 });

    EXPECT_TRUE(object_node.contains("key"));
    EXPECT_TRUE(object_node.contains("another_key"));
    EXPECT_TRUE(object_node.contains("a_number_value"));
    EXPECT_TRUE(object_node.contains("scientific_notation"));
    EXPECT_TRUE(object_node.contains("hex_notation"));
    EXPECT_TRUE(object_node.contains("octal_notation"));

    EXPECT_TRUE(object_node.contains("boolean"));
    EXPECT_TRUE(object_node.contains("null_value"));
    EXPECT_TRUE(object_node.contains("another_null_value"));
    EXPECT_TRUE(object_node.contains("key with spaces"));

    EXPECT_TRUE(object_node.contains("no"));
    EXPECT_TRUE(object_node.contains("yes"));
    EXPECT_TRUE(object_node.contains("not_enclosed"));
    EXPECT_TRUE(object_node.contains("enclosed"));

    EXPECT_TRUE(object_node.contains("Superscript two"));

    EXPECT_TRUE(object_node.contains("literal_block"));
    EXPECT_TRUE(object_node.contains("folded_style"));
    EXPECT_TRUE(object_node.contains("literal_strip"));
    EXPECT_TRUE(object_node.contains("block_strip"));
    EXPECT_TRUE(object_node.contains("literal_keep"));
    EXPECT_TRUE(object_node.contains("block_keep"));

    EXPECT_TRUE(object_node.contains("a_nested_map"));

    EXPECT_TRUE(object_node.contains("0.25"));

    EXPECT_TRUE(object_node.contains("set2"));
}

TEST(dom_read, fail_unknown_file)
{
    using char_type = char;

    auto read_result = yaml::dom::read_document_from_file<char_type>("../test/this_file_does_not_exist.some_extension");
    EXPECT_EQ(read_result.result_code, yaml::read_result_code::cannot_open_file);
    EXPECT_FALSE(read_result);
}