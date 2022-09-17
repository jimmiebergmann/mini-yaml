/*
* MIT License
*
* Copyright(c) 2019 Jimmie Bergmann
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


/*
YAML documentation:
http://yaml.org/spec/1.0/index.html
https://www.codeproject.com/Articles/28720/YAML-Parser-in-C
*/


#ifndef MINI_YAML_HEADER
#define MINI_YAML_HEADER


#include <exception>
#include <memory>
#include <algorithm>
#include <ostream>
#include <sstream>
#include <map>
#include <list>
#include <cstring>
#include <string>
#include <vector>


/** Namespace wrapping mini-yaml classes. */
namespace yaml
{

    /** Forward declarations. */ 
    class node;

    /** Private implementation details. */
    namespace priv
    {
        template<typename Type> struct scalar_default_value;
        template<typename From, typename To> struct data_converter;
        template<typename Writer> class string_writer;

        class node_impl;
        class null_node_impl;
        class map_node_impl;
        class scalar_node_impl;
        class sequence_node_impl;

        class iterator_impl;
        class const_iterator_impl;
        class map_iterator_impl;
        class map_const_iterator_impl;
        class sequence_iterator_impl;
        class sequence_const_iterator_impl;
  
        using node_unique_ptr = std::unique_ptr<node>;
        using node_map = std::map<std::string, node_unique_ptr>;
        using node_list = std::list<node_unique_ptr>;
    }


    /** Enumeration of exception types. */
    enum class exception_type
    {
        internal_error,  ///< Internal error.
        parsing_error,   ///< Invalid parsing data.
        operation_error  ///< User operation error.
    };

    /** Enumeration of node types. */
    enum class node_type
    {
        map,
        null,
        scalar,
        sequence
    };

    /** Enumeration of node data types. */
    enum class node_data_type
    {
        boolean,
        float32,
        float64,
        int32,
        int64,
        null,
        string
    };

    using document = node;                      ///< Alias of node, suits better when parsing, but same thing...
    using documents = std::vector<document>;    ///< Should become it's own container???

    /** Exception class. */
    class exception : public std::runtime_error
    {

    public:

        exception(const std::string& message, const exception_type type);

        exception_type type() const;

    private:

        exception_type m_type;

    };

    /** Error exception class.
     *
     * @see Exception
     */
    class error : public exception
    {

    public:

        error(const std::string& message, const exception_type type);

    };

    /** Internal error exception class.
     *
     * @see Exception
     */
    class internal_error : public error
    {

    public:

        internal_error(const std::string& message);

    };

    /** Parsing error exception class.
     *
     * @see Exception
     */
    class parsing_error : public error
    {

    public:

        parsing_error(const std::string& message);

    };


    /** Operation error exception class.
     *
     * @see Exception
     */
    class operation_error : public error
    {

    public:

        operation_error(const std::string& message);

    };


    /** Node class. */
    class node
    {

    public:

        /** Enumeration of iterator types. */
        enum class iterator_type
        {
            map,
            null,
            sequence
        };

        /** Iterator class. */
        class iterator
        {

        public:

            /** Default constructor. */
            iterator();

            /** Copy constructor. */
            iterator(const iterator& it);

            /**  Constructing iterator as a map iterator. */
            iterator(priv::node_map::iterator it);

            /** Constructing iterator as a sequence iterator. */
            iterator(priv::node_list::iterator it);

            /**  Destructor. */
            ~iterator();

            /** Get type of node iterator. */
            iterator_type type() const;

            /** Get node of iterator.
             *  First pair item is the key of map value, empty if type is sequence.
             */
            std::pair<const std::string&, node&> operator *();

            /** Assignment operator. */
            iterator& operator = (const iterator& it);

            /**  Pre-increment operator. */
            iterator operator ++ ();

            /** Post-increment operator. */
            iterator operator ++ (int);

            /** pre-decrement operator. */
            iterator operator -- ();

            /** Post-decrement operator. */
            iterator operator -- (int);

