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

#ifndef BINARYIO__DETAIL_H
#define BINARYIO__DETAIL_H

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace binaryio
{
namespace detail
{
#ifdef _WIN32
	class mutex
	{
	};
#else
	// Use POSIX threads
	class mutex
	{
	public:
		mutex()
		{
			pthread_mutex_init(&m_mutex, NULL);
		}
		~mutex()
		{
			pthread_mutex_destroy(&m_mutex);
		}
		
		void lock()
		{
			pthread_mutex_lock(&m_mutex);
		}
		
		void unlock()
		{
			pthread_mutex_unlock(&m_mutex);
		}
		
	private:
		pthread_mutex_t m_mutex;
	};
	
	class lock
	{
	public:
		lock(mutex &m) :
			m_mutex(m)
		{
			m_mutex.lock();
		}
		
		~lock()
		{
			m_mutex.unlock();
		}
		
	private:
		mutex &m_mutex;
	};
	
#endif
}
}

#endif
