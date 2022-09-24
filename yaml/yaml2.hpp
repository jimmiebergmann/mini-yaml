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

#include <memory>
#include <array>
#include <vector>
#include <string>
#include <cstdlib>

#if defined(MINIYAML_CUSTOM_NAMESPACE)
#define MINIYAML_NAMESPACE MINIYAML_CUSTOM_NAMESPACE
#else
#define MINIYAML_NAMESPACE yaml
#endif

#if defined(__cpp_inline_variables)
#define MINIYAML_INLINE_VARIABLE inline
#else
#define MINIYAML_INLINE_VARIABLE
#endif

#define MINIYAML_NODISCARD

#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(nodiscard)
#undef MINIYAML_NODISCARD
#define MINIYAML_NODISCARD [[nodiscard]]
#endif
#endif

#if defined(__cpp_if_constexpr)
#define MINIYAML_HAS_IF_CONSTEXPR true
#else
#define MINIYAML_HAS_IF_CONSTEXPR false
#endif

#if defined(__cpp_concepts)
#define MINIYAML_HAS_CONCEPTS true
#else
#define MINIYAML_HAS_CONCEPTS false
#endif

#if defined(__cpp_lib_string_view) && !defined(MINIYAML_NO_STD_STRING_VIEW)
#include <string_view>
#define MINIYAML_HAS_STD_STRING_VIEW true
namespace MINIYAML_NAMESPACE {
    template<typename T>
    using basic_string_view = std::basic_string_view<T>;
    using string_view = std::string_view;
}
#else
#define MINIYAML_HAS_STD_STRING_VIEW false
namespace MINIYAML_NAMESPACE {
    template<typename T>
    class basic_string_view {

    public:

        basic_string_view() :
            m_data(nullptr),
            m_size(0)
        {}

        basic_string_view(const T* s, size_t count) :
            m_data(s),
            m_size(count)
        {}

        MINIYAML_NODISCARD const T* data() const { return m_data; };
        MINIYAML_NODISCARD size_t size() const { return m_size; };
        MINIYAML_NODISCARD bool empty() const { return m_size == 0; };

    private:

        const T* m_data;
        size_t m_size;

    };

    using string_view = basic_string_view<char>;
}
#endif

namespace MINIYAML_NAMESPACE {

    enum class parse_result_code {
        success,
        not_implemented,
        internal_error,
        forbidden_tab_indentation,
        unexpected_token,
        unexpected_key,
        //unexpected_sequence,
        bad_key_indentation,
        missing_key
    };

    namespace token {
        MINIYAML_INLINE_VARIABLE constexpr auto space = ' ';
        MINIYAML_INLINE_VARIABLE constexpr auto tab = '\t';
        MINIYAML_INLINE_VARIABLE constexpr auto carriage = '\r';
        MINIYAML_INLINE_VARIABLE constexpr auto newline = '\n';
        MINIYAML_INLINE_VARIABLE constexpr auto comment = '#';
        MINIYAML_INLINE_VARIABLE constexpr auto quote = '\"';
        MINIYAML_INLINE_VARIABLE constexpr auto single_quote = '\'';
        MINIYAML_INLINE_VARIABLE constexpr auto object = ':';
        MINIYAML_INLINE_VARIABLE constexpr auto object_start = '{';
        MINIYAML_INLINE_VARIABLE constexpr auto object_end = '}';
        MINIYAML_INLINE_VARIABLE constexpr auto sequence = '-';
        MINIYAML_INLINE_VARIABLE constexpr auto sequence_start = '[';
        MINIYAML_INLINE_VARIABLE constexpr auto sequence_end = ']';
        MINIYAML_INLINE_VARIABLE constexpr auto null = '~';
        MINIYAML_INLINE_VARIABLE constexpr auto literal = '|';
        MINIYAML_INLINE_VARIABLE constexpr auto folded = '>';
    }
}


namespace MINIYAML_NAMESPACE {
namespace sax {

    /** Helper function for parsing via SAX style API. */
    template<typename Tchar, typename Tsax_handler>
    parse_result_code parse(
        const Tchar* raw_input,
        size_t size,
        Tsax_handler& handler);

