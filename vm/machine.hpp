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

namespace Caribou
{
	class Continuation;

	class Machine
	{
	public:
		Machine();
		~Machine();

		void push(Context*, Object*);
		Object* pop(Context*);
		void ret(Context*);
		void dup(Context*);
		void swap(Context*);
		void rot3(Context*);
		void save_stack(Context*);
		void restore_stack(Context*);
		void jz(Context*);
		void make_array(Context*);
		void send(Context*);
		void add_symbol(Context*, std::string&);
		void find_symbol(Context*, std::string&);

		void run();
		void process(Context*, uint8_t, Object*);

		void compile(const int, const uintptr_t&);

		Stack<Context*>& get_return_stack() { return rstack; }
		Stack<Context*>* copy_return_stack() { return new Stack<Context*>(rstack); }
		void set_return_stack(Stack<Context*>* rs) { rstack = *rs; }
		uintptr_t get_instruction_pointer() { return ip; }
		void set_instruction_pointer(uintptr_t val) { ip = val; }

		void allocate_instruction_memory(size_t size)
		{
			instruction_size   = size;
			instruction_memory = new uint8_t[size];
		}

		uint8_t*& get_instruction_memory() { return instruction_memory; }

	protected:
		void next(uint64_t val = 1) { ip += val; }

	private:
		Stack<Context*>            rstack;
		std::vector<Continuation*> continuations;
		uint8_t*                   instruction_memory;
		size_t                     instruction_size;
		// The instruction pointer references a byte in the instruction memory above
		uintptr_t                  ip;
		uintptr_t*                 memory;
		Symtab                     symtab;
	};
}

#endif /* !__CARIBOU__MACHINE_HPP__ */
