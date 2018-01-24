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
	* @breif Exception class.
	*
	*/
	class Exception : public std::exception
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
	* @see Excetion
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
	* @see Excetion
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
	* @see Excetion
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
	*			- Scalar
	*			- Sequence
	*			- Mapping
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
			ScalarType,
			SequenceType,
			MappingType
		};

		/**
		* @breif Default constructor.
		*		 Type = None.
		*
		*/
		Node();

	private:

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
		* @throw InternalException	An internal error occured.
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