    template<typename Tchar, typename Tsax_handler>
    parse_result_code parse(
        const std::basic_string<Tchar>& string,
        Tsax_handler& handler);

    template<typename Tchar, typename Tsax_handler>
    parse_result_code parse(
        basic_string_view<Tchar> string_view,
        Tsax_handler& handler);

    template<typename Tsax_handler, typename Tit_begin, typename Tit_end>
    parse_result_code parse(
        Tit_begin first,
        Tit_end last,
        Tsax_handler& handler);


    /** SAX style parser. */
    template<typename Tchar, typename Tsax_handler>
    class parser {

    public:

        using value_type = Tchar;
        using pointer = Tchar*;
        using const_pointer = const Tchar*;
        using size_type = size_t;
        using string_view_type = basic_string_view<Tchar>;
        using sax_handler_type = Tsax_handler;

        parser(sax_handler_type& sax_handler);

        parse_result_code execute(const_pointer raw_input, size_type size);

        parse_result_code execute(std::basic_string<value_type> string);

        parse_result_code execute(string_view_type string_view);

        template<typename Tit_begin, typename Tit_end>
        parse_result_code execute(Tit_begin first, Tit_end last);

    private:

        using state_function_t = void (parser::*)();

        enum class stack_type_t {
            unknown,
            object,
            list,
            scalar
        };

        struct stack_item_t {

            stack_item_t() = default;
            stack_item_t(const stack_item_t&) = delete;
            stack_item_t(stack_item_t&&) = default;
            stack_item_t& operator = (const stack_item_t&) = delete;         
            stack_item_t& operator = (stack_item_t&&) = default;

            state_function_t state_function = nullptr;
            int64_t min_indention = 0;
            stack_type_t type = stack_type_t::unknown;
        };  

        using stack_t = std::vector<stack_item_t>;
        using stack_iterator_t = typename stack_t::iterator;
        using stack_reverse_iterator_t = typename stack_t::reverse_iterator;

        const_pointer m_current_ptr;
        const_pointer m_end_ptr;
        const_pointer m_begin_ptr;
        stack_t m_stack;
        parse_result_code m_current_result_code;
        sax_handler_type& m_sax_handler;
        int64_t m_current_line;
        const_pointer m_current_line_ptr;
        int64_t m_current_line_indention;
        const_pointer m_current_line_indention_ptr;
        const_pointer m_current_value_start_ptr; 
        const_pointer m_current_value_end_ptr;

        static const_pointer skip_utf8_bom(const_pointer begin, const_pointer end);

        void register_newline();
        void register_line_indentation();

        void execute_find_value();
        void execute_find_unknown();
        void execute_find_unknown_potential_key();
        void execute_find_scalar();
        void execute_find_key();
        void execute_parse_comment();

        parse_result_code consume_whitespaces_after_key();
        string_view_type get_current_value_string_view() const;

        void process_scalar();
        void process_key();
        void process_comment();

        void signal_start_object();
        void signal_end_object();
        void signal_start_array();
        void signal_end_array();
        void signal_key(string_view_type value);
        void signal_null();
        void signal_string(string_view_type value);
        void signal_comment(string_view_type value);

        void error(const parse_result_code result_code);

        MINIYAML_NODISCARD stack_item_t& get_stack();
        MINIYAML_NODISCARD stack_iterator_t get_stack_iterator();
        stack_item_t& push_stack(state_function_t state_function);
        void pop_stack();
        void pop_stack_from(stack_iterator_t it);
        void pop_stack_from_indention(int64_t indention);

    };


    /** Private implementation detials. */
    namespace impl {
        template<typename Tsax_handler> constexpr bool sax_handler_has_start_object();
        template<typename Tsax_handler> constexpr bool sax_handler_has_end_object();
        template<typename Tsax_handler> constexpr bool sax_handler_has_start_array();
        template<typename Tsax_handler> constexpr bool sax_handler_has_end_array();
        template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_key();
        template<typename Tsax_handler> constexpr bool sax_handler_has_null();
        template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_string();
        template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_comment();
    }

} }


