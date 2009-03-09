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

/**
 * Things we assume:
 * 1) 2's complement signed integers.
 * 2) IEEE 754 doubles & floats.
 * 3) Probably that bytes are octects (CHAR_BIT == 8)
 */

#ifndef BINARYIO__BIOSTREAM_H
#define BINARYIO__BIOSTREAM_H

#include <cassert>
#include <climits>
#include <streambuf>
#include "binaryio.h"

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
	
		bistream &operator >> (endian &e)
		{
			m_read_opts.endianness = e;
			return *this;
		}
		
		bistream &operator >> (unsigned int &x)
		{
			assert(m_read_opts.size <= sizeof(x));
			deserialize(x);
			return *this;
		}
		
		bistream &operator >> (unsigned short &x)
		{
			assert(m_read_opts.size <= sizeof(x));
			unsigned int n;
			deserialize(n);
			x = n;
			return *this;
		}
		
		bistream &operator >> (unsigned char &x)
		{
			assert(m_read_opts.size <= sizeof(x));
			unsigned int n;
			deserialize(n);
			x = n;
			return *this;
		}
		
	private:
		void deserialize(unsigned int &x)
		{
			size_t i;
			unsigned char data[sizeof(x)];
			
			// Blank result for filling in later.
			x = 0;
			
			// Read the data from our current underlying stream...
			if(!read(data, m_read_opts.size))
			{
				// Read failed.
				return;
			}
			
			// Make sure what we're reading will fit in an unsigned int.
			assert(m_read_opts.size <= sizeof(x));
			
			if(m_read_opts.endianness == little_endian)
			{
				for(i = 0; i < m_read_opts.size; ++i)
				{
					x |= data[i] << (CHAR_BIT * i);
				}
			}
			else if(m_read_opts.endianness == big_endian)
			{
				for(i = 0; i < m_read_opts.size; ++i)
				{
					x |= data[i] << (CHAR_BIT * (m_read_opts.size - i - 1));
				}
			}
		}
		
		bool read(unsigned char *buffer, size_t size)
		{
			size_t ret;
			ret = stream()->sgetn(reinterpret_cast<char *>(buffer), size);
			if(ret == size)
			{
				return true;
			}
			// We hit EOF
			m_eof = true;
			return false;
		}
		
		serialization_info m_read_opts;
	};
	
	class bostream : virtual public bstream_base
	{
	public:
		bostream(std::streambuf *stream) :
			bstream_base(stream),
			m_write_opts(u32le)
		{
		}
		
		bostream &operator << (const serialization_info &si)
		{
			m_write_opts = si;
			return *this;
		}
		
		bostream &operator << (unsigned int n)
		{
			serialize(n);
			return *this;
		}
		
		bostream &operator << (const char *str)
		{
			size_t len = strlen(str);
			serialize(len);
			write(reinterpret_cast<const unsigned char *>(str), len);
			return *this;
		}
		
		bostream &operator << (const std::string &str)
		{
			size_t len = str.size();
			serialize(len);
			write(reinterpret_cast<const unsigned char *>(str.c_str()), len);
			return *this;
		}
		
	private:
		void serialize(unsigned int n)
		{
			unsigned char data[sizeof(unsigned int)];
			size_t i;
			
			// Can we fit n in the size requested by the user?
#ifndef NDEBUG
			unsigned int mask;
			if(m_write_opts.size >= sizeof(unsigned int))
			{
				mask = 0;
			}
			else
			{
				mask = UINT_MAX << (CHAR_BIT * m_write_opts.size);
			}
			assert((n & mask) == 0);
#endif
			
			if(m_write_opts.endianness == little_endian)
			{
				for(i = 0; i < m_write_opts.size; ++i)
				{
					data[i] = (n >> (CHAR_BIT * i)) & UCHAR_MAX;
				}
			}
			else if(m_write_opts.endianness == big_endian)
			{
				for(i = 0; i < m_write_opts.size; ++i)
				{
					data[i] = (n >> (CHAR_BIT * (m_write_opts.size - i - 1)))
						& UCHAR_MAX;
				}
			}
			
			write(data, m_write_opts.size);
		}
	
		bool write(const unsigned char *buffer, size_t size)
		{
			size_t ret;
			ret = stream()->sputn(reinterpret_cast<const char *>(buffer), size);
			if(ret == size)
			{
				return true;
			}
			// Otherwise, we've hit an EOF:
			m_eof = true;
			return false;
		}
		
		serialization_info m_write_opts;
	};
}

#endif

