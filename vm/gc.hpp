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

	protected:
		// Copy-on-Write [NOT IMPLEMENTED **YET**] ref count. Each time an object is referenced
		// by traversing the roots, increment this by one for each successive time we find it in
		// one mark cycle. Decrement it by one if an object that references us goes away. If set
		// and object is mutated, make a copy of the object.
		uint32_t shared_count;

	private:
		size_t object_size;
		bool   marked;
	};

	class GarbageCollector
	{
	private:
		std::vector<GCObject*> free_list;
		std::vector<GCObject*> mark_stack;
		std::vector<GCObject*> roots;
		size_t                 max_heap_size;
		size_t                 current_heap_size;
	};
}

#endif /* !__CARIBOU__GC_HPP__ */