// Implementations.
namespace MINIYAML_NAMESPACE {
namespace sax {

    namespace impl {

#if MINIYAML_HAS_CONCEPTS
        template<typename Tsax_handler>
        constexpr bool sax_handler_has_start_object() {
            return requires(Tsax_handler value) { { value.start_object() }; };
        }
        template<typename Tsax_handler>
        constexpr bool sax_handler_has_end_object() {
            return requires(Tsax_handler value) { { value.end_object() }; };
        }
        template<typename Tsax_handler>
        constexpr bool sax_handler_has_start_array() {
            return requires(Tsax_handler value) { { value.start_array() }; };
        }
        template<typename Tsax_handler>
        constexpr bool sax_handler_has_end_array() {
            return requires(Tsax_handler value) { { value.end_array() }; };
        }
        template<typename Tchar, typename Tsax_handler>
        constexpr bool sax_handler_has_key() {
            return requires(Tsax_handler handler) { { handler.key(typename parser<Tchar, Tsax_handler>::string_view_type{}) }; };
        }
        template<typename Tsax_handler>
        constexpr bool sax_handler_has_null() {
            return requires(Tsax_handler value) { { value.null() }; };
        }
        template<typename Tchar, typename Tsax_handler>
        constexpr bool sax_handler_has_string() {
            return requires(Tsax_handler handler) { { handler.string(typename parser<Tchar, Tsax_handler>::string_view_type{}) }; };
        }
        template<typename Tchar, typename Tsax_handler>
        constexpr bool sax_handler_has_comment() {
            return requires(Tsax_handler handler) { { handler.comment(typename parser<Tchar, Tsax_handler>::string_view_type{}) }; };
        }
#else
        template<typename Tsax_handler> constexpr bool sax_handler_has_start_object() {
            return true;
        }
        template<typename Tsax_handler> constexpr bool sax_handler_has_end_object() {
            return true;
        }
        template<typename Tsax_handler> constexpr bool sax_handler_has_start_array() {
            return true;
        }
        template<typename Tsax_handler> constexpr bool sax_handler_has_end_array() {
            return true;
        }
        template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_key() {
            return true;
        }
        template<typename Tsax_handler> constexpr bool sax_handler_has_null() {
            return true;
        }
        template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_string() {
            return true;
        }
        template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_comment() {
            return true;
        }
#endif

    }

    template<typename Tchar, typename Tsax_handler>
    parser<Tchar, Tsax_handler>::parser(sax_handler_type& sax_handler) :
        m_current_ptr(nullptr),
        m_end_ptr(nullptr),
        m_begin_ptr(nullptr),
        m_stack{},
        m_current_result_code(parse_result_code::success),
        m_sax_handler(sax_handler),
        m_current_line(0),
        m_current_line_ptr(nullptr),
        m_current_line_indention(0),
        m_current_line_indention_ptr(nullptr),
        m_current_value_start_ptr(nullptr),
        m_current_value_end_ptr(nullptr)
    {}

    template<typename Tchar, typename Tsax_handler>
    parse_result_code parser<Tchar, Tsax_handler>::execute(const_pointer raw_input, size_type size) {
        m_end_ptr = raw_input + size;
        m_begin_ptr = skip_utf8_bom(raw_input, m_end_ptr);
        m_current_ptr = m_begin_ptr;
        m_stack.clear();
        m_current_result_code = parse_result_code::success;

        m_current_line = 0;
        m_current_line_ptr = m_begin_ptr;
        m_current_line_indention = 0;
        m_current_line_indention_ptr = m_begin_ptr;
        m_current_value_start_ptr = nullptr;
        m_current_value_end_ptr = nullptr;

        push_stack(&parser::execute_find_value);

        while (m_current_result_code == parse_result_code::success && m_current_ptr < m_end_ptr && !m_stack.empty()) {
            auto& stack_item = m_stack.back();
            auto state_function = stack_item.state_function;
            (this->*(state_function))();
        }

        if (m_current_result_code == parse_result_code::success) {
            pop_stack_from(m_stack.begin());
        }

        return m_current_result_code;
    }

