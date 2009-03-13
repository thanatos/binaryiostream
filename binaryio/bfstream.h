/**
 * Copyright (C) 2008 Roy Wellington IV
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef BINARYIO__BFSTREAM_H
#define BINARYIO__BFSTREAM_H

#include <fstream>
#include "biostream.h"
#include "bistream.h"
#include "bostream.h"

namespace binaryio
{
	// Binary file stream, for input.
	// Like std::ifstream
	class bifstream : public bistream
	{
	public:
		bifstream()
		{
			init(&m_fbuf);
		};
		
		bool is_open()
		{
			return m_fbuf.is_open();
		}
		
		void open(const std::string &fname, std::ios_base::openmode mode = std::ios_base::in)
		{
			m_fbuf.open(fname.c_str(), mode | std::ios_base::binary);
		}
		
		void close()
		{
			m_fbuf.close();
		}
		
	private:
		std::basic_filebuf<char, std::char_traits<char> > m_fbuf;
	};
	
	class bofstream : public bostream
	{
	public:
		bofstream()
		{
			init(&m_fbuf);
		};
		
		bool is_open()
		{
			return m_fbuf.is_open();
		}
		
		void open(const std::string &fname, std::ios_base::openmode mode = std::ios_base::out)
		{
			m_fbuf.open(fname.c_str(), mode | std::ios_base::binary);
		}
		
		void close()
		{
			m_fbuf.close();
		}
		
	private:
		std::basic_filebuf<char, std::char_traits<char> > m_fbuf;
	};

	class bfstream : public biostream
	{
	public:
		bfstream()
			: biostream(NULL)
		{
			init(&m_fbuf);
		};
		
		bool is_open()
		{
			return m_fbuf.is_open();
		}
		
		void open(const std::string &fname, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
		{
			m_fbuf.open(fname.c_str(), mode | std::ios_base::binary);
		}
		
		void close()
		{
			m_fbuf.close();
		}
		
	private:
		std::basic_filebuf<char, std::char_traits<char> > m_fbuf;
	};
}

#endif

