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

#ifndef __CARIBOU__SEGMENT_HPP__
#define __CARIBOU__SEGMENT_HPP__

#include <iostream>
#include <stdint.h>

#define SEGMENT_SIZE       (2 << 12)
#define SEGMENT_SIZE_BYTES (SEGMENT_SIZE * sizeof(uintptr_t))
#define SEGMENT_ERROR      (UINTPTR_MAX - 1)

#define MEMORY_SIZE    0
#define MEMORY_ADDRESS 1

namespace Caribou
{
	// Each segment holds 8k slots. On 32-bit platforms, this means we can
	// store 64 KB of memory, and double that on 64-bit platforms.
	class Segment
	{
	public:
		Segment() {}

		void store(size_t index, std::vector<uintptr_t> elements)
		{
			uintptr_t& current_size = memory[index][MEMORY_SIZE];
			uintptr_t& current_addr = memory[index][MEMORY_ADDRESS];
			uintptr_t original_pos = current_addr;

			used_indices[index] = index;

			std::vector<uintptr_t>::iterator it;
			for(it = elements.begin(); it != elements.end(), current_addr < (original_pos + current_size); it++, current_addr += sizeof(uintptr_t))
				current_addr = *it;

			current_addr = original_pos;
		}

		size_t request_slot(size_t size)
		{
			uintptr_t new_index = SEGMENT_ERROR;

			for(int i = 1; i < SEGMENT_SIZE; i++)
			{
				if(used_indices[i] != i)
				{
					used_indices[i] = i;
					new_index = i;
					break;
				}
			}

			if(memory[new_index][MEMORY_ADDRESS] == NULL)
			{
				memory[new_index][MEMORY_SIZE]    = size;
				memory[new_index][MEMORY_ADDRESS] = (uintptr_t)malloc(size);
			}

			return new_index;
		}

		uintptr_t* get_slot(uintptr_t index)
		{
			if(index < SEGMENT_SIZE)
				return memory[index];
			return NULL;
		}

	private:
		uintptr_t used_indices[SEGMENT_SIZE];
		uintptr_t memory[SEGMENT_SIZE][2];
	};
}

#endif /* !__CARIBOU__SEGMENT_HPP__ */