    template<typename Tchar, typename Tsax_handler>
    parse_result_code parser<Tchar, Tsax_handler>::execute(std::basic_string<value_type> string) {
        return execute(string.c_str(), string.size());
    }

    template<typename Tchar, typename Tsax_handler>
    parse_result_code parser<Tchar, Tsax_handler>::execute(string_view_type string_view) {
        return execute(string_view.data(), string_view.size());
    }

    template<typename Tchar, typename Tsax_handler>
    template<typename Tit_begin, typename Tit_end>
    parse_result_code parser<Tchar, Tsax_handler>::execute(Tit_begin first, Tit_end last) {
        auto raw_input = &*first;
        auto size = static_cast<size_t>(std::distance(first, last));
        return execute(raw_input, size);
    }

    template<typename Tchar, typename Tsax_handler>
    typename parser<Tchar, Tsax_handler>::const_pointer parser<Tchar, Tsax_handler>::skip_utf8_bom(const_pointer begin, const_pointer end) {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (sizeof(Tchar) == 1) {
#else
        // This is a stupid workaround for a MSVC bug, caused when compiling using C++14.
        // "if constexpr" is expected even if __cpp_if_constexpr isn't defined,
        //  if the expression "sizeof(Tchar) == 1" is used directly in if statement.
        const auto char_size_test = sizeof(Tchar) == 1;
        if (char_size_test) {
#endif
            const auto data_length = static_cast<size_t>(end - begin);
            const std::array<uint8_t, 3> bom_chars = { 0xEF, 0xBB, 0xBF };

            if (data_length >= 3 && 
                static_cast<uint8_t>(begin[0]) == bom_chars[0] &&
                static_cast<uint8_t>(begin[1]) == bom_chars[1] &&
                static_cast<uint8_t>(begin[2]) == bom_chars[2])
            {
                return begin + 3;
            }
        }
        
        return begin;
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::register_newline() {
    
        if (m_current_ptr > m_begin_ptr && *(m_current_ptr - 1) == '\r') { // DOS line break check.
            if (m_current_ptr < m_end_ptr && *m_current_ptr == '\n') {
                ++m_current_ptr;
            }
        }

        m_current_line += 1;
        m_current_line_ptr = m_current_ptr;
        m_current_line_indention = 0;
        m_current_line_indention_ptr = m_current_ptr;
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::register_line_indentation() {
        ++m_current_line_indention;
        m_current_line_indention_ptr = m_current_ptr;
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_find_value() {
        bool found_tab_on_current_line = false;
        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token::carriage:
                case token::newline: {
                    register_newline();
                    found_tab_on_current_line = false;
                } break;
                case token::space: register_line_indentation(); break;
                case token::tab: found_tab_on_current_line = true; break;
                case token::comment: push_stack(&parser::execute_parse_comment); return;
                case token::object: return error(parse_result_code::missing_key);
                case token::sequence: return error(parse_result_code::not_implemented);
                case token::null: return error(parse_result_code::not_implemented);
                case token::object_start: return error(parse_result_code::not_implemented);     // json
                case token::object_end: return error(parse_result_code::not_implemented);       // json
                case token::sequence_start: return error(parse_result_code::not_implemented);   // json
                case token::sequence_end: return error(parse_result_code::not_implemented);     // json
                case token::quote: return error(parse_result_code::not_implemented);
                case token::single_quote: return error(parse_result_code::not_implemented);
                default: {
                    if (found_tab_on_current_line) {
                        return error(parse_result_code::forbidden_tab_indentation);
                    }

                    pop_stack_from_indention(m_current_line_indention);
                    if (m_stack.empty()) {
                        return error(parse_result_code::unexpected_token);
                    }

                    m_current_ptr -= 1;
                    m_current_value_start_ptr = m_current_ptr;
                    m_current_value_end_ptr = m_current_value_start_ptr;

                    auto& stack = get_stack();
                    switch (stack.type) {
                        case stack_type_t::unknown: stack.state_function = &parser::execute_find_unknown; return;
                        case stack_type_t::scalar: stack.state_function = &parser::execute_find_scalar; return;
                        case stack_type_t::object: {
                            if (m_current_line_indention > stack.min_indention) {
                                return error(parse_result_code::bad_key_indentation);
                            }
                            push_stack(&parser::execute_find_key);
                        } return;
                        case stack_type_t::list: return error(parse_result_code::not_implemented);
                    }
                    
                } return;
            }
        }
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_find_unknown() {
        auto& stack = get_stack();
        
        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token::carriage:
                case token::newline: {
                    stack.type = stack_type_t::scalar;
                    process_scalar();
                    m_current_ptr -= 1;
                    stack.state_function = &parser::execute_find_value;
                } return;
                case token::space:
                case token::tab: break;
                case token::comment: {
                    process_scalar();
                    push_stack(&parser::execute_parse_comment);
                } return;
                case token::object: {
                    stack.state_function = &parser::execute_find_unknown_potential_key;
                } return;
                case token::literal: return error(parse_result_code::not_implemented);
                case token::folded: return error(parse_result_code::not_implemented);
                case token::sequence: return error(parse_result_code::not_implemented);
                case token::null: return error(parse_result_code::not_implemented);
                case token::object_start: return error(parse_result_code::not_implemented);     // json
                case token::object_end: return error(parse_result_code::not_implemented);       // json
                case token::sequence_start: return error(parse_result_code::not_implemented);   // json
                case token::sequence_end: return error(parse_result_code::not_implemented);     // json
                case token::quote: return error(parse_result_code::not_implemented);
                case token::single_quote: return error(parse_result_code::not_implemented);
                default: m_current_value_end_ptr = m_current_ptr; break;
            }
        }
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_find_unknown_potential_key() {
        auto& stack = get_stack();
        
        auto unknown_to_object_func = [&]() {
            stack.type = stack_type_t::object;
            stack.min_indention = m_current_line_indention;
            signal_start_object();
            process_key();
            push_stack(&parser::execute_find_value);
        };

        if (m_current_ptr < m_end_ptr) {
            const auto peek_codepoint = *m_current_ptr;
            switch (peek_codepoint) {
                case token::space:
                case token::tab: unknown_to_object_func(); break;
                case token::carriage:
                case token::newline: unknown_to_object_func(); return;
                default: stack.state_function = &parser::execute_find_unknown; return;
            }

            consume_whitespaces_after_key();
            return;
        }

        // Out of buffer, so this was a key with null as value.
        unknown_to_object_func();
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_find_scalar() {
        auto& stack = get_stack();

        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token::carriage:
                case token::newline: {
                    process_scalar();
                    m_current_ptr -= 1;
                    stack.state_function = &parser::execute_find_value;
                } return;
                case token::space:
                case token::tab: break;
                case token::comment: {
                    process_scalar();
                    push_stack(&parser::execute_parse_comment);
                } return;
                case token::object: return error(parse_result_code::unexpected_key);
                case token::literal: return error(parse_result_code::not_implemented);
                case token::folded: return error(parse_result_code::not_implemented);
                case token::sequence: return error(parse_result_code::not_implemented);
                case token::null: return error(parse_result_code::not_implemented);
                case token::object_start: return error(parse_result_code::not_implemented);     // json
                case token::object_end: return error(parse_result_code::not_implemented);       // json
                case token::sequence_start: return error(parse_result_code::not_implemented);   // json
                case token::sequence_end: return error(parse_result_code::not_implemented);     // json
                case token::quote: return error(parse_result_code::not_implemented);
                case token::single_quote: return error(parse_result_code::not_implemented);
                default: m_current_value_end_ptr = m_current_ptr; break;
            }
        }
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_find_key() {
        auto& stack = get_stack();

        auto process_expected_whitespace = [&]() {
            auto new_key_func = [&]() {
                process_key();
                stack.state_function = &parser::execute_find_value;
            };

            const auto peek_codepoint = *m_current_ptr;
            switch (peek_codepoint) {
                case token::space:
                case token::tab: new_key_func(); return true;
                case token::carriage:
                case token::newline: new_key_func(); return true;
                default: break;
            }
            
            return false;
        };
        
        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token::carriage:
                case token::newline: return error(parse_result_code::missing_key);
                case token::space:
                case token::tab: break;
                case token::comment: return error(parse_result_code::missing_key);
                case token::object: {
                    if (m_current_ptr >= m_end_ptr) {
                        return error(parse_result_code::missing_key);
                    }

                    if (!process_expected_whitespace()) {
                        break;
                    }
                    
                    consume_whitespaces_after_key();
                } return;
                default: m_current_value_end_ptr = m_current_ptr; break;
            }
        }

        return error(parse_result_code::missing_key);
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_parse_comment() {

        [&]() {
            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *(m_current_ptr++);
                switch (codepoint) {
                    case token::space:
                    case token::tab: break;
                    case token::carriage:
                    case token::newline:
                    default: --m_current_ptr; return;
                }
            }
        }();
     
        m_current_value_start_ptr = m_current_ptr;
        m_current_value_end_ptr = m_current_value_start_ptr;

        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token::space:
                case token::tab: break;
                case token::carriage:
                case token::newline: {
                    m_current_ptr -= 1;
                    process_comment();
                    pop_stack();
                } return;
                default: m_current_value_end_ptr = m_current_ptr; break;
            }
        }

