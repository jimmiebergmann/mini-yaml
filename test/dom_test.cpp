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
    EXPECT_ANY_THROW(node.as_object());
    EXPECT_ANY_THROW(node.as_array());

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
    EXPECT_STREQ(scalar_node.as_string().c_str(), "First line of scalar. Second line of scalar.");
}

TEST(dom_create_node, ok_object)
{
    using char_type = char;

    auto node = yaml::dom::node<char_type>::create_object();

    ASSERT_EQ(node.type(), yaml::dom::node_type::object);
    ASSERT_NO_THROW(node.as_object());
    EXPECT_ANY_THROW(node.as_scalar());
    EXPECT_ANY_THROW(node.as_array());

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

    EXPECT_STREQ(scalar_node.as_string().c_str(), "This is a scalar with multiple lines.");
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

    ASSERT_EQ(root_object_node.size(), size_t{ 25 });

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
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "null");
    }
    {
        auto it = root_object_node.find("another_null_value");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as_string();
        EXPECT_STREQ(string.c_str(), "~");
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
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);
            ASSERT_NO_THROW(node2.as_scalar());
            auto& scalar_node = node2.as_scalar();

            auto string = scalar_node.as_string();
            EXPECT_STREQ(string.c_str(), "null");
        }
        {
            auto it2 = object_node.find("item2");
            ASSERT_NE(it2, object_node.end());

            EXPECT_STREQ(it2->first.c_str(), "item2");

            auto& node2 = *it2->second;
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);
            ASSERT_NO_THROW(node2.as_scalar());
            auto& scalar_node = node2.as_scalar();

            auto string = scalar_node.as_string();
            EXPECT_STREQ(string.c_str(), "null");
        }
        {
            auto it2 = object_node.find("item3");
            ASSERT_NE(it2, object_node.end());

            EXPECT_STREQ(it2->first.c_str(), "item3");

            auto& node2 = *it2->second;
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);
            ASSERT_NO_THROW(node2.as_scalar());
            auto& scalar_node = node2.as_scalar();

            auto string = scalar_node.as_string();
            EXPECT_STREQ(string.c_str(), "null");
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