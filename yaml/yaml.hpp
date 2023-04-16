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

#ifndef MINIYAML_HEADER_FILE
#define MINIYAML_HEADER_FILE

#include <memory>
#include <array>
#include <vector>
#include <map>
#include <string>
#include <limits>
#include <algorithm>
#include <type_traits>
#include <fstream>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <climits>
#include <errno.h>

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
#include <cstring>
namespace MINIYAML_NAMESPACE {
    template<typename T>
    class basic_string_view {

    public:

        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = std::size_t;     

        basic_string_view() :
            m_data(nullptr),
            m_size(0)
        {}

        basic_string_view(const_pointer s) :
            m_data(s),
            m_size(std::strlen(s))
        {}

        basic_string_view(const_pointer s, size_type count) :
            m_data(s),
            m_size(count)
        {}

        value_type operator [](size_type index) const {
            return m_data[index];
        }

        bool operator == (const basic_string_view& rhs) const {
            return m_size == rhs.m_size && std::strncmp(m_data, rhs.m_data, m_size) == 0;
        }

        MINIYAML_NODISCARD const_pointer data() const { return m_data; };
        MINIYAML_NODISCARD size_type size() const { return m_size; };
        MINIYAML_NODISCARD bool empty() const { return m_size == 0; };

    private:

        const_pointer m_data;
        size_type m_size;

    };

    using string_view = basic_string_view<char>;
}
#endif

namespace MINIYAML_NAMESPACE {

    enum class read_result_code {
        success,
        cannot_open_file,
        reached_max_depth,
        reached_max_document_count,
        not_implemented,
        forbidden_tab_indentation,
        bad_indentation,
        expected_line_break,
        expected_key,
        expected_sequence,
        unexpected_eof,
        unexpected_key,
        unexpected_token,    
        tag_duplication,
    };

    template<typename T>
    struct token {
        MINIYAML_INLINE_VARIABLE constexpr static T eof = '\0';
        MINIYAML_INLINE_VARIABLE constexpr static T document_start = '-';
        MINIYAML_INLINE_VARIABLE constexpr static T document_end = '.';
        MINIYAML_INLINE_VARIABLE constexpr static T space = ' ';
        MINIYAML_INLINE_VARIABLE constexpr static T tab = '\t';
        MINIYAML_INLINE_VARIABLE constexpr static T carriage = '\r';
        MINIYAML_INLINE_VARIABLE constexpr static T newline = '\n';
        MINIYAML_INLINE_VARIABLE constexpr static T comment = '#';
        MINIYAML_INLINE_VARIABLE constexpr static T escape = '\\';
        MINIYAML_INLINE_VARIABLE constexpr static T double_quote = '\"';
        MINIYAML_INLINE_VARIABLE constexpr static T single_quote = '\'';
        MINIYAML_INLINE_VARIABLE constexpr static T object = ':';
        //MINIYAML_INLINE_VARIABLE constexpr static T flow_object_start = '{';
        //MINIYAML_INLINE_VARIABLE constexpr static T flow_object_end = '}';
        MINIYAML_INLINE_VARIABLE constexpr static T sequence = '-';
        //MINIYAML_INLINE_VARIABLE constexpr static T flow_sequence_start = '[';
        //MINIYAML_INLINE_VARIABLE constexpr static T flow_sequence_end = ']';
        MINIYAML_INLINE_VARIABLE constexpr static T null = '~';
        MINIYAML_INLINE_VARIABLE constexpr static T literal_block = '|';
        MINIYAML_INLINE_VARIABLE constexpr static T folded_block = '>';
        MINIYAML_INLINE_VARIABLE constexpr static T chomping_strip = '-';
        MINIYAML_INLINE_VARIABLE constexpr static T chomping_keep = '+';
        MINIYAML_INLINE_VARIABLE constexpr static T tag = '!';
    };

    enum class scalar_style_type {
        none,           // ' '
        literal,        // | Keep newlines.
        folded,         // > Replace newlines with spaces.
        double_quoted,  // ""
        single_quoted   // ''
    };

    enum class chomping_type {
        clip,   // ' ' Single newline at end.
        strip,  // '-' No newline at end
        keep,   // '+' All newlines from end
    };
}


// SAX declarations.
namespace MINIYAML_NAMESPACE {
namespace sax {

    /** SAX reader results. */
    template<typename Tchar>
    struct read_document_result {
        read_result_code result_code = read_result_code::success;
        basic_string_view<Tchar> remaining_input = {};
        int64_t current_line = 0;
        const Tchar* current_line_ptr = nullptr;

        operator bool() const;
    };

    template<typename Tchar>
    struct read_document_file_result {
        read_result_code result_code = read_result_code::success;
        int64_t current_line = 0;

        operator bool() const;
    };

    template<typename Tchar>
    using read_documents_result = read_document_result<Tchar>;

    template<typename Tchar>
    using read_documents_file_result = read_document_file_result<Tchar>;


    /** SAX reader options. */
    struct reader_options {
        size_t max_depth = 128;
        size_t max_document_count = 128;
        int64_t start_line_number = 0;
    };
        

    /** Helper function for parsing via SAX style API. */
    template<typename Tchar, typename Tsax_handler>
    read_document_result<Tchar> read_document(
        const Tchar* raw_input,
        size_t size,
        Tsax_handler& handler,
        const reader_options& options = {});

    template<typename Tchar, typename Tsax_handler>
    read_document_result<Tchar> read_document(
        const std::basic_string<Tchar>& string,
        Tsax_handler& handler,
        const reader_options& options = {});

    template<typename Tchar, typename Tsax_handler>
    read_document_result<Tchar> read_document(
        basic_string_view<Tchar> string_view,
        Tsax_handler& handler,
        const reader_options& options = {});

    template<typename Tchar, typename Tsax_handler>
    read_document_file_result<Tchar> read_document_from_file(
        const std::string& filename,
        Tsax_handler& handler,
        const reader_options& options = {});

    template<typename Tchar, typename Tsax_handler>
    read_documents_result<Tchar> read_documents(
        const Tchar* raw_input,
        size_t size,
        Tsax_handler& handler,
        const reader_options& options = {});

    template<typename Tchar, typename Tsax_handler>
    read_documents_result<Tchar> read_documents(
        const std::basic_string<Tchar>& string,
        Tsax_handler& handler,
        const reader_options& options = {});

    template<typename Tchar, typename Tsax_handler>
    read_documents_result<Tchar> read_documents(
        basic_string_view<Tchar> string_view,
        Tsax_handler& handler,
        const reader_options& options = {});

    template<typename Tchar, typename Tsax_handler>
    read_documents_file_result<Tchar> read_documents_from_file(
        const std::string& filename,
        Tsax_handler& handler,
        const reader_options& options = {});


    /** Helper base struct for SAX handler. */
    struct handler_base {
        virtual void start_scalar(scalar_style_type, chomping_type) {}
        virtual void end_scalar() {}
        virtual void start_object() {}
        virtual void end_object() {}
        virtual void start_array() {}
        virtual void end_array() {}
        virtual void null() {}
        virtual void string(basic_string_view<uint8_t>) {}
        virtual void key(basic_string_view<uint8_t>) {}
        virtual void comment(basic_string_view<uint8_t>) {}
    };


    /** SAX reader. */
    template<typename Tchar, typename Tsax_handler>
    class reader {

    public:

        using char_type = Tchar;
        using pointer = char_type*;
        using const_pointer = const char_type*;
        using size_type = std::size_t;
        using string_type = std::basic_string<char_type>;
        using string_view_type = MINIYAML_NAMESPACE::basic_string_view<char_type>;
        using sax_handler_type = Tsax_handler;
        using token_type = token<Tchar>;
        using read_document_result_type = read_document_result<char_type>;
        using read_document_file_result_type = read_document_file_result<char_type>;
        using read_documents_result_type = read_documents_result<char_type>;
        using read_documents_file_result_type = read_documents_file_result<char_type>;

        explicit reader(sax_handler_type& sax_handler, const reader_options& options = {});

        read_document_result_type read_document(const_pointer raw_input, size_type size);
        read_document_result_type read_document(const string_type& string);
        read_document_result_type read_document(string_view_type string_view);
        read_document_file_result_type read_document_from_file(const std::string& filename);

        read_documents_result_type read_documents(const_pointer raw_input, size_type size);
        read_documents_result_type read_documents(const string_type& string);
        read_documents_result_type read_documents(string_view_type string_view);
        read_documents_file_result_type read_documents_from_file(const std::string& filename);

    private:

        using state_function_t = void (reader::*)();

        enum class stack_type {
            unknown,
            scalar,
            object,
            sequence
        };

        struct stack_item_t {
            stack_item_t() = default;
            stack_item_t(const stack_item_t&) = delete;
            stack_item_t(stack_item_t&&) = default;               
            ~stack_item_t() = default;

            stack_item_t& operator = (stack_item_t&&) = default;
            stack_item_t& operator = (const stack_item_t&) = delete;

            bool is_flow_value() const {
                return flow_value_token != token_type::eof;
            }

            state_function_t state_function = nullptr;
            stack_type type = stack_type::unknown;
            int64_t type_indention = 0;
            int64_t processed_lines = 0;
            char_type flow_value_token = token_type::eof;
            bool has_tag = false;
        };

        using stack_t = std::vector<stack_item_t>;
        using stack_iterator_t = typename stack_t::iterator;
        using stack_reverse_iterator_t = typename stack_t::reverse_iterator;

        const_pointer m_current_ptr;
        const_pointer m_end_ptr;
        const_pointer m_begin_ptr;
        stack_t m_stack;
        read_result_code m_current_result_code;
        sax_handler_type& m_sax_handler;
        int64_t m_current_line;
        const_pointer m_current_line_ptr;
        int64_t m_current_line_indention;
        const_pointer m_current_line_indention_ptr;
        bool m_current_is_new_line;
        const reader_options& m_options;

        static const_pointer skip_utf8_bom(const_pointer begin, const_pointer end);

        void initialize_process(const_pointer raw_input, size_type size);
        read_result_code process_document();
        read_result_code process_documents();
        read_document_result_type create_read_document_result(read_result_code result_code) const;
        read_document_file_result_type create_read_document_file_result(read_result_code result_code) const;
        read_documents_result_type create_read_documents_result(read_result_code result_code) const;
        read_documents_file_result_type create_read_documents_file_result(read_result_code result_code) const;
        
        void execute_find_value();
        void execute_read_scalar();
        void execute_read_scalar_block();
        void execute_read_key();
        void execute_read_sequence();
        void execute_read_flow_scalar_quote();

        void register_newline();
        void register_line_indentation();

        void read_comment_until_newline();
        void consume_whitespaces_until_any();
        bool consume_whitespaces_until_newline();
        bool consume_only_whitespaces_until_newline();
        bool consume_only_whitespaces_until_newline_or_comment();

        bool has_min_tokens_left(size_t count) const;
        bool is_next_token(size_t increments, const char_type value) const;
        bool is_prev_token_whitespace(int decrements = 1) const;
        bool is_next_token_whitespace(int increments = 1) const;
        bool is_current_stack_flow_value() const;

        void signal_start_document();
        void signal_end_document();
        void signal_start_scalar(scalar_style_type style, chomping_type chomping);
        void signal_end_scalar();
        void signal_start_object();
        void signal_end_object();
        void signal_start_array();
        void signal_end_array();
        void signal_null();
        void signal_index(size_t value);
        void signal_string(string_view_type value);
        void signal_key(string_view_type value);
        void signal_tag(string_view_type value);
        void signal_comment(string_view_type value);

        void error(const read_result_code result_code);

        stack_item_t& push_stack(state_function_t state_function);
        void pop_stack();
        void pop_stack_if_not_root();
        void pop_stack_from(stack_iterator_t it);
        void signal_stack_item_pop(const stack_item_t& stack_item);

    };

} }


// DOM declarations.
namespace MINIYAML_NAMESPACE {
namespace dom {

    enum class node_type {
        null,
        scalar,
        object,
        array
    };

    template<typename Tchar, typename Ttraits> class node;
    template<typename Tchar, typename Ttraits> class scalar_node;
    template<typename Tchar, typename Ttraits> class object_node;
    template<typename Tchar, typename Ttraits> class array_node;


    template<typename Tchar, typename Tstring = std::basic_string<Tchar>>
    struct default_node_traits {
        using char_type = Tchar;

        using node_value = node<char_type, default_node_traits>;
        using node_reference = node_value&;
        using node_pointer = node_value*;
        using node_owned_pointer = std::unique_ptr<node_value>;

        using scalar_node_type = scalar_node<char_type, default_node_traits>;
        using object_node_type = object_node<char_type, default_node_traits>;
        using array_node_type = array_node<char_type, default_node_traits>;

        using string_type = Tstring;
        using object_collection = std::map<string_type, node_owned_pointer>;
        using array_collection = std::vector<node_owned_pointer>;
    };

