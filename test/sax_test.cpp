/*
* MIT License
*
* Copyright(c) 2022 Jimmie Bergmann
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

enum class test_sax_instruction
{
    start_object,
    end_object,
    start_array,
    end_array,
    key,
    null,
    string,
    comment
};

template<typename TChar>
struct test_sax_handler {

    std::vector<test_sax_instruction> instructions = {};

    size_t start_object_count = 0;
    size_t end_object_count = 0;
    size_t start_array_count = 0;
    size_t end_array_count = 0;
    size_t null_count = 0;
    std::vector<std::basic_string<TChar>> keys = {};
    std::vector<std::basic_string<TChar>> strings = {};
    std::vector<std::basic_string<TChar>> comments = {};

    test_sax_instruction* m_current_read_instruction = nullptr;
    std::basic_string<TChar>* m_current_read_key = nullptr;
    std::basic_string<TChar>* m_current_read_string = nullptr;
    std::basic_string<TChar>* m_current_read_comment = nullptr;

    void prepare_read() {
        m_current_read_instruction = instructions.data();
        m_current_read_key = keys.data();
        m_current_read_string = strings.data();
        m_current_read_comment = comments.data();
    }

    test_sax_instruction get_next_instruction() {
        return *(m_current_read_instruction++);
    }

    std::basic_string<TChar> get_next_key() {
        auto* current_key = m_current_read_key++;
        if (current_key >= keys.data() + keys.size()) {
            return {};
        }
        return *current_key;
    }

    std::basic_string<TChar> get_next_string() {
        auto* current_string = m_current_read_string++;
        if (current_string >= strings.data() + strings.size()) {
            return {};
        }
        return *current_string;
    }

    std::basic_string<TChar> get_next_comment() {
        auto* current_comment = m_current_read_comment++;
        if (current_comment >= comments.data() + comments.size()) {
            return {};
        }
        return *current_comment;
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

    void key(yaml::basic_string_view<TChar> value) {
        keys.push_back(std::basic_string<TChar>{ value.data(), value.size() });
        instructions.push_back(test_sax_instruction::key);
    }

    void null() {
        ++null_count;
        instructions.push_back(test_sax_instruction::null);
    }

    void string(yaml::basic_string_view<TChar> value) {
        strings.push_back(std::basic_string<TChar>{ value.data(), value.size() });
        instructions.push_back(test_sax_instruction::string);
    }

    void comment(yaml::basic_string_view<TChar> value) {
        comments.push_back(std::basic_string<TChar>{ value.data(), value.size() });
        instructions.push_back(test_sax_instruction::comment);
    }
};

template<typename TChar>
void run_sax_parse(const std::function<void(std::basic_string<TChar>)>& func, std::basic_string<TChar> input, const std::string& type) {
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
void run_sax_parse_unix_style(std::basic_string<TChar> input, const std::function<void(std::basic_string<TChar>)>& func) {
    run_sax_parse<TChar>(func, input, "linux_style");
}

template<typename TChar>
void run_sax_parse_win_style(std::basic_string<TChar> input, const std::function<void(std::basic_string<TChar>)>& func) {
#if MINIYAML_HAS_IF_CONSTEXPR
    if constexpr (std::is_same<TChar, char>::value == false) {
        replace_all<TChar>(input, u8"\n", u8"\r\n");
    }
    else
#endif
    {
        replace_all<TChar>(input, "\n", "\r\n");
    }

    run_sax_parse<TChar>(func, input, "windows_style");
}

template<typename TChar>
void run_sax_parse_mac_style(std::basic_string<TChar> input, const std::function<void(std::basic_string<TChar>)>& func) {

#if MINIYAML_HAS_IF_CONSTEXPR
    if constexpr (std::is_same<TChar, char>::value == false) {
        replace_all<TChar>(input, u8"\n", u8"\r");
    }
    else 
#endif
    {
        replace_all<TChar>(input, "\n", "\r");
    }

    run_sax_parse<TChar>(func, input, "mac_style");
}

template<typename TChar>
void run_sax_parse_all_styles(const std::basic_string<TChar>& input, const std::function<void(std::basic_string<TChar>)>& func) {
    run_sax_parse_unix_style<TChar>(input, func);
    run_sax_parse_win_style<TChar>(input, func);
    run_sax_parse_mac_style<TChar>(input, func);
}

TEST(sax_parse, ok_comments) {
    const std::string input =
        "#test comment 1\n"
        "# test comment 2 \n"
        "#\ttest comment 3\t\n"
        "# \ttest comment 4\n"
        "#\t test comment 5 \n"
        "# \t test comment 6\t\n"
        " #test comment 7\n"
        " # test comment 8 \n"
        " #\ttest comment 9\t\n"
        " # \ttest comment 10\n"
        " #\t test comment 11 \n"
        " # \t test comment 12\t\n"
        "\t#test comment 13\n"
        "\t# test comment 14 \n"
        "\t#\ttest comment 15\t\n"
        "\t# \ttest comment 16\n"
        "\t#\t test comment 17 \n"
        "\t# \t test comment 18\t\n";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 19 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 1");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 2");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 3");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 4");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 5");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 6");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 7");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 8");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 9");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 10");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 11");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 12");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 13");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 14");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 15");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 16");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 17");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "test comment 18");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::null);
    });
}

TEST(sax_parse, ok_normal_scalar_single_line)
{
    const std::string input =
        "Hello world \n";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 1 });
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
    });
}


TEST(sax_parse, ok_normal_scalar_single_line_with_comment)
{
    const std::string input =
        "Hello world #Comment goes here\n";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 2 });
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Comment goes here");
    });
}

TEST(sax_parse, ok_normal_scalar_multi_line)
{
    const std::string input =
        "Hello world \n"
        " This is another line, with a leading space.\n"
        "My last line, with ending newline character.\n";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "This is another line, with a leading space.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "My last line, with ending newline character.");
    });
}

TEST(sax_parse, ok_normal_scalar_multi_line_with_comment)
{
    const std::string input =
        "Hello world    #This is my first comment.\n"
        " This is another line, with a leading space. # Another useless comment...   \n"
        "My last line, with ending newline character. # Last comment  \n";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 6 });
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "This is my first comment.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "This is another line, with a leading space.");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Another useless comment...");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "My last line, with ending newline character.");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::comment);
        EXPECT_EQ(handler.get_next_comment(), "Last comment");
    });
}

TEST(sax_parse, ok_normal_scalar_multi_line_without_newline)
{
    const std::string input =
        "  Hello world \n"
        " This is another line, with a leading space.\n"
        "My last line, without any ending newline character.";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "This is another line, with a leading space.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "My last line, without any ending newline character.");
    });
}

TEST(sax_parse, fail_normal_scalar_invalid_tab_1)
{
    const std::string input =
        "\tHello world \n";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::forbidden_tab_indentation);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 0 });
    });
}

TEST(sax_parse, fail_normal_scalar_invalid_tab_2)
{
    const std::string input =
        "Hello world \n"
        "\tThis line is invalid, due to tab.";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::forbidden_tab_indentation);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 1 });
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
    });
}


TEST(sax_parse, ok_null_objects)
{
    const std::string input =
        " aaaa:\n"
        "  bbbb:\n"
        " cccc:\n"
        "    dddd:\n"
        "      eeee: \n"
        "        ffff:\t\n"
        "\n"
        "\t\n"
        " \n"
        "      gggg:\n"
        "      hhhh:\n";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 22 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "aaaa");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "bbbb");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::null);
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "cccc");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "dddd");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "eeee");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "ffff");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::null);
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "gggg");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::null);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "hhhh");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::null);
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);
    });
}

TEST(sax_parse, ok_object_scalar_values)
{
    const std::string input =
        "key:with:colon: Hello world\n"
        "key 2:  Second key value\n"
        "key 3: \t This is my last key";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key:with:colon");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Second key value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 3");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "This is my last key");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);    
    });
}

TEST(sax_parse, ok_object_scalar_values_with_leading_spaces)
{
    const std::string input =
        " key:with:colon: Hello world\n"
        " key 2:  Second key value\n"
        " key 3: \t This is my last key";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key:with:colon");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Second key value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 3");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "This is my last key");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);
    });
}

TEST(sax_parse, ok_object_key_at_EOF)
{
    const std::string input =
        "key 1: Hello world\n"
        "end of line:";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 6 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "end of line");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::null);
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);
    });
}

TEST(sax_parse, fail_object_nested_on_single_line)
{
    const std::string input =
        "key 1: key 2: value\n";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::unexpected_key);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 2 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1");
    });
}

TEST(sax_parse, fail_object_key_missing_at_EOF)
{
    const std::string input =
        "key 1: Hello world\n"
        "end of line";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::missing_key);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
    });
}

TEST(sax_parse, fail_object_key_invalid_indention_1)
{
    const std::string input =
        " key:with:colon: Hello world\n"
        "key 2:  Second key value\n"
        "key 3: \t This is my last key\n";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::unexpected_token);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 4 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key:with:colon");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);
    });
}

TEST(sax_parse, fail_object_key_invalid_indention_2)
{
    const std::string input =
        "key:with:colon: Hello world\n"
        " key 2:  Second key value\n"
        "key 3: \t This is my last key\n";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::unexpected_key);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "key:with:colon");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), "Hello world");
    });
}

TEST(sax_parse, fail_object_key_value_invalid_indention_1)
{
    const std::string input =
        "test:\n"
        "wow";

    using char_type = typename decltype(input)::value_type;

    run_sax_parse_all_styles<char_type>(input, [](std::string input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::missing_key);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), "test");
    });
}

TEST(sax_parse, ok_u8_BOM_1)
{    
    mini_yaml_test::u8_string_type input = u8"...key 1: 歴戦経る素早い黒小鬼、怠けドワアフ達をひらり。裃の鵺、棟誉めて夜露誘う。\nkey 2: test";

    using char_type = typename decltype(input)::value_type;

    const std::array<uint8_t, 3> bom_chars = { 0xEF, 0xBB, 0xBF };
    input[0] = static_cast<char_type>(bom_chars[0]);
    input[1] = static_cast<char_type>(bom_chars[1]);
    input[2] = static_cast<char_type>(bom_chars[2]);

    run_sax_parse_all_styles<char_type>(input, [](mini_yaml_test::u8_string_type input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 6 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        ASSERT_EQ(handler.get_next_key(), u8"key 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        const mini_yaml_test::u8_string_type utf8_value = u8"歴戦経る素早い黒小鬼、怠けドワアフ達をひらり。裃の鵺、棟誉めて夜露誘う。";
        EXPECT_EQ( handler.get_next_string(), utf8_value);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), u8"key 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), u8"test");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);
   });
}

TEST(sax_parse, ok_u8_BOM_2)
{
    mini_yaml_test::u8_string_type input = u8"...key 1: Швидка бура лисиця перестрибує через ледачого\nkey 2: test";

    using char_type = typename decltype(input)::value_type;

    const std::array<uint8_t, 3> bom_chars = { 0xEF, 0xBB, 0xBF };
    input[0] = static_cast<char_type>(bom_chars[0]);
    input[1] = static_cast<char_type>(bom_chars[1]);
    input[2] = static_cast<char_type>(bom_chars[2]);

    run_sax_parse_all_styles<char_type>(input, [](mini_yaml_test::u8_string_type input) {
        test_sax_handler<char_type> handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 6 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        ASSERT_EQ(handler.get_next_key(), u8"key 1");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        const mini_yaml_test::u8_string_type utf8_value = u8"Швидка бура лисиця перестрибує через ледачого";
        EXPECT_EQ(handler.get_next_string(), utf8_value);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::key);
        EXPECT_EQ(handler.get_next_key(), u8"key 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_instruction::string);
        EXPECT_EQ(handler.get_next_string(), u8"test");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_instruction::end_object);
    });
}

// 