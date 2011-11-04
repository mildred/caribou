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
#include "integer.hpp"
#include "array.hpp"
#include "string.hpp"

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

	void Machine::push(Context* ctx, Object* val)
	{
		STACK(ctx).push(val);
		next();
	}

	Object* Machine::pop(Context* ctx)
	{
		next();
		return STACK(ctx).pop();
	}

	void Machine::ret(Context* ctx)
	{
		Object* r = STACK(ctx).pop();
		ip = ctx->ip;
		STACK(ctx->previous).push(r);
	}

	void Machine::dup(Context* ctx)
	{
		Object* a = STACK(ctx).pop();
		STACK(ctx).push(a);
		STACK(ctx).push(a);
		next();
	}

	void Machine::swap(Context* ctx)
	{
		Object* a = STACK(ctx).pop();
		Object* b = STACK(ctx).pop();
		STACK(ctx).push(a);
		STACK(ctx).push(b);
		next();
	}

	void Machine::rot3(Context* ctx)
	{
		Object* a = STACK(ctx).pop();
		Object* b = STACK(ctx).pop();
		Object* c = STACK(ctx).pop();
		STACK(ctx).push(b);
		STACK(ctx).push(c);
		STACK(ctx).push(a);
		next();
	}

	void Machine::add_symbol(Context* ctx)
	{
		String* str = static_cast<String*>(STACK(ctx).pop());
		size_t idx = symtab.add(str);
		Integer* index = new Integer(idx);
		STACK(ctx).push(index);
		next();
	}

	void Machine::find_symbol(Context* ctx)
	{
		String* str = static_cast<String*>(STACK(ctx).pop());
		size_t idx = symtab.lookup(str);
		if(idx != SYMTAB_NOT_FOUND)
		{
			Integer* index = new Integer(idx);
			STACK(ctx).push(index);
		}
		else
			STACK(ctx).push(new Integer(0)); // XXX: Push nil instead
		next();
	}

	void Machine::jz(Context* ctx)
	{
		Integer* a = static_cast<Integer*>(STACK(ctx).pop());
		Integer* c = static_cast<Integer*>(STACK(ctx).pop());
		if(c->c_int() == 0)
			ip = a->c_int();
		else
			next();
	}

	void Machine::make_array(Context* ctx)
	{
		Integer* count = static_cast<Integer*>(STACK(ctx).pop());
		Object* tmp[count->c_int()];
		for(uintptr_t i = 0; i < count->c_int(); i++)
			tmp[i] = STACK(ctx).pop();
		Array* array = new Array(tmp, count->c_int());
		STACK(ctx).push(array);
		next();
	}

	void Machine::push_new_context(Context* old, Object* receiver, Object* sender, Message* message)
	{
		Context* c = new Context();
		c->previous = old;
		c->sender = sender;
		c->target = receiver;
		c->receiver = receiver;
		c->ip = get_instruction_pointer();
		rstack.push(c);
	}

	void Machine::send(Context* ctx)
	{
		Message* message = static_cast<Message*>(STACK(ctx).pop());
		Object* sender = static_cast<Object*>(STACK(ctx).pop());
		Object* receiver = static_cast<Object*>(STACK(ctx).pop());

		next();
		push_new_context(ctx, receiver, sender, message);
		receiver->mailbox->deliver(get_current_context(), *message);
	}

	void Machine::save_stack(Context* ctx)
	{
		Continuation* c = new Continuation(*this);
		c->save_current_stack();
		STACK(ctx).push(reinterpret_cast<Object*>(c));
		next();
	}

	void Machine::restore_stack(Context* ctx)
	{
		Integer* addr = static_cast<Integer*>(STACK(ctx).pop());
		Continuation* c = (Continuation*)memory[addr->c_int()];
		c->restore_stack();
	}

	void Machine::run()
	{
		// TODO: Need to create a default context and push that onto the return stack before evaluating any bytecodes.
		push_new_context(NULL, NULL/*Lobby*/, NULL/*Lobby*/, NULL/*someMessage*/);

		for(uint8_t idx = 0; idx < instruction_size; idx++)
		{
			uint8_t byte = instruction_memory[idx];
			uint64_t operand = 0;
			Object* oper;

			if(byte == Instructions::PUSH)
			{
				operand = (uint64_t)instruction_memory[idx + 1];
				if(big_endian())
					endian_swap(operand);
				idx += sizeof(uint64_t);
			}

			oper = reinterpret_cast<Object*>(operand);
			process(rstack.top(), byte, oper);
		}
	}

	void Machine::process(Context* ctx, uint8_t instr, Object* val)
	{
		switch(instr)
		{
			case Instructions::NOOP:
				next();
				break;
			case Instructions::PUSH:
			{
				push(ctx, val);
				break;
			}
			case Instructions::POP:
				pop(ctx);
				break;
			case Instructions::RET:
				ret(ctx);
				break;
			case Instructions::DUP:
				dup(ctx);
				break;
			case Instructions::SWAP:
				swap(ctx);
				break;
			case Instructions::ROT3:
				rot3(ctx);
				break;
			case Instructions::JZ:
				jz(ctx);
				break;
			case Instructions::MAKE_ARRAY:
				break;
			case Instructions::SEND:
				send(ctx);
				break;
			case Instructions::SAVE_STACK:
				save_stack(ctx);
				break;
			case Instructions::RESTORE_STACK:
				restore_stack(ctx);
				break;
			case Instructions::ADD_SYMBOL:
				break;
			case Instructions::FIND_SYMBOL:
				break;
		}
		next();
	}
}
