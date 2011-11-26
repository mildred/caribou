/*
 * Caribou Virtual Machine
 * Copyright (c) 2011, Jeremy Tregunna, All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __CARIBOU__REGISTER_HPP__
#define __CARIBOU__REGISTER_HPP__

#include "native.hpp"

namespace Caribou
{
	class Register
	{
	private:
		uint8_t number:3;
		uint8_t unused:5;

		union
		{
			native_width  native;
			native_width* ptr;

			union
			{
				half_width h;
				half_width l;
			} half;
		} reg;

	public:
		Register(const uint8_t&, native_width);
		Register(const uint8_t&, native_width*);
		Register(const uint8_t&, half_width, half_width);
		Register(const uint8_t&);
	};
}

#endif /* !__CARIBOU__REGISTER_HPP__ */
