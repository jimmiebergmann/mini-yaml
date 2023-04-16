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

#include "learnyaml_sax_utility.hpp"

#if defined(__cpp_char8_t)
using u8_string_type = std::u8string;
#else
using u8_string_type = std::string;
#endif

TEST(sax_read, fail_bad_indention_objects_1)
{
    const std::string input =
        "   key 1:\n"
        "  key 1_fail:\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_token);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 1");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);
    });
}

TEST(sax_read, fail_bad_indention_objects_2)
{
    const std::string input =
        "key 1:\n"
        "  key 1_1:\n"
        "  key 1_2:\n"
        "key 2:\n"
        " key 2_1:\n"
        "key 3:\n"
        "  key 3_1:\n"
        "      key 3_1_1:\n"
        "  key 3_2:\n"
        "key 4:\n"
        "key 5:\n"
        "   key 5_1:\n"
        "  key 5_bad:\n"
        "key 6:\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::bad_indentation);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 31 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1_1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1_2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object); // End key 1.

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);    
        EXPECT_EQ(handler.get_next_key(), "key 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 2_1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object); // End key 2.


        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 3");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 3_1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 3_1_1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object); // End key 3_1

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 3_2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object); // End key 3.

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 4");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 5");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 5_1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object); // End key 5_1.
    });
}

TEST(sax_read, fail_bad_indention_scalar_multiple_literal_1)
{
    const std::string input =
        "|\n"
        "   This is a scalar value\n"
        "  with invalid indention.";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_token);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 4 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::literal, yaml::chomping_type::clip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "This is a scalar value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);
    });
}

TEST(sax_read, fail_bad_indention_scalar_multiple_literal_2)
{
    const std::string input =
        "|\n"
        "   This is a scalar value.\n"
        "    This is another line,\n"
        "     and another one.\n"
        "    This is the last valid row.\n"
        "  This line fails\n"
        "   and this one is never parsed.\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_token);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 7 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::literal, yaml::chomping_type::clip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "This is a scalar value.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), " This is another line,");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "  and another one.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), " This is the last valid row.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);
    });
}

TEST(sax_read, fail_flow_scalar_double_quote_duplicate_1)
{
    const std::string input =
        "\"Hello world\" \"fail here\"";

    using char_type = typename decltype(input)::value_type;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document(input, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_token);

    handler.prepare_read();
    ASSERT_EQ(handler.instructions.size(), size_t{ 4 });

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::double_quoted, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);
}

TEST(sax_read, fail_flow_scalar_double_quote_duplicate_2)
{
    const std::string input =
        "key 1: \n"
        " \"Hello world\" \"fail here\"\n"
        "key 2: \"Foo bar\"";

    using char_type = typename decltype(input)::value_type;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document(input, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_token);

    handler.prepare_read();
    ASSERT_EQ(handler.instructions.size(), size_t{ 6 });

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::double_quoted, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Hello world");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);
}

TEST(sax_read, fail_flow_scalar_double_quote_duplicate_3)
{
    const std::string input =
        "key 1: \n"
        " \"Hello world\n"
         "test\" \"fail here\"\n"
        "key 2: \"Foo bar\"";

    using char_type = typename decltype(input)::value_type;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document(input, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_token);

    handler.prepare_read();
    ASSERT_EQ(handler.instructions.size(), size_t{ 7 });

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::double_quoted, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Hello world");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "test");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);
}

TEST(sax_read, fail_flow_scalar_single_quote_duplicate_1)
{
    const std::string input =
        "'Hello world' 'fail here'";

    using char_type = typename decltype(input)::value_type;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document(input, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_token);

    handler.prepare_read();
    ASSERT_EQ(handler.instructions.size(), size_t{ 4 });

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::single_quoted, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "Hello world");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);
}

