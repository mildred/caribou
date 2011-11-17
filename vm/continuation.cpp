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

#include "continuation.hpp"

namespace Caribou
{
	Continuation* Continuation::now(Object* locals, Message* msg)
	{
		Continuation* now = new Continuation(machine);
		now->save_current_stack();
		return now;
	}

	void Continuation::save_current_stack()
	{
		saved_stack = machine->copy_return_stack();
		saved_ip    = machine->get_instruction_pointer();
	}

	void Continuation::restore_stack()
	{
		machine->set_return_stack(saved_stack);
		machine->set_instruction_pointer(saved_ip);
	}

	const std::string Continuation::object_name()
	{
		return "Continuation";
	}

	void Continuation::walk()
	{
		// Walk the return stack, and all the contexts stacks too.
	}
}