    template<typename Tchar = char>
    using default_node_view_traits = default_node_traits<Tchar, MINIYAML_NAMESPACE::basic_string_view<Tchar>>;


    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    class node {

    public:

        using traits = Ttraits;
        using scalar_node_type = typename traits::scalar_node_type;
        using object_node_type = typename traits::object_node_type;
        using array_node_type = typename traits::array_node_type;
        using string_type = typename traits::string_type;

        static node create_scalar(scalar_style_type style = scalar_style_type::none, chomping_type chomping = chomping_type::strip);
        static node create_object();
        static node create_array();

        node();
        ~node();

        node(node&&) noexcept;
        node& operator = (node&&) noexcept(false);

        node(const node&) = delete;
        node& operator = (const node&) = delete;
        
        MINIYAML_NODISCARD node_type type() const;
        MINIYAML_NODISCARD bool is_null() const;
        MINIYAML_NODISCARD bool is_scalar() const;
        MINIYAML_NODISCARD bool is_object() const;
        MINIYAML_NODISCARD bool is_array() const;

        MINIYAML_NODISCARD scalar_node_type& as_scalar();
        MINIYAML_NODISCARD const scalar_node_type& as_scalar() const;
        MINIYAML_NODISCARD object_node_type& as_object();
        MINIYAML_NODISCARD const object_node_type& as_object() const;
        MINIYAML_NODISCARD array_node_type& as_array();
        MINIYAML_NODISCARD const array_node_type& as_array() const;

        template<typename Tvalue>
        MINIYAML_NODISCARD Tvalue as(Tvalue default_value = Tvalue{}) const;

        MINIYAML_NODISCARD string_type& tag();
        MINIYAML_NODISCARD const string_type& tag() const;
        void tag(string_type tag);

        MINIYAML_NODISCARD bool empty() const;
        MINIYAML_NODISCARD size_t size() const;

        MINIYAML_NODISCARD bool contains(string_type key) const;
        MINIYAML_NODISCARD bool contains(size_t index) const;

        MINIYAML_NODISCARD node& at(string_type key);
        MINIYAML_NODISCARD const node& at(string_type key) const;
        MINIYAML_NODISCARD node& at(size_t index);
        MINIYAML_NODISCARD const node& at(size_t index) const;

        MINIYAML_NODISCARD node& operator[](string_type key);
        MINIYAML_NODISCARD node& operator[](size_t index);
        MINIYAML_NODISCARD const node& operator[](size_t index) const;

    private:

        void destroy_underlying_node();
        void assign_this_overlying_to_underlying_node();

        node_type m_node_type;
        union underlying_node {
            scalar_node_type* scalar;
            object_node_type* object;
            array_node_type* array;
        } m_underlying_node;
        string_type m_tag;

    };


    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    class scalar_node {

    public:

        using traits = Ttraits;
        using node_value = typename traits::node_value;
        using string_type = typename traits::string_type;
        using lines_collection = std::vector<string_type>;
        using iterator = typename lines_collection::iterator;
        using const_iterator = typename lines_collection::const_iterator;
        using reverse_iterator = typename lines_collection::reverse_iterator;
        using const_reverse_iterator = typename lines_collection::const_reverse_iterator;

        scalar_node(node_value& overlying_node, scalar_style_type style, chomping_type chomping);

        scalar_node(const scalar_node&) = delete;
        scalar_node(scalar_node&&) = delete;
        scalar_node& operator = (const scalar_node&) = delete;
        scalar_node& operator = (scalar_node&&) = delete;

        template<typename Tvalue> 
        MINIYAML_NODISCARD Tvalue as(Tvalue default_value = Tvalue{}) const;

        MINIYAML_NODISCARD scalar_style_type& style();
        MINIYAML_NODISCARD scalar_style_type style() const;
        void style(scalar_style_type value);

        MINIYAML_NODISCARD chomping_type& chomping();
        MINIYAML_NODISCARD chomping_type chomping() const;
        void chomping(const chomping_type value);

        MINIYAML_NODISCARD bool empty() const;
        MINIYAML_NODISCARD size_t size() const;

        MINIYAML_NODISCARD iterator begin();
        MINIYAML_NODISCARD const_iterator begin() const;
        MINIYAML_NODISCARD const_iterator cbegin() const;
        MINIYAML_NODISCARD iterator end();
        MINIYAML_NODISCARD const_iterator end() const;
        MINIYAML_NODISCARD const_iterator cend() const;
        MINIYAML_NODISCARD reverse_iterator rbegin();
        MINIYAML_NODISCARD const_reverse_iterator rbegin() const;
        MINIYAML_NODISCARD const_reverse_iterator crbegin() const;
        MINIYAML_NODISCARD reverse_iterator rend();
        MINIYAML_NODISCARD const_reverse_iterator rend() const;
        MINIYAML_NODISCARD const_reverse_iterator crend() const;

        MINIYAML_NODISCARD string_type& at(size_t index);
        MINIYAML_NODISCARD const string_type& at(size_t index) const;

        MINIYAML_NODISCARD string_type& operator[](size_t index);
        MINIYAML_NODISCARD const string_type& operator[](size_t index) const;

        iterator insert(const_iterator pos, string_type string);
        void push_back(string_type string);
        void push_front(string_type string);

        void pop_back();
        void pop_front();
        iterator erase(iterator pos);
        iterator erase(iterator first, iterator last);
        iterator erase(const_iterator pos);
        iterator erase(const_iterator first, const_iterator last);

        MINIYAML_NODISCARD node_value& overlying_node();
        MINIYAML_NODISCARD const node_value& overlying_node() const;

    private:

        template<typename Tchar_Other, typename Ttraits_Other>
        friend class node;

        MINIYAML_NODISCARD std::basic_string<Tchar> as_non_block_string() const;
        MINIYAML_NODISCARD std::basic_string<Tchar> as_literal_string() const;
        MINIYAML_NODISCARD std::basic_string<Tchar> as_folded_string() const;

        lines_collection m_lines;
        scalar_style_type m_style;
        chomping_type m_chomping;
        node_value* m_overlying_node;

    };


    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    class object_node {

    public:

        using traits = Ttraits;
        using node_value = typename traits::node_value;
        using node_owned_pointer = typename traits::node_owned_pointer;
        using string_type = typename traits::string_type;        
        using collection = typename traits::object_collection;
        using iterator = typename collection::iterator;
        using const_iterator = typename collection::const_iterator;
        using reverse_iterator = typename collection::reverse_iterator;
        using const_reverse_iterator = typename collection::const_reverse_iterator;
        using insert_return_type = std::pair<iterator, bool>;

        explicit object_node(node_value& overlying_node);
        ~object_node() = default;

        object_node(const object_node&) = delete;
        object_node(object_node&&) = delete;
        object_node& operator = (const object_node&) = delete;
        object_node& operator = (object_node&&) = delete;

        MINIYAML_NODISCARD bool empty() const;
        MINIYAML_NODISCARD size_t size() const;

        MINIYAML_NODISCARD bool contains(const string_type& key) const;

        MINIYAML_NODISCARD iterator begin();
        MINIYAML_NODISCARD const_iterator begin() const;
        MINIYAML_NODISCARD const_iterator cbegin() const;
        MINIYAML_NODISCARD iterator end();
        MINIYAML_NODISCARD const_iterator end() const;
        MINIYAML_NODISCARD const_iterator cend() const;
        MINIYAML_NODISCARD reverse_iterator rbegin();
        MINIYAML_NODISCARD const_reverse_iterator rbegin() const;
        MINIYAML_NODISCARD const_reverse_iterator crbegin() const;
        MINIYAML_NODISCARD reverse_iterator rend();
        MINIYAML_NODISCARD const_reverse_iterator rend() const;
        MINIYAML_NODISCARD const_reverse_iterator crend() const;

        MINIYAML_NODISCARD iterator find(const string_type& key);
        MINIYAML_NODISCARD const_iterator find(const string_type& key) const;

        MINIYAML_NODISCARD node_value& at(const string_type& key);
        MINIYAML_NODISCARD const node_value& at(const string_type& key) const;

        MINIYAML_NODISCARD node_value& operator [] (const string_type& key);

        insert_return_type insert(const string_type& key);
        insert_return_type insert(const string_type& key, node_value&& node);

        size_t erase(const string_type& key);
        iterator erase(iterator pos);
        iterator erase(iterator first, iterator last);
        iterator erase(const_iterator pos);
        iterator erase(const_iterator first, const_iterator last);

        MINIYAML_NODISCARD node_value& overlying_node();
        MINIYAML_NODISCARD const node_value& overlying_node() const;

    private:

        template<typename Tchar_Other, typename Ttraits_Other>
        friend class node;

        collection m_map;
        node_value* m_overlying_node;

    };


    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    class array_node {

    public:

        using traits = Ttraits;
        using node_value = typename traits::node_value;
        using node_owned_pointer = typename traits::node_owned_pointer;
        using collection = typename traits::array_collection;
        using iterator = typename collection::iterator;
        using const_iterator = typename collection::const_iterator;
        using reverse_iterator = typename collection::reverse_iterator;
        using const_reverse_iterator = typename collection::const_reverse_iterator;

        explicit array_node(node_value& overlying_node);

        array_node(const array_node&) = delete;
        array_node(array_node&&) = delete;
        array_node& operator = (const array_node&) = delete;
        array_node& operator = (array_node&&) = delete;

        MINIYAML_NODISCARD bool empty() const;
        MINIYAML_NODISCARD size_t size() const;

        MINIYAML_NODISCARD bool contains(size_t index) const;

        MINIYAML_NODISCARD iterator begin();
        MINIYAML_NODISCARD const_iterator begin() const;
        MINIYAML_NODISCARD const_iterator cbegin() const;
        MINIYAML_NODISCARD iterator end();
        MINIYAML_NODISCARD const_iterator end() const;
        MINIYAML_NODISCARD const_iterator cend() const;
        MINIYAML_NODISCARD reverse_iterator rbegin();
        MINIYAML_NODISCARD const_reverse_iterator rbegin() const;
        MINIYAML_NODISCARD const_reverse_iterator crbegin() const;
        MINIYAML_NODISCARD reverse_iterator rend();
        MINIYAML_NODISCARD const_reverse_iterator rend() const;
        MINIYAML_NODISCARD const_reverse_iterator crend() const;

        MINIYAML_NODISCARD node_value& at(size_t index);
        MINIYAML_NODISCARD const node_value& at(size_t index) const;

        MINIYAML_NODISCARD node_value& operator[](size_t index);
        MINIYAML_NODISCARD const node_value& operator[](size_t index) const;

        iterator insert(const_iterator pos);
        iterator insert(const_iterator pos, node_value&& node);
        void push_back();
        void push_back(node_value&& node);

        void pop_back();
        iterator erase(iterator pos);
        iterator erase(iterator first, iterator last);
        iterator erase(const_iterator pos);
        iterator erase(const_iterator first, const_iterator last);

        MINIYAML_NODISCARD node_value& overlying_node();
        MINIYAML_NODISCARD const node_value& overlying_node() const;

    private:

        template<typename Tchar_Other, typename Ttraits_Other>
        friend class node;

        collection m_list;
        node_value* m_overlying_node;

    };


    /** DOM reader results. */
    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    struct read_document_result {
        using node_value = node<Tchar, Ttraits>;

        read_result_code result_code = read_result_code::success;
        basic_string_view<Tchar> remaining_input = {};
        int64_t current_line = 0;
        const Tchar* current_line_ptr = nullptr;
        node_value root_node = {};

        operator bool() const;
    };

    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    struct read_document_file_result {
        using node_value = node<Tchar, Ttraits>;

        read_result_code result_code = read_result_code::success;
        int64_t current_line = 0;
        node_value root_node = {};

        operator bool() const;
    };

    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    struct read_documents_result {
        using node_value = node<Tchar, Ttraits>;
        using node_values = std::vector<node_value>;

        read_result_code result_code = read_result_code::success;
        basic_string_view<Tchar> remaining_input = {};
        int64_t current_line = 0;
        const Tchar* current_line_ptr = nullptr;
        node_values root_nodes = {};

        operator bool() const;
    };

    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    struct read_documents_file_result {
        using node_value = node<Tchar, Ttraits>;
        using node_values = std::vector<node_value>;

        read_result_code result_code = read_result_code::success;
        int64_t current_line = 0;
        node_values root_nodes = {};

        operator bool() const;
    };


    /** DOM reader options. */
    using reader_options = sax::reader_options;


    /** Helper function for parsing via DOM style API. */
    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    read_document_result<Tchar, Ttraits> read_document(
        const Tchar* raw_input,
        size_t size,
        const reader_options& options = {});

    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    read_document_result<Tchar, Ttraits> read_document(
        const std::basic_string<Tchar>& string,
        const reader_options& options = {});

    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    read_document_result<Tchar, Ttraits> read_document(
        basic_string_view<Tchar> string_view,
        const reader_options& options = {});

    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    read_document_file_result<Tchar, Ttraits> read_document_from_file(
        const std::string& filename,
        const reader_options& options = {});

    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    read_documents_result<Tchar, Ttraits> read_documents(
        const Tchar* raw_input,
        size_t size,
        const reader_options& options = {});

    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    read_documents_result<Tchar, Ttraits> read_documents(
        const std::basic_string<Tchar>& string,
        const reader_options& options = {});

    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    read_documents_result<Tchar, Ttraits> read_documents(
        basic_string_view<Tchar> string_view,
        const reader_options& options = {});

    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    read_documents_file_result<Tchar, Ttraits> read_documents_from_file(
        const std::string& filename,
        const reader_options& options = {});


    /** DOM reader. */
    template<typename Tchar = char, typename Ttraits = default_node_traits<Tchar>>
    class reader {

    public:

        using char_type = Tchar;
        using pointer = char_type*;
        using const_pointer = const char_type*;
        using size_type = std::size_t;
        using node_value = typename Ttraits::node_value;
        using string_type = std::basic_string<char_type>;
        using string_view_type = MINIYAML_NAMESPACE::basic_string_view<char_type>;
        using token_type = token<Tchar>;
        using read_document_result_type = read_document_result<char_type, Ttraits>;
        using read_document_file_result_type = read_document_file_result<char_type, Ttraits>;
        using read_documents_result_type = read_documents_result<char_type, Ttraits>;
        using read_documents_file_result_type = read_documents_file_result<char_type, Ttraits>;

        explicit reader(const reader_options& options = {});

        read_document_result_type read_document(const_pointer raw_input, size_type size);
        read_document_result_type read_document(const string_type& string);
        read_document_result_type read_document(string_view_type string_view);
        read_document_file_result_type read_document_from_file(const std::string& filename);

        read_documents_result_type read_documents(const_pointer raw_input, size_type size);
        read_documents_result_type read_documents(const string_type& string);
        read_documents_result_type read_documents(string_view_type string_view);
        read_documents_file_result_type read_documents_from_file(const std::string& filename);

    private:

        using node_values = std::vector<node_value>;

        class sax_handler {

        public:

            sax_handler();

            void initialize();

            void start_document();
            void end_document();
            void start_scalar(scalar_style_type, chomping_type);
            void end_scalar();
            void start_object();
            void end_object();
            void start_array();
            void end_array();
            void null();
            void index(size_t);
            void string(string_view_type);
            void key(string_view_type);
            void tag(string_view_type);
            void comment(string_view_type);

            using node_stack_t = std::vector<node_value*>;
            using string_view_list = std::vector<string_view_type>;
            using string_view_list_iterator = typename string_view_list::iterator;

            void push_stack(node_value* node);
            void pop_stack();

            node_values m_documents;
            node_value* m_current_node;
            node_stack_t m_node_stack;

            string_view_list m_string_views;
            scalar_style_type m_current_style;
            chomping_type m_current_chomping;
        };

        using sax_handler_type = sax_handler;
        using sax_read_document_result_type = sax::read_document_result<Tchar>;
        using sax_read_document_file_result_type = sax::read_document_file_result<Tchar>;
        using sax_read_documents_result_type = sax::read_documents_result<Tchar>;
        using sax_read_documents_file_result_type = sax::read_documents_file_result<Tchar>;

        static read_document_result_type create_read_document_result(sax_read_document_result_type sax_result, node_value&& node);
        static read_document_file_result_type create_read_document_file_result(sax_read_document_file_result_type sax_result, node_value&& node);
        static read_documents_result_type create_read_documents_result(sax_read_documents_result_type sax_result, node_values&& nodes);
        static read_documents_file_result_type create_read_documents_file_result(sax_read_documents_file_result_type sax_result, node_values&& nodes);

        sax_handler_type m_sax_handler;
        sax::reader<Tchar, sax_handler_type> m_sax_reader;

    };

} }


// Hidden implementation details.
namespace MINIYAML_NAMESPACE {
namespace impl {

    enum class read_file_result_code {
        success,
        cannot_open_file
    };

    template<typename Tchar>
    struct read_file_result {
        read_file_result_code result_code = read_file_result_code::success;
        std::vector<Tchar> data = {};
    };

    template<typename Tchar>
    read_file_result<Tchar> read_file(const std::string& filename) {
        static_assert(sizeof(Tchar) == sizeof(char), "Tchar is not compatible with ifstream::read.");

        auto result = read_file_result<Tchar>{};
        auto file = std::ifstream(filename);
        if (!file.is_open()) {
            result.result_code = read_file_result_code::cannot_open_file;
            return result;
        }

        file.seekg(0, std::ifstream::end);
        const auto data_size = static_cast<size_t>(file.tellg());
        if (data_size == 0) {
            return result;
        }

        result.data.resize(data_size);

        file.seekg(0, std::ifstream::beg);
        file.read(reinterpret_cast<char*>(result.data.data()), data_size);

        return result;
    }

    template<typename Tchar>
    const std::basic_string<Tchar>& get_string_from_string_or_view(const std::basic_string<Tchar>& string) {
        return string;
    }

#if MINIYAML_HAS_STD_STRING_VIEW == true
    template<typename Tchar>
    std::basic_string<Tchar> get_string_from_string_or_view(basic_string_view<Tchar> string_view) {
        return std::basic_string<Tchar>{ string_view };
    }

#else
    template<typename Tchar>
    std::basic_string<Tchar> get_string_from_string_or_view(basic_string_view<Tchar> string_view) {
        return std::basic_string<Tchar>{ string_view.data(), string_view.size() };
    }

#endif

    template<typename T>
    bool is_empty_or_whitespace(const T& string) {
        using value_type = typename T::value_type;

        if (string.empty()) {
            return true;
        }

        for (size_t i = 0; i < string.size(); i++) {
            const auto value = string[i];
            if (value == token<value_type>::space || value == token<value_type>::tab) {
                continue;
            }
            return false;
        }

        return true;
    }

} }


