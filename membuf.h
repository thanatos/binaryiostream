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

#ifndef BINARYIO__BMEMSTREAM_H
#define BINARYIO__BMEMSTREAM_H

#include <algorithm>
#include <stdexcept>
#include <string.h>
#include "binaryio.h"
#include "detail.h"

namespace binaryio
{
	class membuf
	{
	public:
		membuf(const unsigned char *data, size_t size) :
			m_buffer(NULL)
		{
			m_buffer = new buffer(data, size);
		}
	
		membuf(size_t size) :
			m_buffer(NULL)
		{
			m_buffer = new buffer(size);
		}
		
		membuf(const membuf &m) :
			m_buffer(m.m_buffer)
		{
			m_buffer->ref();
		}
		
		~membuf()
		{
			m_buffer->deref();
		}
		
		void swap(membuf &b) throw()
		{
			std::swap(m_buffer, b.m_buffer);
		}
		
		membuf &operator = (const membuf &b)
		{
			if(this == &b)
			{
				return *this;
			}
			
			membuf temp(b);
			swap(temp);
			return *this;
		}
		
		operator const unsigned char * () const
		{
			return m_buffer->get();
		}
		
		size_t size() const
		{
			return m_buffer->size();
		}
		
		size_t capacity() const
		{
			return m_buffer->capacity();
		}
		
		void reserve(size_t newsize)
		{
			buffer *b = new buffer(get(), size(), newsize);
			m_buffer->deref();
			m_buffer = b;
		}
		
		const unsigned char *get() const
		{
			return m_buffer->get();
		}
		
		unsigned char *get_mutable()
		{
			if(!m_buffer->unique())
			{
				buffer *b = new buffer(*m_buffer);
				m_buffer->deref();
				m_buffer = b;
			}
			return m_buffer->get_mutable();
		}
		
		void set_size(size_t size)
		{
			m_buffer->set_size(size);
		}
	
	private:
		class buffer;
		buffer *m_buffer;
		
		class buffer
		{
		public:
			buffer(const unsigned char *data, size_t size) :
				m_refcount(1),
				m_used(size),
				m_allocated(size),
				m_data(NULL)
			{
				m_data = new unsigned char[size];
				memcpy(m_data, data, size);
			}
			
			buffer(const unsigned char *data, size_t size, size_t reserve) :
				m_refcount(1),
				m_used(std::min(size, reserve)),
				m_allocated(reserve),
				m_data(NULL)
			{
				m_data = new unsigned char[reserve];
				memcpy(m_data, data, m_used);
			}
			
			buffer(size_t size) :
				m_refcount(1),
				m_used(0),
				m_allocated(size),
				m_data(NULL)
			{
				m_data = new unsigned char[size];
				memset(m_data, 0, size);
			}
			
			buffer(const buffer &b) :
				m_refcount(1),
				m_used(b.m_used),
				m_allocated(b.m_allocated),
				m_data(NULL)
			{
				m_data = new unsigned char[m_allocated];
				memcpy(m_data, b.m_data, m_used);
			}
			
			~buffer()
			{
				delete [] m_data;
			}
			
			size_t size() const
			{
				return m_used;
			}
			
			size_t capacity() const
			{
				return m_allocated;
			}
			
			const unsigned char *get() const
			{
				return m_data;
			}
			
			unsigned char *get_mutable()
			{
				return m_data;
			}
			
			void set_size(size_t size)
			{
				if(size > m_allocated)
				{
					throw std::range_error("set_size() called with size greater than buffer's capacity.");
				}
				m_used = size;
			}
			
			bool unique()
			{
				detail::lock l(m_mutex);
				
				return m_refcount == 1;
			}
		
			void deref()
			{
				bool should_delete = false;
				
				{
					detail::lock l(m_mutex);
				
					--m_refcount;
					if(m_refcount == 0)
					{
						should_delete = true;
					}
				}
				
				if(should_delete)
				{
					delete this;
				}
			}
			void ref()
			{
				detail::lock l(m_mutex);
				
				++m_refcount;
			}
		private:
			size_t m_refcount;
			size_t m_used;
			size_t m_allocated;
			unsigned char *m_data;
			detail::mutex m_mutex;
		};
	};
}

#endif