            /**  Check if iterator is equal to other iterator. */
            bool operator == (const iterator& it) const;

            /** Check if iterator is not equal to other iterator. */
            bool operator != (const iterator& it) const;

        private:

            iterator_type           m_type; ///< Type of iterator. 
            priv::iterator_impl *   m_impl; ///< Pointer of implementation class.

            friend class node;

        };


        /** Constant iterator class. */
        class const_iterator
        {

        public:

            /** Default constructor. */
            const_iterator();

            /** Copy constructor. */
            const_iterator(const const_iterator& it);

            /** Constructing iterator as a map iterator. */
            const_iterator(priv::node_map::const_iterator it);

            /** Constructing iterator as a sequence iterator. */
            const_iterator(priv::node_list::const_iterator it);

            /** Destructor. */
            ~const_iterator();

            /** Get type of node iterator. */
            iterator_type type() const;

            /** Get node of iterator.
             *  First pair item is the key of map value, empty if type is sequence.
             */
            std::pair<const std::string&, const node&> operator *();

            /** Assignment operator. */
            const_iterator& operator = (const const_iterator& it);

            /** Pre-increment operator. */
            const_iterator operator ++ ();

            /** Post-increment operator. */
            const_iterator operator ++ (int);

            /** pre-decrement operator. */
            const_iterator operator -- ();

            /** Post-decrement operator. */
            const_iterator operator -- (int);

            /** Check if iterator is equal to other iterator. */
            bool operator == (const const_iterator& it) const;

            /** Check if iterator is not equal to other iterator. */
            bool operator != (const const_iterator& it) const;

        private:

            iterator_type               m_type; ///< Type of iterator. 
            priv::const_iterator_impl * m_impl; ///< Pointer of implementation class.

            friend class node;

        };


        static const node null;

        /** Default constructor. */
        node();

        /** Constructs node of given type.
         *  Default data type value of given type is set.
         */
        node(const node_type type);

        /** Constructs scalar node.
         *  Default value of given node data type is set.
         */
        node(const node_data_type data_type);

        /** Assignment constructor.
         *  Turns the node into a scalar node and assignes by given value.
         */
        template<typename T>
        node(const T value);

        /** Deleted copy constructor.  */
        node(const node& node) = delete;

        /** Move constructor. */
        node(node&& from);

        /** Destructor. */
        ~node();

        /**  Get node as given template type. */
        template<typename T>
        T as() const;

        /** Get node as given template type.
         *
         * @param default_value Returned if conversion between current data type and T fails.
         */
        template<typename T>
        T as(const T& default_value) const;
        
        /** Get existing child node by key value.
         * 
         * @return Reference to node if found, else yaml::node::null is returned.
         *
         * @see operator[]
         */
        const node& at(const std::string& key) const;

        /** Get start iterator. */
        iterator begin();
        const_iterator begin() const;

        /** Get end iterator. */
        iterator end();
        const_iterator end() const;

        /** Clears content of node.
         *  Scalar's value is set to default value,
         *  map and sequence items are cleared, resulting in size() = 0.
         */
        void clear();

        /** Get data type of scalar.
         *
         * @return Data type of scalar, will return node_data_type::null if type() != node_type::scalar.
         */
        node_data_type data_type() const;

        /** Erase map item.
         *   No action if node type is not equal to map type.
         *
         * @param key Only available if node type is map.
         * @param it iterator of map or sequence node.
         *
         * @return true if key is found and erased, else false.
         */
        bool erase(const std::string & key);
        iterator erase(iterator it);
        const_iterator erase(const_iterator it);

        /* Find node item by key. */
        iterator find(const std::string& key);
        const_iterator find(const std::string& key) const;

        /** Checks if map item exists, by key value. */
        bool exists(const std::string& key) const;