// Hidden SAX implementation details.
namespace MINIYAML_NAMESPACE {
namespace sax {
namespace impl {

#if MINIYAML_HAS_CONCEPTS
    template<typename Tsax_handler>
    constexpr bool sax_handler_has_start_document() {
        return requires(Tsax_handler value) { { value.start_document() }; };
    }
    template<typename Tsax_handler>
    constexpr bool sax_handler_has_end_document() {
        return requires(Tsax_handler value) { { value.end_document() }; };
    }
    template<typename Tsax_handler>
    constexpr bool sax_handler_has_start_scalar() {
        return requires(Tsax_handler value) { { value.start_scalar(scalar_style_type::none, chomping_type::strip) }; };
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
    template<typename Tsax_handler>
    constexpr bool sax_handler_has_index() {
        return requires(Tsax_handler value) { { value.index(size_t{}) }; };
    }
    template<typename Tchar, typename Tsax_handler>
    constexpr bool sax_handler_has_string() {
        return requires(Tsax_handler handler) { { handler.string(typename reader<Tchar, Tsax_handler>::string_view_type{}) }; };
    }
    template<typename Tchar, typename Tsax_handler>
    constexpr bool sax_handler_has_key() {
        return requires(Tsax_handler handler) { { handler.key(typename reader<Tchar, Tsax_handler>::string_view_type{}) }; };
    }
    template<typename Tchar, typename Tsax_handler>
    constexpr bool sax_handler_has_tag() {
        return requires(Tsax_handler handler) { { handler.tag(typename reader<Tchar, Tsax_handler>::string_view_type{}) }; };
    }
    template<typename Tchar, typename Tsax_handler>
    constexpr bool sax_handler_has_comment() {
        return requires(Tsax_handler handler) { { handler.comment(typename reader<Tchar, Tsax_handler>::string_view_type{}) }; };
    }
#else
    template<typename Tsax_handler> constexpr bool sax_handler_has_start_document() {
        return true;
    }
    template<typename Tsax_handler> constexpr bool sax_handler_has_end_document() {
        return true;
    }
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
    template<typename Tsax_handler> constexpr bool sax_handler_has_index() {
        return true;
    }
    template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_string() {
        return true;
    }
    template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_key() {
        return true;
    }
    template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_tag() {
        return true;
    }
    template<typename Tchar, typename Tsax_handler> constexpr bool sax_handler_has_comment() {
        return true;
    }
#endif

} } }


// SAX Implementations.
namespace MINIYAML_NAMESPACE {
namespace sax {

    // Read implementations.
    template<typename Tchar, typename Tsax_handler>
    reader<Tchar, Tsax_handler>::reader(sax_handler_type& sax_handler, const reader_options& options) :
        m_current_ptr(nullptr),
        m_end_ptr(nullptr),
        m_begin_ptr(nullptr),
        m_stack{},
        m_current_result_code(read_result_code::success),
        m_sax_handler(sax_handler),
        m_current_line(0),
        m_current_line_ptr(nullptr),
        m_current_line_indention(0),
        m_current_line_indention_ptr(nullptr),
        m_current_is_new_line(true),
        m_options(options)
    {}

    template<typename Tchar, typename Tsax_handler>
    typename reader<Tchar, Tsax_handler>::read_document_result_type reader<Tchar, Tsax_handler>::read_document(const_pointer raw_input, size_type size) {
        initialize_process(raw_input, size);
        const auto process_result = process_document();
        return create_read_documents_result(process_result);
    }

    template<typename Tchar, typename Tsax_handler>
    typename reader<Tchar, Tsax_handler>::read_document_result_type reader<Tchar, Tsax_handler>::read_document(const string_type& string) {
        return read_document(string.c_str(), string.size());
    }

    template<typename Tchar, typename Tsax_handler>
    typename reader<Tchar, Tsax_handler>::read_document_result_type reader<Tchar, Tsax_handler>::read_document(string_view_type string_view ) {
        return read_document(string_view.data(), string_view.size());
    }

    template<typename Tchar, typename Tsax_handler>
    typename reader<Tchar, Tsax_handler>::read_document_file_result_type reader<Tchar, Tsax_handler>::read_document_from_file(const std::string& filename) {
        auto result = read_document_file_result_type{};

        auto file_result = MINIYAML_NAMESPACE::impl::read_file<Tchar>(filename);
        if (file_result.result_code != MINIYAML_NAMESPACE::impl::read_file_result_code::success) {
            result.result_code = read_result_code::cannot_open_file;
            return result;
        }

        const auto read_result = read_document(file_result.data.data(), file_result.data.size());
        result.result_code = read_result.result_code;
        return result;
    }

    template<typename Tchar, typename Tsax_handler>
    typename reader<Tchar, Tsax_handler>::read_documents_result_type reader<Tchar, Tsax_handler>::read_documents(const_pointer raw_input, size_type size) {
        initialize_process(raw_input, size);
        const auto process_result = process_documents();
        return create_read_documents_result(process_result);
    }

    template<typename Tchar, typename Tsax_handler>
    typename reader<Tchar, Tsax_handler>::read_documents_result_type reader<Tchar, Tsax_handler>::read_documents(const string_type& string) {
        return read_documents(string.c_str(), string.size());
    }

    template<typename Tchar, typename Tsax_handler>
    typename reader<Tchar, Tsax_handler>::read_documents_result_type reader<Tchar, Tsax_handler>::read_documents(string_view_type string_view) {
        return read_documents(string_view.data(), string_view.size());
    }

    template<typename Tchar, typename Tsax_handler>
    typename reader<Tchar, Tsax_handler>::read_documents_file_result_type reader<Tchar, Tsax_handler>::read_documents_from_file(const std::string& filename) {
        auto result = read_documents_file_result_type{};

        auto file_result = MINIYAML_NAMESPACE::impl::read_file<Tchar>(filename);
        if (file_result.result_code != MINIYAML_NAMESPACE::impl::read_file_result_code::success) {  
            result.result_code = read_result_code::cannot_open_file;
            return result;
        }

        const auto read_result = read_documents(file_result.data.data(), file_result.data.size());
        result.result_code = read_result.result_code;
        return result;
    }

