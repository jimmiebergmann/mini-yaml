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

enum class test_sax_instruction {
    start_document,
    end_document,
    start_scalar,
    end_scalar,
    start_object,
    end_object,
    start_array,
    end_array,
    key,
    null,
    string,
    comment
};

struct test_scalar_style {
    yaml::block_style_type block_style;
    yaml::chomping_type chomping;

    test_scalar_style() :
        block_style(yaml::block_style_type::none),
        chomping(yaml::chomping_type::strip),
        m_default_constructed(true)
    {}

    test_scalar_style(yaml::block_style_type p_block_style, yaml::chomping_type p_chomping) :
        block_style(p_block_style),
        chomping(p_chomping),
        m_default_constructed(false)
    {}

    bool operator == (const test_scalar_style& rhs) const {
        return
            m_default_constructed == rhs.m_default_constructed &&
            block_style == rhs.block_style &&
            chomping == rhs.chomping;
    }

private:
    bool m_default_constructed;
};

template<typename TChar>
struct test_sax_handler {

    std::vector<test_sax_instruction> instructions = {};

    size_t start_document_count = 0;
    size_t end_document_count = 0;
    size_t start_scalar_count = 0;
    size_t end_scalar_count = 0;
    size_t start_object_count = 0;
    size_t end_object_count = 0;
    size_t start_array_count = 0;
    size_t end_array_count = 0;
    size_t null_count = 0;
    std::vector<test_scalar_style> scalar_styles = {};
    std::vector<std::basic_string<TChar>> keys = {};
    std::vector<std::basic_string<TChar>> strings = {};
    std::vector<std::basic_string<TChar>> comments = {};

    test_sax_instruction* m_current_read_instruction = nullptr;
    test_scalar_style* m_current_read_scalar_styles = nullptr;
    std::basic_string<TChar>* m_current_read_string = nullptr;
    std::basic_string<TChar>* m_current_read_key = nullptr;
    std::basic_string<TChar>* m_current_read_comment = nullptr;

    void prepare_read() {
        m_current_read_instruction = instructions.data();
        m_current_read_scalar_styles = scalar_styles.data();
        m_current_read_string = strings.data();
        m_current_read_key = keys.data();
        m_current_read_comment = comments.data();
    }

    void reset() {
        instructions.clear();
        start_document_count = 0;
        end_document_count = 0;
        start_scalar_count = 0;
        end_scalar_count = 0;
        start_object_count = 0;
        end_object_count = 0;
        start_array_count = 0;
        end_array_count = 0;
        null_count = 0;
        scalar_styles.clear();
        keys.clear();
        strings.clear();
        comments.clear();
    }

    test_sax_instruction get_next_instruction() {
        return *(m_current_read_instruction++);
    }

    test_scalar_style get_next_scalar_style() {
        auto* current_scalar_style = m_current_read_scalar_styles++;
        if (current_scalar_style >= scalar_styles.data() + scalar_styles.size()) {
            return {};
        }
        return *current_scalar_style;
    }

    std::basic_string<TChar> get_next_string() {
        auto* current_string = m_current_read_string++;
        if (current_string >= strings.data() + strings.size()) {
            return {};
        }
        return *current_string;
    }

    std::basic_string<TChar> get_next_key() {
        auto* current_key = m_current_read_key++;
        if (current_key >= keys.data() + keys.size()) {
            return {};
        }
        return *current_key;
    }

    std::basic_string<TChar> get_next_comment() {
        auto* current_comment = m_current_read_comment++;
        if (current_comment >= comments.data() + comments.size()) {
            return {};
        }
        return *current_comment;
    }

    void null() {
        ++null_count;
        instructions.push_back(test_sax_instruction::null);
    }

    void start_document() {
        ++start_document_count;
        instructions.push_back(test_sax_instruction::start_document);
    }

    void end_document() {
        ++end_document_count;
        instructions.push_back(test_sax_instruction::end_document);
    }

    void start_scalar(yaml::block_style_type block_style, yaml::chomping_type chomping) {
        ++start_scalar_count;
        instructions.push_back(test_sax_instruction::start_scalar);
        scalar_styles.emplace_back(block_style, chomping);
    }

    void end_scalar() {
        ++end_scalar_count;
        instructions.push_back(test_sax_instruction::end_scalar);
    }

