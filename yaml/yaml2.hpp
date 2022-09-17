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

#if defined(__cpp_lib_string_view) && !defined(MINIYAML_NO_STD_STRING_VIEW)
#include <string_view>
#define MINIYAML_HAS_STD_STRING_VIEW true
namespace MINIYAML_NAMESPACE {
    template<typename T>
    using basic_string_view = std::basic_string_view<T>;
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

        const T* data() { return m_data; };
        const T* size() { return m_size; };

    private:

        const T* m_data;
        size_t m_size;

    };
}
#endif

namespace MINIYAML_NAMESPACE {

    enum class parse_result_code {
        success,
        not_implemented,
        internal_error,
        unexpected_end_of_file,
        unexpected_key,
        unexpected_sequence,
        invalid_tab_indentation,
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
            int64_t min_indention_line = 0;
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

        void register_newline();
        void register_line_indentation();

        void execute_find_value();
        void execute_find_scalar();
        void execute_find_scalar_after_newline();
        void execute_process_scalar_end();
        void execute_find_scalar_potential_key();
        void execute_found_key(); 

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

#if defined(__cpp_concepts)
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
        template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_null() {
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
        m_current_ptr = raw_input;
        m_end_ptr = raw_input + size;
        m_begin_ptr = raw_input;    
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

        if (m_stack.empty()) {
            return parse_result_code::internal_error;
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
        auto& stack = get_stack();
        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token::carriage:
                case token::newline: register_newline(); break;
                case token::space: register_line_indentation(); break;
                case token::tab: return error(parse_result_code::invalid_tab_indentation);            
                case token::comment: return error(parse_result_code::not_implemented);

                case token::object: return error(parse_result_code::not_implemented);
                case token::sequence: return error(parse_result_code::not_implemented);
                case token::null: return error(parse_result_code::not_implemented);
                case token::object_start: return error(parse_result_code::not_implemented);     // json
                case token::object_end: return error(parse_result_code::not_implemented);       // json
                case token::sequence_start: return error(parse_result_code::not_implemented);   // json
                case token::sequence_end: return error(parse_result_code::not_implemented);     // json
                case token::quote: return error(parse_result_code::not_implemented);
                case token::single_quote: return error(parse_result_code::not_implemented);
                default: {
                    m_current_ptr -= 1;
                    stack.type = stack_type_t::scalar;
                    stack.state_function = &parser::execute_find_scalar;
                } return;
            }
        }
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_find_scalar() {
        auto& stack = get_stack();

        /*if (m_current_line_indention < stack.min_indention && m_current_line > stack.min_indention_line) {
            stack.state_function = &parser::execute_process_scalar_end;
            return;
        }*/

        m_current_value_start_ptr = m_current_ptr;

        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token::carriage:
                case token::newline: {
                    register_newline();
                    stack.min_indention += 1;
                    stack.state_function = &parser::execute_find_scalar_after_newline;
                } return;
                case token::space:
                case token::tab: break;
                case token::comment: return error(parse_result_code::not_implemented); 

                case token::object: {
                    stack.state_function = &parser::execute_find_scalar_potential_key;
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

        stack.state_function = &parser::execute_process_scalar_end;
        return;
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_find_scalar_after_newline() {
        /*auto clear_whitespaces = [&]() {
            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *(m_current_ptr++);
                switch (codepoint) {
                    case token::carriage:
                    case token::newline: register_newline(); break;
                    case token::space: register_line_indentation(); break;
                    case token::tab: return parse_result_code::invalid_tab_indentation;
                    default: {
                        m_current_ptr -= 1;
                        return parse_result_code::success;
                    }
                }
            }
            return parse_result_code::success;
        };

        const auto clear_whitespaces_result = clear_whitespaces();
        if (clear_whitespaces_result != parse_result_code::success) {
            return error(clear_whitespaces_result);
        }

        auto& stack = get_stack();
        auto min_indention = stack.registry.b.int64_value;
        auto min_indention_line = stack.registry.d.int64_value;
        if (m_current_line > min_indention_line && m_current_line_indention < min_indention) {
            stack.state_function = &parser::execute_process_scalar_end;
            return;
        }*/

        // Process more lines...
        return error(parse_result_code::not_implemented);
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_process_scalar_end() {
        const auto scalar_length = m_current_value_start_ptr == nullptr ? 
            size_t{ 0 } :
            static_cast<size_t>(m_current_value_end_ptr - m_current_value_start_ptr);

        if (scalar_length == 0) {
            signal_null();
        }
        else {
            const auto scalar_value = string_view_type{ m_current_value_start_ptr, scalar_length };
            signal_string(scalar_value);
        }

        pop_stack();
        auto& stack = get_stack();
        stack.state_function = &parser::execute_find_value;
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_find_scalar_potential_key() {
        if (m_current_ptr >= m_end_ptr) {
            return;
        }

        const auto peek_codepoint = *m_current_ptr;
        switch (peek_codepoint) {
            case token::space:
            case token::tab:
            case token::carriage:
            case token::newline: {
                const auto has_multiple_keys_on_line = m_current_value_start_ptr != m_current_line_indention_ptr;
                if (has_multiple_keys_on_line) {
                    return error(parse_result_code::unexpected_key);
                }

                const auto key_length = m_current_value_start_ptr == nullptr ?
                    size_t{ 0 } :
                    static_cast<size_t>(m_current_value_end_ptr - m_current_value_start_ptr);

                if (key_length == 0) {
                    return error(parse_result_code::missing_key);
                }

                const auto key_value = string_view_type{ m_current_value_start_ptr, key_length };

                auto& stack = get_stack();

                if (m_current_line_indention == stack.min_indention) {
                    if (stack.type != stack_type_t::object) {

                        stack.type = stack_type_t::object;
                        stack.min_indention = m_current_line_indention;
                        stack.min_indention_line = m_current_line;

                        signal_start_object();
                    }
                    
                    signal_key(key_value);
                    push_stack(&parser::execute_found_key);
                    return;
                }

                if (m_current_line_indention > stack.min_indention) {
                    stack.type = stack_type_t::object;
                    stack.min_indention = m_current_line_indention;
                    stack.min_indention_line = m_current_line;

                    signal_start_object();
                    signal_key(key_value);
                    push_stack(&parser::execute_found_key);
                    return;
                }

                if (m_current_line_indention < stack.min_indention) {
                    auto find_min_indention_stack = [&](int64_t p_indention) {
                        auto it = m_stack.begin();
                        auto prev_it = it;
                        for (; it != m_stack.end(); ++it) {
                            if (it->min_indention == p_indention) {
                                return it;
                            }
                            if (it->min_indention > p_indention) {
                                return prev_it;
                            }
                            prev_it = it;
                        }
                        return prev_it;
                    };

                    auto min_indention_stack = find_min_indention_stack(m_current_line_indention);
                    pop_stack_from(min_indention_stack);
                    
                    auto& stack_after_pop = get_stack();
                    if (stack_after_pop.min_indention != m_current_line_indention) {
                        return error(parse_result_code::bad_key_indentation);
                    }

                    signal_key(key_value);
                    push_stack(&parser::execute_found_key);
                    return;
                }

            } return;
            default: break;
        }
    }

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::execute_found_key() {
        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token::space:
                case token::tab: break;
                case token::carriage:
                case token::newline:
                default: {
                    m_current_ptr -= 1;
                    m_current_value_start_ptr = nullptr;
                    m_current_value_end_ptr = nullptr;
                    
                    auto& stack = get_stack();
                    stack.min_indention = m_current_line_indention /* + 1*/;
                    stack.min_indention_line = m_current_line;
                    stack.state_function = &parser::execute_find_value;

                } return;
            }
        }
    }
    

    ///*template<typename Tchar, typename Tsax_handler>
    //void parser<Tchar, Tsax_handler>::execute_find_comment_start() {
    //    while (m_current_ptr < m_end_ptr) {
    //        auto prev_ptr = m_current_ptr;
    //        const auto codepoint = *(m_current_ptr++);
    //        switch (codepoint) {
    //            case token::space:
    //            case token::tab: continue;
    //            case token::carriage:
    //            case token::newline: {
    //                signal_comment({});
    //                return pop_stack();
    //            } break;
    //            default: {
    //                return update_stack(&parser::execute_find_comment_end, prev_ptr);
    //            }
    //        }
    //    }

    //    signal_comment({});
    //    return pop_stack();
    //}

    //template<typename Tchar, typename Tsax_handler>
    //void parser<Tchar, Tsax_handler>::execute_find_comment_end() {
    //    auto next_not_space_ptr = m_current_ptr + 1;

    //    while (m_current_ptr < m_end_ptr) {
    //        //auto prev_ptr = m_current_ptr;
    //        const auto codepoint = *(m_current_ptr++);
    //        switch (codepoint) {
    //            case token::carriage:
    //            case token::newline: {
    //                auto& stack = get_stack();
    //                auto length = static_cast<size_t>(next_not_space_ptr - stack.data_ptr);
    //                auto string = string_view_type{ stack.data_ptr, length };
    //                signal_comment(string);
    //                return pop_stack();
    //            }
    //            case token::space:
    //            case token::tab: continue;
    //            default: {
    //                next_not_space_ptr = m_current_ptr;
    //                continue;
    //            }
    //        }
    //    }*/

    //    /*auto next_not_space_iterator = std::next(m_current_it);

    //    for (auto loop : parse_to_end()) {
    //        switch (const auto current_char = *loop.prev_it; current_char) {
    //            case token::carriage:
    //            case token::newline: {
    //                auto string = impl::iterators_to_string_view(get_stack().data_iterator, next_not_space_iterator);
    //                signal_comment(string);
    //                return pop_stack();
    //            }
    //            case token::space:
    //            case token::tab: continue;
    //            default: {
    //                next_not_space_iterator = loop.it;
    //                continue;
    //            }
    //        }
    //    }*/
    ////}


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
        if constexpr (impl::sax_handler_has_null<Tchar, Tsax_handler>() == true) {
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
        m_stack.emplace_back();
        auto& new_stack = m_stack.back();
        new_stack.state_function = state_function;
        return new_stack;
    };

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::pop_stack() {
        m_stack.pop_back();
    };

    template<typename Tchar, typename Tsax_handler>
    void parser<Tchar, Tsax_handler>::pop_stack_from(stack_iterator_t it) {
        if (it == m_stack.end()) {
            return;
        }

        auto erase_from_it = ++it;   

        for (; it != m_stack.end(); ++it) { // WARNING: Need to run in reverse as soon as multiple type checks are implemented.
            if (it->type == stack_type_t::object) {
                signal_end_object();
            }
        }

        m_stack.erase(erase_from_it, m_stack.end());
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