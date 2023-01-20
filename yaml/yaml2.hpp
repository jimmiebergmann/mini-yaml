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

#include <memory>
#include <array>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
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
        internal_error,
        not_implemented,
        forbidden_tab_indentation,
        bad_indentation,
        expected_line_break,
        unexpected_key
    };

    template<typename T>
    struct token {
        MINIYAML_INLINE_VARIABLE constexpr static T eof = '\0';
        MINIYAML_INLINE_VARIABLE constexpr static T space = ' ';
        MINIYAML_INLINE_VARIABLE constexpr static T tab = '\t';
        MINIYAML_INLINE_VARIABLE constexpr static T carriage = '\r';
        MINIYAML_INLINE_VARIABLE constexpr static T newline = '\n';
        MINIYAML_INLINE_VARIABLE constexpr static T comment = '#';
        MINIYAML_INLINE_VARIABLE constexpr static T quote = '\"';
        MINIYAML_INLINE_VARIABLE constexpr static T single_quote = '\'';
        MINIYAML_INLINE_VARIABLE constexpr static T object = ':';
        MINIYAML_INLINE_VARIABLE constexpr static T object_start = '{';
        MINIYAML_INLINE_VARIABLE constexpr static T object_end = '}';
        MINIYAML_INLINE_VARIABLE constexpr static T sequence = '-';
        MINIYAML_INLINE_VARIABLE constexpr static T sequence_start = '[';
        MINIYAML_INLINE_VARIABLE constexpr static T sequence_end = ']';
        MINIYAML_INLINE_VARIABLE constexpr static T null = '~';
        MINIYAML_INLINE_VARIABLE constexpr static T literal_block = '|';
        MINIYAML_INLINE_VARIABLE constexpr static T folded_block = '>';
        MINIYAML_INLINE_VARIABLE constexpr static T chomping_strip = '-';
        MINIYAML_INLINE_VARIABLE constexpr static T chomping_keep = '+';
    };

    enum class block_style {
        none,       ///< ' '
        literal,    ///< '>' - Keep newlines.
        folded,     ///< '|' - Replace newlines with spaces.
    };

    enum class chomping {
        clip,   ///< ' ' - Single newline at end.
        strip,  ///< '-' - No newline at end
        keep,   ///< '+' - All newlines from end
    };
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


    /** Helper base struct for sax handler. */
    struct handler_base {
        virtual void null() {}
        virtual void start_scalar(block_style, chomping) {}
        virtual void end_scalar() {}
        virtual void start_object() {}
        virtual void end_object() {}
        virtual void start_array() {}
        virtual void end_array() {}
        virtual void string(basic_string_view<uint8_t>) {}
        virtual void key(basic_string_view<uint8_t>) {}
        virtual void comment(basic_string_view<uint8_t>) {}
    };


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
        using token_type = token<Tchar>;

        parser(sax_handler_type& sax_handler);

        parse_result_code execute(const_pointer raw_input, size_type size);

        parse_result_code execute(std::basic_string<value_type> string);

        parse_result_code execute(string_view_type string_view);

        template<typename Tit_begin, typename Tit_end>
        parse_result_code execute(Tit_begin first, Tit_end last);

    private:

        //using state_function_t = void (parser::*)();

        enum class stack_type_t {
            unknown,
            scalar,
            object,
            sequence
        };

        struct stack_item_t {
            stack_item_t() = default;
            ~stack_item_t() = default;          
            stack_item_t(stack_item_t&&) = default;               
            stack_item_t& operator = (stack_item_t&&) = default;
            stack_item_t(const stack_item_t&) = delete;
            stack_item_t& operator = (const stack_item_t&) = delete;

            //state_function_t state_function = nullptr; // Obsolete?
            stack_type_t type = stack_type_t::unknown;
            int64_t type_indention = 0;
            int64_t processed_lines = 0;
            block_style block_style = block_style::none;
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
        bool m_current_is_new_line;

        static const_pointer skip_utf8_bom(const_pointer begin, const_pointer end);

        void register_newline();
        void register_line_indentation();

        void execute_find_value();   
        bool consume_only_whitespaces_until_newline();

        void signal_start_scalar(yaml::block_style block_style, yaml::chomping comping);
        void signal_end_scalar();
        void signal_start_object();
        void signal_end_object();
        void signal_start_array();
        void signal_end_array();
        void signal_null();
        void signal_string(string_view_type value);
        void signal_key(string_view_type value);
        void signal_comment(string_view_type value);

        void error(const parse_result_code result_code);

        MINIYAML_NODISCARD stack_item_t& get_stack();
        stack_item_t& push_stack(/*state_function_t state_function*/);
        void pop_stack_from(stack_iterator_t it);

    };


    /** Private implementation detials. */
    namespace impl {
        template<typename Tsax_handler> constexpr bool sax_handler_has_start_scalar();
        template<typename Tsax_handler> constexpr bool sax_handler_has_end_scalar();
        template<typename Tsax_handler> constexpr bool sax_handler_has_start_object();
        template<typename Tsax_handler> constexpr bool sax_handler_has_end_object();
        template<typename Tsax_handler> constexpr bool sax_handler_has_start_array();
        template<typename Tsax_handler> constexpr bool sax_handler_has_end_array();
        template<typename Tsax_handler> constexpr bool sax_handler_has_null();
        template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_string();
        template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_key();      
        template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_comment();
    }

} }


