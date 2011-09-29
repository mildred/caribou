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

#ifndef __CARIBOU__CONTINUATION_HPP__
#define __CARIBOU__CONTINUATION_HPP__

#include <vector>
#include "machine.hpp"
#include "gc.hpp"

namespace Caribou
{
	struct ActivationRecord;

	/* Our continuations are implemented by saving the contents of our call stack onto the heap.
	   They are also per virtual core, meaning you must pass a machine in when you create the
	   continuation. */
	class Continuation : public GCObject
	{
	public:
		Continuation(Machine& m) : machine(m) { }
		~Continuation()
		{
			if(saved_rstack) delete saved_rstack;
			if(saved_dstack) delete saved_dstack;
		}

		void save_current_stacks()
		{
			saved_dstack = machine.copy_data_stack();
			saved_rstack = machine.copy_return_stack();
			saved_ip     = machine.get_instruction_pointer();
		}

		void restore_stacks()
		{
			machine.set_data_stack(saved_dstack);
			machine.set_return_stack(saved_rstack);
			machine.set_instruction_pointer(saved_ip);
		}

		virtual void mark() { }

	private:
		Stack<intptr_t>*          saved_dstack;
		Stack<ActivationRecord*>* saved_rstack;
		uintptr_t                 saved_ip;
		Machine&                  machine;
	};
}

#endif /* !__CARIBOU__CONTINUATION_HPP__ */