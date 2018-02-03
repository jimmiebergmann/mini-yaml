/*
* MIT License
*
* Copyright(c) 2018 Jimmie Bergmann
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

#pragma once

#include <exception>
#include <string>
#include <iostream>
#include <sstream>

/**
* @breif Namespace wrapping mini-yaml classes.
*
*/
namespace Yaml
{

    /**
	* @breif Helper classes and functions
	*
	*/
    namespace impl
	{

	    /**
        * @breif Helper functionality, converting string to any data type.
        *        Strings are left untouched.
        *
        */
	    template<typename T>
        struct StringConverter
        {
            static T Get(const std::string & data)
            {
                T type;
                std::stringstream ss(data);
                ss >> type;
                return type;
            }
        };
        template<>
        struct StringConverter<std::string>
        {
            static std::string Get(const std::string & data)
            {
                return data;
            }
        };

	}


	/**
	* @breif Exception class.
	*
	*/
	class Exception : public std::runtime_error
	{

	public:

		/**
		* @breif Enumeration of exception types.
		*
		*/
		enum eType
		{
			InternalError,	///< Internal error.
			ParsingError,	///< Invalid parsing data.
			OperationError	///< User operation error.
		};

		/**
		* @breif Constructor.
		*
		* @param message	Exception message.
		* @param type		Type of exception.
		*
		*/
		Exception(const std::string & message, const eType type);

		/**
		* @breif Get type of exception.
		*
		*/
		eType Type() const;

		/**
		* @breif Get message of exception.
		*
		*/
		const char * Message() const;

	private:

		eType m_Type;	///< Type of exception.

	};


	/**
	* @breif Internal exception class.
	*
	* @see Exception
	*
	*/
	class InternalException : public Exception
	{

	public:

		/**
		* @breif Constructor.
		*
		* @param message Exception message.
		*
		*/
		InternalException(const std::string & message);

	};


	/**
	* @breif Parsing exception class.
	*
	* @see Exception
	*
	*/
	class ParsingException : public Exception
	{

	public:

		/**
		* @breif Constructor.
		*
		* @param message Exception message.
		*
		*/
		ParsingException(const std::string & message);

	};


	/**
	* @breif Operation exception class.
	*
	* @see Exception
	*
	*/
	class OperationException : public Exception
	{

	public:

		/**
		* @breif Constructor.
		*
		* @param message Exception message.
		*
		*/
		OperationException(const std::string & message);

	};


	/**
	* @breif Node class.
	*		 Types of nodes:
	*			- Sequence
	*			- Map
    *           - Scalar
	*
	*
	*/
	class Node
	{

	public:

		/**
		* @breif Enumeration of node types.
		*
		*/
		enum eType
		{
			None,
			SequenceType,
			MapType,
			ScalarType
		};

		/**
		* @breif Default constructor.
		*
		*/
		Node();

		/**
		* @breif Destructor.
		*
		*/
		~Node();

        /**
		* @breif Functions for checking type of node.
		*
		*/
		eType GetType() const;
		bool IsSequence() const;
		bool IsMap() const;
		bool IsScalar() const;

        /**
		* @breif Completely clear node.
		*
		*/
		void Clear();

        /**
		* @breif Get node as given template type.
		*
		*/
		template<typename T>
		T As() const
		{
		    return impl::StringConverter<T>::Get(AsString());
		}

		/**
		* @breif Get size of node.
		*        'Scalar' types will always return 1.
		*        'None' types will always return 0.
		*
		*/
		size_t Size() const;

		// Sequence operators

		/**
		* @breif Insert sequence item at given index.
		*        Converts node to sequence type if needed.
		*        Adding new item to end of sequence if index is larger than sequence size.
		*
		*/
        Node & Insert(const size_t index);

        /**
		* @breif Add new sequence index to back.
		*        Converts node to sequence type if needed.
		*
		*/
		Node & PushFront();

		 /**
		* @breif Add new sequence index to front.
		*        Converts node to sequence type if needed.
		*
		*/
		Node & PushBack();

		/**
		* @breif Get sequence item.
		*        Converts node to sequence type if needed.
		*        Not creating new Node if index is unknown.
		*
		*/
		Node & operator []  (const size_t index);

		/**
		* @breif Get map item.
		*        Converts node to map type if needed.
		*        Creating new node of type 'None' if key is unknown.
		*
		*/
		Node & operator [] (const std::string & key);

		/**
		* @breif Erase sequence item.
		*        No action if node is not a sequence.
		*
		*/
		void Erase(const size_t index);

		/**
		* @breif Erase map item.
		*        No action if node is not an map.
		*
		*/
		void Erase(const std::string & key);

		/**
		* @breif Set scalar value.
		*        Converts node to scalar type if needed.
		*
		*/
		Node & operator =   (const std::string & value);

	private:

        /**
		* @breif Copy constructor.
		*
		*/
        Node(const Node & node);

        /**
		* @breif Get as string. If type is scalar, else empty.
		*
		*/
		const std::string & AsString() const;

		void * m_pImp; ///< Implementation of node class.

	};


	/**
	* @breif Reader class.
	*		 Populating given node with parsed YAML data.
	*
	*/
	class Reader
	{

	public:

		/**
		* @breif Default constructor.
		*		 Call parsing functions later in order to deserialize.
		*
		*/
		Reader();

		/**
		* @breif Constructing reader class and deserializing.
		*
		* @see Parse
		*
		*/
		Reader(Node & root, const char * filename);
		Reader(Node & root, std::iostream & stream);
		Reader(Node & root, const std::string & string);
		Reader(Node & root, const char * buffer, const size_t size);

		/**
		* @breif Parsing classes.
		*		 Population given root node with deserialized data.
		*
		* @param root		Root node to populate.
		* @param filename	Path of input file.
		* @param stream		Input stream.
		* @param string		String of input data.
		* @param buffer		Char array of input data.
		* @param size		Buffer size.
		*
		* @throw InternalException	An internal error occurred.
		* @throw ParsingException	Invalid input YAML data.
		* @throw OperationException	If filename or buffer pointer is invalid.
		*
		*/
		void Parse(Node & root, const char * filename);
		void Parse(Node & root, std::iostream & stream);
		void Parse(Node & root, const std::string & string);
		void Parse(Node & root, const char * buffer, const size_t size);

	};

}
