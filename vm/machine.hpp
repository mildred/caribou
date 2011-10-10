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
#include "activation_record.hpp"

namespace Caribou
{
	class Continuation;

	class Machine
	{
	public:
		Machine();
		~Machine();

		void push(const uintptr_t&);
		uintptr_t pop();
		void puship(const uintptr_t&);
		void popip();
		void dup();
		void swap();
		void rot3();
		void add();
		void bitwise_shift_left();
		void bitwise_shift_right();
		void bitwise_and();
		void bitwise_or();
		void bitwise_xor();
		void bitwise_not();
		void save_stack();
		void restore_stack();
		void jz();
		void store();
		void load();
		void add_symbol(std::string*);
		void find_symbol(std::string*);

		void run();
		void process(uint8_t, uintptr_t);

		void compile(const int, const uintptr_t&);

		Stack<uintptr_t>& get_data_stack() { return dstack; }
		Stack<uintptr_t>* copy_data_stack() { return new(this) Stack<uintptr_t>(dstack); }
		void set_data_stack(Stack<uintptr_t>* ds) { dstack = *ds; }
		Stack<ActivationRecord*>& get_return_stack() { return rstack; }
		Stack<ActivationRecord*>* copy_return_stack() { return new(this) Stack<ActivationRecord*>(rstack); }
		void set_return_stack(Stack<ActivationRecord*>* rs) { rstack = *rs; }
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
		Stack<uintptr_t>           dstack;
		Stack<ActivationRecord*>   rstack;
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
