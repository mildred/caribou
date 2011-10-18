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

#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "output_writer.hpp"
#include "endian.hpp"
#include "bytecode.hpp"

namespace Caribou
{
	void OutputWriter::dump(const char* filename, const uint8_t* bytes, size_t length)
	{
		// I know we don't need to endian_swap on the constants_size here, but
		// we do it so in the future when we make use of this space, we don't
		// forget to do it.
		uint8_t  release_id = 1;
		uint32_t custom_size = 0;

		if(big_endian())
			endian_swap(custom_size);

		BytecodeHeader header = (BytecodeHeader){ HEADER_MAGIC_NUMBER, release_id, custom_size };
		std::ofstream output_file(filename, std::ios::binary);

		output_file.write((char*)&header, sizeof(header));
		output_file.write((char*)bytes, length);
		output_file.close();
	}
}