    template<typename Tchar, typename Tsax_handler>
    typename reader<Tchar, Tsax_handler>::const_pointer reader<Tchar, Tsax_handler>::skip_utf8_bom(const_pointer begin, const_pointer end) {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (sizeof(Tchar) == 1) {
#else
        // This is a stupid workaround for a MSVC bug, caused when compiling using C++14.
        // "if constexpr" is expected even if __cpp_if_constexpr isn't defined,
        //  if the expression "sizeof(char_type) == 1" is used directly in if statement.
        const auto char_size_test = sizeof(char_type) == 1;
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
    void reader<Tchar, Tsax_handler>::initialize_process(const_pointer raw_input, size_type size) {
        m_end_ptr = raw_input + size;
        m_begin_ptr = skip_utf8_bom(raw_input, m_end_ptr);
        m_current_ptr = m_begin_ptr;
        m_stack.clear();
        m_current_result_code = read_result_code::success;
        m_current_line = m_options.start_line_number;
        m_current_line_ptr = m_begin_ptr;
        m_current_line_indention = 0;
        m_current_line_indention_ptr = m_begin_ptr;
        m_current_is_new_line = true;
    }

    template<typename Tchar, typename Tsax_handler>
    read_result_code reader<Tchar, Tsax_handler>::process_document() {
        if (m_options.max_document_count == 0) {
            return read_result_code::reached_max_document_count;
        }

        push_stack(&reader::execute_find_value);

        auto current_is_newline_or_comment = [&]() {
            return m_current_ptr < m_end_ptr && (
                *m_current_ptr == token_type::newline ||
                *m_current_ptr == token_type::carriage ||
                *m_current_ptr == token_type::comment);
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

        auto process_zero_line_indention = [&]() {
            const auto current_line_ptr = m_current_line_ptr;

            const auto codepoint = *m_current_line_ptr;
            switch (codepoint) {
            case token_type::document_start: {
                if (is_next_token(1, token_type::document_start) && is_next_token(2, token_type::document_start))
                {
                    m_current_ptr += 3;
                    if (is_next_token_whitespace(0)) {
                        if (m_stack.front().type == stack_type::unknown) {
                            if (!consume_only_whitespaces_until_newline_or_comment()) {
                                error(read_result_code::unexpected_token);
                                return false;
                            }

                            return true;
                        }

                        return false;
                    }
                }
            } break;
            case token_type::document_end: {
                if (is_next_token(1, token_type::document_end) && is_next_token(2, token_type::document_end))
                {
                    m_current_ptr += 3;
                    if (is_next_token_whitespace(0)) {
                        m_current_ptr = current_line_ptr;
                        return false;
                    }

                    m_current_ptr = current_line_ptr;
                }
            } break;
            default: break;
            };

            return true;
        };

        auto process_newline_indentation = [&]() {
            if (current_is_newline_or_comment()) {
                return true;
            }
            if (m_current_ptr == m_current_line_ptr) {
                if (!process_zero_line_indention()) {
                    return false;
                }
            }

            auto it = std::find_if(m_stack.begin(), m_stack.end(),
                [&](const stack_item_t& stack_item) {
                    return stack_item.type_indention > m_current_line_indention;
                }
            );

            if (it == m_stack.end()) {
                return true;
            }

            pop_stack_from(it);

            if (!m_stack.empty()) {
                const auto back_indention = m_stack.back().type_indention;
                if (m_current_line_indention != back_indention) {
                    error(read_result_code::bad_indentation);
                    return false;
                }
            }

            return true;
        };

        auto read_remaining_document_buffer = [&]() {
            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *(m_current_ptr++);
                switch (codepoint) {
                    case token_type::space:
                    case token_type::tab: break;
                    case token_type::comment: {
                        read_comment_until_newline();
                    } break;
                    case token_type::carriage:
                    case token_type::newline: {
                        register_newline();
                    } return;
                    default: {
                        error(read_result_code::unexpected_token);
                    } return;
                }
            }
        };

        auto process = [&]() {
            while (m_current_result_code == read_result_code::success && m_current_ptr < m_end_ptr && !m_stack.empty()) {
                if (m_stack.size() > m_options.max_depth) {
                    error(read_result_code::reached_max_depth);
                    return m_current_result_code;
                }

                if (!is_current_stack_flow_value()) {
                    if (m_current_is_new_line) {
                        if (!read_newline_indentation()) {
                            error(read_result_code::forbidden_tab_indentation);
                            return m_current_result_code;
                        }
                        if (!process_newline_indentation()) {
                            if (m_current_result_code == read_result_code::success) {
                                pop_stack_from(m_stack.begin());
                            }
                            return m_current_result_code;
                        }
                    }
                    else {
                        if (consume_whitespaces_until_newline()) {
                            continue;
                        }
                    }
                }

                if (m_stack.empty() || m_current_result_code != read_result_code::success || m_current_ptr >= m_end_ptr) {
                    break;
                }

                m_current_is_new_line = false;

                auto& stack_item = m_stack.back();
                auto state_function = stack_item.state_function;
                (this->*(state_function))();
            }

            if (m_current_result_code != read_result_code::success) {
                return m_current_result_code;
            }

            pop_stack_from(m_stack.begin());
            read_remaining_document_buffer();

            return m_current_result_code;
        };

        signal_start_document();
        const auto process_result = process();
        if (process_result == read_result_code::success) {
            signal_end_document();
        }
        return process_result;
    }

    template<typename Tchar, typename Tsax_handler>
    read_result_code reader<Tchar, Tsax_handler>::process_documents() {      
        auto document_count = size_t{ 0 };

        do {
            if (document_count >= m_options.max_document_count) {
                return read_result_code::reached_max_document_count;
            }

            // TODO: Fix current line number...
            const auto process_result = process_document();
            if (process_result != read_result_code::success) {
                return process_result;
            }

            ++document_count;

            if (m_current_ptr + 3 < m_end_ptr) {
                if (m_current_ptr[0] == token_type::document_end &&
                    m_current_ptr[1] == token_type::document_end &&
                    m_current_ptr[2] == token_type::document_end)
                {
                    return read_result_code::success;
                }
            }

        } while (m_current_ptr < m_end_ptr);

        return read_result_code::success;
    }

    template<typename Tchar, typename Tsax_handler>
    typename reader<Tchar, Tsax_handler>::read_document_result_type reader<Tchar, Tsax_handler>::create_read_document_result(read_result_code result_code) const {    
        const auto remaining_ptr = std::min(m_current_ptr, m_end_ptr);
        const auto remaining_size = static_cast<typename string_view_type::size_type>(m_end_ptr - remaining_ptr);
        
        auto result = read_document_result_type{ };
        result.result_code = result_code; 
        result.remaining_input = string_view_type{ remaining_ptr, remaining_size };
        result.current_line = m_current_line;
        result.current_line_ptr = m_current_line_ptr;
        return result;
    }

    template<typename Tchar, typename Tsax_handler>
    typename reader<Tchar, Tsax_handler>::read_document_file_result_type reader<Tchar, Tsax_handler>::create_read_document_file_result(read_result_code result_code) const {
        auto result = read_document_file_result_type{ };
        result.result_code = result_code;
        result.current_line = m_current_line;
        return result;
    }

    template<typename Tchar, typename Tsax_handler>
    typename reader<Tchar, Tsax_handler>::read_documents_result_type reader<Tchar, Tsax_handler>::create_read_documents_result(read_result_code result_code) const {
        return create_read_document_result(result_code);
    }

    template<typename Tchar, typename Tsax_handler>
    typename reader<Tchar, Tsax_handler>::read_documents_file_result_type reader<Tchar, Tsax_handler>::create_read_documents_file_result(read_result_code result_code) const {
        return create_read_document_file_result(result_code);
    }


    // State execute functions.
    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::execute_find_value() {
        auto value_start_ptr = m_current_ptr;
        auto value_end_ptr = m_current_ptr;

        auto on_found_scalar_end = [&]() {
            const auto scalar_length = static_cast<size_t>(value_end_ptr - value_start_ptr);
            if (scalar_length == 0) {
                return;
            }

            auto& stack_item = m_stack.back();
            stack_item.type = stack_type::scalar;
            stack_item.state_function = &reader::execute_read_scalar;
            signal_start_scalar(scalar_style_type::none, chomping_type::strip);
       
            const auto scalar_string_view = string_view_type{ value_start_ptr, scalar_length };
            signal_string(scalar_string_view);
        };

        auto on_scalar_block_token = [&](const scalar_style_type style) {
            auto chomping = chomping_type::clip;
            const auto next_codepoint = m_current_ptr < m_end_ptr ? *m_current_ptr : token_type::eof;
            switch (next_codepoint) {
                case token_type::eof:
                case token_type::space:
                case token_type::tab:
                case token_type::carriage:
                case token_type::newline: break;
                case token_type::chomping_strip:
                case token_type::chomping_keep: {
                    ++m_current_ptr;
                    chomping = next_codepoint == token_type::chomping_strip ? chomping_type::strip : chomping_type::keep;
                } break;
                default: {
                    error(read_result_code::expected_line_break);
                } return;
            }

            if (!consume_only_whitespaces_until_newline()) {
                error(read_result_code::expected_line_break);
                return;
            }

            auto& stack_item = m_stack.back();
            stack_item.type = stack_type::scalar;
            stack_item.state_function = &reader::execute_read_scalar_block;
            signal_start_scalar(style, chomping);
        };

        auto on_object_token = [&]() {
            auto process_new_object = [&]() {
                if (value_start_ptr != m_current_line_indention_ptr && 
                    (m_stack.size() < 2 || std::next(m_stack.rbegin())->type != stack_type::sequence))
                {
                    error(read_result_code::unexpected_key);
                    return;
                }

                auto& stack_item = m_stack.back();
                stack_item.type = stack_type::object;
                stack_item.type_indention = static_cast<int64_t>(value_start_ptr - m_current_line_ptr);
                stack_item.state_function = &reader::execute_read_key;
                signal_start_object();

                const auto key_length = static_cast<size_t>(value_end_ptr - value_start_ptr);
                const auto key_string_view = string_view_type{ value_start_ptr, key_length };
                signal_key(key_string_view);

                push_stack(&reader::execute_find_value);
            };

            const auto next_codepoint = m_current_ptr < m_end_ptr ? *m_current_ptr : token_type::eof;
            switch (next_codepoint) {
                case token_type::eof: {
                    process_new_object();
                } return true;
                case token_type::space:
                case token_type::tab: {
                    ++m_current_ptr;
                    process_new_object();
                } return true;
                case token_type::carriage:
                case token_type::newline: {
                    process_new_object();
                    ++m_current_ptr;
                    register_newline();
                } return true;
                default: break;
            }
            return false;
        };

        auto on_sequence_token = [&]() {
            auto process_new_sequence = [&]() {
                auto& stack_item = m_stack.back();
                stack_item.type = stack_type::sequence;
                stack_item.type_indention = static_cast<int64_t>(m_current_ptr - m_current_line_ptr) - int64_t{ 1 };
                stack_item.state_function = &reader::execute_read_sequence;

                signal_start_array();
                signal_index(0);
                push_stack(&reader::execute_find_value);
            };

            const auto next_codepoint = m_current_ptr < m_end_ptr ? *m_current_ptr : token_type::eof;
            switch (next_codepoint) {
                case token_type::eof:
                case token_type::space:
                case token_type::tab: {
                    process_new_sequence();
                } return true;
                case token_type::carriage:
                case token_type::newline: {
                    process_new_sequence();
                    ++m_current_ptr;
                    register_newline();
                } return true;
                default: break;
            }
            return false;
        };

        auto on_tag_token = [&]() {
            auto& current_stack = m_stack.back();
            if (current_stack.has_tag) {
                error(read_result_code::tag_duplication);
                return true;
            }

            current_stack.has_tag = true;

            if (is_next_token(0, token_type::tag)) {
                ++m_current_ptr;
            }

            value_start_ptr = m_current_ptr;
            value_end_ptr = m_current_ptr;

            auto process = [&]() {
                while (m_current_ptr < m_end_ptr) {
                    const auto codepoint = *(m_current_ptr++);
                    switch (codepoint) {
                        case token_type::space:
                        case token_type::tab: { 
                            consume_whitespaces_until_any();
                        } return true;
                        case token_type::carriage:
                        case token_type::newline: {
                            register_newline();
                        } return true;
                        default: {
                            value_end_ptr = m_current_ptr;
                        } break;
                    }
                }

                return false;
            };

            const auto process_result = process();

            const auto tag_length = static_cast<size_t>(value_end_ptr - value_start_ptr);
            const auto tag_string_view = string_view_type{ value_start_ptr, tag_length };
            signal_tag(tag_string_view);

            value_start_ptr = m_current_ptr;
            value_end_ptr = m_current_ptr;

            return process_result;
        };

        auto on_flow_scalar_token = [&](const char_type quote_token) {
            const auto scalar_style = quote_token == token_type::double_quote ? 
                scalar_style_type::double_quoted : 
                scalar_style_type::single_quoted;

            auto& stack_item = m_stack.back();
            stack_item.type = stack_type::scalar;
            stack_item.state_function = &reader::execute_read_flow_scalar_quote;
            stack_item.flow_value_token = quote_token;
            signal_start_scalar(scalar_style, chomping_type::strip);

            auto value_start_ptr = m_current_ptr;
            auto value_end_ptr = m_current_ptr;
         
            auto process_string = [&]() {             
                const auto scalar_length = static_cast<size_t>(value_end_ptr - value_start_ptr);
                const auto scalar_string_view = string_view_type{ value_start_ptr, scalar_length };
                signal_string(scalar_string_view);
            };

            auto prev_codepoint = token_type::eof;

            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *(m_current_ptr++);
                switch (codepoint) {
                    case token_type::carriage:
                    case token_type::newline: {
                        register_newline();
                        process_string();
                    } return;
                    case token_type::double_quote: {
                        if (quote_token != token_type::double_quote || prev_codepoint == token_type::escape) {
                            break;
                        }
                        process_string();
                        pop_stack();
                        if (!is_current_stack_flow_value() && !consume_only_whitespaces_until_newline()) {
                            error(read_result_code::unexpected_token);
                        }
                    } return;
                    case token_type::single_quote: {
                        if (quote_token != token_type::single_quote) {
                            break;
                        }
                        if (is_next_token(0, token_type::single_quote)) {
                            ++m_current_ptr;
                            break;
                        }
                        process_string();
                        pop_stack();
                        if (!is_current_stack_flow_value() && !consume_only_whitespaces_until_newline()) {
                            error(read_result_code::unexpected_token);
                        }
                    } return;
                    default: break;
                }

                value_end_ptr = m_current_ptr;
                prev_codepoint = codepoint;
            }
        };

        auto parse_value_type = [&]() {
            enum class first_codepoint_result_code {
                ok,
                repeat,
                exit
            };

            auto parse_first_codepoint = [&]() -> first_codepoint_result_code {
                if (m_current_ptr >= m_end_ptr) {
                    return first_codepoint_result_code::exit;
                }

                const auto first_codepoint = *(m_current_ptr++);
                switch (first_codepoint) {
                    case token_type::carriage:
                    case token_type::newline: {
                        register_newline();
                    } return first_codepoint_result_code::exit;
                    case token_type::comment: {
                        read_comment_until_newline();
                    } return first_codepoint_result_code::exit;
                    case token_type::object: {
                        if (is_next_token_whitespace(0)) {
                            error(read_result_code::expected_key);
                            return first_codepoint_result_code::exit;
                        }
                    } return first_codepoint_result_code::ok;
                    case token_type::sequence: {
                        if (on_sequence_token()) {
                            return first_codepoint_result_code::exit;
                        }
                    } return first_codepoint_result_code::ok;
                    case token_type::folded_block: {
                        on_scalar_block_token(scalar_style_type::folded);
                    } return first_codepoint_result_code::exit;
                    case token_type::literal_block: {
                        on_scalar_block_token(scalar_style_type::literal);
                    } return first_codepoint_result_code::exit;
                    case token_type::tag: {
                        if (on_tag_token()) {
                            return first_codepoint_result_code::exit;
                        }
                    } return first_codepoint_result_code::repeat;
                    case token_type::double_quote: {
                        on_flow_scalar_token(token_type::double_quote);
                    } return first_codepoint_result_code::exit;
                    case token_type::single_quote: {
                        on_flow_scalar_token(token_type::single_quote);
                    } return first_codepoint_result_code::exit;
                    default: break;
                }

                value_end_ptr = m_current_ptr;
                return first_codepoint_result_code::ok;
            };

            auto parse_first_code_point_result = first_codepoint_result_code::repeat;
            while (parse_first_code_point_result == first_codepoint_result_code::repeat) {
                parse_first_code_point_result = parse_first_codepoint();
                if (parse_first_code_point_result == first_codepoint_result_code::exit) {
                    return;
                }
            }

            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *(m_current_ptr++);
                switch (codepoint) {
                    case token_type::space:
                    case token_type::tab: break;
                    case token_type::comment: {
                        if (is_prev_token_whitespace(2)) {
                            on_found_scalar_end();
                            pop_stack_if_not_root();
                            read_comment_until_newline();
                            return;
                        }
                    } break;
                    case token_type::carriage:
                    case token_type::newline: {
                        on_found_scalar_end();
                        register_newline();
                    } return;
                    case token_type::object: {
                        if (on_object_token()) {
                            return;
                        }
                    } break;
                    default: {
                        value_end_ptr = m_current_ptr;
                    } break;
                }
            }

            on_found_scalar_end();
        };

        parse_value_type();
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::execute_read_scalar() {
        auto value_start_ptr = m_current_ptr;
        auto value_end_ptr = m_current_ptr;

        auto process = [&]() {
            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *(m_current_ptr++);
                switch (codepoint) {
                    case token_type::space:
                    case token_type::tab: break;
                    case token_type::comment: {
                        if (is_prev_token_whitespace(2)) {
                            pop_stack();
                            --m_current_ptr;
                            return value_start_ptr != value_end_ptr;
                        }
                    } break;
                    case token_type::carriage:
                    case token_type::newline: {
                        register_newline();
                    } return true;
                    case token_type::object: {
                        const auto next_codepoint = m_current_ptr < m_end_ptr ? *m_current_ptr : token_type::eof;
                        switch (next_codepoint) {
                            case token_type::eof:
                            case token_type::space:
                            case token_type::tab:
                            case token_type::carriage: {
                                error(read_result_code::unexpected_key);
                            } return false;
                            default: {
                                value_end_ptr = m_current_ptr;
                            } break;
                        }
                    } break;
                    default: {
                        value_end_ptr = m_current_ptr;
                    } break;
                }
            }
            return true;
        };

        if(!process()) {
            return;
        }

        const auto scalar_length = static_cast<size_t>(value_end_ptr - value_start_ptr);
        const auto scalar_string_view = string_view_type{ value_start_ptr, scalar_length };
        signal_string(scalar_string_view);    
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::execute_read_scalar_block() {
        const auto current_line_indention = m_current_line_indention;
        auto value_start_ptr = m_current_ptr;
        auto value_end_ptr = m_current_ptr;

        auto process = [&]() {
            const auto first_codepoint = *m_current_ptr;
            if (first_codepoint == token_type::comment) {
                pop_stack();
                return false;
            }

            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *(m_current_ptr++);
                switch (codepoint) {
                    case token_type::carriage:
                    case token_type::newline: {
                        register_newline();
                    } return true;
                    default: {
                        value_end_ptr = m_current_ptr;
                    } break;
                }
            }

            return true;
        };

        if (!process()) {
            return;
        }

        auto& stack_item = m_stack.back();
        const auto scalar_length = static_cast<size_t>(value_end_ptr - value_start_ptr);

        if (stack_item.processed_lines == 0 && scalar_length != 0) {
            ++stack_item.processed_lines;
            stack_item.type_indention = current_line_indention;    
        }
        else if (stack_item.processed_lines > 0) {
            ++stack_item.processed_lines;
        }

        const auto left_padding = std::max(current_line_indention - stack_item.type_indention, int64_t{ 0 });
        const auto scalar_string_view = string_view_type{ value_start_ptr - left_padding, scalar_length + left_padding };
        signal_string(scalar_string_view);
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::execute_read_key() {
        auto value_start_ptr = m_current_ptr;
        auto value_end_ptr = m_current_ptr;

        auto process = [&]() {
            const auto first_codepoint = *m_current_ptr;
            if(first_codepoint == token_type::comment) {
                ++m_current_ptr;
                read_comment_until_newline();
                return false;
            }

            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *(m_current_ptr++);
                switch (codepoint) {
                    case token_type::space:
                    case token_type::tab: break;
                    case token_type::comment: {
                        if (is_prev_token_whitespace(2)) {
                            error(read_result_code::expected_key);
                            return false;
                        }
                    } break;
                    case token_type::carriage:
                    case token_type::newline: {
                        if (value_start_ptr != value_end_ptr) {
                            error(read_result_code::expected_key);
                        }
                    } return false;
                    case token_type::object: {
                        const auto next_codepoint = m_current_ptr < m_end_ptr ? *m_current_ptr : token_type::eof;
                        switch (next_codepoint) {
                            case token_type::eof: return true;
                            case token_type::space:
                            case token_type::tab: {
                                ++m_current_ptr;
                            } return true;
                            case token_type::carriage:
                            case token_type::newline: {
                                ++m_current_ptr;
                                register_newline();
                            } return true;
                            default: break;
                        }
                    } break;
                    default: {
                        value_end_ptr = m_current_ptr;
                    } break;
                }
            }

            error(read_result_code::expected_key);
            return false;
        };

        if (!process()) {
            return;
        }

        const auto key_length = static_cast<size_t>(value_end_ptr - value_start_ptr);
        const auto key_string_view = string_view_type{ value_start_ptr, key_length };
        signal_key(key_string_view);

        push_stack(&reader::execute_find_value);
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::execute_read_sequence() {
        auto process = [&]() {
            const auto first_codepoint = *m_current_ptr;

            switch (first_codepoint) {
                case token_type::carriage:
                case token_type::newline: {
                    ++m_current_ptr;
                    register_newline();
                } return false;
                case token_type::comment: {
                    ++m_current_ptr;
                    read_comment_until_newline();  
                } return false;
                case token_type::sequence: {
                    ++m_current_ptr;
                    const auto next_codepoint = m_current_ptr < m_end_ptr ? *m_current_ptr : token_type::eof;
                    switch (next_codepoint) {
                        case token_type::eof:
                        case token_type::space:
                        case token_type::tab: return true;
                        case token_type::carriage:
                        case token_type::newline: {
                            register_newline();
                        } return true;
                        default: break;
                    }
                } break;
                default: break;
            }

            error(read_result_code::expected_sequence);
            return false;
        };

        if (!process()) {
            return;
        }

        auto& stack_item = m_stack.back();
        ++stack_item.processed_lines;

        signal_index(stack_item.processed_lines);

        push_stack(&reader::execute_find_value);
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::execute_read_flow_scalar_quote() {
        consume_whitespaces_until_any();

        const auto quote_token = m_stack.back().flow_value_token;

        auto prev_codepoint = token_type::eof;
        auto value_start_ptr = m_current_ptr;
        auto value_end_ptr = m_current_ptr;

        auto process_string = [&]() {
            const auto scalar_length = static_cast<size_t>(value_end_ptr - value_start_ptr);
            const auto scalar_string_view = string_view_type{ value_start_ptr, scalar_length };
            signal_string(scalar_string_view);
        };

        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token_type::space:
                case token_type::tab: break;
                case token_type::carriage:
                case token_type::newline: {
                    register_newline();
                    process_string();
                } return;
                case token_type::double_quote: {
                    if (quote_token != token_type::double_quote || prev_codepoint == token_type::escape) {
                        value_end_ptr = m_current_ptr;
                        break;
                    }
                    process_string();
                    pop_stack();
                    if (!is_current_stack_flow_value() && !consume_only_whitespaces_until_newline()) {
                        error(read_result_code::unexpected_token);
                    }
                } return;
                case token_type::single_quote: {
                    if (quote_token != token_type::single_quote) {
                        break;
                    }
                    if (is_next_token(0, token_type::single_quote)) {
                        ++m_current_ptr;
                        break;
                    }
                    process_string();
                    pop_stack();
                    if (!is_current_stack_flow_value() && !consume_only_whitespaces_until_newline()) {
                        error(read_result_code::unexpected_token);
                    }
                } return;
                default: {
                    value_end_ptr = m_current_ptr;
                } break;
            }

            prev_codepoint = codepoint;
        }
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::register_newline() {
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
    void reader<Tchar, Tsax_handler>::register_line_indentation() {
        ++m_current_line_indention;
        ++m_current_line_indention_ptr;
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::read_comment_until_newline() {
        consume_whitespaces_until_any();

        auto comment_start_ptr = m_current_ptr;
        auto comment_end_ptr = m_current_ptr;

        auto read_comment = [&]() {
            while (m_current_ptr < m_end_ptr) {
                const auto codepoint = *(m_current_ptr++);
                switch (codepoint) {
                    case token_type::space:
                    case token_type::tab: break;
                    case token_type::carriage:
                    case token_type::newline: {
                        register_newline();
                    } return;
                    default: {
                        comment_end_ptr = m_current_ptr;
                    } break;
                }
            }
        };
  
        read_comment();

        const auto comment_length = static_cast<size_t>(comment_end_ptr - comment_start_ptr);
        const auto comment_string_view = string_view_type{ comment_start_ptr, comment_length };
        signal_comment(comment_string_view);
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::consume_whitespaces_until_any() {
        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *m_current_ptr;
            switch (codepoint) {
                case token_type::space:
                case token_type::tab: ++m_current_ptr; break;
                default: return;
            }
        }
    }

    template<typename Tchar, typename Tsax_handler>
    bool reader<Tchar, Tsax_handler>::consume_whitespaces_until_newline() {
        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *m_current_ptr;
            switch (codepoint) {
                case token_type::space:
                case token_type::tab: {
                    ++m_current_ptr;
                } break;
                case token_type::carriage:
                case token_type::newline: {
                    ++m_current_ptr;
                    register_newline();
                } return true;
                default: return false;
            }
        }
        return true;
    }

    template<typename Tchar, typename Tsax_handler>
    bool reader<Tchar, Tsax_handler>::consume_only_whitespaces_until_newline() {
        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token_type::space:
                case token_type::tab: break;
                case token_type::carriage:
                case token_type::newline: register_newline(); return true;
                case token_type::comment: read_comment_until_newline(); return true;
                default: return false;
            }
        }
        return true;
    }

