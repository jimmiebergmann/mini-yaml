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

static const std::string g_ErrorInvalidCharacter		= "Invalid character found.";
static const std::string g_ErrorKeyMissing				= "Missing key.";
static const std::string g_ErrorTabInOffset				= "Tab found in offset.";
static const std::string g_ErrorBlockSequenceNotAllowed = "Block sequence entries are not allowed in this context.";
//static const std::string g_ErrorUnableToParse			= "Unable to parse.";
static const std::string g_ErrorUnexpectedDocumentEnd	= "Unexpected document end.";


namespace Yaml
{
	class ReaderLine;

	// Global function definitions. Implemented at end of this source file.
	static std::string ExceptionMessage(const std::string & message, ReaderLine & line);
	static std::string ExceptionMessage(const std::string & message, ReaderLine & line, const size_t errorPos);
	static std::string ExceptionMessage(const std::string & message, const size_t errorLine, const size_t errorPos);

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
			Flags(0),
			NextLine(nullptr)
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
			Flags(0),
			NextLine(nullptr)
		{
		}

		enum eFlag
		{
			LiteralScalarFlag,		///< Litteral scalar type, defined as "|".
			FoldedScalarFlag,		///< Folded scalar type, defined as "<".
			ScalarNewlineFlag,		///< Scalar ends with a newline.
		};

		/**
		* @breif Set flag.
		*
		*/
		void SetFlag(const eFlag flag)
		{
			Flags |= m_FlagMask[static_cast<size_t>(flag)];
		}

		/**
		* @breif Get flag value.
		*
		*/
		bool GetFlag(const eFlag flag) const
		{
			return Flags & m_FlagMask[static_cast<size_t>(flag)];
		}

		/**
		* @breif Copy and replace scalar flags from another ReaderLine.
		*
		*/
		void CopyScalarFlags(ReaderLine * from)
		{
			if (from == nullptr)
			{
				return;
			}

			unsigned char newFlags = from->Flags & (m_FlagMask[0] | m_FlagMask[1] | m_FlagMask[2]);
			Flags |= newFlags;
		}

		std::string		Data;		///< Data of line.
		size_t			No;			///< Line number.
		size_t			Offset;		///< Offset to first character in data.
		Node::eType		Type;		///< Type of line.
		unsigned char	Flags;		///< Flags of line.
		ReaderLine *	NextLine;	///< Pointer to next line.

	private:

