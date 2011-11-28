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
#include "context.hpp"

namespace Caribou
{
	template<typename T>
	class Stack
	{
	public:
		Stack() {}

		Stack(Stack<Context*>& other)
		{
			for(Context* e : other.store)
			{
				Context* a = new Context(*e);
				store.push_back(a);
			}
		}

		void push(const T& val)
		{
			store.push_back(val);
		}

		T top()
		{
			if(!store.empty())
				return store.back();
			return NULL;
		}

		T pop()
		{
			T r = top();
			if(r != NULL)
				store.pop_back();
			return r;
		}

		std::vector<T>& get_store() { return store; }

		size_t size() { return store.size(); }

	private:
		std::vector<T> store;
	};
}

#endif /* !__CARIBOU__STACK_HPP__ */
