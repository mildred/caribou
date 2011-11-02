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
#include "endian.hpp"
#include "gc.hpp"
#include "object.hpp"
#include "mailbox.hpp"

namespace Caribou
{
	class Object;
	class Message;

	GarbageCollector* collector = nullptr;

	Machine::Machine()
	{
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

	void Machine::ret()
	{
		// XXX: This is completely broken, as are most of these instructions now.
		Context* r = rstack.pop();
		ip = r->ip;
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
		else
			dstack.push(0); // XXX: Push nil instead, when you know, nil exists
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

	void Machine::send()
	{
		Message* message = reinterpret_cast<Message*>(dstack.pop());
		Object* sender = reinterpret_cast<Object*>(dstack.pop());
		Object* receiver = reinterpret_cast<Object*>(dstack.pop());

		receiver->mailbox->deliver(*message);
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

	void Machine::run()
	{
		for(uint8_t idx = 0; idx < instruction_size; idx++)
		{
			uint8_t byte = instruction_memory[idx];
			uint64_t operand = 0;
			if(byte == Instructions::PUSH)
			{
				operand = (uint64_t)instruction_memory[idx + 1];
				if(big_endian())
					endian_swap(operand);
				idx += sizeof(uint64_t);
			}
			process(byte, operand);
		}
	}

	void Machine::process(uint8_t instr, uintptr_t val)
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
			case Instructions::RET:
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
			case Instructions::JZ:
				jz();
				break;
			case Instructions::SEND:
				send();
				break;
			case Instructions::SAVE_STACK:
				save_stack();
				break;
			case Instructions::RESTORE_STACK:
				restore_stack();
				break;
			case Instructions::ADD_SYMBOL:
				break;
			case Instructions::FIND_SYMBOL:
				break;
		}
		next();
	}
}
