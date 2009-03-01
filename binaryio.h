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

#ifndef BINARYIO_H__
#define BINARYIO_H__

namespace binaryio
{
	enum endian
	{
		big_endian,
		little_endian,
	};

	class io_error
	{
	public:
		io_error(const std::string &what) :
			m_message(what)
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
	
	// Fixed length serializers
	const serialization_info u8(1, little_endian);
	const serialization_info u16le(2, little_endian);
	const serialization_info u16be(2, big_endian);
	const serialization_info u32le(4, little_endian);
	const serialization_info u32be(4, big_endian);
	const serialization_info u64le(8, little_endian);
	const serialization_info u64be(8, big_endian);
	
	// Variable length serializers
	const serialization_info variable_le(0, little_endian);
	const serialization_info variable_be(0, big_endian);
	
	class bstreambuf_base
	{
	public:
		virtual ~binary_streambuf();
	
		/**
		 * Reads in size bytes from the stream, storing them in buffer.
		 * Returns the number of characters read on completion.
		 * If EOF is encountered, returns 0.
		 * If an error occurs, <0 is returned.
		 */
		virtual int read(unsigned int *buffer, size_t size);
		virtual int write(unsigned int *buffer, size_t size);
		virtual int peek(unsigned int *buffer, size_t size);
	};
	
	class bstream_base
	{
	public:
		bstream_base(bstreambuf_base *stream) :
			m_eof(false),
			m_failbit(false),
			m_badbit(false),
			m_stream(buffer)
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
		bstreambuf_base *stream()
		{
			return m_stream;
		}
		
	private:
		bool m_eof;
		bool m_failbit;
		bool m_badbit;
		
		bstream_base *m_stream;
	};
	
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
	
	class bostream : public bstream_base
	{
	public:
	
	private:
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