// Implementations.
namespace MINIYAML_NAMESPACE {
namespace sax {

    namespace impl {

#if MINIYAML_HAS_CONCEPTS
        template<typename Tsax_handler>
        constexpr bool sax_handler_has_start_scalar() {
            return requires(Tsax_handler value) { { value.start_scalar(yaml::block_style::none, yaml::chomping::strip) }; };
        }
        template<typename Tsax_handler>
        constexpr bool sax_handler_has_end_scalar() {
            return requires(Tsax_handler value) { { value.end_scalar() }; };
        }
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
        template<typename Tsax_handler>
        constexpr bool sax_handler_has_null() {
            return requires(Tsax_handler value) { { value.null() }; };
        }
        template<typename Tchar, typename Tsax_handler>
        constexpr bool sax_handler_has_string() {
            return requires(Tsax_handler handler) { { handler.string(typename parser<Tchar, Tsax_handler>::string_view_type{}) }; };
        }
        template<typename Tchar, typename Tsax_handler>
        constexpr bool sax_handler_has_key() {
            return requires(Tsax_handler handler) { { handler.key(typename parser<Tchar, Tsax_handler>::string_view_type{}) }; };
        }
        template<typename Tchar, typename Tsax_handler>
        constexpr bool sax_handler_has_comment() {
            return requires(Tsax_handler handler) { { handler.comment(typename parser<Tchar, Tsax_handler>::string_view_type{}) }; };
        }
#else
        template<typename Tsax_handler> constexpr bool sax_handler_has_start_scalar() {
            return true;
        }
        template<typename Tsax_handler> constexpr bool sax_handler_has_end_scalar() {
            return true;
        }
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
        template<typename Tsax_handler> constexpr bool sax_handler_has_null() {
            return true;
        }
        template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_string() {
            return true;
        }
        template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_key() {
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
        m_current_is_new_line(true)
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
        m_current_is_new_line = true;
        
        //push_stack(&parser::execute_find_value);
        push_stack();

        auto current_is_newline = [&]() {
            return m_current_ptr < m_end_ptr && (*m_current_ptr == token_type::newline || *m_current_ptr == token_type::carriage);
        };

        auto read_line_indentation = [&]() {
            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *m_current_ptr;
                if (codepoint == token_type::tab) {
                    return false;
                }
                if (codepoint != token_type::space) {
                    return true;
                }
                
                ++m_current_ptr;
            }

            return true;
        };

        auto read_newline_indentation = [&]() {
            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *m_current_ptr;
                if (codepoint == token_type::tab) {
                    return false;
                }
                if (codepoint != token_type::space) {
                    return true;
                }

                register_line_indentation();
                ++m_current_ptr;
            }

            return true;
        };