    template<typename Tchar, typename Tsax_handler>
    bool reader<Tchar, Tsax_handler>::consume_only_whitespaces_until_newline_or_comment() {
        while (m_current_ptr < m_end_ptr) {
            const auto codepoint = *(m_current_ptr++);
            switch (codepoint) {
                case token_type::space:
                case token_type::tab: break;
                case token_type::carriage:
                case token_type::newline: register_newline(); return true;
                case token_type::comment: --m_current_ptr;  return true;
                default: return false;
            }
        }
        return true;
    }

    template<typename Tchar, typename Tsax_handler>
    bool reader<Tchar, Tsax_handler>::has_min_tokens_left(size_t count) const {
        return m_current_ptr + count < m_end_ptr;
    }

    template<typename Tchar, typename Tsax_handler>
    bool reader<Tchar, Tsax_handler>::is_next_token(size_t increments, const char_type value) const {
        if (m_current_ptr + increments >= m_end_ptr) {
            return true;
        }

        return *(m_current_ptr + increments) == value;
    }

    template<typename Tchar, typename Tsax_handler>
    bool reader<Tchar, Tsax_handler>::is_prev_token_whitespace(int decrements) const {
        if (m_current_ptr - decrements < m_begin_ptr) {
            return true;
        }

        const auto codepoint = *(m_current_ptr - decrements);
        switch (codepoint) {
            case token_type::space:
            case token_type::tab:
            case token_type::carriage:
            case token_type::newline: return true;
            default: break;
        }

        return false;  
    }

    template<typename Tchar, typename Tsax_handler>
    bool reader<Tchar, Tsax_handler>::is_next_token_whitespace(int increments) const {
        if (m_current_ptr + increments >= m_end_ptr) {
            return true;
        }

        const auto codepoint = *(m_current_ptr + increments);
        switch (codepoint) {
            case token_type::space:
            case token_type::tab:
            case token_type::carriage:
            case token_type::newline: return true;
            default: break;
        }

        return false;
    }

    template<typename Tchar, typename Tsax_handler>
    bool reader<Tchar, Tsax_handler>::is_current_stack_flow_value() const {
        return !m_stack.empty() && m_stack.back().is_flow_value();
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_start_document() {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_start_document<Tsax_handler>() == true) {
            m_sax_handler.start_document();
        }
#else
        m_sax_handler.start_document();
#endif
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_end_document() {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_end_document<Tsax_handler>() == true) {
            m_sax_handler.end_document();
        }
#else
        m_sax_handler.end_document();
#endif
    }


    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_start_scalar(scalar_style_type style, chomping_type chomping) {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_start_scalar<Tsax_handler>() == true) {
            m_sax_handler.start_scalar(style, chomping);
        }
#else
        m_sax_handler.start_scalar(style, chomping);
#endif
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_end_scalar() {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_end_scalar<Tsax_handler>() == true) {
            m_sax_handler.end_scalar();
        }
 #else
        m_sax_handler.end_scalar();
 #endif
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_start_object() {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_start_object<Tsax_handler>() == true) {
            m_sax_handler.start_object();
        }       
#else
        m_sax_handler.start_object();
#endif
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_end_object() {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_end_object<Tsax_handler>() == true) {
            m_sax_handler.end_object();
        }
#else
        m_sax_handler.end_object();
#endif
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_start_array() {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_start_array<Tsax_handler>() == true) {
            m_sax_handler.start_array();
        }
#else
        m_sax_handler.start_array();
#endif  
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_end_array() {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_end_array<Tsax_handler>() == true) {
            m_sax_handler.end_array();
        }
#else
        m_sax_handler.end_array();
#endif 
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_null() {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_null<Tsax_handler>() == true) {
            m_sax_handler.null();
        }
#else
        m_sax_handler.null();
#endif 
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_index(size_t value) {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_index<Tsax_handler>() == true) {
            m_sax_handler.index(value);
        }
#else
        m_sax_handler.index(value);
#endif 
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_string(string_view_type value) {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_string<Tchar, Tsax_handler>() == true) {
            m_sax_handler.string(value);
        }
#else
        m_sax_handler.string(value);
#endif 
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_key(string_view_type value) {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_key<Tchar, Tsax_handler>() == true) {
            m_sax_handler.key(value);
        }
#else
        m_sax_handler.key(value);
#endif 
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_tag(string_view_type value) {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_tag<Tchar, Tsax_handler>() == true) {
            m_sax_handler.tag(value);
        }
#else
        m_sax_handler.tag(value);
#endif 
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_comment(string_view_type value) {
#if MINIYAML_HAS_IF_CONSTEXPR
        if constexpr (impl::sax_handler_has_comment<Tchar, Tsax_handler>() == true) {
            m_sax_handler.comment(value);
        }
#else
        m_sax_handler.comment(value);
#endif 
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::error(const read_result_code result_code) {
        m_current_result_code = result_code;
    }

    template<typename Tchar, typename Tsax_handler>
    typename reader<Tchar, Tsax_handler>::stack_item_t& reader<Tchar, Tsax_handler>::push_stack(state_function_t state_function) {
        const auto type_indention = m_stack.empty() ? size_t{ 0 } : m_stack.back().type_indention + 1;
        m_stack.emplace_back();
        auto& new_stack = m_stack.back();
        new_stack.state_function = state_function;
        new_stack.type_indention = type_indention;
        return new_stack;
    };

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::pop_stack() {
        if (m_stack.empty()) {
            return;
        }

        auto& stack_item = m_stack.back();  
        signal_stack_item_pop(stack_item);
        m_stack.pop_back();
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::pop_stack_if_not_root() {
        if (m_stack.size() < 2) {
            return;
        }

        auto& stack_item = m_stack.back();
        signal_stack_item_pop(stack_item);
        m_stack.pop_back();
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::pop_stack_from(stack_iterator_t it) {
        const auto rit_end = stack_reverse_iterator_t(it);
        for (auto rit = m_stack.rbegin(); rit != rit_end; ++rit) {
            const auto& stack_item = *rit;
            if (stack_item.is_flow_value() && m_current_ptr >= m_end_ptr) {
                return error(read_result_code::unexpected_eof);
            }
            signal_stack_item_pop(stack_item);
        }
        m_stack.erase(it, m_stack.end());
    }

    template<typename Tchar, typename Tsax_handler>
    void reader<Tchar, Tsax_handler>::signal_stack_item_pop(const stack_item_t& stack_item) {
        switch (stack_item.type) {
            case stack_type::unknown: signal_null(); break;
            case stack_type::scalar: signal_end_scalar(); break;
            case stack_type::object: signal_end_object(); break;
            case stack_type::sequence: signal_end_array(); break;
        }
    }

    // SAX reader result implementations.
    template<typename Tchar>
    read_document_result<Tchar>::operator bool() const {
        return result_code == read_result_code::success;
    }

    template<typename Tchar>
    read_document_file_result<Tchar>::operator bool() const {
        return result_code == read_result_code::success;
    }


    // Global SAX function implementations.
    template<typename Tchar, typename Tsax_handler>
    read_document_result<Tchar> read_document(
        const Tchar* raw_input,
        size_t size,
        Tsax_handler& handler,
        const reader_options& options)
    {
        return reader<Tchar, Tsax_handler>{ handler, options }.read_document(raw_input, size);
    }

    template<typename Tchar, typename Tsax_handler>
    read_document_result<Tchar> read_document(
        const std::basic_string<Tchar>& string,
        Tsax_handler& handler,
        const reader_options& options)
    {
        return reader<Tchar, Tsax_handler>{ handler, options }.read_document(string);
    }

    template<typename Tchar, typename Tsax_handler>
    read_document_result<Tchar> read_document(
        basic_string_view<Tchar> string_view,
        Tsax_handler& handler,
        const reader_options& options)
    {
        return reader<Tchar, Tsax_handler>{ handler, options }.read_document(string_view);
    }

    template<typename Tchar, typename Tsax_handler>
    read_document_file_result<Tchar> read_document_from_file(
        const std::string& filename,
        Tsax_handler& handler,
        const reader_options& options)
    {
        return reader<Tchar, Tsax_handler>{ handler, options }.read_document_from_file(filename);
    }

    template<typename Tchar, typename Tsax_handler>
    read_documents_result<Tchar> read_documents(
        const Tchar* raw_input,
        size_t size,
        Tsax_handler& handler,
        const reader_options& options)
    {
        return reader<Tchar, Tsax_handler>{ handler, options }.read_documents(raw_input, size);
    }

    template<typename Tchar, typename Tsax_handler>
    read_documents_result<Tchar> read_documents(
        const std::basic_string<Tchar>& string,
        Tsax_handler& handler,
        const reader_options& options)
    {
        return reader<Tchar, Tsax_handler>{ handler, options }.read_documents(string);
    }

    template<typename Tchar, typename Tsax_handler>
    read_documents_result<Tchar> read_documents(
        basic_string_view<Tchar> string_view,
        Tsax_handler& handler,
        const reader_options& options)
    {
        return reader<Tchar, Tsax_handler>{ handler, options }.read_documents(string_view);
    }

    template<typename Tchar, typename Tsax_handler>
    read_documents_file_result<Tchar> read_documents_from_file(
        const std::string& filename,
        Tsax_handler& handler,
        const reader_options& options)
    {
        return reader<Tchar, Tsax_handler>{ handler, options }.read_documents_from_file(filename);
    }

} }

// Hidden SAX implementation details.
namespace MINIYAML_NAMESPACE {
namespace dom {
namespace impl {

    template<typename Tchar, typename Ttraits>
    using scalar_line_const_iterator = typename scalar_node<Tchar, Ttraits>::const_iterator;

    template<typename Tchar, typename Ttraits>
    using scalar_line_const_iterator_pairs = std::pair<
        scalar_line_const_iterator<Tchar, Ttraits>, 
        scalar_line_const_iterator<Tchar, Ttraits>>;

    template<typename Tchar, typename Ttraits>
    scalar_line_const_iterator_pairs<Tchar, Ttraits> get_non_empty_scalar_node_lines(
        const scalar_node<Tchar, Ttraits>& scalar)
    {
        auto begin_it = std::find_if(scalar.begin(), scalar.end(),
            [](const typename scalar_node<Tchar, Ttraits>::string_type& line) {
                return !MINIYAML_NAMESPACE::impl::is_empty_or_whitespace(line); 
        });

        if (begin_it == scalar.end()) {
            return { begin_it, begin_it };
        }

        auto rend_it = std::find_if(scalar.rbegin(), scalar.rend(),
            [](const typename scalar_node<Tchar, Ttraits>::string_type& line) {
                return !MINIYAML_NAMESPACE::impl::is_empty_or_whitespace(line); 
        });

        auto end_it = rend_it.base();

        return { begin_it, end_it };
    }

    template<typename Tchar, typename Ttraits>
    size_t get_line_count(const scalar_line_const_iterator_pairs<Tchar, Ttraits>& it_pair)
    {
        return std::distance(it_pair.first, it_pair.second);
    }

    template<typename Tchar, typename Ttraits>
    void append_chomping(
        std::basic_string<Tchar>& string,
        const scalar_node<Tchar, Ttraits>& scalar,
        scalar_line_const_iterator<Tchar, Ttraits> first_empty_end_it)
    {
        const auto chomping = scalar.chomping();
        if (chomping == chomping_type::keep && first_empty_end_it != scalar.end()) {
            auto post_newline_count = static_cast<size_t>(std::distance(first_empty_end_it, scalar.end()));
            string.append(post_newline_count, '\n');
        }
        if (chomping != chomping_type::strip) {
            string += '\n';
        }
    }

    template<typename Tchar, typename Ttraits>
    static std::basic_string<Tchar> get_as_non_block_string(
        const scalar_node<Tchar, Ttraits>& scalar,
        const std::basic_string<Tchar> default_value)
    {
        const auto it_pair = impl::get_non_empty_scalar_node_lines(scalar);
        const auto begin_it = it_pair.first;
        const auto end_it = it_pair.second;

        if (begin_it == end_it) {
            return default_value;
        }

        auto prev_end_it = std::prev(end_it);

        auto result = std::basic_string<Tchar>{};

        auto reserved_size = size_t{ 0 };
        for (auto it = begin_it; it != end_it; ++it) {
            reserved_size += it->size() + 1;
        }
        result.reserve(reserved_size);

        bool prev_line_has_value = false;
        for (auto it = begin_it; it != prev_end_it; ++it) {
            if (it->empty()) {
                result += '\n';
            }
            else if (prev_line_has_value) {
                result += ' ';
            }

            result += *it;
            prev_line_has_value = !it->empty();
        }

        if (prev_line_has_value) {
            result += ' ';
        }
        result += *prev_end_it;

        return result;
    }

    template<typename Tchar, typename Ttraits>
    static std::basic_string<Tchar> get_as_literal_string(
        const scalar_node<Tchar, Ttraits>& scalar,
        const std::basic_string<Tchar> default_value)
    {
        const auto it_pair = impl::get_non_empty_scalar_node_lines(scalar);
        const auto begin_it = it_pair.first;
        const auto end_it = it_pair.second;

        if (begin_it == end_it) {
            return default_value;
        }

        auto result = std::basic_string<Tchar>{};

        auto pre_newline_count = static_cast<size_t>(std::distance(scalar.begin(), begin_it));
        result.append(pre_newline_count, '\n');

        auto prev_end_it = std::prev(end_it);
        for (auto it = begin_it; it != prev_end_it; ++it) {
            result += *it;
            result += '\n';
        }
        result += *prev_end_it;

        append_chomping(result, scalar, end_it);

        return result;
    }

    template<typename Tchar, typename Ttraits>
    static std::basic_string<Tchar> get_as_folded_string(
        const scalar_node<Tchar, Ttraits>& scalar,
        const std::basic_string<Tchar> default_value)
    {
        const auto it_pair = impl::get_non_empty_scalar_node_lines(scalar);
        const auto begin_it = it_pair.first;
        const auto end_it = it_pair.second;

        if (begin_it == end_it) {
            return default_value;
        }  

        auto result = std::basic_string<Tchar>{};

        auto pre_newline_count = static_cast<size_t>(std::distance(scalar.begin(), begin_it));
        result.append(pre_newline_count, '\n');

        bool prev_line_has_value = false;
        for (auto it = begin_it; it != end_it; ++it) {
            auto& line = *it;

            if (line.empty() || line.front() == ' ') {
                result += '\n';
            }
            else if (prev_line_has_value) {
                result += ' ';
            }
            result += *it;

            prev_line_has_value = !line.empty();
        }

        append_chomping(result, scalar, end_it);

        return result;
    }

    template<typename Tchar, typename Ttraits, typename TappendLineCallback>
    static std::basic_string<Tchar> get_as_quoted_string(
        const scalar_node<Tchar, Ttraits>& scalar,
        TappendLineCallback appendLineCallback,
        const std::basic_string<Tchar> default_value) 
    {
        auto it_pair = impl::get_non_empty_scalar_node_lines(scalar);
        auto begin_empty_it = it_pair.first;
        auto end_empty_it = it_pair.second;

        if (begin_empty_it == end_empty_it) {
            return default_value;
        }

        auto result = std::basic_string<Tchar>{};

        auto append_start_end = [&](
            scalar_line_const_iterator<Tchar, Ttraits> begin, 
            scalar_line_const_iterator<Tchar, Ttraits> end)
        {
            auto count = static_cast<size_t>(std::distance(begin, end));
            if (count > 0) {
                if (count == 1) {
                    result += " ";
                }
                else {
                    result.append(count - 1, '\n');
                }
            }
        };

        append_start_end(scalar.begin(), begin_empty_it);

        bool prev_has_value = false;
        auto it = begin_empty_it;
        while (it != end_empty_it) {
            auto empty_end_it = std::find_if(it, end_empty_it,
                [](const typename scalar_node<Tchar, Ttraits>::string_type& line) { 
                    return !MINIYAML_NAMESPACE::impl::is_empty_or_whitespace(line); 
            });

            auto empty_count = static_cast<size_t>(std::distance(it, empty_end_it));
            it = empty_end_it;

            if (empty_count > 0) {
                prev_has_value = false;
                result.append(empty_count, '\n');
            }

            if (it == end_empty_it) {
                break;
            }

            if (prev_has_value) {
                result += " ";
            }

            appendLineCallback(result, *it);
            ++it;
            prev_has_value = true;
        }

        append_start_end(end_empty_it, scalar.end());

        return result;
    }

    template<typename Tchar, typename Ttraits>
    static std::basic_string<Tchar> get_as_double_quoted_string(
        const scalar_node<Tchar, Ttraits>& scalar,
        const std::basic_string<Tchar> default_value)
    {
        auto append_line = [](
            std::basic_string<Tchar>& string, 
            const typename scalar_node<Tchar, Ttraits>::string_type& line) 
        {
            using token_type = token<Tchar>;
            const auto line_size = line.size();
            for (size_t i = 0; i < line.size(); i++) {       
                if (i + 1 >= line_size || line[i] != token_type::escape) {
                    string += line[i];
                    continue;
                }
                ++i;
                switch (line[i]) {
                    case '0': string += '\0'; break;
                    case 'r': string += '\r'; break;
                    case 'n': string += '\n'; break;
                    case 't': string += '\t'; break;
                    case '"': string += '\"'; break;
                    case '\'': string += '\''; break;
                    default: string += '\\' + line[i]; break;
                }
            }
        };

        return get_as_quoted_string(scalar, append_line, default_value);
    }

    template<typename Tchar, typename Ttraits>
    static std::basic_string<Tchar> get_as_single_quoted_string(
        const scalar_node<Tchar, Ttraits>& scalar,
        const std::basic_string<Tchar> default_value)
    {
        auto append_line = [](
            std::basic_string<Tchar>& string,
            const typename scalar_node<Tchar, Ttraits>::string_type& line)
        {
            using token_type = token<Tchar>;
            const auto line_size = line.size();
            for (size_t i = 0; i < line.size(); i++) {
                if (i + 1 >= line_size || line[i] != token_type::single_quote || line[i + 1] != token_type::single_quote) {
                    string += line[i];
                    continue;
                }
                ++i;       
                string += '\'';
            }
        };

        return get_as_quoted_string(scalar, append_line, default_value);
    }


    // node_converter - T
    template<typename Tchar, typename Ttraits, typename Tvalue>
    struct node_converter {
        static Tvalue get(const node<Tchar, Ttraits>& node, const Tvalue default_value) {
            switch (node.type()) {
                case node_type::scalar: return node.as_scalar().template as<Tvalue>(default_value);
                case node_type::null: 
                case node_type::object:
                case node_type::array:
                default: break;
            }
            return default_value;
        }
    };

    // node_converter - T
    template<typename Tchar, typename Ttraits>
    struct node_converter<Tchar, Ttraits, std::basic_string<Tchar>> {
        static std::basic_string<Tchar> get(
            const node<Tchar, Ttraits>& node, 
            const std::basic_string<Tchar> default_value) 
        {
            switch (node.type()) {
                case node_type::scalar: return node.as_scalar().template as<std::basic_string<Tchar>>(default_value);
                case node_type::null: return "null";
                case node_type::object:
                case node_type::array:
                default: break;
            }
            return default_value;
        }
    };

    // scalar_converter - T
    template<typename Tchar, typename Ttraits, typename Tvalue>
    struct scalar_converter {
        static Tvalue get(const scalar_node<Tchar, Ttraits>& scalar, const Tvalue default_value);
    };

    // scalar_converter - bool
    template<typename Tchar, typename Ttraits>
    struct scalar_converter<Tchar, Ttraits, bool> {
        static bool get(const scalar_node<Tchar, Ttraits>& scalar, const bool default_value) {
            const auto it_pair = impl::get_non_empty_scalar_node_lines(scalar);
            if (impl::get_line_count<Tchar, Ttraits>(it_pair) != 1) {
                return default_value;
            }

            const auto string = MINIYAML_NAMESPACE::impl::get_string_from_string_or_view(*it_pair.first);

            switch (string[0]) {
                case 'f': if (string == "false") { return false; } break;
                case 'F': if (string.size() == 5 && (string == "False" || string == "FALSE")) { return false; } break;
                case 't': if (string == "true") { return true; } break;
                case 'T': if (string.size() == 4 && (string == "True" || string == "TRUE")) { return true; } break;
                case 'y': if (string == "yes") { return true; } break;
                case 'Y': if (string.size() == 3 && (string == "Yes" || string == "YES")) { return true; } break;
                case 'n': if (string == "no") { return false; } break;
                case 'N': if (string.size() == 2 && (string == "No" || string == "NO")) { return false; } break;
                default: break;
            }
          
            return default_value;
        }
    };

    // scalar_converter - int32_t
    template<typename Tchar, typename Ttraits>
    struct scalar_converter<Tchar, Ttraits, int32_t> {
        static int32_t get(const scalar_node<Tchar, Ttraits>& scalar, const int32_t default_value) {
            const auto default_value_64 = static_cast<int64_t>(default_value);
            const auto result = scalar_converter<Tchar, Ttraits, int64_t>::get(scalar, default_value_64);
            if (result > static_cast<int64_t>(std::numeric_limits<int32_t>::max()) ||
                result < static_cast<int64_t>(std::numeric_limits<int32_t>::min()))
            {
                return default_value;
            }
            return static_cast<int32_t>(result);
        }
    };

    // scalar_converter - int64_t
    template<typename Tchar, typename Ttraits>
    struct scalar_converter<Tchar, Ttraits, int64_t> {
        static int64_t get(const scalar_node<Tchar, Ttraits>& scalar_node, const int64_t default_value) {
            static_assert(sizeof(Tchar) == 1, "Cannot convert scalar_node<T, Ttraits> when sizeof(Tchar) != 1.");

            const auto it_pair = impl::get_non_empty_scalar_node_lines(scalar_node);
            if (impl::get_line_count<Tchar, Ttraits>(it_pair) != 1) {
                return default_value;
            }

            auto string = MINIYAML_NAMESPACE::impl::get_string_from_string_or_view(*it_pair.first);
            auto convert_start = reinterpret_cast<const char*>(string.data());
        
            errno = 0;
            char* convert_end = nullptr;
            const auto result = std::strtoll(convert_start, &convert_end, 0);

            return convert_end == convert_start || (errno == ERANGE && (result == LLONG_MAX || result == LLONG_MIN)) ?
                default_value :
                result;
        }
    };

    // scalar_converter - int32_t
    template<typename Tchar, typename Ttraits>
    struct scalar_converter<Tchar, Ttraits, uint32_t> {
        static uint32_t get(const scalar_node<Tchar, Ttraits>& scalar, const uint32_t default_value) {
            const auto default_value_64 = static_cast<uint64_t>(default_value);
            const auto result = scalar_converter<Tchar, Ttraits, uint64_t>::get(scalar, default_value_64);
            if (result > static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()))
            {
                return default_value;
            }
            return static_cast<uint32_t>(result);
        }
    };

    // scalar_converter - uint64_t
    template<typename Tchar, typename Ttraits>
    struct scalar_converter<Tchar, Ttraits, uint64_t> {
        static uint64_t get(const scalar_node<Tchar, Ttraits>& scalar, const uint64_t default_value) {
            static_assert(sizeof(Tchar) == 1, "Cannot convert scalar_node<T, Ttraits> when sizeof(Tchar) != 1.");

            const auto it_pair = impl::get_non_empty_scalar_node_lines(scalar);
            if (impl::get_line_count<Tchar, Ttraits>(it_pair) != 1) {
                return default_value;
            }

            auto string = MINIYAML_NAMESPACE::impl::get_string_from_string_or_view(*it_pair.first);
            auto convert_start = reinterpret_cast<const char*>(string.data());

            errno = 0;
            char* convert_end = nullptr;
            const auto result = std::strtoull(convert_start, &convert_end, 0);

            return convert_end == convert_start || (errno == ERANGE && result == ULLONG_MAX) ?
                default_value :
                result;
        }
    };

    // scalar_converter - float
    template<typename Tchar, typename Ttraits>
    struct scalar_converter<Tchar, Ttraits, float> {
        static float get(const scalar_node<Tchar, Ttraits>& scalar, const float default_value) {
            static_assert(sizeof(Tchar) == 1, "Cannot convert scalar_node<T, Ttraits> when sizeof(Tchar) != 1.");

            const auto it_pair = impl::get_non_empty_scalar_node_lines(scalar);
            if (impl::get_line_count<Tchar, Ttraits>(it_pair) != 1) {
                return default_value;
            }

            auto string = MINIYAML_NAMESPACE::impl::get_string_from_string_or_view(*it_pair.first);
            auto convert_start = reinterpret_cast<const char*>(string.data());

            errno = 0;
            char* convert_end = nullptr;
            const auto result = std::strtof(convert_start, &convert_end);

            return convert_end == convert_start || (errno == ERANGE && result == HUGE_VALF) || std::isinf(result) ?
                default_value :
                result;
        }
    };

    // scalar_converter - double
    template<typename Tchar, typename Ttraits>
    struct scalar_converter<Tchar, Ttraits, double> {
        static double get(const scalar_node<Tchar, Ttraits>& scalar, const double default_value) {
            static_assert(sizeof(Tchar) == 1, "Cannot convert scalar_node<T, Ttraits> when sizeof(Tchar) != 1.");

            const auto it_pair = impl::get_non_empty_scalar_node_lines(scalar);
            if (impl::get_line_count<Tchar, Ttraits>(it_pair) != 1) {
                return default_value;
            }

            auto string = MINIYAML_NAMESPACE::impl::get_string_from_string_or_view(*it_pair.first);
            auto convert_start = reinterpret_cast<const char*>(string.data());

            errno = 0;
            char* convert_end = nullptr;
            const auto result = std::strtod(convert_start, &convert_end);

            return convert_end == convert_start || (errno == ERANGE && result == HUGE_VAL) || std::isinf(result) ?
                default_value :
                result;
        }
    };

    // scalar_converter - double
    template<typename Tchar, typename Ttraits>
    struct scalar_converter<Tchar, Ttraits, long double> {
        static long double get(const scalar_node<Tchar, Ttraits>& scalar, const long double default_value) {
            static_assert(sizeof(Tchar) == 1, "Cannot convert scalar_node<T, Ttraits> when sizeof(Tchar) != 1.");

            const auto it_pair = impl::get_non_empty_scalar_node_lines(scalar);
            if (impl::get_line_count<Tchar, Ttraits>(it_pair) != 1) {
                return default_value;
            }

            auto string = MINIYAML_NAMESPACE::impl::get_string_from_string_or_view(*it_pair.first);
            auto convert_start = reinterpret_cast<const char*>(string.data());

            errno = 0;
            char* convert_end = nullptr;
            const auto result = std::strtold(convert_start, &convert_end);

            return convert_end == convert_start || (errno == ERANGE && result == HUGE_VALL) || std::isinf(result) ?
                default_value :
                result;
        }
    };

    // scalar_converter - std::basic_string<Tchar>
    template<typename Tchar, typename Ttraits>
    struct scalar_converter<Tchar, Ttraits, std::basic_string<Tchar>> {
        static std::basic_string<Tchar> get(const scalar_node<Tchar, Ttraits>& scalar, const std::basic_string<Tchar> default_value) {
            static_assert(sizeof(Tchar) == 1, "Cannot convert scalar_node<T, Ttraits> when sizeof(Tchar) != 1.");

            switch (scalar.style()) {
                case scalar_style_type::none: return get_as_non_block_string(scalar, default_value);
                case scalar_style_type::literal: return get_as_literal_string(scalar, default_value);
                case scalar_style_type::folded: return get_as_folded_string(scalar, default_value);
                case scalar_style_type::double_quoted: return get_as_double_quoted_string(scalar, default_value);
                case scalar_style_type::single_quoted: return get_as_single_quoted_string(scalar, default_value);
            }

            return default_value;
        }
    };

} } }


// DOM implementations
namespace MINIYAML_NAMESPACE {
namespace dom {


