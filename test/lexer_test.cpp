/*
* MIT License
*
* Copyright(c) 2025 Jimmie Bergmann
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

#if defined(__cpp_char8_t)
using u8_string_type = std::u8string;
#else
using u8_string_type = std::string;
#endif

namespace lexer_tests
{
    TEST(string_literals, document_start)
    {
        const std::string input = "---";

        using char_type = typename decltype(input)::value_type;

        run_read_all_styles<char_type>(input, [](std::string input) {
            const auto lexer_result = yaml::lexer::tokenize(input);

            ASSERT_EQ(lexer_result.result_code, yaml::result_code::success);

            ASSERT_EQ(lexer_result.tokens.size(), 2);

            ASSERT_EQ(lexer_result.tokens[0].type, yaml::lexer::token_type::string_literal);
            ASSERT_EQ(lexer_result.tokens[0].value, yaml::basic_string_view< char_type>{"---"});

            ASSERT_EQ(lexer_result.tokens[1].type, yaml::lexer::token_type::eof);
        });
    }

    TEST(double_quoted_string_literals, double_quoted_string_literals)
    {
        const std::string input = " \"test# \\\"1\\\\\"  \"test 2\"   ";

        using char_type = typename decltype(input)::value_type;

        run_read_all_styles<char_type>(input, [](std::string input) {
            const auto lexer_result = yaml::lexer::tokenize(input);

            ASSERT_EQ(lexer_result.result_code, yaml::result_code::success);

            ASSERT_EQ(lexer_result.tokens.size(), 6);

            ASSERT_EQ(lexer_result.tokens[0].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[0].value, yaml::basic_string_view< char_type>{" "});

            ASSERT_EQ(lexer_result.tokens[1].type, yaml::lexer::token_type::string_literal);
            ASSERT_EQ(lexer_result.tokens[1].value, yaml::basic_string_view< char_type>{"\"test# \\\"1\\\\\""});

            ASSERT_EQ(lexer_result.tokens[2].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[2].value, yaml::basic_string_view< char_type>{"  "});

            ASSERT_EQ(lexer_result.tokens[3].type, yaml::lexer::token_type::string_literal);
            ASSERT_EQ(lexer_result.tokens[3].value, yaml::basic_string_view< char_type>{"\"test 2\""});

            ASSERT_EQ(lexer_result.tokens[4].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[4].value, yaml::basic_string_view< char_type>{"   "});

            ASSERT_EQ(lexer_result.tokens[5].type, yaml::lexer::token_type::eof);
        });
    }

    TEST(single_quoted_string_literals, single_quoted_string_literals)
    {
        const std::string input = " 'test# \\'1\\\\\'  'test 2'   ";

        using char_type = typename decltype(input)::value_type;

        run_read_all_styles<char_type>(input, [](std::string input) {
            const auto lexer_result = yaml::lexer::tokenize(input);

            ASSERT_EQ(lexer_result.result_code, yaml::result_code::success);

            ASSERT_EQ(lexer_result.tokens.size(), 6);

            ASSERT_EQ(lexer_result.tokens[0].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[0].value, yaml::basic_string_view< char_type>{" "});

            ASSERT_EQ(lexer_result.tokens[1].type, yaml::lexer::token_type::string_literal);
            ASSERT_EQ(lexer_result.tokens[1].value, yaml::basic_string_view< char_type>{"'test# \\'1\\\\'"});

            ASSERT_EQ(lexer_result.tokens[2].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[2].value, yaml::basic_string_view< char_type>{"  "});

            ASSERT_EQ(lexer_result.tokens[3].type, yaml::lexer::token_type::string_literal);
            ASSERT_EQ(lexer_result.tokens[3].value, yaml::basic_string_view< char_type>{"'test 2'"});

            ASSERT_EQ(lexer_result.tokens[4].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[4].value, yaml::basic_string_view< char_type>{"   "});

            ASSERT_EQ(lexer_result.tokens[5].type, yaml::lexer::token_type::eof);
        });
    }

    TEST(object, object)
    {
        const std::string input = " :::  ::::test    ";

        using char_type = typename decltype(input)::value_type;

        run_read_all_styles<char_type>(input, [](std::string input) {
            const auto lexer_result = yaml::lexer::tokenize(input);

            ASSERT_EQ(lexer_result.result_code, yaml::result_code::success);

            ASSERT_EQ(lexer_result.tokens.size(), 6);

            ASSERT_EQ(lexer_result.tokens[0].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[0].value, yaml::basic_string_view< char_type>{" "});

            ASSERT_EQ(lexer_result.tokens[1].type, yaml::lexer::token_type::string_literal);
            ASSERT_EQ(lexer_result.tokens[1].value, yaml::basic_string_view< char_type>{"::"});

            ASSERT_EQ(lexer_result.tokens[2].type, yaml::lexer::token_type::object);
            ASSERT_EQ(lexer_result.tokens[2].value, yaml::basic_string_view< char_type>{":"});

            ASSERT_EQ(lexer_result.tokens[3].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[3].value, yaml::basic_string_view< char_type>{"  "});

            ASSERT_EQ(lexer_result.tokens[4].type, yaml::lexer::token_type::string_literal);
            ASSERT_EQ(lexer_result.tokens[4].value, yaml::basic_string_view< char_type>{"::::test"});

            ASSERT_EQ(lexer_result.tokens[5].type, yaml::lexer::token_type::eof);
        });
    }

    TEST(sequence, nested)
    {
        const std::string input = "- -  -";

        using char_type = typename decltype(input)::value_type;

        run_read_all_styles<char_type>(input, [](std::string input) {
            const auto lexer_result = yaml::lexer::tokenize(input);

            ASSERT_EQ(lexer_result.result_code, yaml::result_code::success);

            ASSERT_EQ(lexer_result.tokens.size(), 6);

            ASSERT_EQ(lexer_result.tokens[0].type, yaml::lexer::token_type::sequence);
            ASSERT_EQ(lexer_result.tokens[0].value, yaml::basic_string_view< char_type>{"-"});

            ASSERT_EQ(lexer_result.tokens[1].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[1].value, yaml::basic_string_view< char_type>{" "});

            ASSERT_EQ(lexer_result.tokens[2].type, yaml::lexer::token_type::sequence);
            ASSERT_EQ(lexer_result.tokens[2].value, yaml::basic_string_view< char_type>{"-"});

            ASSERT_EQ(lexer_result.tokens[3].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[3].value, yaml::basic_string_view< char_type>{"  "});

            ASSERT_EQ(lexer_result.tokens[4].type, yaml::lexer::token_type::sequence);
            ASSERT_EQ(lexer_result.tokens[4].value, yaml::basic_string_view< char_type>{"-"});

            ASSERT_EQ(lexer_result.tokens[5].type, yaml::lexer::token_type::eof);
        });
    }

    TEST(sequence, nested_to_string_literal)
    {
        const std::string input = "- --  -";

        using char_type = typename decltype(input)::value_type;

        run_read_all_styles<char_type>(input, [](std::string input) {
            const auto lexer_result = yaml::lexer::tokenize(input);

            ASSERT_EQ(lexer_result.result_code, yaml::result_code::success);

            ASSERT_EQ(lexer_result.tokens.size(), 4);

            ASSERT_EQ(lexer_result.tokens[0].type, yaml::lexer::token_type::sequence);
            ASSERT_EQ(lexer_result.tokens[0].value, yaml::basic_string_view< char_type>{"-"});

            ASSERT_EQ(lexer_result.tokens[1].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[1].value, yaml::basic_string_view< char_type>{" "});

            ASSERT_EQ(lexer_result.tokens[2].type, yaml::lexer::token_type::string_literal);
            ASSERT_EQ(lexer_result.tokens[2].value, yaml::basic_string_view< char_type>{"--  -"});

            ASSERT_EQ(lexer_result.tokens[3].type, yaml::lexer::token_type::eof);
        });
    }

    TEST(set, to_string_literal)
    {
        const std::string input = "? test";

        using char_type = typename decltype(input)::value_type;

        run_read_all_styles<char_type>(input, [](std::string input) {
            const auto lexer_result = yaml::lexer::tokenize(input);

            ASSERT_EQ(lexer_result.result_code, yaml::result_code::success);

            ASSERT_EQ(lexer_result.tokens.size(), 4);

            ASSERT_EQ(lexer_result.tokens[0].type, yaml::lexer::token_type::set);
            ASSERT_EQ(lexer_result.tokens[0].value, yaml::basic_string_view< char_type>{"?"});

            ASSERT_EQ(lexer_result.tokens[1].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[1].value, yaml::basic_string_view< char_type>{" "});

            ASSERT_EQ(lexer_result.tokens[2].type, yaml::lexer::token_type::string_literal);
            ASSERT_EQ(lexer_result.tokens[2].value, yaml::basic_string_view< char_type>{"test"});

            ASSERT_EQ(lexer_result.tokens[3].type, yaml::lexer::token_type::eof);
        });
    }

    TEST(whitespaces, spaces_and_tabs)
    {
        const std::string input = " \t  \t\t   \t\t\t";

        using char_type = typename decltype(input)::value_type;

        run_read_all_styles<char_type>(input, [](std::string input) {
            const auto lexer_result = yaml::lexer::tokenize(input);

            ASSERT_EQ(lexer_result.result_code, yaml::result_code::success);

            ASSERT_EQ(lexer_result.tokens.size(), 7);

            ASSERT_EQ(lexer_result.tokens[0].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[0].value, yaml::basic_string_view< char_type>{" "});

            ASSERT_EQ(lexer_result.tokens[1].type, yaml::lexer::token_type::tabs);
            ASSERT_EQ(lexer_result.tokens[1].value, yaml::basic_string_view< char_type>{"\t"});

            ASSERT_EQ(lexer_result.tokens[2].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[2].value, yaml::basic_string_view< char_type>{"  "});

            ASSERT_EQ(lexer_result.tokens[3].type, yaml::lexer::token_type::tabs);
            ASSERT_EQ(lexer_result.tokens[3].value, yaml::basic_string_view< char_type>{"\t\t"});

            ASSERT_EQ(lexer_result.tokens[4].type, yaml::lexer::token_type::spaces);
            ASSERT_EQ(lexer_result.tokens[4].value, yaml::basic_string_view< char_type>{"   "});

            ASSERT_EQ(lexer_result.tokens[5].type, yaml::lexer::token_type::tabs);
            ASSERT_EQ(lexer_result.tokens[5].value, yaml::basic_string_view< char_type>{"\t\t\t"});

            ASSERT_EQ(lexer_result.tokens[6].type, yaml::lexer::token_type::eof);
        });
    }

    TEST(bom, utf_8)
    {
        u8_string_type input = u8"...歴戦経る素早い黒小鬼、怠けドワアフ達をひらり。裃の鵺、棟誉めて夜露誘う。";
           
        using char_type = typename decltype(input)::value_type;

        const std::array<uint8_t, 3> bom_chars = { 0xEF, 0xBB, 0xBF };
        input[0] = static_cast<char_type>(bom_chars[0]);
        input[1] = static_cast<char_type>(bom_chars[1]);
        input[2] = static_cast<char_type>(bom_chars[2]);

        run_read_all_styles<char_type>(input, [](u8_string_type input) {
            const auto lexer_result = yaml::lexer::tokenize(input);

            ASSERT_EQ(lexer_result.result_code, yaml::result_code::success);

            ASSERT_EQ(lexer_result.tokens.size(), 2);

            ASSERT_EQ(lexer_result.tokens[0].type, yaml::lexer::token_type::string_literal);
            ASSERT_EQ(lexer_result.tokens[0].value, yaml::basic_string_view<char_type>{u8"歴戦経る素早い黒小鬼、怠けドワアフ達をひらり。裃の鵺、棟誉めて夜露誘う。"});

            ASSERT_EQ(lexer_result.tokens[1].type, yaml::lexer::token_type::eof);
        }); 
    }
}