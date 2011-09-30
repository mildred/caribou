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

#include <vector>
#include <iostream>
#include <cstring>
#include <stdint.h>
#include "gc.hpp"
#include "machine.hpp"

namespace Caribou
{
	GarbageCollector::GarbageCollector(Machine& m, size_t size) : machine(m), space_size(size)
	{
		heap = new char[size * 2];
		tospace = heap;
		top_of_space = tospace + space_size;
		fromspace = top_of_space + 1;
		freep = tospace;
		scan = tospace;
	}

	GarbageCollector::~GarbageCollector()
	{
		delete[] heap;
	}

	void* GarbageCollector::allocate(size_t size)
	{
		if(freep + size > top_of_space)
			flip();

		if(freep + size > top_of_space)
		{
			std::cerr << "Memory exhausted!" << std::endl;
			abort();
		}

		void* ptr = freep;
		freep += size;

		return ptr;
	}

	void GarbageCollector::flip()
	{
		char* tmpspace = fromspace;
		fromspace = tospace;
		tospace = tmpspace;
		top_of_space = tospace + space_size;
		scan = freep = tospace;

		walk_roots();

		while(scan < freep)
		{
			// Iterate over children of scan where P = current child
			//   *P = copy(*P);
			scan += size;
		}
	}

	void* GarbageCollector::copy(void* ptr)
	{
		if(forwarded(ptr))
			return forwarded(ptr);
		else
		{
			void* addr = freep;
			size_t size = ((GCObject*)ptr)->object_size();
			memcpy(freep, ptr, size);
			freep += size;
			set_forward(ptr, addr);
			return addr;
		}
	}

	void walk_roots()
	{
		std::vector<ActivationRecord*>& rstack = machine.get_return_stack().get_store();
		std::vector<intptr_t>&          dstack = machine.get_data_stack().get_store();
		std::vector<ActivationRecord*>::iterator it;

		for(it = rstack.begin(); it < rstack.end(); it++)
			*it = copy(*it);
	}
}
