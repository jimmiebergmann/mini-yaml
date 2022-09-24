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

struct test_sax_handler {
    enum class instruction
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

    std::vector<instruction> instructions = {};

    size_t start_object_count = 0;
    size_t end_object_count = 0;
    size_t start_array_count = 0;
    size_t end_array_count = 0;
    size_t null_count = 0;
    std::vector<std::string> keys = {};
    std::vector<std::string> strings = {};
    std::vector<std::string> comments = {};

    instruction* m_current_read_instruction = nullptr;
    std::string* m_current_read_key = nullptr;
    std::string* m_current_read_string = nullptr;
    std::string* m_current_read_comment = nullptr;

    void prepare_read() {
        m_current_read_instruction = instructions.data();
        m_current_read_key = keys.data();
        m_current_read_string = strings.data();
        m_current_read_comment = comments.data();
    }

    instruction get_next_instruction() {
        return *(m_current_read_instruction++);
    }

    const char* get_next_key() {
        auto* current_key = m_current_read_key++;
        if (current_key >= keys.data() + keys.size()) {
            return nullptr;
        }
        return current_key->c_str();
    }

    const char* get_next_string() {
        auto* current_string = m_current_read_string++;
        if (current_string >= strings.data() + strings.size()) {
            return nullptr;
        }
        return current_string->c_str();
    }

    const char* get_next_comment() {
        auto* current_comment = m_current_read_comment++;
        if (current_comment >= comments.data() + comments.size()) {
            return nullptr;
        }
        return current_comment->c_str();
    }

    void start_object() {
        ++start_object_count;
        instructions.push_back(instruction::start_object);
    }

    void end_object() {
        ++end_object_count;
        instructions.push_back(instruction::end_object);
    }

    void start_array() {
        ++start_array_count;
        instructions.push_back(instruction::start_array);
    }

    void end_array() {
        ++end_array_count;
        instructions.push_back(instruction::end_array);
    }

    void key(yaml::string_view value) {
        keys.push_back(std::string{ value.data(), value.size() });
        instructions.push_back(instruction::key);
    }

    void null() {
        ++null_count;
        instructions.push_back(instruction::null);
    }

    void string(yaml::string_view value) {
        strings.push_back(std::string{ value.data(), value.size() });
        instructions.push_back(instruction::string);
    }

    void comment(yaml::string_view value) {
        comments.push_back(std::string{ value.data(), value.size() });
        instructions.push_back(instruction::comment);
    }
};


void run_sax_parse(const std::function<void(std::string)>& func, std::string input, const std::string& type) {
    mini_yaml_test::print_info(type);
    func(input);
}

void replace_all(std::string& source, const std::string& from, const std::string& to)
{
    std::string new_string;
    new_string.reserve(source.length());

    std::string::size_type last_pos = 0;
    std::string::size_type find_pos = 0;

    while (std::string::npos != (find_pos = source.find(from, last_pos)))
    {
        new_string.append(source, last_pos, find_pos - last_pos);
        new_string += to;
        last_pos = find_pos + from.length();
    }

    new_string += source.substr(last_pos);
    source = std::move(new_string);
}

void run_sax_parse_unix_style(std::string input, const std::function<void(std::string)>& func) {
    run_sax_parse(func, input, "linux_style");
}

void run_sax_parse_win_style(std::string input, const std::function<void(std::string)>& func) {
    replace_all(input, "\n", "\r\n");
    run_sax_parse(func, input, "windows_style");
}

void run_sax_parse_mac_style(std::string input, const std::function<void(std::string)>& func) {
    replace_all(input, "\n", "\r");
    run_sax_parse(func, input, "mac_style");
}

void run_sax_parse_all_styles(const std::string& input, const std::function<void(std::string)>& func) {
    run_sax_parse_unix_style(input, func);
    run_sax_parse_win_style(input, func);
    run_sax_parse_mac_style(input, func);
}

TEST(sax_parse, ok_comments) {
    std::string input =
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

    run_sax_parse_all_styles(input, [](std::string input) {
        test_sax_handler handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 19 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 1");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 2");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 3");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 4");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 5");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 6");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 7");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 8");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 9");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 10");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 11");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 12");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 13");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 14");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 15");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 16");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 17");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "test comment 18");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::null);
    });
}

TEST(sax_parse, ok_normal_scalar_single_line)
{
    std::string input =
        "Hello world \n";

    run_sax_parse_all_styles(input, [](std::string input) {
        test_sax_handler handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 1 });
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "Hello world");
    });
}

TEST(sax_parse, ok_normal_scalar_single_line_with_comment)
{
    std::string input =
        "Hello world #Comment goes here\n";

    run_sax_parse_all_styles(input, [](std::string input) {
        test_sax_handler handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 2 });
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "Hello world");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "Comment goes here");
    });
}