TEST(sax_read, fail_flow_scalar_single_quote_duplicate_2)
{
    const std::string input =
        "key 1: \n"
        " 'Hello world' 'fail here'\n"
        "key 2: 'Foo bar'";

    using char_type = typename decltype(input)::value_type;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document(input, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_token);

    handler.prepare_read();
    ASSERT_EQ(handler.instructions.size(), size_t{ 6 });

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "key 1");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::single_quoted, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);
}

TEST(sax_read, fail_flow_scalar_single_quote_duplicate_3)
{
    const std::string input =
        "key 1: \n"
        " 'Hello world\n"
        "test' 'fail here'\n"
        "key 2: 'Foo bar'";

    using char_type = typename decltype(input)::value_type;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document(input, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_token);

    handler.prepare_read();
    ASSERT_EQ(handler.instructions.size(), size_t{ 7 });

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "key 1");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::single_quoted, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "test");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);
}

TEST(sax_read, fail_forbidden_tab_indentation_empty_file)
{
    const std::string input =
        "\t";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::forbidden_tab_indentation);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 1 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);
    });
}

TEST(sax_read, fail_forbidden_tab_indentation_scalar_1)
{
    const std::string input =
        "\tHello world \n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::forbidden_tab_indentation);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 1 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);
    });
}

TEST(sax_read, fail_forbidden_tab_indentation_scalar_2)
{
    const std::string input =
        "Hello world \n"
        "\tThis line is invalid, due to tab.";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::forbidden_tab_indentation);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
    });
}

TEST(sax_read, fail_object_expected_key_1)
{
    const std::string input =
        "key 1: value\n"
        "key 2 # comment\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::expected_key);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 6 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    });
}

TEST(sax_read, fail_object_expected_key_2)
{
    const std::string input =
        "key 1: value\n"
        "key 2";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::expected_key);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 6 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    });
}

TEST(sax_read, fail_object_expected_key_3)
{
    const std::string input =
        ": value";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::expected_key);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 1 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

    });
}

TEST(sax_read, fail_object_unexpected_key_1)
{
    const std::string input =
        "key 1: key 2:\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_key);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1");
    });
}

TEST(sax_read, fail_object_unexpected_key_2)
{
    const std::string input =
        "key 1: value\n"
        " unexpected:";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_key);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "value");
    });
}

TEST(sax_read, fail_reached_max_depth)
{
    const std::string input =
        "key:\n"
        "  key:\n"
        "    key:\n"
        "      key:\n"
        "        key:\n"
        "          key:\n"
        "            key:\n"
        "              key:\n"
        "                key: value\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto reader_options = yaml::sax::reader_options{};
        reader_options.max_depth = 9;

        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler, reader_options);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::reached_max_depth);
    });
}

TEST(sax_read, fail_scalar_single_literal_expected_line_break_1)
{
    const std::string input =
        "| a\n"
        " This is a scalar value";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::expected_line_break);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 1 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);
    });
}

TEST(sax_read, fail_scalar_single_literal_expected_line_break_2)
{
    const std::string input =
        "|a\n"
        " This is a scalar value";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::expected_line_break);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 1 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);
    });
}

TEST(sax_read, fail_scalar_single_literal_expected_line_break_3)
{
    const std::string input =
        "|-a\n"
        " This is a scalar value";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::expected_line_break);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 1 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);
    });
}

TEST(sax_read, fail_scalar_unexpected_token_at_end)
{
    const std::string input =
        "value\n"
        "# Comment\n"
        "unexpected value\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_token);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Comment");
    });
}

TEST(sax_read, fail_sequence_expected_sequence_1)
{
    const std::string input =
        "- value\n"
        "dummy\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::expected_sequence);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 6 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), size_t{ 0 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    });
}

TEST(sax_read, fail_sequence_expected_sequence_2)
{
    const std::string input =
        "- value\n"
        "-dummy\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::expected_sequence);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 6 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), size_t{ 0 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    });
}

