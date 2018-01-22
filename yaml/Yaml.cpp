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

#include "Yaml.hpp"
#include <memory>
#include <fstream>
#include <sstream>
#include <list>

// Implementation access definitions.
// ...

namespace Yaml
{

	// Exception implementations
	Exception::Exception(const std::string & message, const eType type) :
		std::exception(message.c_str()),
		m_Type(type)
	{
	}

	Exception::eType Exception::Type() const
	{
		return m_Type;
	}

	const char * Exception::Message() const
	{
		return what();
	}

	InternalException::InternalException(const std::string & message) :
		Exception(message, InternalError)
	{

	}

	ParsingException::ParsingException(const std::string & message) :
		Exception(message, ParsingError)
	{

	}

	OperationException::OperationException(const std::string & message) :
		Exception(message, OperationError)
	{

	}


	// Node class
	Node::Node()
	{

	}


	// Reader implementation
	class ReaderImp
	{

	public:

		/**
		* @breif Constructor.
		*
		*/
		ReaderImp(Node & root, std::iostream & stream)
		{
			ReadLines(stream);
			PostProcessLines();
		}

		/**
		* @breif Destructor.
		*
		*/
		~ReaderImp()
		{
			ClearLines();
		}

		/**
		* @breif Read all lines.
		*		 Ignoring:
		*			- Empty lines.
		*			- Comments.
		*			- Document start/end.
		*
		*/
		bool ReadLines(std::iostream & stream)
		{
			std::string line = "";
			size_t		lineNo = 0;
			bool		documentStartFound = false;

			// Read all lines, as long as the stream is ok.
			while (!stream.eof() && !stream.fail())
			{
				// Read line
				std::getline(stream, line);
				lineNo++;

				// Start of document.
				if (documentStartFound == false && line == "---")
				{
					// Erase all lines before this line.
					for (auto it = m_Lines.begin(); it != m_Lines.end(); it++)
					{
						delete *it;
					}
					m_Lines.clear();
					documentStartFound = true;
					continue;
				}

				// End of document.
				if (line == "...")
				{
					break;
				}

				// Remove trailing return.
				if (line.size())
				{
					if (line[line.size() - 1] == '\r')
					{
						line.resize(line.size() - 1);
					}
				}

				// This is an empty line, ignore.
				if (line.size() == 0)
				{
					continue;
				}

				m_Lines.push_back(new Line(line, lineNo, 0));
			}

			for (auto it = m_Lines.begin(); it != m_Lines.end();)
			{
				Line * pLine = *it;

				// Find first/last of tabs and spaces.
				const size_t firstTabPos	= pLine->Data.find_first_of('\t');
				const size_t startOffset	= pLine->Data.find_first_not_of(" \t");
				const size_t endOffset		= pLine->Data.find_last_not_of(" \t");

				// No data in line
				if (startOffset == std::string::npos)
				{
					delete pLine;
					it = m_Lines.erase(it);
					continue;
				}

				// Do not allow tabs in offset.
				if (firstTabPos < startOffset)
				{
					throw ParsingException("Tab found in offset. Line " + std::to_string(lineNo) + " column " + std::to_string(firstTabPos + 1) + "." );
				}

				// Remove starting/ending spaces
				pLine->Data = pLine->Data.substr(startOffset).substr(0, endOffset - startOffset + 1);
				pLine->Offset = startOffset;

				// Move to next line.
				++it;
			}
		}

		/**
		* @breif Run post-processing on all lines.
		*		 Basically split lines into multiple lines if needed, to follow the parsing algorithm.
		*
		*/
		void PostProcessLines()
		{
			for (auto it = m_Lines.begin(); it != m_Lines.end();)
			{
				if (PostProcessSequenceToken(it) == true)
				{
					++it;
					continue;
				}

				if (PostProcessScalar(it) == true)
				{
					++it;
					continue;
				}

				// No processing done on this line, move on.
				++it;
			}
		}

		/**
		* @breif Clear all read lines.
		*
		*/
		void ClearLines()
		{
			for (auto it = m_Lines.begin(); it != m_Lines.end(); it++)
			{
				delete *it;
			}
			m_Lines.clear();
		}

	private:


		/**
		* @breif Line information structure.
		*
		*/
		class Line
		{

		public:

