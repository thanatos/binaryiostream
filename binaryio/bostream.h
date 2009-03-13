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

#ifndef BINARYIO__BOSTREAM_H
#define BINARYIO__BOSTREAM_H

#include <cassert>
#include <climits>
#include <streambuf>
#include <limits>
#include "bio_base.h"

namespace binaryio
{
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
		
		bostream &operator << (endian &e)
		{
			m_write_opts.endianness = e;
			return *this;
		}
		
		bostream &operator << (unsigned char n)
		{
			serialize(n);
			return *this;
		}
		
		bostream &operator << (unsigned short n)
		{
			serialize(n);
			return *this;
		}
		
		bostream &operator << (unsigned int n)
		{
			serialize(n);
			return *this;
		}
		
		bostream &operator << (unsigned long long n)
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
	protected:
		bostream() :
			m_write_opts(u32le)
		{
		}
		
	private:
		template<typename T>
		void serialize(T n)
		{
			// Basic debug check
#ifndef NDEBUG
			T mask;
			// Is the write size big enough?
			if(m_write_opts.size >= sizeof(T))
			{
				mask = 0;
			}
			// ... or is there a possibility the input will get clipped?
			else
			{
				mask = std::numeric_limits<T>::min() << (CHAR_BIT * m_write_opts.size);
			}
			assert((n & mask) == 0);
#endif
			unsigned char *data(NULL);
			try
			{
				data = new unsigned char[m_write_opts.size];
				
				size_t i;
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
				delete [] data;
			}
			catch(...)
			{
				delete [] data;
				throw;
			}
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