    // DOM node implementations.
    template<typename Tchar, typename Ttraits>
    node<Tchar, Ttraits> node<Tchar, Ttraits>::create_scalar(scalar_style_type style, chomping_type chomping) {
        auto result = node{};
        result.m_node_type = node_type::scalar;
        result.m_underlying_node.scalar = new scalar_node_type{ result, style, chomping };
        return result;
    }

    template<typename Tchar, typename Ttraits>
    node<Tchar, Ttraits> node<Tchar, Ttraits>::create_object() {
        auto result = node{};
        result.m_node_type = node_type::object;
        result.m_underlying_node.object = new object_node_type{ result };
        return result;
    }

    template<typename Tchar, typename Ttraits>
    node<Tchar, Ttraits> node<Tchar, Ttraits>::create_array() {
        auto result = node{};
        result.m_node_type = node_type::array;
        result.m_underlying_node.array = new array_node_type{ result };
        return result;
    }

    template<typename Tchar, typename Ttraits>
    node<Tchar, Ttraits>::node() :
        m_node_type{ node_type::null },
        m_underlying_node{ nullptr },
        m_tag{}
    {}

    template<typename Tchar, typename Ttraits>
    node<Tchar, Ttraits>::~node() {
        destroy_underlying_node();
    }

    template<typename Tchar, typename Ttraits>
    node<Tchar, Ttraits>::node(node&& rhs) noexcept :
        m_node_type{ rhs.m_node_type },
        m_underlying_node{ rhs.m_underlying_node },
        m_tag{ std::move(rhs.m_tag) }
    {
        assign_this_overlying_to_underlying_node();

        rhs.m_node_type = node_type::null;
        rhs.m_underlying_node.scalar = nullptr;
    }

    template<typename Tchar, typename Ttraits>
    node<Tchar, Ttraits>& node<Tchar, Ttraits>::operator = (node&& rhs) noexcept(false) {
        destroy_underlying_node();

        m_node_type = rhs.m_node_type;
        m_underlying_node = rhs.m_underlying_node;
        m_tag = std::move(rhs.m_tag);

        assign_this_overlying_to_underlying_node();

        rhs.m_node_type = node_type::null;
        rhs.m_underlying_node.scalar = nullptr;

        return *this;
    }

    template<typename Tchar, typename Ttraits>
    node_type node<Tchar, Ttraits>::type() const {
        return m_node_type;
    }

    template<typename Tchar, typename Ttraits>
    bool node<Tchar, Ttraits>::is_null() const {
        return m_node_type == node_type::null;
    }

    template<typename Tchar, typename Ttraits>
    bool node<Tchar, Ttraits>::is_scalar() const {
        return m_node_type == node_type::scalar;
    }

    template<typename Tchar, typename Ttraits>
    bool node<Tchar, Ttraits>::is_object() const {
        return m_node_type == node_type::object;
    }

    template<typename Tchar, typename Ttraits>
    bool node<Tchar, Ttraits>::is_array() const {
        return m_node_type == node_type::array;
    }

    template<typename Tchar, typename Ttraits>
    typename node<Tchar, Ttraits>::scalar_node_type& node<Tchar, Ttraits>::as_scalar() {
        if (m_node_type != node_type::scalar) {
            throw std::runtime_error("Cannot call as_scalar on node of this node type.");
        }
        return *m_underlying_node.scalar;
    }
    template<typename Tchar, typename Ttraits>
    const typename node<Tchar, Ttraits>::scalar_node_type& node<Tchar, Ttraits>::as_scalar() const {
        if (m_node_type != node_type::scalar) {
            throw std::runtime_error("Cannot call as_scalar on node of this node type.");
        }
        return *m_underlying_node.scalar;
    }