    void start_object() {
        ++start_object_count;
        instructions.push_back(test_sax_instruction::start_object);
    }

    void end_object() {
        ++end_object_count;
        instructions.push_back(test_sax_instruction::end_object);
    }

    void start_array() {
        ++start_array_count;
        instructions.push_back(test_sax_instruction::start_array);
    }

    void end_array() {
        ++end_array_count;
        instructions.push_back(test_sax_instruction::end_array);
    }

    void string(yaml::basic_string_view<TChar> value) {
        strings.push_back(std::basic_string<TChar>{ value.data(), value.size() });
        instructions.push_back(test_sax_instruction::string);
    }

    void key(yaml::basic_string_view<TChar> value) {
        keys.push_back(std::basic_string<TChar>{ value.data(), value.size() });
        instructions.push_back(test_sax_instruction::key);
    }

    void comment(yaml::basic_string_view<TChar> value) {
        comments.push_back(std::basic_string<TChar>{ value.data(), value.size() });
        instructions.push_back(test_sax_instruction::comment);
    }
};

template<typename TChar>
void run_sax_read(const std::function<void(std::basic_string<TChar>)>& func, std::basic_string<TChar> input, const std::string& type) {
    mini_yaml_test::print_info(type);
    func(input);
}

template<typename TChar>
void replace_all(std::basic_string<TChar>& source, const std::basic_string<TChar>& from, const std::basic_string<TChar>& to)
{
    std::basic_string<TChar> new_string;
    new_string.reserve(source.length());

    typename std::basic_string<TChar>::size_type last_pos = 0;
    typename std::basic_string<TChar>::size_type find_pos = 0;

    while (std::string::npos != (find_pos = source.find(from, last_pos)))
    {
        new_string.append(source, last_pos, find_pos - last_pos);
        new_string += to;
        last_pos = find_pos + from.length();
    }

    new_string += source.substr(last_pos);
    source = std::move(new_string);
}

template<typename TChar>
void run_sax_read_unix_style(std::basic_string<TChar> input, const std::function<void(std::basic_string<TChar>)>& func) {
    run_sax_read<TChar>(func, input, "linux_style");
}

template<typename TChar>
void run_sax_read_win_style(std::basic_string<TChar> input, const std::function<void(std::basic_string<TChar>)>& func) {
#if MINIYAML_HAS_IF_CONSTEXPR
    if constexpr (std::is_same<TChar, char>::value == false) {
        replace_all<TChar>(input, u8"\n", u8"\r\n");
    }
    else
#endif
    {
        replace_all<TChar>(input, "\n", "\r\n");
    }

    run_sax_read<TChar>(func, input, "windows_style");
}

template<typename TChar>
void run_sax_read_mac_style(std::basic_string<TChar> input, const std::function<void(std::basic_string<TChar>)>& func) {

#if MINIYAML_HAS_IF_CONSTEXPR
    if constexpr (std::is_same<TChar, char>::value == false) {
        replace_all<TChar>(input, u8"\n", u8"\r");
    }
    else 
#endif
    {
        replace_all<TChar>(input, "\n", "\r");
    }

    run_sax_read<TChar>(func, input, "mac_style");
}

template<typename TChar>
void run_sax_read_all_styles(const std::basic_string<TChar>& input, const std::function<void(std::basic_string<TChar>)>& func) {
    run_sax_read_unix_style<TChar>(input, func);
    run_sax_read_win_style<TChar>(input, func);
    run_sax_read_mac_style<TChar>(input, func);
}


// =====================================================================
// Tests

TEST(sax_read, fail_bad_indention_objects_1)
{
    const std::string input =
        "   key 1:\n"
        "  key 1_fail:\n";

    using char_type = typename decltype(input)::value_type;

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_token);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 4 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::literal, yaml::chomping_type::clip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_token);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 7 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::literal, yaml::chomping_type::clip));
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

TEST(sax_read, fail_forbidden_tab_indentation_empty_file)
{
    const std::string input =
        "\t";

    using char_type = typename decltype(input)::value_type;

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::forbidden_tab_indentation);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
    });
}

