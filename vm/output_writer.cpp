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

#include <iostream>
#include <fstream>
#include "output_writer.hpp"

#define MAGIC_HEADER_NUMBER 0x3FB10A5C

namespace Caribou
{
	struct MagicHeader
	{
		uint32_t     magic_number;
		const char   name[4];

		// The following two fields represent the version of the file format.
		// Releases are in this format: XXXX.YY where XXXX is the year, YY is
		// the a serial number representing the release in that year.
		unsigned short format_year;
		unsigned short format_release;

		// Reserve space for a constants header. Needs to have at a minimum a size
		// field to indicate how many bytes it is going to run.
		size_t         constants_size;
	};

	OutputWriter::OutputWriter(const char* filename)
	{
		output_file = new std::ofstream;
		output_file->open(filename, std::ios::binary);
	}

	OutputWriter::~OutputWriter()
	{
		if(output_file->is_open())
			output_file->close();
		delete output_file;
	}

	void OutputWriter::dump(const char* bytes, size_t length)
	{
		MagicHeader header = { .magic_number   = htonl(MAGIC_HEADER_NUMBER),
		                       .name           = { 'C', 'B', 'V', 'M' },
		                       .format_year    = htons(2011),
		                       .format_release = htons(1),
		                       .constants_size = htonl(0) };
		output_file->write((char*)&header, sizeof(header));
		output_file->write(bytes, length);
		output_file->close();
	}
}
