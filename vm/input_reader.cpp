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
#include <stdint.h>
#include "input_reader.hpp"
#include "machine.hpp"
#include "endian.hpp"
#include "bytecode.hpp"

namespace Caribou
{
	void InputReader::load(const char* filename)
	{
		std::ifstream file(filename, std::ios::binary);
		size_t instruction_size = 0;
		BytecodeHeader header;
		char magic[5] = {0};

		file.seekg(0, std::ios::end);
		instruction_size = file.tellg();
		instruction_size -= sizeof(header);
		file.seekg(0, std::ios::beg);

		machine.allocate_instruction_memory(instruction_size);
		uint8_t*& instruction_memory = machine.get_instruction_memory();

		file.read((char*)&header, sizeof(BytecodeHeader));
		strncpy(magic, header.name, 4);
		if(strcmp(magic, "CRBU") == 0)
			file.read((char*)instruction_memory, instruction_size);
		else
			std::cerr << "Invalid file format." << std::endl;
		file.close();

		machine.run();
	}
}
