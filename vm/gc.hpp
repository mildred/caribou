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

#define GC_FORWARDED_TAG 0x1

namespace Caribou
{
	class Machine;
	class GCObject;

	class GarbageCollector
	{
	public:
		GarbageCollector(Machine*, size_t);
		~GarbageCollector();

		GCObject* allocate(size_t size);
		void flip();
		GCObject* copy(GCObject*);
		GCObject* get_object_at_address(uintptr_t addr);

	protected:
		inline GCObject* forwarded(GCObject* ptr)
		{
			return (GCObject*)((uintptr_t)ptr | GC_FORWARDED_TAG);
		}

		inline void set_forward(GCObject* ptr, GCObject* to)
		{
			ptr = (GCObject*)((uintptr_t)to | GC_FORWARDED_TAG);
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
		Machine* machine;
	};

	extern void* gc_allocate(Machine*, size_t);

	class GCObject
	{
	public:
		void* operator new(size_t size, Machine* m)
		{
			return gc_allocate(m, size);
			//return m->get_collector()->allocate(size);
		}

		// This doesn't get used, so yes this needs to be a noop.
		void operator delete(void*) { }

		virtual void mark() = 0;

		inline size_t object_size()
		{
			return sizeof(this);
		}

		inline size_t get_memory_index()
		{
			return memory_index;
		}

		inline void set_memory_index(size_t idx)
		{
			memory_index = idx;
		}

	private:
		size_t memory_index;
		bool   marked;
	};
}

#endif /* !__CARIBOU__GC_HPP__ */
