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

TEST(dom_read_document, fail_unknown_file)
{
    using char_type = char;

    auto read_result = yaml::dom::read_document_from_file<char_type>("../test/this_file_does_not_exist.some_extension");
    EXPECT_EQ(read_result.result_code, yaml::read_result_code::cannot_open_file);
    EXPECT_FALSE(read_result);
    EXPECT_EQ(read_result.root_node.type(), yaml::dom::node_type::null);
}

TEST(dom_read_document, ok_learnyaml_read_document_from_file)
{
    using char_type = char;

    const auto read_result = yaml::dom::read_document_from_file<char_type>("../examples/learnyaml.yaml");
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_TRUE(read_result);

    check_dom_learnyaml_tests(read_result.root_node);
}

TEST(dom_read_document, ok_learnyaml_read_document_raw_input)
{
    using char_type = char;

    auto file_result = yaml::impl::read_file<char>("../examples/learnyaml.yaml");
    ASSERT_EQ(file_result.result_code, yaml::impl::read_file_result_code::success);

    auto* data_ptr = file_result.data.data();
    auto data_size = file_result.data.size();

    const auto read_result = yaml::dom::read_document<char_type>(data_ptr, data_size);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_TRUE(read_result);

    check_dom_learnyaml_tests(read_result.root_node);
}

TEST(dom_read_document, ok_learnyaml_read_document_string)
{
    using char_type = char;

    auto file_result = yaml::impl::read_file<char>("../examples/learnyaml.yaml");
    ASSERT_EQ(file_result.result_code, yaml::impl::read_file_result_code::success);

    auto string = std::basic_string<char_type>{ file_result.data.begin(), file_result.data.end() };

    const auto read_result = yaml::dom::read_document<char_type>(string);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_TRUE(read_result);

    check_dom_learnyaml_tests(read_result.root_node);
}

TEST(dom_read_document, ok_learnyaml_read_document_string_view)
{
    using char_type = char;

    auto file_result = yaml::impl::read_file<char>("../examples/learnyaml.yaml");
    ASSERT_EQ(file_result.result_code, yaml::impl::read_file_result_code::success);

    auto string_view = yaml::basic_string_view<char_type>{ file_result.data.data(), file_result.data.size() };

    const auto read_result = yaml::dom::read_document<char_type>(string_view);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_TRUE(read_result);

    check_dom_learnyaml_tests(read_result.root_node);
}

TEST(dom_read_document, ok_multiple_documents)
{
    const std::string input =
        "--- # test comment 1\n"
        "key 1: value 1\n"
        "key 2: value 2\n"
        "--- # test comment 2\n"
        "not reached: value 3\n"
        "key 4: value 4\n"
        "key 5: value 5\n"
        "...\n"
        "Not reached";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto reader_options = yaml::dom::reader_options{};
        reader_options.max_document_count = 2;
        
        const auto read_result = yaml::dom::read_document(input, reader_options);

        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        EXPECT_TRUE(read_result);
        EXPECT_EQ(read_result.current_line, 3);

        auto& root_node = read_result.root_node;
        ASSERT_NO_THROW_IGNORE_NODISCARD(root_node.as_object());
        auto& root_object_node = root_node.as_object();

        ASSERT_EQ(root_object_node.size(), size_t{ 2 });

        {
            auto it = root_object_node.find("key 1");
            ASSERT_NE(it, root_object_node.end());

            auto& node = *it->second;
            ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
            ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
            auto& scalar_node = node.as_scalar();

            auto string = scalar_node.as<std::string>();
            EXPECT_STREQ(string.c_str(), "value 1");
        }
        {
            auto it = root_object_node.find("key 2");
            ASSERT_NE(it, root_object_node.end());

            auto& node = *it->second;
            ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
            ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
            auto& scalar_node = node.as_scalar();

            auto string = scalar_node.as<std::string>();
            EXPECT_STREQ(string.c_str(), "value 2");
        }
      
    });
}

TEST(dom_read_document, ok_quickstart)
{
    const std::string input =
        "scalar: hello world\n"
        "list:\n"
        " - \"foo bar\"\n"
        " - boolean: true\n"
        "   integer: 123\n"
        "   floating point: 2.75";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    auto root = std::move(read_result.root_node);
    ASSERT_EQ(root.type(), yaml::dom::node_type::object);

    {
        const auto str1 = root.as_object().at("scalar").as_scalar().as<std::string>();
        const auto str2 = root.as_object().at("list").as_array().at(0).as_scalar().as<std::string>();
        const auto bool1 = root.as_object().at("list").as_array().at(1).as_object().at("boolean").as_scalar().as<bool>();
        const auto int1 = root.as_object().at("list").as_array().at(1).as_object().at("integer").as_scalar().as<int>();
        const auto float1 = root.as_object().at("list").as_array().at(1).as_object().at("floating point").as_scalar().as<float>();

        EXPECT_STREQ(str1.c_str(), "hello world");
        EXPECT_STREQ(str2.c_str(), "foo bar");
        EXPECT_TRUE(bool1);
        EXPECT_EQ(int1, int{ 123 });
        EXPECT_FLOAT_EQ(float1, float{ 2.75f });
    }
    {
        const auto str1 = root["scalar"].as<std::string>();
        const auto str2 = root["list"][0].as<std::string>();
        const auto bool1 = root["list"][1]["boolean"].as<bool>();
        const auto int1 = root["list"][1]["integer"].as<int>();
        const auto float1 = root["list"][1]["floating point"].as<float>();

        EXPECT_STREQ(str1.c_str(), "hello world");
        EXPECT_STREQ(str2.c_str(), "foo bar");
        EXPECT_TRUE(bool1);
        EXPECT_EQ(int1, int{ 123 });
        EXPECT_FLOAT_EQ(float1, float{ 2.75f });
    }
}