TEST(sax_read, fail_object_unexpected_key_1)
{
    const std::string input =
        "key 1: key 2:\n";

    using char_type = typename decltype(input)::value_type;

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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

TEST(sax_read, fail_scalar_single_literal_expected_line_break)
{
    const std::string input =
        "| a\n"
        " This is a scalar value";

    using char_type = typename decltype(input)::value_type;

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::unexpected_token);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Comment");
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 3");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_2");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 4");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "Comment at key 3_2.");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_3");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 5");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_4");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 6");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "comment between 3_4 and 3_5");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 3_5");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 6 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Comment here.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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

TEST(sax_read, ok_file_learnyaml)
{
    using char_type = char;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_document_from_file<char_type>("../test/learnyaml.yaml", handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "first value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::literal, yaml::chomping_type::clip));
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
                    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::folded, yaml::chomping_type::keep));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "Another value here...");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object); // End 3

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 4");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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

TEST(sax_read, ok_object_single_with_scalar__inline)
{
    const std::string input =
        "key: hello world";

    using char_type = typename decltype(input)::value_type;

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
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
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value 1");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 2");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 3");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 4");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "value 4");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "key 5");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 7 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);

        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);

        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::folded, yaml::chomping_type::clip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);

        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::folded, yaml::chomping_type::keep));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::folded, yaml::chomping_type::keep));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);

        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::folded, yaml::chomping_type::strip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 16 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);

        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::literal, yaml::chomping_type::clip));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);

        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::literal, yaml::chomping_type::keep));
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

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);

        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::literal, yaml::chomping_type::strip));
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

TEST(sax_read, ok_scalar_single)
{
    const std::string input =
        "This is a scalar value";

    using char_type = typename decltype(input)::value_type;

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);

        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "This is a scalar value");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_scalar_single_literal)
{
    const std::string input =
        "|\n"
        " This is a scalar value";

    using char_type = typename decltype(input)::value_type;

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        
        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 5 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::literal, yaml::chomping_type::clip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "This is a scalar value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_scalar_single_literal_comment_after_token)
{
    const std::string input =
        "| #comment \n"
        "    Value\n";

    using char_type = typename decltype(input)::value_type;

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_document(input, handler);
        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 6 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "comment");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::literal, yaml::chomping_type::clip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

TEST(sax_read, ok_u8_BOM_1)
{    
    mini_yaml_test::u8_string_type input = 
        u8"..."
        u8"key 1: 歴戦経る素早い黒小鬼、怠けドワアフ達をひらり。裃の鵺、棟誉めて夜露誘う。\n"
        u8"key 2: test";

    using char_type = typename decltype(input)::value_type;

    const std::array<uint8_t, 3> bom_chars = { 0xEF, 0xBB, 0xBF };
    input[0] = static_cast<char_type>(bom_chars[0]);
    input[1] = static_cast<char_type>(bom_chars[1]);
    input[2] = static_cast<char_type>(bom_chars[2]);

    run_sax_read_all_styles<char_type>(input, [](mini_yaml_test::u8_string_type input) {
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
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            const mini_yaml_test::u8_string_type utf8_value = u8"歴戦経る素早い黒小鬼、怠けドワアフ達をひらり。裃の鵺、棟誉めて夜露誘う。";
            EXPECT_EQ(handler.get_next_string(), utf8_value);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        ASSERT_EQ(handler.get_next_key(), u8"key 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), u8"test");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
   });
}

TEST(sax_read, ok_u8_BOM_2)
{
    mini_yaml_test::u8_string_type input = 
        u8"..."
        u8"key 1: Швидка бура лисиця перестрибує через ледачого\n"
        u8"key 2: test";

    using char_type = typename decltype(input)::value_type;

    const std::array<uint8_t, 3> bom_chars = { 0xEF, 0xBB, 0xBF };
    input[0] = static_cast<char_type>(bom_chars[0]);
    input[1] = static_cast<char_type>(bom_chars[1]);
    input[2] = static_cast<char_type>(bom_chars[2]);

    run_sax_read_all_styles<char_type>(input, [](mini_yaml_test::u8_string_type input) {
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
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        const mini_yaml_test::u8_string_type utf8_value = u8"Швидка бура лисиця перестрибує через ледачого";
        EXPECT_EQ(handler.get_next_string(), utf8_value);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        ASSERT_EQ(handler.get_next_key(), u8"key 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), u8"test");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}

// sax_read_documents
TEST(sax_read_documents, ok_file_learnyaml)
{
    using char_type = char;

    auto handler = test_sax_handler<char_type>{};
    const auto read_result = yaml::sax::read_documents_from_file<char_type>("../test/learnyaml.yaml", handler);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
    EXPECT_TRUE(read_result);

    handler.prepare_read();
    ASSERT_EQ(handler.instructions.size(), size_t{ 284 });

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
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "value");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "another_key");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Another value goes here.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "a_number_value");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "100");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "scientific_notation");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "1e+12");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "hex_notation");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "0x123");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "evaluates to 291");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "octal_notation");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
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
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "true");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "null_value");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "null");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "another_null_value");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "~");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "key with spaces");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "value");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Yes and No (doesn't matter the case) will be evaluated to boolean");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "true and false values respectively.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "To use the actual value use single or double quotes.");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "no");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "no");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "evaluates to \"false\": false");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "yes");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "No");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "evaluates to \"true\": false");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "not_enclosed");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "yes");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "evaluates to \"not_enclosed\": true");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "enclosed");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "\"yes\"");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "evaluates to \"enclosed\": yes");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Notice that strings don't need to be quoted. However, they can be.");
    for (size_t i = 0; i < 4; i++) {
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
        handler.get_next_comment();
    }

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "UTF-8/16/32 characters need to be encoded");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "Superscript two");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "\\u00B2");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
    EXPECT_EQ(handler.get_next_comment(), "Special characters must be enclosed in single or double quotes");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
    handler.get_next_comment();

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
    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::literal, yaml::chomping_type::clip));
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
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::folded, yaml::chomping_type::clip));
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
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::literal, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "This entire block of text will be the value of the 'literal_block' key,");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "with trailing blank line being stripped.");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "block_strip");
    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::folded, yaml::chomping_type::strip));
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
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::literal, yaml::chomping_type::keep));
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
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::folded, yaml::chomping_type::keep));
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
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "another_key");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "Another Value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "another_nested_map");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
            EXPECT_EQ(handler.get_next_key(), "hello");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                EXPECT_EQ(handler.get_next_string(), "hello");
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "Maps don't have to have string keys.");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
    EXPECT_EQ(handler.get_next_key(), "0.25");

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "a float key");
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
    
    for (size_t i = 0; i < 11; i++) {
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
        handler.get_next_comment();
    }

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

    for (size_t i = 0; i < 6; i++) {
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment); // SKIP unsupported feature.
        handler.get_next_comment();
    }

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
        EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "null");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "item2");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "null");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "item3");

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
            EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
            EXPECT_EQ(handler.get_next_string(), "null");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
}

