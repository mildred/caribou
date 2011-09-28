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
#include "symtab.hpp"
#include "instructions.hpp"

namespace Caribou
{
	typedef std::vector<intptr_t> Stack;

	class Continuation;

	class Machine
	{
	public:
		Machine();

		void push(const intptr_t&);
		void push(const std::string&);
		intptr_t pop();
		void puship(const intptr_t&);
		intptr_t popip();

		void run(const int, const intptr_t&);

		Stack* copy_data_stack() { return new Stack(dstack); }
		void set_data_stack(Stack* ds) { dstack = *ds; }
		Stack* copy_return_stack() { return new Stack(rstack); }
		void set_return_stack(Stack* rs) { rstack = *rs; }
		intptr_t get_instruction_pointer() { return ip; }
		void set_instruction_pointer(intptr_t val) { ip = val; }

	protected:
		void next() { ip += 1; }

	private:
		Stack                      dstack;
		Stack                      rstack;
		std::vector<Continuation*> continuations;
		intptr_t                   ip;
		Symtab                     symtab;
	};
}

#endif /* !__CARIBOU__MACHINE_HPP__ */
