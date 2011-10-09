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

#ifndef __CARIBOU__BYTECODE_HPP__
#define __CARIBOU__BYTECODE_HPP__

#define HEADER_MAGIC_NUMBER { 'C', 'a', 'r', 'i', 'b', 'o', 'u', '!' }

namespace Caribou
{
	struct BytecodeHeader
	{
		// Magic string.
		char name[8];

		// The following two fields represent the version of the file format.
		// Releases are in this format: XXXX.YY where XXXX is the year, YY is
		// the a serial number representing the release in that year.
		uint16_t   format_year;
		uint16_t   format_release;

		// Reserve space for a custom header. Needs to have at a minimum a size
		// field to indicate how many bytes it is going to run.
		uint32_t   custom_size;
	};
}

#endif /* !__CARIBOU__BYTECODE_HPP__ */