        /** Inserts or updates map item.
         *   Converts node to map type if needed.
         *
         * @tparam T = scalar value / node_type / node_data_type.
         *
         * @return Pair of iterator and bool. pair::first is an iterator of the newly inserted or already existing node.
         *         pair::second is a boolean, set to true if a new node was inserted, false if an already existing node were updated.
         */
        template<typename T>
        node& insert(const std::string& key, const T value = node_type::null);

        template<typename T>
        std::pair<iterator, bool> insert(const std::pair<const std::string&, const T>& pair);        

        /** Pushing new sequence item to back.
         *  Converts node to sequence type if needed.
         *
         * @tparam T = scalar value / node_type / node_data_type.
         *
         * @return Reference of pushed node.
         */
        template<typename T>
        node& push_back(const T value);

        /** Pushing new sequence item to front.
         *  Converts node to sequence type if needed.
         *
         * @tparam T = scalar value / node_type / node_data_type.
         *
         * @return Reference of pushed node.
         */
        template<typename T>
        node& push_front(const T value);

        /** Methods for checking node type. */     
        bool is_map() const;
        bool is_null() const;
        bool is_scalar() const;
        bool is_sequence() const;
        node_type type() const;
      
        /** Get item count of map or sequence.
         *
         * @return Number of sequence or map entries in node. 0 if node_type != map or sequence.
         */
        size_t size() const;

        /** Converts node to given type if needed.
         *  Any existing node data is lost and the node's data type is sett to null.
         */
        node& operator = (const node_type type);

        /** Assigns default value of given node data type.
         *  This is one way to explicitly assign null values.
         *  Converts node to scalar type if needed.
         */
        node& operator = (const node_data_type data_type);

        /** Assigns scalar value.
         *  Converts node to scalar type if needed.
         */
        template<typename T>
        node& operator = (const T value);

        /** Get existing or insert new map item.
         *  Converts node to map type if the current type is different from node_type::map.
         *
         * @param key Map key. Creates a new node if key is unknown.
         */
        node& operator [] (const std::string& key);

    private:

        node_type                           m_type; ///< Type of node..
        std::unique_ptr<priv::node_impl>    m_impl; ///< Implementation class.

    };


    /** Parsing functions.
     *
     * @tparam Return    Return type of parsed object.
     *                   Allowing types yaml::documents, yaml::document and yaml::node.
     *
     * @param string     String of input data.
     * @param buffer     Char array of input data.
     * @param size       Buffer size.
     * @param stream     Input stream.
     * @param filename   Path of input file.
     *
     * @throw ParsingWarning     Parsing succeeded, but got warnings.
     * @throw InternalError      An internal error occurred.
     * @throw ParsingError       Invalid input YAML data.
     * @throw OperationError     If filename or buffer pointer is invalid.
     */   
    template<typename Return> Return parse(const std::string & string);
    template<typename Return> Return parse(const char * buffer, const size_t size);
    template<typename Return> Return parse_file(const std::string & filename);


    /** Serialization configuration structure, describing output behavior. */
	struct dump_config
	{

        /** Constructor.
         *
         * @param indentation            Number of spaces per indentation. Default: 2.
         * @param explicit_start         Always puts "---" to start of document if set to true.. Default: false.
         * @param explicit_end           Always puts "..." to start of document if set to true. Default: false.
         * @param scalar_fold_length     Maximum length of scalars. Serialized as folder scalars if exceeded.
         *                               Ignored if equal to 0. Default: 64.
         */
		dump_config(
            const size_t indentation = 2,
            const bool explicit_start = false,
            const bool explicit_end = false,
			const size_t scalar_fold_length = 64);

        size_t indentation;         ///< Number of spaces per indentation.
        bool explicit_start;        ///< Always puts "---" to start of document.
        bool explicit_end;          ///< Always puts "..." to end of document.
        size_t scalar_fold_length;  ///< Scalars are serialized as folded scalars if exceeded.
    };