        auto process_newline_indentation = [&]() {      
            if (current_is_newline()) {
                return;
            }
            
            auto it = std::find_if(m_stack.begin(), m_stack.end(), 
                [&](const stack_item_t& stack_item) {
                    return stack_item.type_indention > m_current_line_indention;
                }
            );

            if (it == m_stack.end()) {
                return;
            }

            pop_stack_from(it);

            const auto back_indention = !m_stack.empty() ? m_stack.back().type_indention : std::numeric_limits<int64_t>::max();
            if (m_current_line_indention != back_indention) {
                return error(parse_result_code::bad_indentation);
            }
        };

        while (m_current_result_code == parse_result_code::success && m_current_ptr < m_end_ptr && !m_stack.empty()) {
            if (m_current_is_new_line) {
                if (!read_newline_indentation()) {
                    error(parse_result_code::forbidden_tab_indentation);
                    return m_current_result_code;
                }
                process_newline_indentation();
            }
            else {
                if (!read_line_indentation()) {
                    error(parse_result_code::forbidden_tab_indentation);
                    return m_current_result_code;
                }
            }

            if (m_stack.empty() || m_current_result_code != parse_result_code::success) {
                break;
            }
            
            execute_find_value();

            //auto& stack_item = m_stack.back();
            //auto state_function = stack_item.state_function;
            //(this->*(state_function))();
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
        if (m_current_ptr > m_begin_ptr && *(m_current_ptr - 1) == token_type::carriage) { // DOS line break check.
            if (m_current_ptr < m_end_ptr && *m_current_ptr == token_type::newline) {
                ++m_current_ptr;
            }
        }

        m_current_line += 1;
        m_current_line_ptr = m_current_ptr;
        m_current_line_indention = 0;
        m_current_line_indention_ptr = m_current_ptr;
        m_current_is_new_line = true;
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::register_line_indentation() {
        ++m_current_line_indention;
        ++m_current_line_indention_ptr;
    }


    // State execute functions.
    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_find_value() {
        m_current_is_new_line = false;

        if (m_current_ptr >= m_end_ptr) {
            return;
        }

        const auto current_line_indention = m_current_line_indention;
        auto value_type = stack_type_t::unknown;
        auto value_start_ptr = m_current_ptr;
        auto value_end_ptr = m_current_ptr;
        auto block_style = block_style::none;
        auto chomping_type = chomping::strip;

        auto parse_value_type = [&]() {

            const auto first_codepoint = *(m_current_ptr++);
            switch (first_codepoint) {
                case token_type::carriage:
                case token_type::newline: return register_newline();
                case token_type::literal_block: {
                    value_type = stack_type_t::scalar;
                    value_end_ptr = m_current_ptr;
                    block_style = block_style::literal;
                    if (!consume_only_whitespaces_until_newline()) {
                        return error(parse_result_code::expected_line_break);
                    }
                    value_type = stack_type_t::scalar;
                } return;
                default: {
                    value_type = stack_type_t::scalar; 
                    value_end_ptr = m_current_ptr;
                } break;
            }

            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *(m_current_ptr++);
                switch (codepoint) {
                    case token_type::space:
                    case token_type::tab: break;
                    case token_type::carriage:
                    case token_type::newline: return register_newline();
                    case token_type::object: {
                        const auto next_codepoint = m_current_ptr < m_end_ptr ? *m_current_ptr : token_type::eof;
                        switch (next_codepoint) {
                            case token_type::eof:
                            case token_type::space:
                            case token_type::tab:
                            case token_type::carriage:
                            case token_type::newline: {
                                value_type = stack_type_t::object;
                            } return;
                            default: break;
                        }
                    } break;
                    default: {
                        value_end_ptr = m_current_ptr;
                    } break;
                }
            }
        };
        
        auto process_value_type = [&]() {
            auto& stack_item = get_stack();

            switch (value_type) {
                case stack_type_t::unknown: {
                    if (stack_item.type == stack_type_t::scalar && stack_item.block_style != block_style::none) {
                        const auto scalar_length = static_cast<size_t>(value_end_ptr - value_start_ptr);
                        
                        if (scalar_length == 0) {
                            if (stack_item.processed_lines > 0) {
                                ++stack_item.processed_lines;
                            }

                            const auto left_padding = std::max(current_line_indention - stack_item.type_indention, int64_t{0});
                            const auto scalar_string_view = string_view_type{ value_start_ptr - left_padding, scalar_length + left_padding };
                            signal_string(scalar_string_view);
                        }
                        else {
                            return error(parse_result_code::internal_error);
                        }
                    }
                } break;
                case stack_type_t::scalar: {
                    if (stack_item.type == stack_type_t::unknown) {
                        stack_item.type = stack_type_t::scalar;
                        stack_item.block_style = block_style;
                        signal_start_scalar(block_style, chomping_type);

                        if (block_style != block_style::none) {
                            break;
                        }
                    }
                    else if (stack_item.type != stack_type_t::scalar) {
                        return error(parse_result_code::internal_error);
                    }
       
                    if (stack_item.block_style == block_style::none) {
                        const auto scalar_length = static_cast<size_t>(value_end_ptr - value_start_ptr);
                        const auto scalar_string_view = string_view_type{ value_start_ptr, scalar_length };
                        signal_string(scalar_string_view);
                    }
                    else {
                        if (stack_item.processed_lines == 0) {
                            stack_item.type_indention = current_line_indention;
                        }
                        ++stack_item.processed_lines;

                        const auto left_padding = current_line_indention - stack_item.type_indention;
                        const auto scalar_length = static_cast<size_t>(value_end_ptr - value_start_ptr + left_padding);
                        const auto scalar_string_view = string_view_type{ value_start_ptr - left_padding, scalar_length };
                        signal_string(scalar_string_view);
                    }
                } break;
                case stack_type_t::object: {
                    if (value_start_ptr != m_current_line_indention_ptr) {
                        return error(parse_result_code::unexpected_key);
                    }

                    if (stack_item.type == stack_type_t::unknown) {
                        stack_item.type = stack_type_t::object;
                        stack_item.type_indention = m_current_line_indention;
                        signal_start_object();
                    }
                    else if (stack_item.type != stack_type_t::object) {
                        return error(parse_result_code::internal_error);
                    }

                    const auto key_length = static_cast<size_t>(value_end_ptr - value_start_ptr);
                    const auto key_string_view = string_view_type{ value_start_ptr, key_length };
                    signal_key(key_string_view);

                    push_stack();

                } break;
                case stack_type_t::sequence: return error(parse_result_code::not_implemented);
            }
        };

        auto process_remaining_value = [&]() {
            if (m_current_is_new_line) {
                return;
            }

            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *(m_current_ptr++);
                switch (codepoint) {
                    case token_type::space:
                    case token_type::tab: break;
                    case token_type::carriage:
                    case token_type::newline: return register_newline();
                    default: --m_current_ptr; return;
                }
            }
        };