TEST(sax_read, fail_tag_duplicate)
{
    const std::string input =
        "!!str\n"
        "!!str\n"
        "wow 1\n"
        "!!str\n"
        "wow 2\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::tag_duplication);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 2 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::tag);
        EXPECT_EQ(handler.get_next_tag(), "str");
    });
}

TEST(sax_read, fail_tag_nested_objects)
{
    const std::string input =
        "key_1: !!map key_2: value\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_key);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 4 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key_1");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::tag);
        EXPECT_EQ(handler.get_next_tag(), "map");
    });
}

TEST(sax_read, ok_comments__multiple)
{
    const std::string input =
        "#No indent, no space.\n"
        " #One indent, no space.\n"
        "  #Two indents, no space.\n"
        "   #Three indents, no space.\n"

        "# No indent, one space.\n"
        " # One indent, one space.\n"
        "  # Two indents, one space.\n"
        "   # Three indents, one space.\n"

        "#  No indent, two spaces.\n"
        " #  One indent, two spaces.\n"
        "  #  Two indents, two spaces.\n"
        "   #  Three indents, two spaces.\n"

        "#   No indent, three spaces.\n"
        " #   One indent, three spaces.\n"
        "  #   Two indents, three spaces.\n"
        "   #   Three indents, three spaces.\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 19 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "No indent, no space.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "One indent, no space.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Two indents, no space.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Three indents, no space.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "No indent, one space.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "One indent, one space.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Two indents, one space.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Three indents, one space.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "No indent, two spaces.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "One indent, two spaces.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Two indents, two spaces.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Three indents, two spaces.");
        
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "No indent, three spaces.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "One indent, three spaces.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Two indents, three spaces.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Three indents, three spaces.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_comments_objects_null_values)
{
    const std::string input =
        "key 1: # Hello first comment.\n"
        "# Comment between key 1 and 2.\n"
        "key 2: # Another comment\n"
        "key 3:\n"
        "# Comment between key 3 and key 3_1   \n"
        "   key 3_1:\n"
        "   key 3_2: # Comment at key 3_2.\n"
        "   key 3_3:\n"
        "   key 3_4:\n"
        " # comment between 3_4 and 3_5\n"
        "   key 3_5: # Multiple comments\n"
        "   # after\n"
        "#key 3_5\n"
        "key 4:\n"
        "# Double\n"
        " # comments between key 4 and 5.\n"
        "key 5:\n"
        "# Ending comment.\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 37 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1");        
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Hello first comment.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Comment between key 1 and 2.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 2");       
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Another comment");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 3");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Comment between key 3 and key 3_1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_1");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_2");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "Comment at key 3_2.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_3");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_4");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "comment between 3_4 and 3_5");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_5");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "Multiple comments");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "after");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "key 3_5");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object); // End key 3

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 4");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Double");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "comments between key 4 and 5.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 5");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Ending comment.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object); // End root

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_comments_objects_values)
{
    const std::string input =
        "key 1: value 1 # Hello first comment.\n"
        "# Comment between key 1 and 2.\n"
        "key 2: value 2 # Another comment\n"
        "key 3:\n"
        "# Comment between key 3 and key 3_1   \n"
        "   key 3_1: value 3\n"
        "   key 3_2: value 4 # Comment at key 3_2.\n"
        "   key 3_3: value 5\n"
        "   key 3_4: value 6\n"
        " # comment between 3_4 and 3_5\n"
        "   key 3_5: value 7 # Multiple comments\n"
        "   # after\n"
        "   #key 3_5\n"
        "key 4: value 8\n"
        "# Double\n"
        " # comments between key 4 and 5.\n"
        "key 5: value 9\n"
        "# Ending comment.\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 55 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1");        
        
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Hello first comment.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Comment between key 1 and 2.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 2");       
        
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Another comment");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 3");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Comment between key 3 and key 3_1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_1");
            
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 3");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_2");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 4");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "Comment at key 3_2.");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_3");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 5");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_4");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 6");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "comment between 3_4 and 3_5");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_5");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 7");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "Multiple comments");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "after");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "key 3_5");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object); // End key 3

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 4");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 8");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Double");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "comments between key 4 and 5.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 5");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 9");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Ending comment.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object); // End root

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_comments__single)
{
    const std::string input =
        "# Hello World";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 4 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Hello World");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_document_end)
{
    const std::string input =
        "value 1\n"
        "...\n"
        "value 2\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_document_end_at_start)
{
    const std::string input =
        "... # early end\n"
        "value 1\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_document_start)
{
    const std::string input = 
        "---\n"
        "value 1\n"
        "---\n"
        "value 2\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_document_start_after_comment)
{
    const std::string input =
        "# Comment here.\n"
        "---\n"
        "value 1\n"
        "---\n"
        "value 2\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 6 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Comment here.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_document_start_as_end)
{
    const std::string input =
        "value 1\n"
        "---\n"
        "value 2\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_document_start_end)
{
    const std::string input =
        "---\n"
        "value 1\n"
        "...\n"
        "value 2\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_empty_file)
{
    const std::string input = "";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_empty_file_document_start)
{
    const std::string input = "---";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_empty_file_document_end)
{
    const std::string input = "...";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_empty_file_empty_lines)
{
    const std::string input =
        "\n"
        "\r\n"
        "\r";

    using char_type = typename decltype(input)::value_type;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document(input, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
   
    handler.prepare_read();
    ASSERT_EQ(handler.instructions.size(), size_t{ 3 });

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
}

TEST(sax_read, ok_empty_file_empty_lines_with_spaces)
{
    const std::string input =
        " \n"
        "\n"
        "    \n"
        " \r"
        "\r"
        "    \r"
        " \r\n"
        "\r\n"
        "    \r\n";

    using char_type = typename decltype(input)::value_type;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document(input, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    
    handler.prepare_read();
    ASSERT_EQ(handler.instructions.size(), size_t{ 3 });

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
}

TEST(sax_read, ok_flow_scalar_double_quote__not_unexpected_eof)
{
    const std::string input =
        "\\\"";

    using char_type = typename decltype(input)::value_type;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document(input, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    handler.prepare_read();
    ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "\\\"");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
}

TEST(sax_read, ok_flow_scalar_double_quote__unexpected_eof)
{
    const std::string input =
        "\"";

    using char_type = typename decltype(input)::value_type;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document(input, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_eof);

    handler.prepare_read();
    ASSERT_EQ(handler.instructions.size(), size_t{ 2 });

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
}

TEST(sax_read, ok_flow_scalar_double_quote_single)
{
    const std::string input =
        "\"  Hello world  \"";

    using char_type = typename decltype(input)::value_type;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document(input, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    handler.prepare_read();
    ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::double_quoted, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "  Hello world  ");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
}

TEST(sax_read, ok_flow_scalar_double_quote_multiple)
{
    const std::string input =
        "\" This is my first line \t\n"
        "\n"
        "  second    line \\\"and\\\" worlds   \n"
        "third\n"
        "  fourth\"\n";

    using char_type = typename decltype(input)::value_type;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document(input, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    handler.prepare_read();
    ASSERT_EQ(handler.instructions.size(), size_t{ 9 });

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::double_quoted, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), " This is my first line \t");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "second    line \\\"and\\\" worlds");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "third");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "fourth");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
}

TEST(sax_read, ok_object_multiple_nested_objects)
{
    const std::string input =
        "key 1: first value\n"
        "key 2 : | \n"
        "    second value\n"
        "Key 3:\n"
        "  key 3_1: \n"
        "   key 3_1_1: \n"
        "    value on new line.\n"
        "  key 3_2: \n"
        "  key 3_3: >+  \n"
        "              Another value here...\n"
        "key 4: This is my very last value";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 32 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "first value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::literal, yaml::chomping_type::clip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "second value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "Key 3");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_1");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
                EXPECT_EQ(handler.get_next_key(), "key 3_1_1");
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
                    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                    EXPECT_EQ(handler.get_next_string(), "value on new line.");
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object); // End 3_1

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_2");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_3");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::folded, yaml::chomping_type::keep));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "Another value here...");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object); // End 3

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 4");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "This is my very last value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_object_single_nested_objects)
{
    const std::string input =
        " key 1:\n"
        "  key 2 : \n"
        "   key:3:\n"
        "    -key 4: Scalar value here  \n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 17 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 2");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key:3");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "-key 4");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Scalar value here");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_object_single_with_scalar)
{
    const std::string input =
        "key: hello world";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "hello world");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_object_single_with_scalar_tab_separated)
{
    const std::string input =
        "key:\thello world";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "hello world");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_object_single_with_scalar_with_with_comments)
{
    const std::string input =
        "key: \n"
        "   # Comment before.\n"
        "   # Another comment before.\n"
        "  hello world\n"
        "   # Comment after.\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 11 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Comment before.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Another comment before.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "hello world");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Comment after.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_reuse_reader)
{
    const std::string input =
        "--- # test comment 1\n"
        "key 1: value 1\n"
        "key 2: value 2\n"
        "--- # test comment 2\n"
        "key 3: value 3\n"
        "key 4: value 4\n"
        "key 5: value 5\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        auto reader_options = yaml::sax::reader_options{};
        auto reader = yaml::sax::reader<char_type, test_sax_handler<char_type>>{ handler, reader_options };

        const auto read_result_1 = reader.read_document(input);
        ASSERT_EQ(read_result_1.result_code, yaml::read_result_code::success);
        EXPECT_EQ(read_result_1.current_line, 3);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 13 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 1");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 1");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 1");
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 2");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 2");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);

        handler.reset();
        reader_options.start_line_number = read_result_1.current_line;
        const auto read_result_2 = reader.read_document(read_result_1.remaining_input);
        ASSERT_EQ(read_result_2.result_code, yaml::read_result_code::success);
        EXPECT_EQ(read_result_2.current_line, 7);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 17 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 2");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 3");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 4");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 4");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 5");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 5");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_scalar_multiple)
{
    const std::string input =
        "This is a scalar value, \n"
        "with multiple rows.\n"
        " Block style is none and chomping is set to strip. ";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 7 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "This is a scalar value,");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "with multiple rows.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Block style is none and chomping is set to strip.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_scalar_multiple__folded)
{
    const std::string input =
        ">\n"
        "\n"
        "    Row 1 here.\n"
        "    Row 2 here.\n"
        "    Row 3 here. After empty row.\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::folded, yaml::chomping_type::clip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 1 here.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 2 here.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 3 here. After empty row.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_scalar_multiple__folded_keep)
{
    const std::string input =
        ">+\n"
        "\n"
        "    Row 1 here.\n"
        "    Row 2 here.\n"
        "    Row 3 here. After empty row.\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::folded, yaml::chomping_type::keep));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 1 here.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 2 here.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 3 here. After empty row.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_scalar_multiple__folded_keep_end_with_comment)
{
    const std::string input =
        ">+\n"
        "    Row 1 here.\n"
        "    Row 2 here.\n"
        "    Row 3 here. After empty row.\n"
        "# goofy";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::folded, yaml::chomping_type::keep));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 1 here.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 2 here.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 3 here. After empty row.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "goofy");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);

    });
}