    /** Dump/serialization functions.
     *  Indentation is set to default value: 2, if dump_config::indentation is 0.
     *
     * @param doc        Document to serialize.
     * @param docs       Documents to serialize.
     * @param filename   Path of output file.
     * @param stream     Output stream.
     * @param string     String of output data.
     * @param config     Serialization configurations.
     *
     * @throw internal_error  An internal error occurred.
     * @throw operation_error If filename is invalid, or passed dump_config is invalid.
     */
    std::string dump(const document & doc, const dump_config & config = { });
    std::string dump(const documents & docs, const dump_config & config = { });

    void dump_file(const document & doc, const std::string & filename, const dump_config & config = { });
    void dump_file(const documents & docs, const std::string & filename, const dump_config & config = { });

    void dump_stream(const document & doc, std::ostream & stream, const dump_config & config = { });
    void dump_stream(const documents & docs, std::ostream & stream, const dump_config & config = { });

    void dump_string(const document & doc, std::string & string, const dump_config & config = { });
    void dump_string(const documents & docs, std::string & string, const dump_config & config = { });

}

// END OF PUBLIC API.


// Inline implementations.
namespace yaml
{

    /**
    * @brief Private mini-yaml API namespace, containing helper classes and functions.
    *
    */
    namespace priv
    {

        /**
        * @brief Traits struct, to retrieve default values of different types.
        *
        */
        template<typename Type>
        struct scalar_default_value
        {
        };
        template<>
        struct scalar_default_value<bool>
        {
            static const bool value;
        };
        template<>
        struct scalar_default_value<float>
        {
            static const float value;
        };
        template<>
        struct scalar_default_value<double>
        {
            static const double value;
        };
        template<>
        struct scalar_default_value<int32_t>
        {
            static const int32_t value;
        };
        template<>
        struct scalar_default_value<int64_t>
        {
            static const int64_t value;
        };
        template<>
        struct scalar_default_value<std::string>
        {
            static const std::string value;
        };


        /**
        * @brief Traits struct, to converting string to any data type.
        *        std::string are left untouched and bool converting checks for "true"/"yes"/"1".
        *
        */
        template<typename From, typename To>
        struct data_converter
        {
            static To get(const From data);
            static To get(const From data, const To);
        };
        template<>
        struct data_converter<std::string, std::string>
        {
            static const std::string & get(const std::string & data);
            static const std::string & get(const std::string & data, const std::string & defaultValue);
        };
        template<typename To>
        struct data_converter<std::string, To>
        {
            static To get(const std::string & data);
            static To get(const std::string & data, const To default_value);
        };
        template<typename From>
        struct data_converter<From, std::string>
        {
            static std::string get(const From data);
            static std::string get(const From data, const std::string & default_value);
        };
        template<>
        struct data_converter<std::string, bool>
        {
            static bool get(const std::string & data);
            static bool get(const std::string & data, const bool default_value);
        };
        template<>
        struct data_converter<bool, std::string>
        {
            static std::string get(const bool data);
        };




        /**
        * @brief Helper class for writing strings.
        *
        */
        template<typename Writer>
        class string_writer
        {

        public:

            string_writer(Writer & writer);

            void write(const std::string & out);
            void write(const char * out);

        private:

            Writer & m_out;

        };
        template<>
        class string_writer<std::string>
        {

        public:

            string_writer(std::string & out);
            void write(const std::string & out);
            void write(const char * out);

        private:

            std::string & m_out;

        };


        /**
        * @brief Base class for node implementation.
        *        Inherited by null, map, scalar and sequence node implementation classes.
        *
        */
        class node_impl
        {

        public:

            virtual ~node_impl();

            virtual const node & at(const std::string & key) const;

            virtual void clear();

            virtual node_data_type data_type() const;

            virtual size_t size() const;

        };

        class null_node_impl : public node_impl
        {

        public:

        };

        class map_node_impl : public node_impl
        {

        public:

            const node & at(const std::string & key) const;

            node_map::iterator begin();
            node_map::const_iterator begin() const;
            node_map::iterator end();
            node_map::const_iterator end() const;

            void clear();

            bool erase(const std::string & key);
            node_map::iterator erase(node_map::iterator & it);
            node_map::const_iterator erase(node_map::const_iterator & it);

