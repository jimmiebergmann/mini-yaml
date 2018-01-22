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
#include <cstdio>
#include <stdarg.h>


// Implementation access definitions.
// ...

// Exception message definitions.
static const std::string g_ErrorKeyMissing = "Missing key.";
static const std::string g_ErrorTabInOffset = "Tab found in offset.";
static const std::string g_ErrorUnableToParse = "Unable to parse.";


namespace Yaml
{
	class ReaderLine;

	// Global function definitions. Implemented at end of this source file.
	static std::string ExceptionMessage(const std::string & message, ReaderLine & line);
	static std::string ExceptionMessage(const std::string & message, ReaderLine & line, const size_t errorPos);

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


	// Reader implementations
	/**
	* @breif Line information structure.
	*
	*/
	class ReaderLine
	{

	public:

		/**
		* @breif Default constructor.
		*
		*/
		ReaderLine() :
			Data(""),
			No(0),
			Offset(0),
			Type(Node::None),
			m_Flags(0)
		{
		}

		/**
		* @breif Constructor.
		*
		*/
		ReaderLine(const std::string & data, size_t no, size_t offset) :
			Data(data),
			No(no),
			Offset(offset),
			Type(Node::None),
			m_Flags(0)
		{
		}

		enum eFlag
		{
			LiteralTextFlag,
			FoldedTextFlag,
			NewLineFlag
		};

		void SetFlag(const eFlag flag)
		{
			m_Flags |= m_FlagBits[static_cast<size_t>(flag)];
		}

		bool GetFlag(const eFlag flag) const
		{
			return m_Flags & m_FlagBits[static_cast<size_t>(flag)];
		}

		std::string		Data;	///< Data of line.
		size_t			No;		///< Line number.
		size_t			Offset;	///< Offset to first character in data.
		Node::eType		Type;	///< Type of line.

	private:

		static const unsigned char m_FlagBits[3];

		/**
		* @breif Flags of line
		*		 Bits:
		*			0:  
		*
		*/
		unsigned char	m_Flags; ///< Flags of line.
	};

	const unsigned char ReaderLine::m_FlagBits[3] = { 0x01, 0x02, 0x04 };

	class ReaderImp
	{

	public:

		/**
		* @breif Default constructor.
		*
		*/
		ReaderImp()
		{
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
		* @breif Run full parsing procedure.
		*
		*/
		void Parse(Node & root, std::iostream & stream)
		{
			ReadLines(stream);
			PostProcessLines();
			Print();
		}

		/**
		* @breif Read all lines.
		*		 Ignoring:
		*			- Empty lines.
		*			- Comments.
		*			- Document start/end.
		*
		*/
		void ReadLines(std::iostream & stream)
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

				m_Lines.push_back(new ReaderLine(line, lineNo, 0));
			}

			for (auto it = m_Lines.begin(); it != m_Lines.end();)
			{
				ReaderLine * pLine = *it;

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
					throw ParsingException(ExceptionMessage(g_ErrorTabInOffset, *pLine, firstTabPos));
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
				ReaderLine * pLine = *it;
				if (PostProcessSequence(it) == true)
				{
					pLine->Type = Node::SequenceType;
					++it;
					continue;
				}

				if (PostProcessMapping(it) == true)
				{
					pLine->Type = Node::MappingType;
					++it;
					continue;
				}

				if (PostProcessScalar(it) == true)
				{
					pLine->Type = Node::ScalarType;
					++it;
					continue;
				}

				throw ParsingException(ExceptionMessage(g_ErrorUnableToParse, *pLine));
			}
		}