        parse_value_type();
        if (m_current_result_code != parse_result_code::success) {
            return;
        }
        process_value_type();
        if (m_current_result_code != parse_result_code::success) {
            return;
        }
        process_remaining_value();
    }

    template<typename Tchar, typename Tsax_handler>
    bool parser<Tchar, Tsax_handler>::consume_only_whitespaces_until_newline() {
        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token_type::space:
                case token_type::tab: break;
                case token_type::carriage:
                case token_type::newline: register_newline(); return true;
                default: return false;
            }
        }
        return true;
    }

    /*template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_parse_comment() {
        auto execute_until_comment_start = [&]() {
            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *(m_current_ptr++);
                switch (codepoint) {
                    case token_type::space:
                    case token_type::tab: break;
                    case token_type::carriage:
                    case token_type::newline:
                    default: --m_current_ptr; return;
                }
            }
        };

        execute_until_comment_start();

        m_current_value_start_ptr = m_current_ptr;
        m_current_value_end_ptr = m_current_value_start_ptr;

        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token_type::space:
                case token_type::tab: break;
                case token_type::carriage:
                case token_type::newline: {
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
    void parser<Tchar, Tsax_handler>::execute_find_value() {
        bool found_tab_on_current_line = false;
        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token_type::carriage:
                case token_type::newline: {
                    register_newline();
                    found_tab_on_current_line = false;
                } break;
                case token_type::space: register_line_indentation(); break;
                case token_type::tab: found_tab_on_current_line = true; break;
                case token_type::comment: push_stack(&parser::execute_parse_comment); return;
                case token_type::object: return error(parse_result_code::missing_key);
                case token_type::sequence: return error(parse_result_code::not_implemented);
                case token_type::null: return error(parse_result_code::not_implemented);
                case token_type::object_start: return error(parse_result_code::not_implemented);     // json
                //case token_type::object_end: return error(parse_result_code::not_implemented);       // json
                case token_type::sequence_start: return error(parse_result_code::not_implemented);   // json
                //case token_type::sequence_end: return error(parse_result_code::not_implemented);     // json

                case token_type::literal_block:
                case token_type::folded_block: {
                    m_current_value_start_ptr = m_current_ptr - 1;
                    m_current_value_end_ptr = m_current_value_start_ptr;
                } break;
                case token_type::quote: return error(parse_result_code::not_implemented);
                case token_type::single_quote: return error(parse_result_code::not_implemented);
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
                case token_type::carriage:
                case token_type::newline: {
                    stack.type = stack_type_t::scalar;
                    process_scalar();
                    m_current_ptr -= 1;
                    stack.state_function = &parser::execute_find_value;
                } return;
                case token_type::space:
                case token_type::tab: break;
                case token_type::comment: {
                    process_scalar();
                    push_stack(&parser::execute_parse_comment);
                } return;
                case token_type::object: {
                    stack.state_function = &parser::execute_find_unknown_potential_key;
                } return;
                //case token_type::literal_block: return error(parse_result_code::not_implemented);
                //case token_type::folded_block: return error(parse_result_code::not_implemented);
                case token_type::sequence: return error(parse_result_code::not_implemented);
                case token_type::null: return error(parse_result_code::not_implemented);
                case token_type::object_start: return error(parse_result_code::not_implemented);     // json
                case token_type::object_end: return error(parse_result_code::not_implemented);       // json
                case token_type::sequence_start: return error(parse_result_code::not_implemented);   // json
                case token_type::sequence_end: return error(parse_result_code::not_implemented);     // json
                case token_type::quote: return error(parse_result_code::not_implemented);
                case token_type::single_quote: return error(parse_result_code::not_implemented);
                default: m_current_value_end_ptr = m_current_ptr; break;
            }
        }
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_find_unknown_potential_key() {
        auto& stack = get_stack();
        
        auto found_key_func = [&]() {
            if (m_last_key_line == m_current_line) {
                return error(parse_result_code::unexpected_key);
            }
            m_last_key_line = m_current_line;

            stack.type = stack_type_t::object;
            stack.min_indention = m_current_line_indention;
            signal_start_object();
            process_key();
            push_stack(&parser::execute_find_value);
            consume_whitespaces_after_key();
        };

        if (m_current_ptr < m_end_ptr) {
            const auto peek_codepoint = *m_current_ptr;
            switch (peek_codepoint) {
                case token_type::space:
                case token_type::tab:
                case token_type::carriage:
                case token_type::newline: return found_key_func();
                default: stack.state_function = &parser::execute_find_unknown; break;
            }

            return;
        }

        // Out of buffer, so this must be a key
        found_key_func();
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_find_scalar() {
        auto& stack = get_stack();

        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token_type::carriage:
                case token_type::newline: {
                    process_scalar();
                    m_current_ptr -= 1;
                    stack.state_function = &parser::execute_find_value;
                } return;
                case token_type::space:
                case token_type::tab: break;
                case token_type::comment: {
                    process_scalar();
                    push_stack(&parser::execute_parse_comment);
                } return;
                case token_type::object: return error(parse_result_code::unexpected_key);
                //case token_type::literal_block: return error(parse_result_code::not_implemented);
                //case token_type::folded_block: return error(parse_result_code::not_implemented);
                case token_type::sequence: return error(parse_result_code::not_implemented);
                case token_type::null: return error(parse_result_code::not_implemented);
                case token_type::object_start: return error(parse_result_code::not_implemented);     // json
                case token_type::object_end: return error(parse_result_code::not_implemented);       // json
                case token_type::sequence_start: return error(parse_result_code::not_implemented);   // json
                case token_type::sequence_end: return error(parse_result_code::not_implemented);     // json
                case token_type::quote: return error(parse_result_code::not_implemented);
                case token_type::single_quote: return error(parse_result_code::not_implemented);
                default: m_current_value_end_ptr = m_current_ptr; break;
            }
        }
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_find_key() {
        auto& stack = get_stack();

        auto new_key_func = [&]() {
            if (m_last_key_line == m_current_line) {
                return error(parse_result_code::unexpected_key);
            }
            m_last_key_line = m_current_line;

            process_key();
            stack.state_function = &parser::execute_find_value;
            consume_whitespaces_after_key();
        };

        auto process_expected_whitespace = [&]() {
            const auto peek_codepoint = *m_current_ptr;
            switch (peek_codepoint) {
                case token_type::space:
                case token_type::tab:
                case token_type::carriage:
                case token_type::newline: new_key_func(); return true;
                default: break;
            }
            
            return false;
        };
        
        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token_type::carriage:
                case token_type::newline: return error(parse_result_code::missing_key);
                case token_type::space:
                case token_type::tab: break;
                case token_type::comment: return error(parse_result_code::missing_key);
                case token_type::object: {
                    if (m_current_ptr >= m_end_ptr) {
                        new_key_func();
                        return;
                    }

                    if (!process_expected_whitespace()) {
                        break;
                    }
                } return;
                default: m_current_value_end_ptr = m_current_ptr; break;
            }
        }

        return error(parse_result_code::missing_key);
    }*/

    /*template<typename Tchar, typename Tsax_handler>
    parse_result_code parser<Tchar, Tsax_handler>::consume_whitespaces_after_key() {     
        auto result = [&]() {
            auto& current_stack = get_stack();
            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *(m_current_ptr++);
                switch (codepoint) {
                    case token_type::space:
                    case token_type::tab: break;
                    case token_type::carriage:
                    case token_type::newline: {
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
    }*/

    /*template<typename Tchar, typename Tsax_handler>
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
    }*/


    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::signal_start_scalar(yaml::block_style block_style, yaml::chomping comping) {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_start_scalar<Tsax_handler>() == true) {
            m_sax_handler.start_scalar(block_style, comping);
        }
