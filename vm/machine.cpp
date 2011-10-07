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

#include <stdint.h>
#include "machine.hpp"
#include "continuation.hpp"

namespace Caribou
{
	Machine::Machine(size_t imem_size)
	{
		instruction_memory = new uint8_t[imem_size];
	}

	Machine::~Machine()
	{
		delete[] instruction_memory;
	}

	void Machine::push(const uintptr_t& val)
	{
		dstack.push(val);
		next();
	}

	uintptr_t Machine::pop()
	{
		next();
		return dstack.pop();
	}

	void Machine::puship(const uintptr_t& val)
	{
		ActivationRecord* record = new(this) ActivationRecord;
		record->ip = val;
		record->locals_index = dstack.pop();
		rstack.push(record);
		next(8);
	}

	void Machine::popip()
	{
		ActivationRecord* r = rstack.pop();
		ip = r->ip;
		dstack.push(r->locals_index);
		delete r;
	}

	void Machine::dup()
	{
		uintptr_t a = dstack.pop();
		dstack.push(a);
		dstack.push(a);
		next();
	}

	void Machine::swap()
	{
		uintptr_t a = dstack.pop();
		uintptr_t b = dstack.pop();
		dstack.push(a);
		dstack.push(b);
		next();
	}

	void Machine::rot3()
	{
		uintptr_t a = dstack.pop();
		uintptr_t b = dstack.pop();
		uintptr_t c = dstack.pop();
		dstack.push(b);
		dstack.push(c);
		dstack.push(a);
		next();
	}

	void Machine::add()
	{
		intptr_t a = (intptr_t)dstack.pop();
		intptr_t b = (intptr_t)dstack.pop();
		intptr_t c = a + b;
		dstack.push((uintptr_t)c);
		next();
	}

	void Machine::bitwise_shift_left()
	{
		uintptr_t a = dstack.pop();
		uintptr_t b = dstack.pop();
		dstack.push(a << b);
		next();
	}

	void Machine::bitwise_shift_right()
	{
		uintptr_t a = dstack.pop();
		uintptr_t b = dstack.pop();
		dstack.push(a >> b);
		next();
	}

	void Machine::bitwise_and()
	{
		uintptr_t a = dstack.pop();
		uintptr_t b = dstack.pop();
		dstack.push(a & b);
		next();
	}

	void Machine::bitwise_or()
	{
		uintptr_t a = dstack.pop();
		uintptr_t b = dstack.pop();
		dstack.push(a | b);
		next();
	}

	void Machine::bitwise_xor()
	{
		uintptr_t a = dstack.pop();
		uintptr_t b = dstack.pop();
		dstack.push(a ^ b);
		next();
	}

	void Machine::bitwise_not()
	{
		uintptr_t a = dstack.pop();
		intptr_t b  = (intptr_t)~a;
		dstack.push((uintptr_t)b);
		next();
	}

	void Machine::add_symbol(std::string* str)
	{
		size_t idx = symtab.add(*str);
		dstack.push((uintptr_t)idx);
		next(8);
		delete str;
	}

	void Machine::find_symbol(std::string* str)
	{
		size_t idx = symtab.lookup(*str);
		if(idx != SYMTAB_NOT_FOUND)
			dstack.push((uintptr_t)idx);
		next(8);
		delete str;
	}

	void Machine::jz()
	{
		uintptr_t a = dstack.pop();
		uintptr_t c = dstack.pop();
		if(c == 0)
			ip = a;
		else
			next();
	}

	void Machine::save_stack()
	{
		uintptr_t index = 0;
		Continuation* c = new(this) Continuation(*this);
		c->save_current_stacks();
		dstack.push((uintptr_t)c);
		next();
	}

	void Machine::restore_stack()
	{
		uintptr_t addr = dstack.pop();
		Continuation* c = (Continuation*)memory[addr];
		c->restore_stacks();
		delete c;
	}

	void Machine::store()
	{
		uintptr_t addr = dstack.pop();
		uintptr_t data = dstack.pop();
		memory[addr] = data;
		next();
	}

	void Machine::load()
	{
		uintptr_t addr = dstack.pop();
		dstack.push(memory[addr]);
		next();
	}

	void Machine::run(const int instr, const uintptr_t& val)
	{
		switch(instr)
		{
			case Instructions::NOOP:
				next();
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
				dup();
				break;
			case Instructions::SWAP:
				swap();
				break;
			case Instructions::ROT3:
				rot3();
				break;
			case Instructions::ADD:
				add();
				break;
			case Instructions::LSHIFT:
				bitwise_shift_left();
				break;
			case Instructions::RSHIFT:
				bitwise_shift_right();
				break;
			case Instructions::AND:
				bitwise_and();
				break;
			case Instructions::OR:
				bitwise_or();
				break;
			case Instructions::XOR:
				bitwise_xor();
				break;
			case Instructions::NOT:
				bitwise_not();
				break;
			case Instructions::JZ:
				jz();
				break;
			case Instructions::SAVE_STACK:
				save_stack();
				break;
			case Instructions::RESTORE_STACK:
				restore_stack();
				break;
			case Instructions::STORE:
				store();
				break;
			case Instructions::LOAD:
				load();
				break;
		}
		next();
	}
}