		void Print()
		{
			for (auto it = m_Lines.begin(); it != m_Lines.end(); it++)
			{

				ReaderLine * pLine = *it;

				// Print type
				if (pLine->Type == Node::SequenceType)
				{
					std::cout << "seq ";
				}
				else if (pLine->Type == Node::MappingType)
				{
					std::cout << "map ";
				}
				else if (pLine->Type == Node::ScalarType)
				{
					std::cout << "sca ";
				}
				else
				{
					std::cout << "    ";
				}

				// Print flags
				if (pLine->GetFlag(ReaderLine::FoldedTextFlag))
				{
					std::cout << "f";
				}
				else
				{
					std::cout << " ";
				}
				if (pLine->GetFlag(ReaderLine::LiteralTextFlag))
				{
					std::cout << "l";
				}
				else
				{
					std::cout << " ";
				}
				if (pLine->GetFlag(ReaderLine::NewLineFlag))
				{
					std::cout << "n";
				}
				else
				{
					std::cout << " ";
				}

				std::cout << "| ";
				std::cout << std::string(pLine->Offset, ' ');
				std::cout << pLine->Data << std::endl;

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
		* @breif Copy constructor.
		*
		*/
		ReaderImp(const ReaderImp & copy)
		{

		}


		/**
		* @breif Run post-processing and check for sequence.
		*		 Split line into two lines if sequence token is not on it's own line.
		*		 
		* @return true if line is sequence, else false.
		*
		*/
		bool PostProcessSequence(std::list<ReaderLine *>::iterator & it)
		{
			ReaderLine * pLine = *it;

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
			if (splitPos == 1)
			{
				return false;
			}
			if (splitPos == std::string::npos)
			{
				return true;
			}

			// Create new line and insert
			std::string newLine = pLine->Data.substr(splitPos);
			it = m_Lines.insert(++it, new ReaderLine(newLine, pLine->No, pLine->Offset + splitPos));
			--it;
			pLine->Data = "-";

			return true;
		}

		/**
		* @breif Run post-processing and check for mapping.
		*		 Split line into two lines if mapping value is not on it's own line.
		*
		* @return true if line is mapping, else false.
		*
		*/
		bool PostProcessMapping(std::list<ReaderLine *>::iterator & it)
		{
			ReaderLine * pLine = *it;
			size_t tokenPos = pLine->Data.find_first_of(":");
			size_t expectedTokenPos = tokenPos;
			if (tokenPos == std::string::npos)
			{
				return false;
			}

			bool missingKey = false;
			if (tokenPos == 0)
			{
				missingKey = true;
			}
			else
			{
				expectedTokenPos = pLine->Data.find_last_not_of(" \t", tokenPos - 1);
				if (expectedTokenPos == std::string::npos)
				{
					missingKey = true;
				}
				expectedTokenPos++;
			}
			if (missingKey)
			{
				throw ParsingException(ExceptionMessage(g_ErrorKeyMissing, *pLine));
			}

			size_t eraseSize = 0;
			if (tokenPos != expectedTokenPos)
			{
				eraseSize = tokenPos - expectedTokenPos;
				pLine->Data.erase(expectedTokenPos, eraseSize);
				tokenPos -= eraseSize;
			}
			
			if (tokenPos + 1 == pLine->Data.size())
			{
				return true;
			}

			// Create new line and insert
			const size_t splitStart = pLine->Data.find_first_not_of(" \t", tokenPos + 1);
			std::string newLine = pLine->Data.substr(splitStart);
			it = m_Lines.insert(++it, new ReaderLine(newLine, pLine->No, pLine->Offset + splitStart + eraseSize));
			--it;

			// Resize old line.
			pLine->Data.resize(tokenPos + 1);

			return true;
		}

		/**
		* @breif Run post-processing and check for scalar.
		*		 Checking for multi-line scalars.
		*
		* @return true if line is scalar, else false.
		*
		*/
		bool PostProcessScalar(std::list<ReaderLine *>::iterator & it)
		{
			return true;
		}

		std::list<ReaderLine *>	m_Lines;	///< List of lines.

	};

	// Reader class
	Reader::Reader()
	{
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
		ReaderImp * pImp = nullptr;

		try
		{
			pImp = new ReaderImp;
			pImp->Parse(root, stream);
			delete pImp;
		}
		catch (const Exception & e)
		{
			delete pImp;
			throw;
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



	// Static function implementations
	std::string ExceptionMessage(const std::string & message, ReaderLine & line)
	{
		return message + std::string(" Line ") + std::to_string(line.No) + std::string(": ") + line.Data;
	}

	std::string ExceptionMessage(const std::string & message, ReaderLine & line, const size_t errorPos)
	{
		return message + std::string(" Line ") + std::to_string(line.No) + std::string(" column ") + std::to_string(errorPos + 1) + std::string(": ") + line.Data;
	}

}