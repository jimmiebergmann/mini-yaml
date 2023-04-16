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

#ifndef MINIYAML_TEST_DOM_LEARNYAML_UTILITY_HEADER_FILE
#define MINIYAML_TEST_DOM_LEARNYAML_UTILITY_HEADER_FILE

#include "test_utility.hpp"

template<typename Tnode>
void check_dom_learnyaml_tests(Tnode& root_node) {
    ASSERT_EQ(root_node.type(), yaml::dom::node_type::object);
    ASSERT_NO_THROW_IGNORE_NODISCARD(root_node.as_object());
    auto& root_object_node = root_node.as_object();

    ASSERT_EQ(root_object_node.size(), size_t{ 36 });

    {
        auto it = root_object_node.find("key");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "value");
    }
    {
        auto it = root_object_node.find("another_key");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "Another value goes here.");
    }
    {
        auto it = root_object_node.find("a_number_value");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "100");
    }
    {
        auto it = root_object_node.find("scientific_notation");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "1e+12");
    }
    {
        auto it = root_object_node.find("hex_notation");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "0x123");
    }
    {
        auto it = root_object_node.find("octal_notation");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "0123");
    }

    {
        auto it = root_object_node.find("boolean");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
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
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
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
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "no");
    }
    {
        auto it = root_object_node.find("yes");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "No");
    }
    {
        auto it = root_object_node.find("not_enclosed");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "yes");
    }
    {
        auto it = root_object_node.find("enclosed");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "yes");
    }

    {
        auto it = root_object_node.find("however");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "A string, enclosed in quotes.");
    }
    {
        auto it = root_object_node.find("single quotes");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "have 'one' escape pattern");
    }
    {
        auto it = root_object_node.find("double quotes");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "have many: \", \0, \t, \\u263A, \\x0d\\x0a == \r\n, and more.");
    }

    {
        auto it = root_object_node.find("Superscript two");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "\\u00B2");
    }

    {
        auto it = root_object_node.find("special_characters");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "[ John ] & { Jane } - <Doe>");
    }

    {
        auto it = root_object_node.find("literal_block");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_EQ(scalar_node.style(), yaml::scalar_style_type::literal);
        EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::clip);

        auto string = scalar_node.as<std::string>();

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
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_EQ(scalar_node.style(), yaml::scalar_style_type::folded);
        EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::clip);

        auto string = scalar_node.as<std::string>();

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
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_EQ(scalar_node.style(), yaml::scalar_style_type::literal);
        EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::strip);

        auto string = scalar_node.as<std::string>();

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
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_EQ(scalar_node.style(), yaml::scalar_style_type::folded);
        EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::strip);

        auto string = scalar_node.as<std::string>();

        static const std::string compare_str =
            "This entire block of text will be the value of 'folded_style', but this time, all newlines will be replaced with a single space and  trailing blank line being stripped.";

        EXPECT_STREQ(string.c_str(), compare_str.c_str());
    }
    {
        auto it = root_object_node.find("literal_keep");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_EQ(scalar_node.style(), yaml::scalar_style_type::literal);
        EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::keep);

        auto string = scalar_node.as<std::string>();

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
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_EQ(scalar_node.style(), yaml::scalar_style_type::folded);
        EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::keep);

        auto string = scalar_node.as<std::string>();

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
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_object());
        auto& object_node = node.as_object();

        EXPECT_EQ(object_node.size(), size_t{ 3 });
        {
            auto it2 = object_node.find("key");
            ASSERT_NE(it2, object_node.end());

            EXPECT_STREQ(it2->first.c_str(), "key");

            auto& node2 = *it2->second;
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);
            ASSERT_NO_THROW_IGNORE_NODISCARD(node2.as_scalar());
            auto& scalar_node = node2.as_scalar();

            auto string = scalar_node.as<std::string>();
            EXPECT_STREQ(string.c_str(), "value");
        }
        {
            auto it2 = object_node.find("another_key");
            ASSERT_NE(it2, object_node.end());

            EXPECT_STREQ(it2->first.c_str(), "another_key");

            auto& node2 = *it2->second;
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);
            ASSERT_NO_THROW_IGNORE_NODISCARD(node2.as_scalar());
            auto& scalar_node = node2.as_scalar();

            auto string = scalar_node.as<std::string>();
            EXPECT_STREQ(string.c_str(), "Another Value");
        }
        {
            auto it2 = object_node.find("another_nested_map");
            ASSERT_NE(it2, object_node.end());

            EXPECT_STREQ(it2->first.c_str(), "another_nested_map");

            auto& node2 = *it2->second;
            ASSERT_EQ(node2.type(), yaml::dom::node_type::object);
            ASSERT_NO_THROW_IGNORE_NODISCARD(node2.as_object());
            auto& object_node2 = node2.as_object();

            EXPECT_EQ(object_node2.size(), size_t{ 1 });
            {
                auto it3 = object_node2.find("hello");
                ASSERT_NE(it3, object_node2.end());

                EXPECT_STREQ(it3->first.c_str(), "hello");

                auto& node3 = *it3->second;
                ASSERT_EQ(node3.type(), yaml::dom::node_type::scalar);
                ASSERT_NO_THROW_IGNORE_NODISCARD(node3.as_scalar());
                auto& scalar_node2 = node3.as_scalar();

                auto string2 = scalar_node2.as<std::string>();
                EXPECT_STREQ(string2.c_str(), "hello");
            }
        }

    }

    {
        auto it = root_object_node.find("0.25");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "a float key");
    }

    {
        auto it = root_object_node.find("a_sequence");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::array);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_array());
        auto& array_node = node.as_array();

        ASSERT_EQ(array_node.size(), size_t{ 7 });
        {
            auto& node2 = array_node.at(0);
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);

            ASSERT_NO_THROW_IGNORE_NODISCARD(node2.as_scalar());
            auto& scalar_node2 = node2.as_scalar();

            auto string = scalar_node2.as<std::string>();
            EXPECT_STREQ(string.c_str(), "Item 1");
        }
        {
            auto& node2 = array_node.at(1);
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);

            ASSERT_NO_THROW_IGNORE_NODISCARD(node2.as_scalar());
            auto& scalar_node2 = node2.as_scalar();

            auto string = scalar_node2.as<std::string>();
            EXPECT_STREQ(string.c_str(), "Item 2");
        }
        {
            auto& node2 = array_node.at(2);
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);

            ASSERT_NO_THROW_IGNORE_NODISCARD(node2.as_scalar());
            auto& scalar_node2 = node2.as_scalar();

            auto string = scalar_node2.as<std::string>();
            EXPECT_STREQ(string.c_str(), "0.5");
        }
        {
            auto& node2 = array_node.at(3);
            ASSERT_EQ(node2.type(), yaml::dom::node_type::scalar);

            ASSERT_NO_THROW_IGNORE_NODISCARD(node2.as_scalar());
            auto& scalar_node2 = node2.as_scalar();

            auto string = scalar_node2.as<std::string>();
            EXPECT_STREQ(string.c_str(), "Item 4");
        }
        {
            auto& node2 = array_node.at(4);
            ASSERT_EQ(node2.type(), yaml::dom::node_type::object);

            ASSERT_NO_THROW_IGNORE_NODISCARD(node2.as_object());
            auto& object_node2 = node2.as_object();

            ASSERT_EQ(object_node2.size(), size_t{ 2 });
            {
                auto& node3 = object_node2.at("key");
                ASSERT_EQ(node3.type(), yaml::dom::node_type::scalar);

                ASSERT_NO_THROW_IGNORE_NODISCARD(node3.as_scalar());
                auto& scalar_node3 = node3.as_scalar();

                auto string = scalar_node3.as<std::string>();
                EXPECT_STREQ(string.c_str(), "value");
            }
            {
                auto& node3 = object_node2.at("another_key");
                ASSERT_EQ(node3.type(), yaml::dom::node_type::scalar);

                ASSERT_NO_THROW_IGNORE_NODISCARD(node3.as_scalar());
                auto& scalar_node3 = node3.as_scalar();

                auto string = scalar_node3.as<std::string>();
                EXPECT_STREQ(string.c_str(), "another_value");
            }
        }
        {
            auto& node2 = array_node.at(5);
            ASSERT_EQ(node2.type(), yaml::dom::node_type::array);

            ASSERT_NO_THROW_IGNORE_NODISCARD(node2.as_array());
            auto& array_node2 = node2.as_array();

            ASSERT_EQ(array_node2.size(), size_t{ 2 });
            {
                auto& node3 = array_node2.at(0);
                ASSERT_EQ(node3.type(), yaml::dom::node_type::scalar);

                ASSERT_NO_THROW_IGNORE_NODISCARD(node3.as_scalar());
                auto& scalar_node3 = node3.as_scalar();

                auto string = scalar_node3.as<std::string>();
                EXPECT_STREQ(string.c_str(), "This is a sequence");
            }
            {
                auto& node3 = array_node2.at(1);
                ASSERT_EQ(node3.type(), yaml::dom::node_type::scalar);

                ASSERT_NO_THROW_IGNORE_NODISCARD(node3.as_scalar());
                auto& scalar_node3 = node3.as_scalar();

                auto string = scalar_node3.as<std::string>();
                EXPECT_STREQ(string.c_str(), "inside another sequence");
            }
        }
        {
            auto& node2 = array_node.at(6);
            ASSERT_EQ(node2.type(), yaml::dom::node_type::array);

            ASSERT_NO_THROW_IGNORE_NODISCARD(node2.as_array());
            auto& array_node2 = node2.as_array();

            ASSERT_EQ(array_node2.size(), size_t{ 1 });
            {
                auto& node3 = array_node2.at(0);
                ASSERT_EQ(node3.type(), yaml::dom::node_type::array);

                ASSERT_NO_THROW_IGNORE_NODISCARD(node3.as_array());
                auto& array_node3 = node3.as_array();

                ASSERT_EQ(array_node3.size(), size_t{ 2 });
                {
                    auto& node4 = array_node3.at(0);
                    ASSERT_EQ(node4.type(), yaml::dom::node_type::scalar);

                    ASSERT_NO_THROW_IGNORE_NODISCARD(node4.as_scalar());
                    auto& scalar_node4 = node4.as_scalar();

                    auto string = scalar_node4.as<std::string>();
                    EXPECT_STREQ(string.c_str(), "Nested sequence indicators");
                }
                {
                    auto& node4 = array_node3.at(1);
                    ASSERT_EQ(node4.type(), yaml::dom::node_type::scalar);

                    ASSERT_NO_THROW_IGNORE_NODISCARD(node4.as_scalar());
                    auto& scalar_node4 = node4.as_scalar();

                    auto string = scalar_node4.as<std::string>();
                    EXPECT_STREQ(string.c_str(), "can be collapsed");
                }
            }
        }

    }

    {
        auto it = root_object_node.find("explicit_boolean");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_STREQ(node.tag().c_str(), "bool");

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "true");
    }
    {
        auto it = root_object_node.find("explicit_integer");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_STREQ(node.tag().c_str(), "int");

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "42");
    }
    {
        auto it = root_object_node.find("explicit_float");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_STREQ(node.tag().c_str(), "float");

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "-42.24");
    }
    {
        auto it = root_object_node.find("explicit_string");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_STREQ(node.tag().c_str(), "str");

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "0.5");
    }
    {
        auto it = root_object_node.find("explicit_datetime");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
        auto& scalar_node = node.as_scalar();

        EXPECT_STREQ(node.tag().c_str(), "timestamp");

        auto string = scalar_node.as<std::string>();
        EXPECT_STREQ(string.c_str(), "2022-11-17 12:34:56.78 +9");
    }
    {
        auto it = root_object_node.find("explicit_null");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::null);

        EXPECT_STREQ(node.tag().c_str(), "null");
    }

    {
        auto it = root_object_node.find("set2");
        ASSERT_NE(it, root_object_node.end());

        auto& node = *it->second;
        ASSERT_EQ(node.type(), yaml::dom::node_type::object);
        ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_object());
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

#endif