            bool exists(const std::string & key) const;

            node_map::iterator find(const std::string & key);
            node_map::const_iterator find(const std::string & key) const;

            node & find_or_insert(const std::string & key);

            template<typename T>
            node & insert(const std::string & key, const T value);

            template<typename T>
            std::pair<node_map::iterator, bool> insert(const std::pair<const std::string &, const T> & pair);

            size_t size() const;

        private:

            node_map m_nodes;

        };

        class scalar_node_impl : public node_impl
        {

        public:

            scalar_node_impl(const node_data_type data_type = node_data_type::null);
            scalar_node_impl(const scalar_node_impl & scalar_node);

            scalar_node_impl(const bool boolean);
            scalar_node_impl(const float float32);
            scalar_node_impl(const double float64);
            scalar_node_impl(const int32_t int32);
            scalar_node_impl(const int64_t int64);
            scalar_node_impl(const std::string & string);
            scalar_node_impl(const char * string);

            ~scalar_node_impl();

            template<typename T>
            T as() const;

            template<typename T>
            T as(const T & default_value) const;

            void clear();

            node_data_type data_type() const;

            void set(const node_data_type data_type);

            template<typename T>
            void set(T value);
            void set(const std::string &);

        private:

            union value
            {
                bool boolean;
                float float32;
                double float64;
                int32_t int32;
                int64_t int64;
                std::string * string;
            };

            node_data_type m_data_type;    ///< Data type of scalar node.
            value m_value;                 ///< Union of the different data type values.

        };


        class sequence_node_impl : public node_impl
        {

        public:

            node_list::iterator begin();
            node_list::const_iterator begin() const;
            node_list::iterator end();
            node_list::const_iterator end() const;

            void clear();

            node_list::iterator erase(node_list::iterator & it);
            node_list::const_iterator erase(node_list::const_iterator & it);

            template<typename T>
            node & push_back(const T value);

            template<typename T>
            node & push_front(const T value);

            size_t size() const;

        private:

            node_list m_nodes;

        };


        class iterator_impl
        {

        public:

            virtual ~iterator_impl();

        };

        class const_iterator_impl
        {

        public:

            virtual ~const_iterator_impl();

        };

        class map_iterator_impl : public iterator_impl
        {

        public:

            map_iterator_impl(node_map::iterator it);

            node_map::iterator it;

        };
        class map_const_iterator_impl : public const_iterator_impl
        {

        public:

            map_const_iterator_impl(node_map::const_iterator it);

            node_map::const_iterator it;

        };


        class sequence_iterator_impl : public iterator_impl
        {

        public:

            sequence_iterator_impl(node_list::iterator it);

            node_list::iterator it;

        };
        class sequence_const_iterator_impl : public const_iterator_impl
        {

        public:

            sequence_const_iterator_impl(node_list::const_iterator it);

            node_list::const_iterator it;

        };


        // data_converter implementations.
        template<typename From, typename To>
        inline To data_converter<From, To>::get(const From data)
        {
            return static_cast<To>(data);
        }
        template<typename From, typename To>
        inline To data_converter<From, To>::get(const From data, const To)
        {
            return static_cast<To>(data);
        }

        inline const std::string & data_converter<std::string, std::string>::get(const std::string & data)
        {
            return data;
        }
        inline const std::string & data_converter<std::string, std::string>::get(const std::string & data, const std::string & default_value)
        {
            if (data.size() == 0)
            {
                return default_value;
            }
            return data;
        }
        
        template<typename To>
        inline To data_converter<std::string, To>::get(const std::string & data)
        {
            To type;
            std::stringstream ss(data);
            ss >> type;
            if (ss.fail())
            {
                return static_cast<To>(0);
            }
            return type;
        }
        template<typename To>
        inline To data_converter<std::string, To>::get(const std::string & data, const To default_value)
        {
            To type;
            std::stringstream ss(data);
            ss >> type;
            if (ss.fail())
            {
                return default_value;
            }
            return type;
        }
        