#else
        m_sax_handler.start_scalar(block_style, comping);
#endif  
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::signal_end_scalar() {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_end_scalar<Tsax_handler>() == true) {
            m_sax_handler.end_scalar();
        }
 #else
        m_sax_handler.end_scalar();
 #endif 
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

    /*template<typename Tchar, typename Tsax_handler>
    typename parser<Tchar, Tsax_handler>::stack_iterator_t parser<Tchar, Tsax_handler>::get_stack_iterator() {
        return m_stack.size() > 1 ? static_cast<stack_iterator_t>(std::prev(m_stack.end())) : m_stack.begin();
    }*/

    template<typename Tchar, typename Tsax_handler>
    typename parser<Tchar, Tsax_handler>::stack_item_t& parser<Tchar, Tsax_handler>::push_stack(/*state_function_t state_function*/) {
        const auto type_indention = m_stack.empty() ? size_t{ 0 } : m_stack.back().type_indention + 1;
        m_stack.emplace_back();
        auto& new_stack = m_stack.back();
        //new_stack.state_function = state_function;
        new_stack.type_indention = type_indention;
        return new_stack;
    };

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::pop_stack_from(stack_iterator_t it) {
        const auto rit_end = stack_reverse_iterator_t(it);
        for (auto rit = m_stack.rbegin(); rit != rit_end; ++rit) {
            switch (rit->type) {
                case stack_type_t::unknown: signal_null(); break;
                case stack_type_t::scalar: signal_end_scalar(); break;
                case stack_type_t::object: signal_end_object(); break;
                case stack_type_t::sequence: signal_end_array(); break;
            }
        }
        m_stack.erase(it, m_stack.end());
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