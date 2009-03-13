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

#ifndef BINARYIO__BISTREAM_H
#define BINARYIO__BISTREAM_H

#include <cassert>
#include <climits>
#include <streambuf>
#include "bio_base.h"

namespace binaryio
{
	class bistream : virtual public bstream_base
	{
	public:
		bistream(std::streambuf *stream) :
			bstream_base(stream),
			m_read_opts(u32le)
		{
		}
		
		bistream &operator >> (const serialization_info &si)
		{
			m_read_opts = si;
			return *this;
		}
	
		bistream &operator >> (endian &e)
		{
			m_read_opts.endianness = e;
			return *this;
		}
		
		bistream &operator >> (unsigned long long &x)
		{
			deserialize_unsigned<unsigned long long>(x);
			return *this;
		}
		
		bistream &operator >> (unsigned int &x)
		{
			deserialize_unsigned<unsigned int>(x);
			return *this;
		}
		
		bistream &operator >> (unsigned short &n)
		{
			deserialize_unsigned<unsigned short>(n);
			return *this;
		}
		
		bistream &operator >> (unsigned char &n)
		{
			deserialize_unsigned<unsigned char>(n);
			return *this;
		}
		
		bistream &operator >> (std::string &str)
		{
			// Get the string's size
			size_t size, size_read;
			deserialize_unsigned<size_t>(size);
			
			// Read the string
			char *buffer(NULL);
			try
			{
				buffer = new char[size];
				size_read = read(reinterpret_cast<unsigned char *>(buffer), size);
				if(size_read != size)
				{
					delete [] buffer;
					return *this;
				}
				std::string result(buffer, size);
				result.swap(str);
				delete [] buffer;
			}
			catch(...)
			{
				delete [] buffer;
				throw;
			}
		}
		
	public:
		bistream() :
			m_read_opts(u32le)
		{
		}
		
	private:
		template<typename ST, typename UT>
		void deserialize_signed(ST &x)
		{
			UT *counterpart = reinterpret_cast<UT *>(&x);
			deserialize_unsigned<UT>(*counterpart);
		}
		
		template<typename T>
		void deserialize_unsigned(T &x)
		{
			// Make sure what we're reading will fit in an unsigned int.
			assert(m_read_opts.size <= sizeof(T));

			size_t i;
			unsigned char data[sizeof(T)];
			
			// Blank result for filling in later.
			x = 0;
			
			// Read the data from our current underlying stream...
			if(m_read_opts.size != read(data, m_read_opts.size))
			{
				// Read failed.
				return;
			}
						
			if(m_read_opts.endianness == little_endian)
			{
				for(i = 0; i < m_read_opts.size; ++i)
				{
					x |= (T) data[i] << (CHAR_BIT * i);
				}
			}
			else if(m_read_opts.endianness == big_endian)
			{
				for(i = 0; i < m_read_opts.size; ++i)
				{
					x |= (T) data[i] << (CHAR_BIT * (m_read_opts.size - i - 1));
				}
			}
		}
		
		size_t read(unsigned char *buffer, size_t size)
		{
			size_t ret;
			ret = stream()->sgetn(reinterpret_cast<char *>(buffer), size);
			if(ret == size)
			{
				return ret;
			}
			// We hit EOF
			m_eof = true;
			return ret;
		}
		
		serialization_info m_read_opts;
	};
}

#endif