        process_comment();
        pop_stack();
    }

    template<typename Tchar, typename Tsax_handler>
    parse_result_code parser<Tchar, Tsax_handler>::consume_whitespaces_after_key() {
        
        auto result = [&]() {
            auto& current_stack = get_stack();
            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *(m_current_ptr++);
                switch (codepoint) {
                    case token::space:
                    case token::tab: break;
                    case token::carriage:
                    case token::newline: {
                        m_current_ptr -= 1;
                        current_stack.min_indention += 1;
                    } return parse_result_code::success;
                    default: {
                        m_current_ptr -= 2;
                        if (m_current_ptr < m_begin_ptr) {
                            return parse_result_code::internal_error;
                        }
                        current_stack.min_indention += 1;
                    } return parse_result_code::success;
                }
            }
            return parse_result_code::success;
        }();
        
        if (result != parse_result_code::success) {
            error(result);
        }
        
        return result;
    }

    template<typename Tchar, typename Tsax_handler>
    typename parser<Tchar, Tsax_handler>::string_view_type parser<Tchar, Tsax_handler>::get_current_value_string_view() const {
        const auto value_length = static_cast<size_t>(m_current_value_end_ptr - m_current_value_start_ptr);
        return string_view_type{ m_current_value_start_ptr, value_length };
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::process_scalar() {
        const auto value = get_current_value_string_view();
        if (!value.empty()) {
            signal_string(value);
        }
        m_current_value_start_ptr = nullptr;
        m_current_value_end_ptr = nullptr;
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::process_key() {
        const auto value = get_current_value_string_view();
        if (!value.empty()) {
            signal_key(value);
        }
        m_current_value_start_ptr = nullptr;
        m_current_value_end_ptr = nullptr;
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::process_comment() {
        const auto value = get_current_value_string_view();
        if (!value.empty()) {
            signal_comment(value);
        }
        m_current_value_start_ptr = nullptr;
        m_current_value_end_ptr = nullptr;
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::signal_start_object() {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_start_object<Tsax_handler>() == true) {
            m_sax_handler.start_object();
        }       
#else
        m_sax_handler.start_object();
#endif      
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::signal_end_object() {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_end_object<Tsax_handler>() == true) {
            m_sax_handler.end_object();
        }
#else
        m_sax_handler.end_object();
#endif  
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::signal_start_array() {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_start_array<Tsax_handler>() == true) {
            m_sax_handler.start_array();
        }
#else
        m_sax_handler.start_array();
#endif  
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::signal_end_array() {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_end_array<Tsax_handler>() == true) {
            m_sax_handler.end_array();
        }
#else
        m_sax_handler.end_array();
#endif 
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::signal_key(string_view_type value) {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_key<Tchar, Tsax_handler>() == true) {
            m_sax_handler.key(value);
        }
#else
        m_sax_handler.key(value);
#endif 
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::signal_null() {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_null<Tsax_handler>() == true) {
            m_sax_handler.null();
        }
#else
        m_sax_handler.null();
#endif 
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::signal_string(string_view_type value) {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_string<Tchar, Tsax_handler>() == true) {
            m_sax_handler.string(value);
        }
#else
        m_sax_handler.string(value);
#endif 
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::signal_comment(string_view_type value) {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_comment<Tchar, Tsax_handler>() == true) {
            m_sax_handler.comment(value);
        }
#else
        m_sax_handler.comment(value);
#endif 
    }


    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::error(const parse_result_code result_code) {
        m_current_result_code = result_code;
    }

    template<typename Tchar, typename Tsax_handler>
    typename parser<Tchar, Tsax_handler>::stack_item_t& parser<Tchar, Tsax_handler>::get_stack() {
        return m_stack.back();
    }

    template<typename Tchar, typename Tsax_handler>
    typename parser<Tchar, Tsax_handler>::stack_iterator_t parser<Tchar, Tsax_handler>::get_stack_iterator() {
        return m_stack.size() > 1 ? static_cast<stack_iterator_t>(std::prev(m_stack.end())) : m_stack.begin();
    }

    template<typename Tchar, typename Tsax_handler>
    typename parser<Tchar, Tsax_handler>::stack_item_t& parser<Tchar, Tsax_handler>::push_stack(state_function_t state_function) {

        const auto min_indention = m_stack.empty() ? size_t{ 0 } : m_stack.back().min_indention;

        m_stack.emplace_back();
        auto& new_stack = m_stack.back();
        new_stack.state_function = state_function;
        new_stack.min_indention = min_indention;
        return new_stack;
    };

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::pop_stack() {
        m_stack.pop_back();
    };

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::pop_stack_from(stack_iterator_t it) {
        const auto rit_end = stack_reverse_iterator_t(it);
        for (auto rit = m_stack.rbegin(); rit != rit_end; ++rit) {
            if (rit->type == stack_type_t::scalar) {
                process_scalar();
            }
            else if (rit->type == stack_type_t::object) {
                signal_end_object();
            }
            else if (rit->type == stack_type_t::unknown) {
                const auto unknown_length = static_cast<size_t>(m_current_value_end_ptr - m_current_value_start_ptr);
                if (unknown_length == 0 || (unknown_length == 1 && (*m_current_value_start_ptr) == '~')) {
                    signal_null();
                }
                else{
                    const auto string_value = string_view_type{ m_current_value_start_ptr, unknown_length };
                    signal_string(string_value);
                }
            }
        }

        m_stack.erase(it, m_stack.end());
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::pop_stack_from_indention(int64_t indention) {
        auto it = m_stack.begin();
        for (; it != m_stack.end(); ++it) {
            if (it->min_indention > indention) {
                break;
            }
        }

        if (it == m_stack.end()) {
            return;
        }

        pop_stack_from(it);
    }

    template<typename Tchar, typename Tsax_handler>
    parse_result_code parse(
        const Tchar* raw_input,
        size_t size,
        Tsax_handler& handler)
    {
        return parser<Tchar, Tsax_handler>{ handler }.execute(raw_input, size);
    }

    template<typename Tchar, typename Tsax_handler>
    parse_result_code parse(
        const std::basic_string<Tchar>& string,
        Tsax_handler& handler)
    {
        return parser<Tchar, Tsax_handler>{ handler }.execute(string);
    }

    template<typename Tchar, typename Tsax_handler>
    parse_result_code parse(
        basic_string_view<Tchar> string_view,
        Tsax_handler& handler)
    {
        return parser<Tchar, Tsax_handler>{ handler }.execute(string_view);
    }

    template<typename Tsax_handler, typename Tit_begin, typename Tit_end>
    parse_result_code parse(
        Tit_begin first,
        Tit_end last,
        Tsax_handler& handler)
    {
        using begin_char_type = typename std::iterator_traits<Tit_begin>::value_type;
        using end_char_type = typename std::iterator_traits<Tit_end>::value_type;

        static_assert(std::is_same<begin_char_type, end_char_type>::value, 
            "Mismatching iterator value types for sax::parse(first, last, handler).");

        return parser<begin_char_type, Tsax_handler>{ handler }.execute(first, last);
    }

} }