TEST(sax_read, ok_scalar_multiple__folded_strip)
{
    const std::string input =
        ">-\n"
        "\n"
        "    Row 1 here.\n"
        "    Row 2 here.\n"
        "    Row 3 here. After empty row.\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::folded, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 1 here.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 2 here.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 3 here. After empty row.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_scalar_multiple__literal)
{
    const std::string input =
        "|\n"
        "    Row 1 here.\n"
        "    Row 2 here.\n"
        "\n"
        "    Row 3 here. After empty row.\n"
        "    \n"
        "    Row 4 here. After empty row.\n"
        "     \n"
        "    Row 5 here. After 1 space row.\n"
        "   \n"
        "     Row 6 here with indention. After empty row. 3 leading spaces   \n"
        "    Finally my last row to test.\n"
        "\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 16 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::literal, yaml::chomping_type::clip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 1 here.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 2 here.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 3 here. After empty row.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 4 here. After empty row.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), " ");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 5 here. After 1 space row.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), " Row 6 here with indention. After empty row. 3 leading spaces   ");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Finally my last row to test.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_scalar_multiple__literal_keep)
{
    const std::string input =
        "|+ \n"
        "\n"
        "    Row 1 here.\n"
        "    Row 2 here.\n"
        "    Row 3 here. After empty row.\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::literal, yaml::chomping_type::keep));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 1 here.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Row 2 here.");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Row 3 here. After empty row.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_scalar_multiple__literal_strip)
{
    const std::string input =
        "|-\n"
        "\n"
        "    Row 1 here.\n"
        "    Row 2 here.\n"
        "    Row 3 here. After empty row.\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);

        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::literal, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Row 1 here.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Row 2 here.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Row 3 here. After empty row.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_scalar_multiple__with_comma)
{
    const std::string input =
        "This is a scalar value:with comma\n"
        "and another line:with comma";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 6 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "This is a scalar value:with comma");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "and another line:with comma");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_scalar_multiple__with_gaps)
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

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 13 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "first");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "second");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "third");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "fourth");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_scalar_single)
{
    const std::string input =
        "This is a scalar value";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "This is a scalar value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_scalar_single__literal)
{
    const std::string input =
        "|\n"
        " This is a scalar value";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::literal, yaml::chomping_type::clip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "This is a scalar value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_scalar_single__literal_comment_after_token)
{
    const std::string input =
        "| #comment \n"
        "    Value\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 6 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "comment");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::literal, yaml::chomping_type::clip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_sequence)
{
    const std::string input =
        "- value 1 \n"
        "- value 2 \n"
        "- value 3 \n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 16 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 0);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 1);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 2);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "value 3");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_array);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_sequence_newline_gap)
{
    const std::string input =
        "- value 1\n"
        "\n"
        "- value 2\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 13 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), size_t{ 0 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 1");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), size_t{ 1 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_array);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);

    });
}

