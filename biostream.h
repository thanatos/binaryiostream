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

#ifndef BINARYIO__BIOSTREAM_H
#define BINARYIO__BIOSTREAM_H

#include "binaryio.h"

namespace binaryio
{
	class bistream : virtual public bstream_base
	{
	public:
		bistream &operator >> (endianness &e)
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
					x |= data[i] << (CHAR_BITS * i);
				}
			}
			else if(m_read_opts.endianness == big_endian)
			{
				for(i = 0; i < m_read_opts.size; ++i)
				{
					x |= data[i] << (CHAR_BITS * (m_read_opts.size - i - 1));
				}
			}
		}
		
		void read(unsigned char *buffer, size_t size)
		{
			int ret;
			ret = stream()->read(buffer, size);
			if(ret == size)
			{
				return true;
			}
			else if(ret >= 0)
			{
				// Either EOF, or partial read,
				// which we count as EOF.
				m_eof = true;
				return false;
			}
			else // ret < 0  // An error occurred
			{
				m_bad_bit = true;
				return false;
			}
		}
		
		serialization_info m_read_opts;
	};
	
	class bostream : virtual public bstream_base
	{
	public:
		bostream &operator << (unsigned int n)
		{
			serialize(n);
			return *this;
		}
		
	private:
		void serialize(unsigned int n)
		{
			unsigned char data[sizeof(unsigned int)];
			size_t i;
			
			// Can we fit n in the size requested by the user?
#ifndef NDEBUG
			unsigned int mask =
				UINT_MAX <<
					(CHAR_BITS * (sizeof(unsigned int) - m_write_opts.size));
			assert((n & mask) == 0);
#endif
		}
	
		bool write(const unsigned char *buffer, size_t size)
		{
			int ret;
			ret = stream()->write(buffer, size);
			if(ret == size)
			{
				return true;
			}
			else if(ret >= 0)
			{
				// Either EOF, or partial write,
				// we we count as EOF.
				m_eof = true;
				return false;
			}
			else // ret < 0	 // An error occurred
			{
				m_badbit = true;
				return false;
			}
		}
	};
}

#endif

