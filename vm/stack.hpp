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

#ifndef __CARIBOU__STACK_HPP__
#define __CARIBOU__STACK_HPP__

#include <vector>
#include "gc.hpp"

namespace Caribou
{
	class Machine;

	extern void* gc_allocate(Machine*, size_t);

	template<typename T>
	class Stack : public GCObject
	{
	public:
		void* operator new(size_t size, Machine* m)
		{
			return gc_allocate(m, size);
		}

		void push(const T& val)
		{
			store.push_back(val);
		}

		T pop()
		{
			T r = store.back();
			store.pop_back();
			return r;
		}

		std::vector<T>& get_store() { return store; }

		virtual void mark()
		{

		}

	private:
		std::vector<T> store;
	};
}

#endif /* !__CARIBOU__STACK_HPP__ */