        template<typename From>
        inline std::string data_converter<From, std::string>::get(const From data)
        {
            std::stringstream ss;
            ss << data;
            if (ss.fail())
            {
                return "";
            }
            return ss.str();
        }
        template<typename From>
        inline std::string data_converter<From, std::string>::get(const From data, const std::string & default_value)
        {
            std::stringstream ss;
            ss << data;
            if (ss.fail())
            {
                return default_value;
            }
            return ss.str();
        }
        
        inline bool data_converter<std::string, bool>::get(const std::string & data)
        {
            if (data.size() > 4)
            {
                return false;
            }

            std::string tmp_data = data;
            std::transform(tmp_data.begin(), tmp_data.end(), tmp_data.begin(), [](const std::string::value_type c)
            {
                return static_cast<const char>(::tolower(static_cast<int>(c)));
            });

            if (!data.size())
            {
                return false;
            }

            switch (tmp_data[0])
            {
                case 't': return data.size() == 4 && strcmp(&tmp_data[1], "rue") == 0;
                case 'y': return data.size() == 3 && strcmp(&tmp_data[1], "es") == 0;
                case '1': return data.size() == 1;
                default: break;
            }

            return false;
        }
        inline bool data_converter<std::string, bool>::get(const std::string & data, const bool default_value)
        {
            if (data.size() > 5)
            {
                return default_value;
            }

            std::string tmp_data = data;
            std::transform(tmp_data.begin(), tmp_data.end(), tmp_data.begin(), [](const std::string::value_type c)
            {
                return static_cast<const char>(::tolower(static_cast<int>(c)));
            });

            if (!data.size())
            {
                return default_value;
            }

            switch (tmp_data[0])
            {
                case 't': if (data.size() == 4 && strcmp(&tmp_data[1], "rue") == 0) { return true; } break;
                case 'y': if (data.size() == 3 && strcmp(&tmp_data[1], "es") == 0) { return true; } break;
                case '1': if (data.size() == 1) { return true; } break;
                case 'f': if (data.size() == 5 && strcmp(&tmp_data[1], "alse") == 0) { return false; } break;
                case 'n': if (data.size() == 2 && strcmp(&tmp_data[1], "o") == 0) { return false; } break;
                case '0': if (data.size() == 1) { return false; } break;
                default: break;
            }

            return default_value;
        }
        
        inline std::string data_converter<bool, std::string>::get(const bool data)
        {
            return data ? "true" : "false";
        }


        // string_writer implementations
        template<typename Writer>
        inline string_writer<Writer>::string_writer(Writer & writer) :
            m_out(writer)
        { }

        template<typename Writer>
        inline void string_writer<Writer>::write(const std::string & out)
        {
            m_out << out;
        }
        template<typename Writer>
        inline void string_writer<Writer>::write(const char * out)
        {
            m_out << out;
        }

        inline string_writer<std::string>::string_writer(std::string & out) :
                m_out(out)
        { }

        void inline string_writer<std::string>::write(const std::string & out)
        {
            m_out += out;
        }
        void inline string_writer<std::string>::write(const char * out)
        {
            m_out += out;
        }


        // map_node_impl implementations.
        template<typename T>
        node & map_node_impl::insert(const std::string & key, const T value)
        {
            auto it = m_nodes.find(key);
            if (it != m_nodes.end())
            {
                node & old_node = *it->second;
                old_node = value;
                return old_node;
            }

            node * new_node = new node(value);
            node_map::value_type node_pair{key, node_unique_ptr(new_node)};
            m_nodes.insert(std::move(node_pair));

            return *new_node;
        }

        template<typename T>
        std::pair<node_map::iterator, bool> map_node_impl::insert(const std::pair<const std::string &, const T> & pair)
        {
            auto it = m_nodes.find(pair.first);
            if (it != m_nodes.end())
            {
                node & old_node = *it->second;
                old_node = pair.second;
                return { it, false };
            }

            node * new_node = new node(pair.second);
            node_map::value_type node_pair{pair.first, node_unique_ptr(new_node)};
            return m_nodes.insert(std::move(node_pair));
        }


