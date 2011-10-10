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

#ifndef __CARIBOU__ENDIAN_HPP__
#define __CARIBOU__ENDIAN_HPP__

#include <stdint.h>

namespace Caribou
{
	inline bool big_endian()
	{
		uint32_t num = 1;
		return (*(uint8_t*)&num != 1);
	}

	inline void endian_swap(uint16_t& x)
	{
		x = (x>>8) | (x<<8);
	}

	inline void endian_swap(uint32_t& x)
	{
		x = (x>>24) | ((x<<8) & 0x00FF0000) | ((x>>8) & 0x0000FF00) | (x<<24);
	}

	inline void endian_swap(uint64_t& x)
	{
		x = (x>>56) | ((x<<40) & 0x00FF000000000000) | ((x<<24) & 0x0000FF0000000000) |
			((x<<8)  & 0x000000FF00000000) | ((x>>8)  & 0x00000000FF000000) |
			((x>>24) & 0x0000000000FF0000) | ((x>>40) & 0x000000000000FF00) | (x<<56);
	}
}

#endif /* !__CARIBOU__ENDIAN_HPP__ */