			/**
			* @breif Default constructor.
			*
			*/
			Line() :
				Data(""),
				No(0),
				Offset(0)
			{
			}

			/**
			* @breif Constructor.
			*
			*/
			Line(const std::string & data, size_t no, size_t offset) :
				Data(data),
				No(no),
				Offset(offset)
			{
			}


			std::string		Data;	///< Data of line.
			size_t			No;		///< Line number.
			size_t			Offset;	///< Offset to first character in data.
		};


		/**
		* @breif Run post-processing and check for sequence.
		*		 Split line into two lines if sequence token is not on it's own line.
		*		 
		* @return true if line is sequence start, else false.
		*
		*/
		bool PostProcessSequenceToken(std::list<Line *>::iterator & it)
		{
			Line * pLine = *it;

			// Sequence split
			if (pLine->Data[0] != '-')
			{
				return false;
			}

			if (pLine->Data.size() == 1)
			{
				return true;
			}

			const size_t splitPos = pLine->Data.find_first_not_of(" \t", 1);
			if (splitPos == std::string::npos)
			{
				return true;
			}

			// Create new line and insert
			std::string newLine = pLine->Data.substr(splitPos);
			it = m_Lines.insert(++it, new Line(newLine, pLine->No, pLine->Offset + splitPos));
			pLine->Data = "-";

			return true;
		}

		/**
		* @breif Run post-processing and check for scalar.
		*		 Split line into two lines if scalar value is not on it's own line.
		*
		* @return true if line is scalar start, else false.
		*
		*/
		bool PostProcessScalar(std::list<Line *>::iterator & it)
		{
			Line * pLine = *it;
			const size_t tokenPos = pLine->Data.find_first_of(":");

			if (tokenPos == std::string::npos)
			{
				return false;
			}
			if (tokenPos == pLine->Data.size())
			{
				return true;
			}

			const size_t splitStart = pLine->Data.find_first_not_of(" \t", tokenPos + 1);
			if (splitStart == std::string::npos)
			{
				return true;
			}

			// Create new line and insert
			std::string newLine = pLine->Data.substr(splitStart);
			it = m_Lines.insert(++it, new Line(newLine, pLine->No, pLine->Offset + splitStart));
			
			// Resize old line.
			pLine->Data.resize(tokenPos + 1);


			return true;
		}

		std::list<Line *>	m_Lines;	///< List of lines.

	};

	// Reader class
	Reader::Reader() :
		m_pImp(nullptr)
	{
	}

	Reader::~Reader()
	{
		if (m_pImp != nullptr)
		{
			delete m_pImp;
		}
	}


	Reader::Reader(Node & root, const char * filename) :
		Reader()
	{
		Parse(root, filename);
	}

	Reader::Reader(Node & root, std::iostream & stream) :
		Reader()
	{
		Parse(root, stream);
	}

	Reader::Reader(Node & root, const std::string & string) :
		Reader()
	{
		Parse(root, string);
	}

	Reader::Reader(Node & root, const char * buffer, const size_t size) :
		Reader()
	{
		Parse(root, buffer, size);
	}

	void Reader::Parse(Node & root, const char * filename)
	{
		std::ifstream f(filename, std::ifstream::binary);
		if (f.is_open() == false)
		{
			throw OperationException("Cannot open file.");
		}

		f.seekg(0, f.end);
		size_t fileSize = static_cast<size_t>(f.tellg());
		f.seekg(0, f.beg);

		std::unique_ptr<char[]> data(new char[fileSize]);
		f.read(data.get(), fileSize);
		f.close();

		Parse(root, data.get(), fileSize);
	}

	void Reader::Parse(Node & root, std::iostream & stream)
	{
		if (m_pImp != nullptr)
		{
			delete m_pImp;
			m_pImp = nullptr;
		}

		try
		{
			m_pImp = new ReaderImp(root, stream);
			delete m_pImp;
			m_pImp = nullptr;
		}
		catch (const Exception & e)
		{
			delete m_pImp;
			m_pImp = nullptr;
			throw e;
		}
	}

	void Reader::Parse(Node & root, const std::string & string)
	{
		std::stringstream ss(string);
		Parse(root, ss);
	}

	void Reader::Parse(Node & root, const char * buffer, const size_t size)
	{
		std::stringstream ss(std::string(buffer, size));
		Parse(root, ss);
	}

}