    template<typename Tchar, typename Ttraits>
    typename node<Tchar, Ttraits>::object_node_type& node<Tchar, Ttraits>::as_object() {
        if (m_node_type != node_type::object) {
            throw std::runtime_error("Cannot call as_object on node of this node type.");
        }
        return *m_underlying_node.object;
    }
    template<typename Tchar, typename Ttraits>
    const typename node<Tchar, Ttraits>::object_node_type& node<Tchar, Ttraits>::as_object() const {
        if (m_node_type != node_type::object) {
            throw std::runtime_error("Cannot call as_object on node of this node type.");
        }
        return *m_underlying_node.object;
    }

    template<typename Tchar, typename Ttraits>
    typename node<Tchar, Ttraits>::array_node_type& node<Tchar, Ttraits>::as_array() {
        if (m_node_type != node_type::array) {
            throw std::runtime_error("Cannot call as_array on node of this node type.");
        }
        return *m_underlying_node.array;
    }
    template<typename Tchar, typename Ttraits>
    const typename node<Tchar, Ttraits>::array_node_type& node<Tchar, Ttraits>::as_array() const {
        if (m_node_type != node_type::array) {
            throw std::runtime_error("Cannot call as_array on node of this node type.");
        }
        return *m_underlying_node.array;
    }

    template<typename Tchar, typename Ttraits>
    template<typename Tvalue>
    Tvalue node<Tchar, Ttraits>::as(Tvalue default_value) const {
        return impl::node_converter<Tchar, Ttraits, Tvalue>::get(*this, default_value);
    }

    template<typename Tchar, typename Ttraits>
    typename node<Tchar, Ttraits>::string_type& node<Tchar, Ttraits>::tag() {
        return m_tag;
    }

    template<typename Tchar, typename Ttraits>
    const typename node<Tchar, Ttraits>::string_type& node<Tchar, Ttraits>::tag() const {
        return m_tag;
    }

    template<typename Tchar, typename Ttraits>
    void node<Tchar, Ttraits>::tag(string_type tag) {
        m_tag = std::move(tag);
    }

    template<typename Tchar, typename Ttraits>
    bool node<Tchar, Ttraits>::empty() const {
        switch (m_node_type) {
            case node_type::scalar: return as_scalar().empty();
            case node_type::object: return as_object().empty();
            case node_type::array: return as_array().empty();
            default: break;
        }
        return true;
    }

    template<typename Tchar, typename Ttraits>
    size_t node<Tchar, Ttraits>::size() const {
        switch (m_node_type) {
            case node_type::scalar: return as_scalar().size();
            case node_type::object: return as_object().size();
            case node_type::array: return as_array().size();
            default: break;
        }
        return 0;
    }

    template<typename Tchar, typename Ttraits>
    bool node<Tchar, Ttraits>::contains(string_type key) const {
        if (m_node_type == node_type::object) {
            return as_object().contains(key);
        }
        return false;
    }

    template<typename Tchar, typename Ttraits>
    bool node<Tchar, Ttraits>::contains(size_t index) const {
        if (m_node_type == node_type::array) {
            return as_array().contains(index);
        }
        return false;
    }

    template<typename Tchar, typename Ttraits>
    node<Tchar, Ttraits>& node<Tchar, Ttraits>::at(string_type key) {
        auto& object = as_object();
        return object.at(key);
    }

    template<typename Tchar, typename Ttraits>
    const node<Tchar, Ttraits>& node<Tchar, Ttraits>::at(string_type key) const {
        auto& object = as_object();
        return object.at(key);
    }

    template<typename Tchar, typename Ttraits>
    node<Tchar, Ttraits>& node<Tchar, Ttraits>::at(size_t index) {
        auto& array = as_array();
        return array.at(index);
    }

    template<typename Tchar, typename Ttraits>
    const node<Tchar, Ttraits>& node<Tchar, Ttraits>::at(size_t index) const {
        auto& array = as_array();
        return array.at(index);
    }

    template<typename Tchar, typename Ttraits>
    node<Tchar, Ttraits>& node<Tchar, Ttraits>::operator[](string_type key) {
        auto& object = as_object();
        return object[key];
    }

    template<typename Tchar, typename Ttraits>
    node<Tchar, Ttraits>& node<Tchar, Ttraits>::operator[](size_t index) {
        auto& array = as_array();
        return array[index];
    }

    template<typename Tchar, typename Ttraits>
    const node<Tchar, Ttraits>& node<Tchar, Ttraits>::operator[](size_t index) const {
        auto& array = as_array();
        return array[index];
    }

    template<typename Tchar, typename Ttraits>
    void node<Tchar, Ttraits>::destroy_underlying_node() {
        switch (m_node_type) {
            case node_type::null: break;
            case node_type::scalar: delete m_underlying_node.scalar; break;
            case node_type::object: delete m_underlying_node.object; break;
            case node_type::array: delete m_underlying_node.array; break;
        }
    }

    template<typename Tchar, typename Ttraits>
    void node<Tchar, Ttraits>::assign_this_overlying_to_underlying_node() {
        switch (m_node_type) {
            case node_type::null: break;
            case node_type::scalar: m_underlying_node.scalar->m_overlying_node = this; break;
            case node_type::object: m_underlying_node.object->m_overlying_node = this; break;
            case node_type::array: m_underlying_node.array->m_overlying_node = this; break;
        }
    }


    // DOM scalar node implementations.
    template<typename Tchar, typename Ttraits>
    scalar_node<Tchar, Ttraits>::scalar_node(node_value& overlying_node, scalar_style_type style, chomping_type chomping) :
        m_lines{},
        m_style(style),
        m_chomping(chomping),
        m_overlying_node(&overlying_node)
    {}

    template<typename Tchar, typename Ttraits>
    template<typename Tvalue>
    Tvalue scalar_node<Tchar, Ttraits>::as(Tvalue default_value) const {
        return impl::scalar_converter<Tchar, Ttraits, Tvalue>::get(*this, default_value);
    }


    template<typename Tchar, typename Ttraits>
    scalar_style_type& scalar_node<Tchar, Ttraits>::style() {
        return m_style;
    }

    template<typename Tchar, typename Ttraits>
    scalar_style_type scalar_node<Tchar, Ttraits>::style() const {
        return m_style;
    }

    template<typename Tchar, typename Ttraits>
    void scalar_node<Tchar, Ttraits>::style(scalar_style_type value) {
        m_style = value;
    }

    template<typename Tchar, typename Ttraits>
    chomping_type& scalar_node<Tchar, Ttraits>::chomping() {
        return m_chomping;
    }

    template<typename Tchar, typename Ttraits>
    chomping_type scalar_node<Tchar, Ttraits>::chomping() const {
        return m_chomping;
    }

    template<typename Tchar, typename Ttraits>
    void scalar_node<Tchar, Ttraits>::chomping(chomping_type value) {
        m_chomping = value;
    }

    template<typename Tchar, typename Ttraits>
    bool scalar_node<Tchar, Ttraits>::empty() const {
        return m_lines.empty();
    }