TEST(sax_read, ok_sequence_newline)
{
    const std::string input =
        "-\n"
        " value 1\n"
        "-\n"
        " value 2\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 12 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), size_t{ 0 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "value 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), size_t{ 1 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "value 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_array);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);

    });
}

TEST(sax_read, ok_sequence_nested)
{
    const std::string input =
        "- - - value 1 \n"
        "  - value 2 \n"
        "- value 3 \n"
        "- - \n"
        "  - value 5 \n"
        "  -    - value 6 \n"
        "       - value 7_1 \n"
        "         value 7_2 \n"
        "  - value 8 \n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 47 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        // Line 1 - 1
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 0);

        // Line 1 - 2
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 0);

        // Line 1 - 3
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 0);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        // End line 1 - 3
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_array);

        // Line 2 - 2
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 1);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        // End line 1 - 2
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_array);

        // Line 3 - 1
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 1);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 3");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        // Line 4 - 1
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 2);

        // Line 4 - 2  
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 0);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);
        
        // Line 5 - 2
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 1);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 5");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        // Line 6 - 2
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 2);

        // Line 6 - 3
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 0);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 6");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        // Line 7 - 3
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 1);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 7_1");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 7_2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        // End line 6 - 3
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_array);

        // Line 8 - 2
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 3);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 8");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        // End line 4 - 2
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_array);

        // End line 1 - 1
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_array);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_sequence_object_value)
{
    const std::string input =
        "- key: value\n"
        "  another_key: another_value\n";

    using char_type = typename decltype(input)::value_type;
 
    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 15 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
        EXPECT_EQ(handler.get_next_index(), 0);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "another_key");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "another_value");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_array);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);

    });
}

