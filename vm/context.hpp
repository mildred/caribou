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

#ifndef __CARIBOU__CONTEXT_HPP__
#define __CARIBOU__CONTEXT_HPP__

#include <sys/types.h>
#include <stdint.h>

namespace Caribou
{
	class Object;
	class CompiledMethod;

	struct Context
	{
		Context*        previous;
		Object*         sender;
		Object*         target;
		uintptr_t       ip;
		uintptr_t       sp;
		CompiledMethod* cm;
		Object*         receiver;
		Object*         stk[];

		inline void push(Object* val)
		{
			stk[sp] = val;
			++sp;
		}

		inline Object* pop()
		{
			uintptr_t old_sp = sp;
			--sp;
			return stk[old_sp];
		}
	};
}

#endif /* !__CARIBOU__CONTEXT_HPP__ */
