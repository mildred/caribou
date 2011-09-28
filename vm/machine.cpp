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

#include "machine.hpp"

namespace Caribou
{
	Machine::Machine()
	{

	}

	void Machine::push(const intptr_t& val)
	{
		dstack.push(val);
	}

	void Machine::push(const std::string& str)
	{
		intptr_t idx = symtab.lookup(str);
		if(idx < 0)
			idx = symtab.add(str);
		push(idx);
	}

	intptr_t Machine::pop()
	{
		return dstack.pop();
	}

	void Machine::puship(const intptr_t& val)
	{
		rstack.push(val);
	}

	intptr_t Machine::popip()
	{
		return rstack.pop();
	}

	void Machine::run(const int instr, const intptr_t& val)
	{
		switch(instr)
		{
			case Instructions::NOOP:
				break;
			case Instructions::PUSH:
				push(val);
				break;
			case Instructions::POP:
				pop();
				break;
			case Instructions::PUSHIP:
				puship(ip);
				break;
			case Instructions::POPIP:
				popip();
				break;
			case Instructions::DUP:
				break;
			case Instructions::SWAP:
				break;
			case Instructions::ADD_SYMBOL:
				break;
			case Instructions::FIND_SYMBOL:
				break;
			case Instructions::LOAD:
				break;
			case Instructions::STORE:
				break;
			case Instructions::JZ:
				break;
			case Instructions::SAVE_STACK:
				break;
			case Instructions::RESTORE_STACK:
				break;
			case Instructions::SEND:
				break;
		}
		next();
	}
}
