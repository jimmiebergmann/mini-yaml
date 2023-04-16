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

TEST(sax_read_document, fail_reached_max_document_count)
{
    const std::string input =
        "--- # test comment\n"
        "Not reached";

    using char_type = typename decltype(input)::value_type;

    run_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        auto reader_options = yaml::sax::reader_options{};
        reader_options.max_document_count = 0;
        const auto read_result = yaml::sax::read_document(input, handler, reader_options);

        ASSERT_EQ(read_result.result_code, yaml::read_result_code::reached_max_document_count);
        EXPECT_EQ(read_result.current_line, 0);
    });
}

TEST(sax_read_document, fail_unknown_file)
{
    using char_type = char;

    auto handler = test_sax_handler<char_type>{};
    auto read_result = yaml::sax::read_document_from_file<char_type>("../test/this_file_does_not_exist.some_extension", handler);
    EXPECT_EQ(read_result.result_code, yaml::read_result_code::cannot_open_file);
    EXPECT_FALSE(read_result);
}

TEST(sax_read_document, ok_learnyaml_read_document_from_file)
{
    using char_type = char;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document_from_file<char_type>("../examples/learnyaml.yaml", handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_TRUE(read_result);

    handler.prepare_read();
    check_sax_learnyaml_tests(handler);
}

TEST(sax_read_document, ok_learnyaml_read_document_raw_input)
{
    using char_type = char;

    auto file_result = yaml::impl::read_file<char>("../examples/learnyaml.yaml");
    ASSERT_EQ(file_result.result_code, yaml::impl::read_file_result_code::success);

    auto* data_ptr = file_result.data.data();
    auto data_size = file_result.data.size();

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document<char_type>(data_ptr, data_size, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_TRUE(read_result);

    handler.prepare_read();
    check_sax_learnyaml_tests(handler);
}

TEST(sax_read_document, ok_learnyaml_read_document_string)
{
    using char_type = char;

    auto file_result = yaml::impl::read_file<char>("../examples/learnyaml.yaml");
    ASSERT_EQ(file_result.result_code, yaml::impl::read_file_result_code::success);

    auto string = std::basic_string<char_type>{ file_result.data.begin(), file_result.data.end() };

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document<char_type>(string, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_TRUE(read_result);

    handler.prepare_read();
    check_sax_learnyaml_tests(handler);
}

TEST(sax_read_document, ok_learnyaml_read_document_string_view)
{
    using char_type = char;

    auto file_result = yaml::impl::read_file<char>("../examples/learnyaml.yaml");
    ASSERT_EQ(file_result.result_code, yaml::impl::read_file_result_code::success);

    auto string_view = yaml::basic_string_view<char_type>{ file_result.data.data(), file_result.data.size() };

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document<char_type>(string_view, handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_TRUE(read_result);

    handler.prepare_read();
    check_sax_learnyaml_tests(handler);
}

TEST(sax_read_document, ok_multiple_documents)
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
        auto reader_options = yaml::sax::reader_options{};
        reader_options.max_document_count = 2;
        
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler, reader_options);

        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        EXPECT_EQ(read_result.current_line, 3);

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
    });
}