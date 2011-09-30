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

#ifndef __CARIBOU__GC_HPP__
#define __CARIBOU__GC_HPP__

#include <vector>
#include <stdint.h>

namespace Caribou
{
	class Machine;

	class GCObject
	{
	public:
		virtual void mark() = 0;

		inline size_t object_size()
		{
			return sizeof(this);
		}

	private:
		bool marked;
	};

	class GarbageCollector
	{
	public:
		GarbageCollector(Machine&, size_t);
		~GarbageCollector();

		void* allocate(size_t size);
		void flip();
		void* copy(void*);

	protected:
		void* forwarded(void* ptr)
		{
			return (void*)((uintptr_t)ptr | 0x1);
		}

		void set_forward(void* ptr, void* to)
		{
			ptr = (void*)((uintptr_t)to | 0x1);
		}

	private:
		void walk_roots();

		char*    heap;
		char*    tospace;
		char*    fromspace;
		char*    top_of_space;
		char*    freep;
		char*    scan;
		size_t   space_size;
		Machine& machine;
	};
}

#endif /* !__CARIBOU__GC_HPP__ */
