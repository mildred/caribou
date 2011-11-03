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
		ctx->stk.push(val);
		next();
	}

	Object* Machine::pop(Context* ctx)
	{
		next();
		return ctx->stk.pop();
	}

	void Machine::ret(Context* ctx)
	{
		Object* r = ctx->stk.pop();
		ip = ctx->ip;
		ctx->previous->stk.push(r);
	}

	void Machine::dup(Context* ctx)
	{
		Object* a = ctx->stk.pop();
		ctx->stk.push(a);
		ctx->stk.push(a);
		next();
	}

	void Machine::swap(Context* ctx)
	{
		Object* a = ctx->stk.pop();
		Object* b = ctx->stk.pop();
		ctx->stk.push(a);
		ctx->stk.push(b);
		next();
	}

	void Machine::rot3(Context* ctx)
	{
		Object* a = ctx->stk.pop();
		Object* b = ctx->stk.pop();
		Object* c = ctx->stk.pop();
		ctx->stk.push(b);
		ctx->stk.push(c);
		ctx->stk.push(a);
		next();
	}

	void Machine::add_symbol(Context* ctx, std::string& str)
	{
		intptr_t idx = static_cast<intptr_t>(symtab.add(str));
		Integer* index = new Integer(idx);
		// TODO: Add to GC
		ctx->stk.push(index);
		next(8);
	}

	void Machine::find_symbol(Context* ctx, std::string& str)
	{
		intptr_t idx = static_cast<intptr_t>(symtab.lookup(str));
		if(idx != SYMTAB_NOT_FOUND)
		{
			Integer* index = new Integer(idx);
			// TODO: Add to GC
			ctx->stk.push(index);
		}
		else
			ctx->stk.push(new Integer(0)); // XXX: Push nil instead
		next(8);
	}

	void Machine::jz(Context* ctx)
	{
		Integer* a = static_cast<Integer*>(ctx->stk.pop());
		Integer* c = static_cast<Integer*>(ctx->stk.pop());
		if(c->c_int() == 0)
			ip = a->c_int();
		else
			next();
	}

	void Machine::make_array(Context* ctx)
	{
		Integer* count = static_cast<Integer*>(ctx->stk.pop());
		Object* tmp[count->c_int()];
		for(uintptr_t i = 0; i < count->c_int(); i++)
			tmp[i] = ctx->stk.pop();
		Array* array = new Array(tmp, count->c_int());
	}

	void Machine::send(Context* ctx)
	{
		Message* message = static_cast<Message*>(ctx->stk.pop());
		Object* sender = static_cast<Object*>(ctx->stk.pop());
		Object* receiver = static_cast<Object*>(ctx->stk.pop());

		receiver->mailbox->deliver(*message);
	}

	void Machine::save_stack(Context* ctx)
	{
		Continuation* c = new Continuation(*this);
		c->save_current_stack();
		ctx->stk.push(reinterpret_cast<Object*>(c));
		next();
	}

	void Machine::restore_stack(Context* ctx)
	{
		Integer* addr = static_cast<Integer*>(ctx->stk.pop());
		Continuation* c = (Continuation*)memory[addr->c_int()];
		c->restore_stack();
	}

	void Machine::run()
	{
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
