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

#ifndef MINIYAML_TEST_SAX_UTILITY_HEADER_FILE
#define MINIYAML_TEST_SAX_UTILITY_HEADER_FILE

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
    index,
    null,
    string,
    tag,
    comment
};

struct test_scalar_style {
    yaml::scalar_style_type style;
    yaml::chomping_type chomping;

    inline test_scalar_style() :
        style(yaml::scalar_style_type::none),
        chomping(yaml::chomping_type::strip),
        m_default_constructed(true)
    {}

    inline test_scalar_style(yaml::scalar_style_type p_style, yaml::chomping_type p_chomping) :
        style(p_style),
        chomping(p_chomping),
        m_default_constructed(false)
    {}

    inline bool operator == (const test_scalar_style& rhs) const {
        return
            m_default_constructed == rhs.m_default_constructed &&
            style == rhs.style &&
            chomping == rhs.chomping;
    }

private:
    bool m_default_constructed;
};

template<typename TChar>
struct test_sax_handler {

    std::vector<test_sax_instruction> instructions = {};

    std::vector<test_scalar_style> scalar_styles = {};
    std::vector<std::basic_string<TChar>> keys = {};
    std::vector<std::basic_string<TChar>> strings = {};
    std::vector<size_t> indices = {};
    std::vector<std::basic_string<TChar>> tags = {};
    std::vector<std::basic_string<TChar>> comments = {};

    test_sax_instruction* m_current_read_instruction = nullptr;
    test_scalar_style* m_current_read_scalar_styles = nullptr;
    std::basic_string<TChar>* m_current_read_string = nullptr;
    std::basic_string<TChar>* m_current_read_key = nullptr;
    size_t* m_current_read_index = nullptr;
    std::basic_string<TChar>* m_current_read_tag = nullptr;
    std::basic_string<TChar>* m_current_read_comment = nullptr;

    void prepare_read() {
        m_current_read_instruction = instructions.data();
        m_current_read_scalar_styles = scalar_styles.data();
        m_current_read_string = strings.data();
        m_current_read_key = keys.data();
        m_current_read_index = indices.data();
        m_current_read_tag = tags.data();
        m_current_read_comment = comments.data();
    }

    void reset() {
        instructions.clear();
        scalar_styles.clear();
        keys.clear();
        strings.clear();
        indices.clear();
        tags.clear();
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

    size_t get_next_index() {
        auto* current_index = m_current_read_index++;
        if (current_index >= indices.data() + indices.size()) {
            return size_t{ 0 };
        }
        return *current_index;
    }

    std::basic_string<TChar> get_next_comment() {
        auto* current_comment = m_current_read_comment++;
        if (current_comment >= comments.data() + comments.size()) {
            return {};
        }
        return *current_comment;
    }

    std::basic_string<TChar> get_next_tag() {
        auto* current_tag = m_current_read_tag++;
        if (current_tag >= tags.data() + tags.size()) {
            return {};
        }
        return *current_tag;
    }


    void null() {
        instructions.push_back(test_sax_instruction::null);
    }

    void start_document() {
        instructions.push_back(test_sax_instruction::start_document);
    }

    void end_document() {
        instructions.push_back(test_sax_instruction::end_document);
    }

    void start_scalar(yaml::scalar_style_type block_style, yaml::chomping_type chomping) {
        instructions.push_back(test_sax_instruction::start_scalar);
        scalar_styles.emplace_back(block_style, chomping);
    }

    void end_scalar() {
        instructions.push_back(test_sax_instruction::end_scalar);
    }

    void start_object() {
        instructions.push_back(test_sax_instruction::start_object);
    }

    void end_object() {
        instructions.push_back(test_sax_instruction::end_object);
    }

    void start_array() {
        instructions.push_back(test_sax_instruction::start_array);
    }

    void end_array() {
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

    void index(size_t value) {
        indices.push_back(value);
        instructions.push_back(test_sax_instruction::index);
    }

    void tag(yaml::basic_string_view<TChar> value) {
        tags.push_back(std::basic_string<TChar>{ value.data(), value.size() });
        instructions.push_back(test_sax_instruction::tag);
    }

    void comment(yaml::basic_string_view<TChar> value) {
        comments.push_back(std::basic_string<TChar>{ value.data(), value.size() });
        instructions.push_back(test_sax_instruction::comment);
    }
};

#endif