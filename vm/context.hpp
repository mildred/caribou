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
#include "vmmethod.hpp"

#define CARIBOU_MAX_STACK_SIZE 256

namespace Caribou
{
	class Object;

	struct Context
	{
		Context*  previous;
		VMMethod* method;
		uintptr_t return_address;
		Object**  registers;
		uint8_t   num_registers;
		uint8_t   sp;
		Object*   stk[CARIBOU_MAX_STACK_SIZE];

		Context() {}

		Context(Context* ctx, VMMethod* meth, uintptr_t ra)
		{
			previous = ctx;
			method = meth;
			return_address = ra;
			num_registers = meth->nargs + meth->nlocals;
			registers = new Object*[num_registers];
		}

		Context(const Context& ctx)
		{
			previous       = ctx.previous;
			method         = ctx.method;
			return_address = ctx.return_address;
			num_registers  = ctx.num_registers;
			sp             = ctx.sp;
			memcpy(registers, ctx.registers, num_registers);
			memcpy(stk, ctx.stk, CARIBOU_MAX_STACK_SIZE);
		}

		~Context()
		{
			delete[] registers;
		}

		inline void push(Object* val)
		{
			stk[sp++] = val;
		}

		inline Object* pop()
		{
			return stk[--sp];
		}
	};
}

#endif /* !__CARIBOU__CONTEXT_HPP__ */