TEST(sax_parse, ok_normal_scalar_multi_line)
{
    std::string input =
        "Hello world \n"
        " This is another line, with a leading space.\n"
        "My last line, with ending newline character.\n";

    run_sax_parse_all_styles(input, [](std::string input) {
        test_sax_handler handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "Hello world");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "This is another line, with a leading space.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "My last line, with ending newline character.");
    });
}

TEST(sax_parse, ok_normal_scalar_multi_line_with_comment)
{
    std::string input =
        "Hello world    #This is my first comment.\n"
        " This is another line, with a leading space. # Another useless comment...   \n"
        "My last line, with ending newline character. # Last comment  \n";

    run_sax_parse_all_styles(input, [](std::string input) {
        test_sax_handler handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 6 });
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "Hello world");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "This is my first comment.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "This is another line, with a leading space.");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "Another useless comment...");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "My last line, with ending newline character.");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::comment);
        EXPECT_STREQ(handler.get_next_comment(), "Last comment");
    });
}

TEST(sax_parse, ok_normal_scalar_multi_line_without_newline)
{
    std::string input =
        "  Hello world \n"
        " This is another line, with a leading space.\n"
        "My last line, without any ending newline character.";

    run_sax_parse_all_styles(input, [](std::string input) {
        test_sax_handler handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "Hello world");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "This is another line, with a leading space.");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "My last line, without any ending newline character.");
    });
}

TEST(sax_parse, fail_normal_scalar_invalid_tab_1)
{
    std::string input =
        "\tHello world \n";

    run_sax_parse_all_styles(input, [](std::string input) {
        test_sax_handler handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::forbidden_tab_indentation);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 0 });
    });
}

TEST(sax_parse, fail_normal_scalar_invalid_tab_2)
{
    std::string input =
        "Hello world \n"
        "\tThis line is invalid, due to tab.";

    run_sax_parse_all_styles(input, [](std::string input) {
        test_sax_handler handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::forbidden_tab_indentation);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 1 });
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "Hello world");
    });
}


TEST(sax_parse, ok_null_objects)
{
    std::string input =
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

    run_sax_parse_all_styles(input, [](std::string input) {
        test_sax_handler handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 22 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "aaaa");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "bbbb");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::null);
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::end_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "cccc");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "dddd");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "eeee");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "ffff");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::null);
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::end_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "gggg");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::null);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "hhhh");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::null);
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::end_object);
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::end_object);
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::end_object);
    });
}

TEST(sax_parse, ok_scalar_key_values)
{
    std::string input =
        "key:with:colon: Hello world\n"
        "key 2:  Second key value\n"
        "key 3: \t This is my last key";

    run_sax_parse_all_styles(input, [](std::string input) {
        test_sax_handler handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "key:with:colon");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "Hello world");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "key 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "Second key value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "key 3");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "This is my last key");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::end_object);    
    });
}

TEST(sax_parse, ok_scalar_key_values_with_leading_spaces)
{
    std::string input =
        " key:with:colon: Hello world\n"
        " key 2:  Second key value\n"
        " key 3: \t This is my last key";

    run_sax_parse_all_styles(input, [](std::string input) {
        test_sax_handler handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::success);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 8 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "key:with:colon");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "Hello world");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "key 2");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "Second key value");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "key 3");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "This is my last key");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::end_object);
    });
}

TEST(sax_parse, fail_scalar_key_values_invalid_indention_1)
{
    std::string input =
        " key:with:colon: Hello world\n"
        "key 2:  Second key value\n"
        "key 3: \t This is my last key\n";

    run_sax_parse_all_styles(input, [](std::string input) {
        test_sax_handler handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::unexpected_token);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 4 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "key:with:colon");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "Hello world");
        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::end_object);
    });
}

TEST(sax_parse, fail_scalar_key_values_invalid_indention_2)
{
    std::string input =
        "key:with:colon: Hello world\n"
        " key 2:  Second key value\n"
        "key 3: \t This is my last key\n";

    run_sax_parse_all_styles(input, [](std::string input) {
        test_sax_handler handler = {};
        ASSERT_EQ(yaml::sax::parse(input, handler), yaml::parse_result_code::unexpected_key);
        handler.prepare_read();

        ASSERT_EQ(handler.instructions.size(), size_t{ 3 });

        EXPECT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::start_object);
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::key);
        EXPECT_STREQ(handler.get_next_key(), "key:with:colon");
        ASSERT_EQ(handler.get_next_instruction(), test_sax_handler::instruction::string);
        EXPECT_STREQ(handler.get_next_string(), "Hello world");
    });
}