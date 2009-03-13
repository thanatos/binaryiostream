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

#ifndef BINARYIO__BSSTREAM_H
#define BINARYIO__BSSTREAM_H

#include <sstream>
#include "biostream.h"

namespace binaryio
{
	// Binary I/O with binary strings.
	// Like std::stringstream
	class bstringstream : public biostream
	{
	public:
		bstringstream() :
			biostream(NULL)
		{
			init(&m_sbuf);
		}
		
		std::string str()
		{
			return m_sbuf.str();
		}
		/*
		binary_string bstr()
		{
			std::string strtemp = m_sbuf.str();
			return binary_string(reinterpret_cast<const unsigned char *>(strtemp.c_str()), strtemp.size());
		}
		*/
		void str(const std::string &s)
		{
			m_sbuf.str(s);
		}
		/*
		void str(const binary_string &s)
		{
			std::string strtemp(reinterpret_cast<const char *>(s.c_str()), s.size());
			m_sbuf.str(strtemp);
		}
		*/
		void clear()
		{
			std::string temp("");
			m_sbuf.str(temp);
		}
		
	private:
		std::basic_stringbuf<char, std::char_traits<char> > m_sbuf;
	};
}

#endif