		static const unsigned char m_FlagMask[3];
		
	};

	const unsigned char ReaderLine::m_FlagMask[3] = { 0x01, 0x02, 0x04 };

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

				for (size_t i = 0; i < line.size(); i++)
				{
					if (line[i] != '\t' && (line[i] < 32 || line[i] > 125))
					{
						throw ParsingException(ExceptionMessage(g_ErrorInvalidCharacter, lineNo, i + 1));
					}
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

				// Sequence.
				if (PostProcessSequence(it) == true)
				{
					continue;
				}

				// Mapping.
				if (PostProcessMapping(it) == true)
				{
					continue;
				}

				// Scalar.
				PostProcessScalar(it);
			}

			// Set next line of all lines.
			if (m_Lines.size())
			{
				if (m_Lines.back()->Type != Node::ScalarType)
				{
					throw ParsingException(ExceptionMessage(g_ErrorUnexpectedDocumentEnd, *m_Lines.back()));
				}

				if (m_Lines.size() > 1)
				{
					auto prevEnd = m_Lines.end();
					--prevEnd;

					for (auto it = m_Lines.begin(); it != prevEnd; it++)
					{
						auto nextIt = it;
						++nextIt;

						(*it)->NextLine = *nextIt;
					}
				}
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
				if (pLine->GetFlag(ReaderLine::FoldedScalarFlag))
				{
					std::cout << "f";
				}
				else
				{
					std::cout << "-";
				}
				if (pLine->GetFlag(ReaderLine::LiteralScalarFlag))
				{
					std::cout << "l";
				}
				else
				{
					std::cout << "-";
				}
				if (pLine->GetFlag(ReaderLine::ScalarNewlineFlag))
				{
					std::cout << "n";
				}
				else
				{
					std::cout << "-";
				}
				if (pLine->NextLine == nullptr)
				{
					std::cout << "e";
				}
				else
				{
					std::cout << "-";
				}


				std::cout << "| ";
				std::cout << std::string(pLine->Offset, ' ');

				if (pLine->Type == Node::ScalarType)
				{
					std::string scalarValue = pLine->Data;
					for (size_t i = 0; (i = scalarValue.find("\n", i)) != std::string::npos;)
					{
						scalarValue.replace(i, 1, "\\n");
						i += 2;
					}
					std::cout << scalarValue << std::endl;
				}
				else if (pLine->Type == Node::MappingType)
				{
					std::cout << pLine->Data + ":" << std::endl;
				}
				else if (pLine->Type == Node::SequenceType)
				{
					std::cout << "-" << std::endl;
				}
				else
				{
					std::cout << "> UNKOWN TYPE <" << std::endl;
				}
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
			if (IsSequenceStart(pLine->Data) == false)
			{
				return false;
			}

			pLine->Type = Node::SequenceType;

			const size_t valueStart = pLine->Data.find_first_not_of(" \t", 1);
			if (valueStart == std::string::npos)
			{
				++it;
				return true;
			}

			// Create new line and insert
			std::string newLine = pLine->Data.substr(valueStart);
			it = m_Lines.insert(++it, new ReaderLine(newLine, pLine->No, valueStart));
			pLine->Data = "";

			return false;
		}

		/**
		* @breif Run post-processing and check for mapping.
		*		 Split line into two lines if mapping value is not on it's own line.
		*
		* @return true if line is mapping, else move on to scalar parsing.
		*
		*/
		bool PostProcessMapping(std::list<ReaderLine *>::iterator & it)
		{
			ReaderLine * pLine = *it;

			// Find map token.
			size_t tokenPos = pLine->Data.find_first_of(":");
			if (tokenPos == std::string::npos)
			{
				return false;
			}

			pLine->Type = Node::MappingType;

			// Get key
			std::string key = pLine->Data.substr(0, tokenPos);
			const size_t keyEnd = key.find_last_not_of(" \t");
			if (keyEnd == std::string::npos)
			{
				throw ParsingException(ExceptionMessage(g_ErrorKeyMissing, *pLine));
			}
			key.resize(keyEnd + 1);

			// Get value
			std::string value = "";
			size_t valueStart = std::string::npos;
			if (tokenPos + 1 != pLine->Data.size())
			{
				valueStart = pLine->Data.find_first_not_of(" \t", tokenPos + 1);
				if (valueStart != std::string::npos)
				{
					value = pLine->Data.substr(valueStart);
				}
			}

			// Make sure the value is not a sequence start.
			if (IsSequenceStart(value) == true)
			{
				throw ParsingException(ExceptionMessage(g_ErrorBlockSequenceNotAllowed, *pLine, valueStart));
			}

			// Set line data as key
			pLine->Data = key;

			// Move to next line
			++it;

			// Add new line with value
			if (value.size())
			{
				if (value == "|")
				{
					pLine->SetFlag(ReaderLine::LiteralScalarFlag);
					pLine->SetFlag(ReaderLine::ScalarNewlineFlag);
				}
				else if (value == ">")
				{
					pLine->SetFlag(ReaderLine::FoldedScalarFlag);
					pLine->SetFlag(ReaderLine::ScalarNewlineFlag);
				}
				else if (value == "|-")
				{
					pLine->SetFlag(ReaderLine::LiteralScalarFlag);
				}
				else if (value == ">-")
				{
					pLine->SetFlag(ReaderLine::FoldedScalarFlag);
				}
				else
				{
					it = m_Lines.insert(it, new ReaderLine(value, pLine->No, valueStart));
					return false;
				}

				return false;
			}
			else
			{
				// Add empty scalar value if next offset is the same or lower than this one.
				if (it != m_Lines.end() && (*it)->Offset <= pLine->Offset)
				{
					it = m_Lines.insert(it, new ReaderLine("", pLine->No, tokenPos + 1));
					(*it)->Type = Node::ScalarType;
					++it;
					return true;
				}
			}

			return true;
		}

		/**
		* @breif Run post-processing and check for scalar.
		*		 Checking for multi-line scalars.
		*
		* @return true if scalar search should continue, else false.
		*
		*/
		void PostProcessScalar(std::list<ReaderLine *>::iterator & it)
		{
			ReaderLine * pLine = *it;
			pLine->Type = Node::ScalarType;

			// Copy scalar flags.
			if (it != m_Lines.begin())
			{
				// Get flags from previous line.
				std::list<ReaderLine *>::iterator prevIt = it;
				--prevIt;
				ReaderLine * pPrevLine = *prevIt;
				pLine->CopyScalarFlags(pPrevLine);
			}

			// Move to next line
			++it;

			// Check if next line should be merged.
			if (pLine->GetFlag(ReaderLine::FoldedScalarFlag) || pLine->GetFlag(ReaderLine::LiteralScalarFlag))
			{
				while (1)
				{
					if (it == m_Lines.end() || (*it)->Offset < pLine->Offset)
					{
						if (pLine->GetFlag(ReaderLine::ScalarNewlineFlag) == true)
						{
							pLine->Data += "\n"
						}
						return;
					}

					ReaderLine * pNextLine = *it;
					pLine->Data += "\n";
					pLine->Data += std::string(pNextLine->Offset - pLine->Offset, ' ');
					pLine->Data += pNextLine->Data;
					it = m_Lines.erase(it);
				}
			}	
		}

		bool IsSequenceStart(const std::string & data) const
		{
			if (data.size() == 0 || data[0] != '-')
			{
				return false;
			}

			if (data.size() >= 2 && data[1] != ' ')
			{
				return false;
			}

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
		catch (const Exception e)
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

	std::string ExceptionMessage(const std::string & message, const size_t errorLine, const size_t errorPos)
	{
		return message + std::string(" Line ") + std::to_string(errorLine) + std::string(" column ") + std::to_string(errorPos);
	}

}