    template<typename Tchar, typename Ttraits>
    size_t scalar_node<Tchar, Ttraits>::size() const {
        return m_lines.size();
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::iterator scalar_node<Tchar, Ttraits>::begin() {
        return m_lines.begin();
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::const_iterator scalar_node<Tchar, Ttraits>::begin() const {
        return m_lines.begin();
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::const_iterator scalar_node<Tchar, Ttraits>::cbegin() const {
        return m_lines.cbegin();
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::iterator scalar_node<Tchar, Ttraits>::end() {
        return m_lines.end();
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::const_iterator scalar_node<Tchar, Ttraits>::end() const {
        return m_lines.end();
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::const_iterator scalar_node<Tchar, Ttraits>::cend() const {
        return m_lines.cend();
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::reverse_iterator scalar_node<Tchar, Ttraits>::rbegin() {
        return m_lines.rbegin();
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::const_reverse_iterator scalar_node<Tchar, Ttraits>::rbegin() const {
        return m_lines.rbegin();
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::const_reverse_iterator scalar_node<Tchar, Ttraits>::crbegin() const {
        return m_lines.crbegin();
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::reverse_iterator scalar_node<Tchar, Ttraits>::rend() {
        return m_lines.rend();
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::const_reverse_iterator scalar_node<Tchar, Ttraits>::rend() const {
        return m_lines.rend();
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::const_reverse_iterator scalar_node<Tchar, Ttraits>::crend() const {
        return m_lines.crend();
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::string_type& scalar_node<Tchar, Ttraits>::at(size_t index) {
        return m_lines.at(index);
    }

    template<typename Tchar, typename Ttraits>
    const typename scalar_node<Tchar, Ttraits>::string_type& scalar_node<Tchar, Ttraits>::at(size_t index) const {
        return m_lines.at(index);
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::string_type& scalar_node<Tchar, Ttraits>::operator[](size_t index) {
        return m_lines.at(index);
    }

    template<typename Tchar, typename Ttraits>
    const typename scalar_node<Tchar, Ttraits>::string_type& scalar_node<Tchar, Ttraits>::operator[](size_t index) const {
        return m_lines.at(index);
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::iterator scalar_node<Tchar, Ttraits>::insert(const_iterator pos, string_type string) {
        return m_lines.insert(pos, std::move(string));
    }

    template<typename Tchar, typename Ttraits>
    void scalar_node<Tchar, Ttraits>::push_back(string_type string) {
        m_lines.emplace_back(std::move(string));
    }

    template<typename Tchar, typename Ttraits>
    void scalar_node<Tchar, Ttraits>::push_front(string_type string) {
        m_lines.insert(m_lines.begin(), std::move(string));
    }

    template<typename Tchar, typename Ttraits>
    void scalar_node<Tchar, Ttraits>::pop_back() {
        m_lines.pop_back();
    }

    template<typename Tchar, typename Ttraits>
    void scalar_node<Tchar, Ttraits>::pop_front() {
        m_lines.erase(m_lines.begin());
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::iterator scalar_node<Tchar, Ttraits>::erase(iterator pos) {
        m_lines.erase(pos);
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::iterator scalar_node<Tchar, Ttraits>::erase(iterator first, iterator last) {
        m_lines.erase(first, last);
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::iterator scalar_node<Tchar, Ttraits>::erase(const_iterator pos) {
        m_lines.erase(pos);
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::iterator scalar_node<Tchar, Ttraits>::erase(const_iterator first, const_iterator last) {
        m_lines.erase(first, last);
    }

    template<typename Tchar, typename Ttraits>
    typename scalar_node<Tchar, Ttraits>::node_value& scalar_node<Tchar, Ttraits>::overlying_node() {
        return *m_overlying_node;
    }
    template<typename Tchar, typename Ttraits>
    const typename scalar_node<Tchar, Ttraits>::node_value& scalar_node<Tchar, Ttraits>::overlying_node() const {
        return *m_overlying_node;
    }


    // DOM object node implementations.
    template<typename Tchar, typename Ttraits>
    object_node<Tchar, Ttraits>::object_node(node_value& overlying_node) :
        m_overlying_node(&overlying_node)
    {}

    template<typename Tchar, typename Ttraits>
    bool object_node<Tchar, Ttraits>::empty() const {
        return m_map.empty();
    }

    template<typename Tchar, typename Ttraits>
    size_t object_node<Tchar, Ttraits>::size() const {
        return m_map.size();
    }

    template<typename Tchar, typename Ttraits>
    bool object_node<Tchar, Ttraits>::contains(const string_type& key) const {
        return m_map.find(key) != m_map.end();
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::iterator object_node<Tchar, Ttraits>::begin() {
        return m_map.begin();
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::const_iterator object_node<Tchar, Ttraits>::begin() const {
        return m_map.begin();
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::const_iterator object_node<Tchar, Ttraits>::cbegin() const {
        return m_map.cbegin();
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::iterator object_node<Tchar, Ttraits>::end() {
        return m_map.end();
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::const_iterator object_node<Tchar, Ttraits>::end() const {
        return m_map.end();
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::const_iterator object_node<Tchar, Ttraits>::cend() const {
        return m_map.cend();
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::reverse_iterator object_node<Tchar, Ttraits>::rbegin() {
        return m_map.rbegin();
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::const_reverse_iterator object_node<Tchar, Ttraits>::rbegin() const {
        return m_map.rbegin();
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::const_reverse_iterator object_node<Tchar, Ttraits>::crbegin() const {
        return m_map.crbegin();
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::reverse_iterator object_node<Tchar, Ttraits>::rend() {
        return m_map.rend();
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::const_reverse_iterator object_node<Tchar, Ttraits>::rend() const {
        return m_map.rend();
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::const_reverse_iterator object_node<Tchar, Ttraits>::crend() const {
        return m_map.crend();
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::iterator object_node<Tchar, Ttraits>::find(const string_type& key) {
        return m_map.find(key);
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::const_iterator object_node<Tchar, Ttraits>::find(const string_type& key) const {
        return m_map.find(key);
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::node_value& object_node<Tchar, Ttraits>::at(const string_type& key) {
        auto it = m_map.find(key);
        if (it == m_map.end()) {
            throw std::out_of_range("Provided node object key is unknown.");
        }
        return *it->second;
    }

    template<typename Tchar, typename Ttraits>
    const typename object_node<Tchar, Ttraits>::node_value& object_node<Tchar, Ttraits>::at(const string_type& key) const {
        auto it = m_map.find(key);
        if (it == m_map.end()) {
            throw std::out_of_range("Provided node object key is unknown.");
        }
        return *it->second;
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::node_value& object_node<Tchar, Ttraits>::operator [] (const string_type& key) {
        auto it = m_map.find(key);
        if (it != m_map.end()) {
            return *it->second;
        }
        
        auto insert_result = m_map.insert({ key, node_owned_pointer{ new node_value{} } });
        return *insert_result.first->second;
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::insert_return_type object_node<Tchar, Ttraits>::insert(const string_type& key) {
        auto found_it = m_map.find(key);
        if (found_it != m_map.end()) {
            return { found_it, false };
        }

        auto it = m_map.insert({ key, node_owned_pointer{ new node_value{} } });
        return it;
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::insert_return_type object_node<Tchar, Ttraits>::insert(const string_type& key, node_value&& node) {
        auto insert_result = insert(key);
        if (insert_result.second) {
            auto& new_node = *insert_result.first->second;
            new_node = std::move(node);
        }
        return insert_result;
    }

    template<typename Tchar, typename Ttraits>
    size_t object_node<Tchar, Ttraits>::erase(const string_type& key) {
        return m_map.erase(key);
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::iterator object_node<Tchar, Ttraits>::erase(iterator pos) {
        return m_map.erase(pos);
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::iterator object_node<Tchar, Ttraits>::erase(iterator first, iterator last) {
        return m_map.erase(first, last);
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::iterator object_node<Tchar, Ttraits>::erase(const_iterator pos) {
        return m_map.erase(pos);
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::iterator object_node<Tchar, Ttraits>::erase(const_iterator first, const_iterator last) {
        return m_map.erase(first, last);
    }

    template<typename Tchar, typename Ttraits>
    typename object_node<Tchar, Ttraits>::node_value& object_node<Tchar, Ttraits>::overlying_node() {
        return *m_overlying_node;
    }
    template<typename Tchar, typename Ttraits>
    const typename object_node<Tchar, Ttraits>::node_value& object_node<Tchar, Ttraits>::overlying_node() const {
        return *m_overlying_node;
    }
    

    // DOM array node implementations.
    template<typename Tchar, typename Ttraits>
    array_node<Tchar, Ttraits>::array_node(node_value& overlying_node) :
        m_overlying_node(&overlying_node)
    {}

    template<typename Tchar, typename Ttraits>
    bool array_node<Tchar, Ttraits>::empty() const {
        return m_list.empty();
    }

    template<typename Tchar, typename Ttraits>
    size_t array_node<Tchar, Ttraits>::size() const {
        return m_list.size();
    }

    template<typename Tchar, typename Ttraits>
    bool array_node<Tchar, Ttraits>::contains(size_t index) const {
        return index < m_list.size();
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::iterator array_node<Tchar, Ttraits>::begin() {
        return m_list.begin();
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::const_iterator array_node<Tchar, Ttraits>::begin() const {
        return m_list.begin();
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::const_iterator array_node<Tchar, Ttraits>::cbegin() const {
        return m_list.cbegin();
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::iterator array_node<Tchar, Ttraits>::end() {
        return m_list.end();
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::const_iterator array_node<Tchar, Ttraits>::end() const {
        return m_list.end();
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::const_iterator array_node<Tchar, Ttraits>::cend() const {
        return m_list.cend();
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::reverse_iterator array_node<Tchar, Ttraits>::rbegin() {
        return m_list.rbegin();
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::const_reverse_iterator array_node<Tchar, Ttraits>::rbegin() const {
        return m_list.rbegin();
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::const_reverse_iterator array_node<Tchar, Ttraits>::crbegin() const {
        return m_list.crbegin();
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::reverse_iterator array_node<Tchar, Ttraits>::rend() {
        return m_list.rend();
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::const_reverse_iterator array_node<Tchar, Ttraits>::rend() const {
        return m_list.rend();
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::const_reverse_iterator array_node<Tchar, Ttraits>::crend() const {
        return m_list.crend();
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::node_value& array_node<Tchar, Ttraits>::at(size_t index) {
        return *m_list.at(index);
    }

    template<typename Tchar, typename Ttraits>
    const typename array_node<Tchar, Ttraits>::node_value& array_node<Tchar, Ttraits>::at(size_t index) const {
        return *m_list.at(index);
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::node_value& array_node<Tchar, Ttraits>::operator[](size_t index) {
        return *m_list.at(index);
    }

    template<typename Tchar, typename Ttraits>
    const typename array_node<Tchar, Ttraits>::node_value& array_node<Tchar, Ttraits>::operator[](size_t index) const {
        return *m_list.at(index);
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::iterator array_node<Tchar, Ttraits>::insert(const_iterator pos) {
        auto new_node = node_owned_pointer{ new node_value{} };
        return m_list.insert(pos, std::move(new_node));
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::iterator array_node<Tchar, Ttraits>::insert(const_iterator pos, node_value&& node) {
        auto new_node = node_owned_pointer{ new node_value{} };
        *new_node = std::move(node);
        return m_list.insert(pos, std::move(new_node));
    }

    template<typename Tchar, typename Ttraits>
    void array_node<Tchar, Ttraits>::push_back() {
        auto new_node = node_owned_pointer{ new node_value{} };
        m_list.push_back(std::move(new_node));
    }

    template<typename Tchar, typename Ttraits>
    void array_node<Tchar, Ttraits>::push_back(node_value&& node) {
        auto new_node = node_owned_pointer{ new node_value{} };
        *new_node = std::move(node);
        m_list.push_back(std::move(new_node));
    }

    template<typename Tchar, typename Ttraits>
    void array_node<Tchar, Ttraits>::pop_back() {
        m_list.pop_back();
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::iterator array_node<Tchar, Ttraits>::erase(iterator pos) {
        return m_list.erase(pos);
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::iterator array_node<Tchar, Ttraits>::erase(iterator first, iterator last) {
        return m_list.erase(first, last);
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::iterator array_node<Tchar, Ttraits>::erase(const_iterator pos) {
        return m_list.erase(pos);
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::iterator array_node<Tchar, Ttraits>::erase(const_iterator first, const_iterator last) {
        return m_list.erase(first, last);
    }

    template<typename Tchar, typename Ttraits>
    typename array_node<Tchar, Ttraits>::node_value& array_node<Tchar, Ttraits>::overlying_node() {
        return *m_overlying_node;
    }
    template<typename Tchar, typename Ttraits>
    const typename array_node<Tchar, Ttraits>::node_value& array_node<Tchar, Ttraits>::overlying_node() const {
        return *m_overlying_node;
    }


    // DOM reader implementations.
    template<typename Tchar, typename Ttraits>
    reader<Tchar, Ttraits>::reader(const reader_options& options) :
        m_sax_handler(),
        m_sax_reader(m_sax_handler, options)
    {}

    template<typename Tchar, typename Ttraits>
    typename reader<Tchar, Ttraits>::read_document_result_type reader<Tchar, Ttraits>::read_document(const_pointer raw_input, size_type size) {
        m_sax_handler.initialize();
        auto read_result = m_sax_reader.read_document(raw_input, size);

        return m_sax_handler.m_documents.empty() ?
            create_read_document_result(read_result, node_value{}) :
            create_read_document_result(read_result, std::move(m_sax_handler.m_documents.front()));
    }

    template<typename Tchar, typename Ttraits>
    typename reader<Tchar, Ttraits>::read_document_result_type reader<Tchar, Ttraits>::read_document(const string_type& string) {
        return read_document(string.c_str(), string.size());
    }

    template<typename Tchar, typename Ttraits>
    typename reader<Tchar, Ttraits>::read_document_result_type reader<Tchar, Ttraits>::read_document(string_view_type string_view) {
        return read_document(string_view.data(), string_view.size());
    }

    template<typename Tchar, typename Ttraits>
    typename reader<Tchar, Ttraits>::read_document_file_result_type reader<Tchar, Ttraits>::read_document_from_file(const std::string& filename) {
        m_sax_handler.initialize();
        auto read_result = m_sax_reader.read_document_from_file(filename);

        return m_sax_handler.m_documents.empty() ?
            create_read_document_file_result(read_result, node_value{}) :
            create_read_document_file_result(read_result, std::move(m_sax_handler.m_documents.front()));
    }

    template<typename Tchar, typename Ttraits>
    typename reader<Tchar, Ttraits>::read_documents_result_type reader<Tchar, Ttraits>::read_documents(const_pointer raw_input, size_type size) {
        m_sax_handler.initialize();
        auto read_result = m_sax_reader.read_documents(raw_input, size);

        return m_sax_handler.m_documents.empty() ?
            create_read_documents_result(read_result, {}) :
            create_read_documents_result(read_result, std::move(m_sax_handler.m_documents));
    }

    template<typename Tchar, typename Ttraits>
    typename reader<Tchar, Ttraits>::read_documents_result_type reader<Tchar, Ttraits>::read_documents(const string_type& string) {
        return read_documents(string.c_str(), string.size());
    }

    template<typename Tchar, typename Ttraits>
    typename reader<Tchar, Ttraits>::read_documents_result_type reader<Tchar, Ttraits>::read_documents(string_view_type string_view) {
        return read_documents(string_view.data(), string_view.size());
    }

    template<typename Tchar, typename Ttraits>
    typename reader<Tchar, Ttraits>::read_documents_file_result_type reader<Tchar, Ttraits>::read_documents_from_file(const std::string& filename) {
        m_sax_handler.initialize();
        auto read_result = m_sax_reader.read_documents_from_file(filename);

        return m_sax_handler.m_documents.empty() ?
            create_read_documents_file_result(read_result, {}) :
            create_read_documents_file_result(read_result, std::move(m_sax_handler.m_documents));
    }

    template<typename Tchar, typename Ttraits>
    typename reader<Tchar, Ttraits>::read_document_result_type reader<Tchar, Ttraits>::create_read_document_result(
        sax_read_document_result_type sax_result,
        node_value&& node
    ) {
        auto result = read_document_result_type{}; 
        result.result_code = sax_result.result_code;
        result.remaining_input = sax_result.remaining_input;
        result.current_line = sax_result.current_line;
        result.current_line_ptr = sax_result.current_line_ptr;
        result.root_node = std::move(node);
        return result;
    }

    template<typename Tchar, typename Ttraits>
    typename reader<Tchar, Ttraits>::read_document_file_result_type reader<Tchar, Ttraits>::create_read_document_file_result(
        sax_read_document_file_result_type sax_result,
        node_value&& node)
    {
        auto result = read_document_file_result_type{};
        result.result_code = sax_result.result_code;
        result.current_line = sax_result.current_line;
        result.root_node = std::move(node);
        return result;
    }

    template<typename Tchar, typename Ttraits>
    typename reader<Tchar, Ttraits>::read_documents_result_type reader<Tchar, Ttraits>::create_read_documents_result(
        sax_read_documents_result_type sax_result,
        node_values&& nodes) 
    {
        auto result = read_documents_result_type{};
        result.result_code = sax_result.result_code;
        result.remaining_input = sax_result.remaining_input;
        result.current_line = sax_result.current_line;
        result.current_line_ptr = sax_result.current_line_ptr;
        result.root_nodes = std::move(nodes);
        return result;
    }

    template<typename Tchar, typename Ttraits>
    typename reader<Tchar, Ttraits>::read_documents_file_result_type reader<Tchar, Ttraits>::create_read_documents_file_result(
        sax_read_documents_file_result_type sax_result,
        node_values&& nodes) 
    {
        auto result = read_documents_file_result_type{};
        result.result_code = sax_result.result_code;
        result.current_line = sax_result.current_line;
        result.root_nodes = std::move(nodes);
        return result;
    }


    // DOM's sax handler implementations
    template<typename Tchar, typename Ttraits>
    reader<Tchar, Ttraits>::sax_handler::sax_handler() :
        m_documents{},
        m_current_node{ nullptr },
        m_node_stack{},
        m_string_views{},
        m_current_style{ scalar_style_type::none },
        m_current_chomping{ chomping_type::strip }
    {}

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::initialize() {
        m_documents.clear();
        m_current_node = nullptr;
        m_node_stack.clear();
        m_string_views.clear();
    }

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::start_document()
    {
        m_documents.emplace_back();

        m_current_node = &m_documents.back();;

        m_node_stack.clear();
        m_node_stack.push_back(m_current_node);
    }

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::end_document()
    {}

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::start_scalar(scalar_style_type style, chomping_type chomping) {
        m_string_views.clear();
        m_current_style = style;
        m_current_chomping = chomping;
    }

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::end_scalar() {
        auto get_iterators = [&]() -> std::pair<string_view_list_iterator, string_view_list_iterator> {
            if (m_current_style == scalar_style_type::none) {
                auto begin_it = std::find_if(m_string_views.begin(), m_string_views.end(),
                    [](const string_view_type& line) { return !MINIYAML_NAMESPACE::impl::is_empty_or_whitespace(line); });

                auto rend_it = std::find_if(m_string_views.rbegin(), m_string_views.rend(),
                    [](const string_view_type& line) { return !MINIYAML_NAMESPACE::impl::is_empty_or_whitespace(line); });

                auto end_it = rend_it.base();

                return { begin_it, end_it };
            }

            return { m_string_views.begin(), m_string_views.end() };
        };

        auto iterators = get_iterators();
        auto begin_it = iterators.first;
        auto end_it = iterators.second;

        auto check_null_token = [](string_view_type line) {
            return line.size() == 1 && line[0] == token<Tchar>::null;
        };

        auto check_null_keyword = [](string_view_type line) {
            return  line.size() == 4 && (
                ((line[0] == 'n') && (line == "null")) ||
                ((line[0] == 'N') && (line == "Null" || line == "NULL"))
            );
        };

        auto line_count = std::distance(begin_it, end_it);
        if (line_count == 1 && m_current_style == scalar_style_type::none) {
            auto single_line = *begin_it;
            if (check_null_token(single_line) || check_null_keyword(single_line)) {
                pop_stack();
                return;
            }
        }
        auto tag = std::move(m_current_node->tag());
        *m_current_node = node_value::create_scalar(m_current_style, m_current_chomping);
        auto& scalar = m_current_node->as_scalar();
        m_current_node->tag(tag);

        for (auto it = begin_it; it != end_it; ++it) {
            scalar.push_back(MINIYAML_NAMESPACE::impl::get_string_from_string_or_view(*it));
        }

        pop_stack();
    }

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::start_object() {
        auto tag = std::move(m_current_node->tag());
        *m_current_node = node_value::create_object();
        m_current_node->tag(tag);
    }

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::end_object() {
        pop_stack();
    }

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::start_array() {
        auto tag = std::move(m_current_node->tag());
        *m_current_node = node_value::create_array();
        m_current_node->tag(tag);
    }

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::end_array() {
        pop_stack();
    }

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::null() {
        pop_stack();
    }

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::index(size_t) {
        auto& array = m_current_node->as_array();
        auto insert_result = array.insert(array.end());

        auto* new_node = insert_result->get();
        push_stack(new_node);
    }

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::string(string_view_type string_view) {
        m_string_views.push_back(string_view);
    }

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::key(string_view_type key) {
        auto& object = m_current_node->as_object();
        auto insert_result = object.insert(MINIYAML_NAMESPACE::impl::get_string_from_string_or_view(key));

        auto* new_node = insert_result.first->second.get();
        push_stack(new_node);
    }

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::tag(string_view_type key) {
        m_current_node->tag(MINIYAML_NAMESPACE::impl::get_string_from_string_or_view(key));
    }

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::comment(string_view_type)
    {}

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::push_stack(node_value* node) {
        m_node_stack.push_back(node);
        m_current_node = node;
    }

    template<typename Tchar, typename Ttraits>
    void reader<Tchar, Ttraits>::sax_handler::pop_stack() {
        m_node_stack.pop_back();
        if (!m_node_stack.empty()) {
            m_current_node = m_node_stack.back();
        }
    }


    // DOM reader result implementations.
    template<typename Tchar, typename Ttraits>
    read_document_result<Tchar, Ttraits>::operator bool() const {
        return result_code == read_result_code::success;
    }

    template<typename Tchar, typename Ttraits>
    read_document_file_result<Tchar, Ttraits>::operator bool() const {
        return result_code == read_result_code::success;
    }

    template<typename Tchar, typename Ttraits>
    read_documents_result<Tchar, Ttraits>::operator bool() const {
        return result_code == read_result_code::success;
    }

    template<typename Tchar, typename Ttraits>
    read_documents_file_result<Tchar, Ttraits>::operator bool() const {
        return result_code == read_result_code::success;
    }


    // Global DOM function implementations.
    template<typename Tchar, typename Ttraits>
    read_document_result<Tchar, Ttraits> read_document(
        const Tchar* raw_input,
        size_t size,
        const reader_options& options)
    {
        return reader<Tchar, Ttraits>{ options }.read_document(raw_input, size);
    }

    template<typename Tchar, typename Ttraits>
    read_document_result<Tchar, Ttraits> read_document(
        const std::basic_string<Tchar>& string,
        const reader_options& options)
    {
        return reader<Tchar, Ttraits>{ options }.read_document(string);
    }

    template<typename Tchar, typename Ttraits>
    read_document_result<Tchar, Ttraits> read_document(
        basic_string_view<Tchar> string_view,
        const reader_options& options)
    {
        return reader<Tchar, Ttraits>{ options }.read_document(string_view);
    }

    template<typename Tchar, typename Ttraits>
    read_document_file_result<Tchar, Ttraits> read_document_from_file(
        const std::string& filename,
        const reader_options& options)
    {
        return reader<Tchar, Ttraits>{ options }.read_document_from_file(filename);
    }

    template<typename Tchar, typename Ttraits>
    read_documents_result<Tchar, Ttraits> read_documents(
        const Tchar* raw_input,
        size_t size,
        const reader_options& options)
    {
        return reader<Tchar, Ttraits>{ options }.read_documents(raw_input, size);
    }

    template<typename Tchar, typename Ttraits>
    read_documents_result<Tchar, Ttraits> read_documents(
        const std::basic_string<Tchar>& string,
        const reader_options& options)
    {
        return reader<Tchar, Ttraits>{ options }.read_documents(string);
    }

    template<typename Tchar, typename Ttraits>
    read_documents_result<Tchar, Ttraits> read_documents(
        basic_string_view<Tchar> string_view,
        const reader_options& options)
    {
        return reader<Tchar, Ttraits>{ options }.read_documents(string_view);
    }

    template<typename Tchar, typename Ttraits>
    read_documents_file_result<Tchar, Ttraits> read_documents_from_file(
        const std::string& filename,
        const reader_options& options)
    {
        return reader<Tchar, Ttraits>{ options }.read_documents_from_file(filename);
    }

} }

#endif