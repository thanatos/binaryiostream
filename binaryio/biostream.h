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
#include "bistream.h"
#include "bostream.h"

namespace binaryio
{
	class biostream : public bistream, public bostream
	{
	public:
		biostream(std::streambuf *buffer) :
			bistream(), bostream()
		{
			init(buffer);
		}
	};
}

#endif

