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

#ifndef MINIYAML_TEST_SAX_LEARNYAML_UTILITY_HEADER_FILE
#define MINIYAML_TEST_SAX_LEARNYAML_UTILITY_HEADER_FILE

#include "sax_utility.hpp"

template<typename Thandler>
void check_sax_learnyaml_tests(Thandler& handler) {
    ASSERT_EQ(handler.instructions.size(), size_t{ 375 });

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "License: https://creativecommons.org/licenses/by-sa/3.0/deed.en_US");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Origin: https://learnxinyminutes.com/docs/yaml/");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Modified version of learnxinyminutes.com - YAML.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Unsupported mini-yaml features are commented away with 3 '#'.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Additional values are commented with ### Change: << message here >>");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "---------------------------------------------------------------------");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "document start");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Comments in YAML look like this.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "YAML support single-line comments.");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "###############");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "SCALAR TYPES #");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "###############");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Our root object (which continues for the entire document) will be a map,");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "which is equivalent to a dictionary, hash or object in other languages.");

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
    EXPECT_EQ(handler.get_next_string(), "Another value goes here.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "a_number_value");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "100");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "scientific_notation");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "1e+12");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "hex_notation");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "0x123");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "evaluates to 291");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "octal_notation");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "0123");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "evaluates to 83");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "The number 1 will be interpreted as a number, not a boolean.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "If you want it to be interpreted as a boolean, use true.");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "boolean");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "true");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "null_value");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "null");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "another_null_value");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "~");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "key with spaces");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "value");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "empty_value");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "## Change: << Empty values are equal to null. >>");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Yes and No (doesn't matter the case) will be evaluated to boolean");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "true and false values respectively.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "To use the actual value use single or double quotes.");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "no");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "no");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "evaluates to \"false\": false");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "yes");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "No");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "evaluates to \"true\": false");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "not_enclosed");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "yes");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "evaluates to \"not_enclosed\": true");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "enclosed");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::double_quoted, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "yes");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "evaluates to \"enclosed\": yes");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Notice that strings don't need to be quoted. However, they can be.");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "however");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::single_quoted, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "A string, enclosed in quotes.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    for (size_t i = 0; i < 1; i++) {
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
        handler.get_next_comment();
    }

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "single quotes");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::single_quoted, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "have ''one'' escape pattern");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "double quotes");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::double_quoted, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "have many: \\\", \\0, \\t, \\u263A, \\x0d\\x0a == \\r\\n, and more.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "UTF-8/16/32 characters need to be encoded");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "Superscript two");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "\\u00B2");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Special characters must be enclosed in single or double quotes");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "special_characters");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::double_quoted, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "[ John ] & { Jane } - <Doe>");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Multiple-line strings can be written either as a 'literal block' (using |),");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "or a 'folded block' (using '>').");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Literal block turn every newline within the string into a literal newline (\\n).");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Folded block removes newlines within the string.");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "literal_block");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::literal, yaml::chomping_type::clip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "This entire block of text will be the value of the 'literal_block' key,");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "with line breaks being preserved.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "The literal continues until de-dented, and the leading indentation is");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "stripped.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "    Any lines that are 'more-indented' keep the rest of their indentation -");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "    these lines will be indented by 4 spaces.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "folded_style");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::folded, yaml::chomping_type::clip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "This entire block of text will be the value of 'folded_style', but this");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "time, all newlines will be replaced with a single space.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "Blank lines, like above, are converted to a newline character.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "    'More-indented' lines keep their newlines, too -");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "    this text will appear over two lines.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "|- and >- removes the trailing blank lines (also called literal/block \"strip\")");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "literal_strip");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::literal, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "This entire block of text will be the value of the 'literal_block' key,");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "with trailing blank line being stripped.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "block_strip");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::folded, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "This entire block of text will be the value of 'folded_style', but this");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "time, all newlines will be replaced with a single space and ");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "trailing blank line being stripped.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "|+ and >+ keeps trailing blank lines (also called literal/block \"keep\")");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "literal_keep");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::literal, yaml::chomping_type::keep));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "This entire block of text will be the value of the 'literal_block' key,");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "with trailing blank line being kept.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "block_keep");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::folded, yaml::chomping_type::keep));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "This entire block of text will be the value of 'folded_style', but this");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "time, all newlines will be replaced with a single space and ");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "trailing blank line being kept.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "###################");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "COLLECTION TYPES #");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "###################");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Nesting uses indentation. 2 space indent is preferred (but not required).");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "a_nested_map");

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
    EXPECT_EQ(handler.get_next_string(), "Another Value");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "another_nested_map");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "hello");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "hello");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Maps don't have to have string keys.");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "0.25");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "a float key");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Keys can also be complex, like multi-line objects");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "We use ? followed by a space to indicate the start of a complex key.");

    for (size_t i = 0; i < 4; i++) {
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
        handler.get_next_comment();
    }

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "YAML also allows mapping between sequences with the complex key syntax");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Some language parsers might complain");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "An example");

    for (size_t i = 0; i < 3; i++) {
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
        handler.get_next_comment();
    }

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Sequences (equivalent to lists or arrays) look like this");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "(note that the '-' counts as indentation):");


    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "a_sequence");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
    EXPECT_EQ(handler.get_next_index(), 0);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "Item 1");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
    EXPECT_EQ(handler.get_next_index(), 1);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "Item 2");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
    EXPECT_EQ(handler.get_next_index(), 2);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "0.5");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "sequences can contain disparate types.");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
    EXPECT_EQ(handler.get_next_index(), 3);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "Item 4");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
    EXPECT_EQ(handler.get_next_index(), 4);

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

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
    EXPECT_EQ(handler.get_next_index(), 5);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
    EXPECT_EQ(handler.get_next_index(), 0);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "This is a sequence");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
    EXPECT_EQ(handler.get_next_index(), 1);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "inside another sequence");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_array);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
    EXPECT_EQ(handler.get_next_index(), 6);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
    EXPECT_EQ(handler.get_next_index(), 0);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_array);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
    EXPECT_EQ(handler.get_next_index(), 0);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "Nested sequence indicators");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::index);
    EXPECT_EQ(handler.get_next_index(), 1);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "can be collapsed");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);


    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Since YAML is a superset of JSON, you can also write JSON-style maps and");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "sequences:");

    for (size_t i = 0; i < 3; i++) {
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
        handler.get_next_comment();
    }

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "######################");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "EXTRA YAML FEATURES #");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "######################");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "YAML also has a handy feature called 'anchors', which let you easily duplicate");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "content across your document.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Anchors identified by & character which define the value.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Aliases identified by * character which acts as \"see above\" command.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Both of these keys will have the same value:");

    for (size_t i = 0; i < 2; i++) {
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
        handler.get_next_comment();
    }

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Anchors can be used to duplicate/inherit properties");

    for (size_t i = 0; i < 2; i++) {
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
        handler.get_next_comment();
    }

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "The regexp << is called 'Merge Key Language-Independent Type'. It is used to");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "indicate that all the keys of one or more specified maps should be inserted");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "into the current map.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "NOTE: If key already exists alias will not be merged");

    for (size_t i = 0; i < 7; i++) {
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
        handler.get_next_comment();
    }

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "foo and bar would also have name: Everyone has same name");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "YAML also has tags, which you can use to explicitly declare types.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Syntax: !![typeName] [value]");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_array);
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_array);
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_array);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "explicit_boolean");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::tag);
    EXPECT_EQ(handler.get_next_tag(), "bool");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "true");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "explicit_integer");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::tag);
    EXPECT_EQ(handler.get_next_tag(), "int");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "42");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "explicit_float");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::tag);
    EXPECT_EQ(handler.get_next_tag(), "float");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "-42.24");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "explicit_string");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::tag);
    EXPECT_EQ(handler.get_next_tag(), "str");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "0.5");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "explicit_datetime");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::tag);
    EXPECT_EQ(handler.get_next_tag(), "timestamp");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "2022-11-17 12:34:56.78 +9");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "explicit_null");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::tag);
    EXPECT_EQ(handler.get_next_tag(), "null");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "null");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);


    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Some parsers implement language specific tags, like this one for Python's");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "complex number type.");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
    handler.get_next_comment();

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "We can also use yaml complex keys with language specific tags");

    for (size_t i = 0; i < 2; i++) {
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
        handler.get_next_comment();
    }

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Would be {(5, 7): 'Fifty Seven'} in Python");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "###################");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "EXTRA YAML TYPES #");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "###################");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Strings and numbers aren't the only scalars that YAML can understand.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "ISO-formatted date and datetime literals are also parsed.");

    for (size_t i = 0; i < 4; i++) {
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
        handler.get_next_comment();
    }

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "The !!binary tag indicates that a string is actually a base64-encoded");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "representation of a binary blob.");

    for (size_t i = 0; i < 5; i++) {
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
        handler.get_next_comment();
    }

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "YAML also has a set type, which looks like this:");

    for (size_t i = 0; i < 5; i++) {
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
        handler.get_next_comment();
    }

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Sets are just maps with null values; the above is equivalent to:");


    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "set2");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "item1");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "null");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "item2");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "null");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "item3");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::scalar_style_type::none, yaml::chomping_type::strip));
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "null");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
    EXPECT_EQ(handler.get_next_string(), "");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
}

#endif