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
#include "continuation.hpp"

namespace Caribou
{
	Machine::Machine() : memory_mapper()
	{
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
		index = memory_mapper.request_slot(sizeof(*c));
		memory_mapper.store(index, std::vector<uintptr_t>((uintptr_t)c));
		dstack.push(index);
		next();
	}

	void Machine::restore_stack()
	{
		uintptr_t idx = dstack.pop();
		Continuation* c = (Continuation*)memory_mapper.get_slot(idx)[MEMORY_ADDRESS];
		c->restore_stacks();
		delete c;
	}

	void Machine::store()
	{
		uintptr_t num_elems = dstack.pop();
		size_t size = 0;
		std::vector<uintptr_t> elements;
		size_t index;

		for(size_t i = 0; i < num_elems; i++)
		{
			uintptr_t data = dstack.pop();
			elements.push_back(data);
			size += sizeof(data);
		}

		if(size > 0)
		{
			index = memory_mapper.request_slot(size);
			memory_mapper.store(index, elements);
			dstack.push(index);
		}

		next();
	}

	void Machine::load()
	{
		uintptr_t index = pop();
		uintptr_t* slot = memory_mapper.get_slot(index);
		size_t size     = slot[MEMORY_SIZE];
		uintptr_t* data = (uintptr_t*)slot[MEMORY_ADDRESS];

		for(size_t i = 0; i < size; i++)
			dstack.push(data[i]);

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
