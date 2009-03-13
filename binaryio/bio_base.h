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

#ifndef BINARYIO__BIO_BASE_H
#define BINARYIO__BIO_BASE_H

#include <streambuf>
#include <stdexcept>

namespace binaryio
{
	enum endian
	{
		big_endian,
		little_endian,
	};
	
	inline endian native_endian()
	{
		unsigned char bytes[] = {0xAA, 0xBB, 0xCC, 0xDD};
		unsigned int *num = reinterpret_cast<unsigned int *>(bytes);
		
		if(*num == 0xAABBCCDD)
		{
			return big_endian;
		}
		else if(*num == 0xDDCCBBAA)
		{
			return little_endian;
		}
		
		throw std::runtime_error("Unable to determine endianness!");
	}

	class io_error : public std::exception
	{
	public:
		io_error(const std::string &what) :
			m_message(what)
		{
		}
		
		~io_error() throw()
		{
		}

		const char *what() const throw()
		{
			return m_message.c_str();
		}
	private:
		std::string m_message;
	};
	
	struct serialization_info
	{
		serialization_info(unsigned int s, endian e) :
			size(s),
			endianness(e)
		{
		}
	
		size_t size;
		endian endianness;
	};
	
	const size_t VARIABLE_SIZE = 0;
	
	// Fixed length serializers
	const serialization_info u8(1, little_endian);
	const serialization_info u16le(2, little_endian);
	const serialization_info u16be(2, big_endian);
	const serialization_info u32le(4, little_endian);
	const serialization_info u32be(4, big_endian);
	const serialization_info u64le(8, little_endian);
	const serialization_info u64be(8, big_endian);
	
	// Variable length serializers
	const serialization_info variable_le(VARIABLE_SIZE, little_endian);
	
	class bstream_base
	{
	public:
		virtual ~bstream_base()
		{
		}
		
		bool good() const
		{
			return !m_eof && !m_failbit && !m_badbit;
		}
		bool bad() const
		{
			return m_badbit;
		}
		bool eof() const
		{
			return m_eof;
		}
		bool fail() const
		{
			return m_failbit;
		}
		void clear()
		{
			m_eof = m_failbit = m_badbit = false;
		}
		
	protected:
		bstream_base(std::streambuf *stream = NULL) :
			m_eof(false),
			m_failbit(false),
			m_badbit(false),
			m_stream(stream)
		{
		}

		void init(std::streambuf *buf)
		{
			m_stream = buf;
		}

		std::streambuf *stream()
		{
			return m_stream;
		}
		
		bool m_eof;
		bool m_failbit;
		bool m_badbit;
		
	private:
		std::streambuf *m_stream;
	};
}

#endif

