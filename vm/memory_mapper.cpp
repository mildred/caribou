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
#include <vector>
#include <cmath>
#include "segment.hpp"
#include "memory_mapper.hpp"

namespace Caribou
{
	size_t MemoryMapper::request_slot(size_t size)
	{
		std::vector<Segment>::iterator it;
		uintptr_t segment_count = 0;

		for(it = segments.begin(); it != segments.end(); it++)
		{
			segment_count += 1;
			uintptr_t index = it->request_slot(size);
			if(index != SEGMENT_ERROR)
				return (segment_count * SEGMENT_SIZE) + index;
		}

		return SEGMENT_ERROR;
	}

	uintptr_t* MemoryMapper::get_slot(uintptr_t index)
	{
		size_t segment_number = (size_t)ceil((double)index / segments.size());
		uintptr_t normalized_index = index / segment_number;
		uintptr_t* r = segments.at(segment_number).get_slot(normalized_index);
		if(r)
			return r;
		return NULL;
	}

	void MemoryMapper::store(size_t index, std::vector<uintptr_t> elements)
	{
		size_t segment_number = (size_t)ceil((double)index / segments.size());
		uintptr_t normalized_index = index / segment_number;
		segments.at(segment_number).store(index, elements);
	}
}