TEST(sax_read, ok_tag_1_token_scalar)
{
    const std::string input =
        "!str\n"
        "wow 1\n"
        "!!str\n"
        "wow 2\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::tag);
        EXPECT_EQ(handler.get_next_tag(), "str");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "wow 1");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "!!str");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "wow 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_tag_2_token_scalar)
{
    const std::string input =
        "!!str\n"
        "wow 1\n"
        "!!str\n"
        "wow 2\n";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::tag);
        EXPECT_EQ(handler.get_next_tag(), "str");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "wow 1");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "!!str");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "wow 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_tag_object)
{
    const std::string input =
        "key_1: !!map\n"
        "  key_1_1:\n"
        "    !map\n"
        "  key_1_2:\n" 
        "    !!seq\n"
        "    - !!str value";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 20 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key_1");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::tag);
            EXPECT_EQ(handler.get_next_tag(), "map");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
                EXPECT_EQ(handler.get_next_key(), "key_1_1");

                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::tag);
                EXPECT_EQ(handler.get_next_tag(), "map");

                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
                EXPECT_EQ(handler.get_next_key(), "key_1_2");

                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::tag);
                EXPECT_EQ(handler.get_next_tag(), "seq");

                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);

                    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
                    EXPECT_EQ(handler.get_next_index(), size_t{ 0 });

                    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::tag);
                    EXPECT_EQ(handler.get_next_tag(), "str");

                    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
                        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                        EXPECT_EQ(handler.get_next_string(), "value");
                    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_array);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_u8_BOM_1)
{    
    u8_string_type input = 
        u8"..."
        u8"key 1: 歴戦経る素早い黒小鬼、怠けドワアフ達をひらり。裃の鵺、棟誉めて夜露誘う。\n"
        u8"key 2: test";

    using char_type = typename decltype(input)::value_type;

    const std::array<uint8_t, 3> bom_chars = { 0xEF, 0xBB, 0xBF };
    input[0] = static_cast<char_type>(bom_chars[0]);
    input[1] = static_cast<char_type>(bom_chars[1]);
    input[2] = static_cast<char_type>(bom_chars[2]);

    run_read_all_styles<char_type>(input, [](u8_string_type input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 12 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        ASSERT_EQ(handler.get_next_key(), u8"key 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            const u8_string_type utf8_value = u8"歴戦経る素早い黒小鬼、怠けドワアフ達をひらり。裃の鵺、棟誉めて夜露誘う。";
            EXPECT_EQ(handler.get_next_string(), utf8_value);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        ASSERT_EQ(handler.get_next_key(), u8"key 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), u8"test");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
   });
}

TEST(sax_read, ok_u8_BOM_2)
{
    u8_string_type input = 
        u8"..."
        u8"key 1: Швидка бура лисиця перестрибує через ледачого\n"
        u8"key 2: test";

    using char_type = typename decltype(input)::value_type;

    const std::array<uint8_t, 3> bom_chars = { 0xEF, 0xBB, 0xBF };
    input[0] = static_cast<char_type>(bom_chars[0]);
    input[1] = static_cast<char_type>(bom_chars[1]);
    input[2] = static_cast<char_type>(bom_chars[2]);

    run_read_all_styles<char_type>(input, [](u8_string_type input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 12 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        ASSERT_EQ(handler.get_next_key(), u8"key 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            const u8_string_type utf8_value = u8"Швидка бура лисиця перестрибує через ледачого";
            EXPECT_EQ(handler.get_next_string(), utf8_value);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        ASSERT_EQ(handler.get_next_key(), u8"key 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), u8"test");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