TEST(sax_read_documents, ok_read_documents)
{
    const std::string input =
        "--- # test comment 1\n"
        "key 1: value 1\n"
        "key 2: value 2\n"
        "--- # test comment 2\n"
        "key 3: value 3\n"
        "key 4: value 4\n"
        "key 5: value 5\n"
        "...\n"
        "Not reached";

    using char_type = typename decltype(input)::value_type;

    run_sax_read_all_styles<char_type>(input, [](std::string input) {
        auto handler = test_sax_handler<char_type>{};
        const auto read_result = yaml::sax::read_documents(input, handler);

        ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);
        EXPECT_EQ(read_result.current_line, 7);

        handler.prepare_read();
        ASSERT_EQ(handler.instructions.size(), size_t{ 30 });

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "test comment 1");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
                EXPECT_EQ(handler.get_next_key(), "key 1");
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
                    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                    EXPECT_EQ(handler.get_next_string(), "value 1");
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
                EXPECT_EQ(handler.get_next_key(), "key 2");
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
                    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                    EXPECT_EQ(handler.get_next_string(), "value 2");
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_document);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
            EXPECT_EQ(handler.get_next_comment(), "test comment 2");

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);

                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
                EXPECT_EQ(handler.get_next_key(), "key 3");
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
                    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                    EXPECT_EQ(handler.get_next_string(), "value 3");
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
                EXPECT_EQ(handler.get_next_key(), "key 4");
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
                    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                    EXPECT_EQ(handler.get_next_string(), "value 4");
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
                EXPECT_EQ(handler.get_next_key(), "key 5");
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::start_scalar);
                    EXPECT_EQ(handler.get_next_scalar_style(), test_scalar_style(yaml::block_style_type::none, yaml::chomping_type::strip));
                    ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
                    EXPECT_EQ(handler.get_next_string(), "value 5");
                ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_scalar);

            ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);

        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::end_document);
    });
}
