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

#ifndef __CARIBOU__MACHINE_HPP__
#define __CARIBOU__MACHINE_HPP__

#include <vector>
#include <stdint.h>
#include "stack.hpp"
#include "symtab.hpp"
#include "instructions.hpp"
#include "context.hpp"

#define MAX_REGISTERS 256

namespace Caribou
{
	class Continuation;
	class Message;

	class Machine
	{
	private:
		uint8_t         opcode;
		union {
			struct
			{
				uint8_t a;
				uint8_t b;
				uint8_t c;
			};
			uint16_t    bmore;
			uint32_t    immed;
		} fields;

		intptr_t        fp;
		uintptr_t       ip;
		uint32_t*       instructions;
		size_t          icount;
		Stack<Context*> rstack;
		Object**        constants;
		size_t          const_count;
		Symtab          symtab;

	public:
		Machine();
		~Machine();

		uint32_t fetch();
		void decode(uint32_t);
		void execute();

		void move(Object** regs, uint8_t a, uint8_t b);
		void loadi(Object** regs, uint8_t a, uint16_t i);
		void bitwise_not(Object** regs, uint8_t a, uint8_t b);
		void jmp(uint32_t loc);
		void push(Object** regs, uint8_t a);
		void pop(Object** regs, uint8_t a);
		void dup();
		void swap();
		void rotate(Object** regs, uint8_t a);
		void add(Object** regs, uint8_t a, uint8_t b, uint8_t c);
		void sub(Object** regs, uint8_t a, uint8_t b, uint8_t c);
		void mul(Object** regs, uint8_t a, uint8_t b, uint8_t c);
		void div(Object** regs, uint8_t a, uint8_t b, uint8_t c);
		void mod(Object** regs, uint8_t a, uint8_t b, uint8_t c);
		void pow(Object** regs, uint8_t a, uint8_t b, uint8_t c);
		void bitwise_not(Object** regs, uint8_t a, uint8_t b, uint8_t c);
		void save(Object** regs, uint8_t a);
		void restore(Object** regs, uint8_t a);
		void eq(Object** regs, uint8_t a, uint8_t b, uint8_t c);
		void lt(Object** regs, uint8_t a, uint8_t b, uint8_t c);
		void lte(Object** regs, uint8_t a, uint8_t b, uint8_t c);
		void gt(Object** regs, uint8_t a, uint8_t b, uint8_t c);
		void gte(Object** regs, uint8_t a, uint8_t b, uint8_t c);
		void make_array(Object** regs, uint8_t a);
		void make_string(Object** regs, uint8_t a);

		Stack<Context*>& get_return_stack() { return rstack; }
		Stack<Context*>* copy_return_stack() { return new Stack<Context*>(rstack); }
		void set_return_stack(Stack<Context*>* rs)
		{
			Stack<Context*> tmp(*rs);
			rstack = tmp;
		}

		uintptr_t get_instruction_pointer() { return ip; }
		void set_instruction_pointer(uintptr_t val) { ip = val; }

		void allocate_instruction_memory(size_t size)
		{
			icount       = size;
			instructions = new uint32_t[size];
		}

		uint32_t*& get_instructions() { return instructions; }

		Context* get_current_context() { return rstack.top(); }

	protected:
		void next(uintptr_t val = 1) { ip += val; }

	private:
		inline void process(uint8_t, Object**);
	};
}

#endif /* !__CARIBOU__MACHINE_HPP__ */
