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
#include "gc.hpp"
#include "activation_record.hpp"

namespace Caribou
{
	class Continuation;

	class Machine
	{
	public:
		Machine();
		~Machine() { delete memory; }

		void push(const intptr_t&);
		void push(const std::string&);
		intptr_t pop();
		void puship(const intptr_t&);
		void popip();
		void dup();
		void swap();
		void add_symbol(std::string*);
		void find_symbol(std::string*);
		void jz();
		void save_stack();
		void restore_stack();
		void send();

		void run(const int, const intptr_t&);

		void compile(const int, const intptr_t&);

		Stack<intptr_t>& get_data_stack() { return dstack; }
		Stack<intptr_t>* copy_data_stack() { return new Stack<intptr_t>(dstack); }
		void set_data_stack(Stack<intptr_t>* ds) { dstack = *ds; }
		Stack<ActivationRecord*>& get_return_stack() { return rstack; }
		Stack<ActivationRecord*>* copy_return_stack() { return new Stack<ActivationRecord*>(rstack); }
		void set_return_stack(Stack<ActivationRecord*>* rs) { rstack = *rs; }
		intptr_t get_instruction_pointer() { return ip; }
		void set_instruction_pointer(intptr_t val) { ip = val; }

	protected:
		void next(int64_t val = 1) { ip += val; }

	private:
		Stack<intptr_t>            dstack;
		Stack<ActivationRecord*>   rstack;
		std::vector<Continuation*> continuations;
		intptr_t                   ip;
		Symtab                     symtab;
		std::vector<GCObject*>*    memory;
	};
}

#endif /* !__CARIBOU__MACHINE_HPP__ */
