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

#define SEGMENT_SIZE       (2 << 16)
#define SEGMENT_SIZE_BYTES (SEGMENT_SIZE * sizeof(uintptr_t))
#define SEGMENT_ERROR      (UINTPTR_MAX - 1)

namespace Caribou
{
	// Each segment holds 8k slots. On 32-bit platforms, this means we can
	// store 32 KB of memory, and double that on 64-bit platforms.
	class Segment
	{
	public:
		Segment() : next(NULL) {}

		void store(size_t index, uintptr_t address)
		{
			used_indices[index] = index;
			memory[index] = address;
		}

		// request_slot() will return SEGMENT_ERROR if there is no more space left. CHECK IT!
		size_t request_slot()
		{
			uintptr_t new_index = find_slot();
			// We didn't find an empty slot, make a new segment!
			if(new_index == SEGMENT_ERROR)
			{
				Segment* other = new Segment;
				next = other;
				new_index = 1;
				other->store(new_index, NULL);
				return SEGMENT_SIZE + new_index;
			}

			return new_index;
		}

		void* load(size_t index)
		{
			return (void*)memory[index];
		}

		void set_next(Segment* other)
		{
			next = other;
		}

	private:
		size_t find_slot()
		{
			uintptr_t slot = SEGMENT_ERROR;

			for(int i = 1; i < SEGMENT_SIZE; i++)
			{
				if(used_indices[i] != i)
				{
					used_indices[i] = i;
					slot = i;
					break;
				}
			}

			return slot;
		}

		uintptr_t used_indices[SEGMENT_SIZE];
		uintptr_t memory[SEGMENT_SIZE];
		Segment*  next;
	};
}

#endif /* !__CARIBOU__SEGMENT_HPP__ */