        // scalar_node_impl implementations.
        template<typename T>
        inline T scalar_node_impl::as() const
        {
            switch (m_data_type)
            {
                case node_data_type::boolean:   return static_cast<T>(m_value.boolean);
                case node_data_type::float32:   return static_cast<T>(m_value.float32);
                case node_data_type::float64:   return static_cast<T>(m_value.float64);
                case node_data_type::int32:     return static_cast<T>(m_value.int32);
                case node_data_type::int64:     return static_cast<T>(m_value.int64);
                case node_data_type::string:    return data_converter<std::string, T>::get(*m_value.string);
                default: break;
            }
            return scalar_default_value<T>::value;
        }
        template<>
        inline std::string scalar_node_impl::as<std::string>() const
        {
            switch (m_data_type)
            {
                case node_data_type::boolean:   return data_converter<bool, std::string>::get(m_value.boolean);
                case node_data_type::float32:   return data_converter<float, std::string>::get(m_value.float32);
                case node_data_type::float64:   return data_converter<double, std::string>::get(m_value.float64);
                case node_data_type::int32:     return data_converter<int32_t, std::string>::get(m_value.int32);
                case node_data_type::int64:     return data_converter<int64_t, std::string>::get(m_value.int64);
                case node_data_type::string:    return *m_value.string;
                default: break;
            }
            return scalar_default_value<std::string>::value;
        }

        template<typename T>
        inline T scalar_node_impl::as(const T & default_value) const
        {
            switch (m_data_type)
            {
                case node_data_type::boolean:   return static_cast<T>(m_value.boolean);
                case node_data_type::float32:   return static_cast<T>(m_value.float32);
                case node_data_type::float64:   return static_cast<T>(m_value.float64);
                case node_data_type::int32:     return static_cast<T>(m_value.int32);
                case node_data_type::int64:     return static_cast<T>(m_value.int64);
                case node_data_type::string:    return data_converter<std::string, T>::get(*m_value.string, default_value);
                default: break;
            }
            return default_value;
        }
        template<>
        inline std::string scalar_node_impl::as<std::string>(const std::string & default_value) const
        {
            switch (m_data_type)
            {
                case node_data_type::boolean:   return data_converter<bool, std::string>::get(m_value.boolean);
                case node_data_type::float32:   return data_converter<float, std::string>::get(m_value.float32);
                case node_data_type::float64:   return data_converter<double, std::string>::get(m_value.float64);
                case node_data_type::int32:     return data_converter<int32_t, std::string>::get(m_value.int32);
                case node_data_type::int64:     return data_converter<int64_t, std::string>::get(m_value.int64);
                case node_data_type::string:    return *m_value.string;
                default: break;
            }
            return default_value;
        }

        template<>
        inline void scalar_node_impl::set<bool>(const bool value)
        {
            if (m_data_type == node_data_type::string)
            {
                delete m_value.string;
            }
            m_data_type = node_data_type::boolean;
            m_value.boolean = value;
        }
        template<>
        inline void scalar_node_impl::set<float>(const float value)
        {
            if (m_data_type == node_data_type::string)
            {
                delete m_value.string;
            }
            m_data_type = node_data_type::float32;
            m_value.float32 = value;
        }
        template<>
        inline void scalar_node_impl::set<double>(const double value)
        {
            if (m_data_type == node_data_type::string)
            {
                delete m_value.string;
            }
            m_data_type = node_data_type::float64;
            m_value.float64 = value;
        }
        template<>
        inline void scalar_node_impl::set<int32_t>(const int32_t value)
        {
            if (m_data_type == node_data_type::string)
            {
                delete m_value.string;
            }
            m_data_type = node_data_type::int32;
            m_value.int32 = value;
        }
        template<>
        inline void scalar_node_impl::set<int64_t>(const int64_t value)
        {
            if (m_data_type == node_data_type::string)
            {
                delete m_value.string;
            }
            m_data_type = node_data_type::int64;
            m_value.int64 = value;
        }
        template<>
        inline void scalar_node_impl::set<const char *>(const char * value)
        {
            if (m_data_type != node_data_type::string)
            {
                m_value.string = new std::string;
            }
            m_data_type = node_data_type::string;
            *m_value.string = value;
        }

        inline void scalar_node_impl::set(const std::string & value)
        {
            if (m_data_type != node_data_type::string)
            {
                m_value.string = new std::string;
            }
            m_data_type = node_data_type::string;
            *m_value.string = value;
        }  


        // map_node_impl implementations.
        template<typename T>
        inline node & sequence_node_impl::push_back(const T value)
        {
            node * new_node = new node(value);
            m_nodes.push_back(node_unique_ptr(new_node));
            return *new_node;
        }

        template<typename T>
        inline node & sequence_node_impl::push_front(const T value)
        {
            node * new_node = new node(value);
            m_nodes.push_front(node_unique_ptr(new_node));
            return *new_node;
        }

    } //< End of private API namespace.



    // Node implementations.
    template<typename T>
    inline node::node(const T value) :
        m_type(node_type::scalar),
        m_impl(new priv::scalar_node_impl(value))
    { }

    template<typename T>
    inline T node::as() const
    {
        switch (m_type)
        {
            case node_type::scalar: if (m_impl) { return static_cast<priv::scalar_node_impl *>(m_impl.get())->as<T>(); }
            default: break;
        }

        return priv::scalar_default_value<T>::value;
    }

    template<typename T>
    inline T node::as(const T & default_value) const
    {
        switch (m_type)
        {
            case node_type::scalar: if (m_impl) { return static_cast<priv::scalar_node_impl *>(m_impl.get())->as<T>(default_value); }
            default: break;
        }

        return default_value;
    }

    template<typename T>
    node & node::insert(const std::string & key, const T value)
    {
        if (m_type != node_type::map)
        {
            m_type = node_type::map;
            m_impl.reset(new priv::map_node_impl);
        }

        return static_cast<priv::map_node_impl *>(m_impl.get())->insert(key, value);
    }

    template<typename T>
    std::pair<node::iterator, bool> node::insert(const std::pair<const std::string &, const T> & pair)
    {
        if (m_type != node_type::map)
        {
            m_type = node_type::map;
            m_impl.reset(new priv::map_node_impl);
        }

        return static_cast<priv::map_node_impl *>(m_impl.get())->insert(pair);
    }

    template<typename T>
    node & node::push_back(const T value)
    {
        if (m_type != node_type::sequence)
        {
            m_type = node_type::sequence;
            m_impl.reset(new priv::sequence_node_impl);
        }

        return static_cast<priv::sequence_node_impl *>(m_impl.get())->push_back(value);
    }

    template<typename T>
    node & node::push_front(const T value)
    {
        if (m_type != node_type::sequence)
        {
            m_type = node_type::sequence;
            m_impl.reset(new priv::sequence_node_impl);
        }

        return static_cast<priv::sequence_node_impl *>(m_impl.get())->push_front(value);
    }

    template<typename T>
    inline node & node::operator = (const T value)
    {
        if (m_type != node_type::scalar)
        {
            m_type = node_type::scalar;
            m_impl.reset(new priv::scalar_node_impl(value));
            return *this;
        }

        static_cast<priv::scalar_node_impl *>(m_impl.get())->set(value);
        return *this;
    }


    // Parsing implementations.
    template<typename Return>
    inline Return parse(const std::string &)
    {
        Return a;// (node_type::scalar);
        return a;
    }

    template<typename Return>
    inline Return parse(const char * /*buffer*/, const size_t /*size*/)
    {
        Return a;
        return a;
    }

    template<typename Return>
    inline Return parse_file(const std::string & /*filename*/)
    {
        Return a;
        return a;
    }
}

#endif
