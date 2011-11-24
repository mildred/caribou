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

#include <iostream>
#include <stdint.h>
#include <math.h>
#include "machine.hpp"
#include "continuation.hpp"
#include "endian.hpp"
#include "gc.hpp"
#include "object.hpp"
#include "mailbox.hpp"
#include "integer.hpp"
#include "array.hpp"
#include "string.hpp"
#include "nil.hpp"
#include "boolean.hpp"

namespace Caribou
{
	class Object;
	class Message;

	GarbageCollector* collector = nullptr;

	Machine::Machine() : rstack()
	{
		collector = new GarbageCollector(this);
	}

	Machine::~Machine()
	{
		delete[] instruction_memory;
	}

	void Machine::push(Context* ctx, Object* val)
	{
		ctx->push(val);
		next();
	}

	Object* Machine::pop(Context* ctx)
	{
		next();
		return ctx->pop();
	}

	void Machine::dup(Context* ctx)
	{
		Object* a = ctx->pop();
		ctx->push(a);
		ctx->push(a);
		next();
	}

	void Machine::swap(Context* ctx)
	{
		Object* a = ctx->pop();
		Object* b = ctx->pop();
		ctx->push(a);
		ctx->push(b);
		next();
	}

	void Machine::rotate(Context* ctx)
	{
		size_t count = static_cast<Integer*>(ctx->pop())->c_int();
		Object* tmp[count];
		size_t i;

		for(i = 0; i < count; i++)
			tmp[i] = ctx->pop();

		for(i = 0; i < count; i++)
			ctx->push(tmp[i]);
		ctx->print_stack();

		next();
	}

	void Machine::add(Context* ctx)
	{
		Integer* a = static_cast<Integer*>(ctx->pop());
		Integer* b = static_cast<Integer*>(ctx->pop());
		ctx->push(new Integer(a->c_int() + b->c_int()));
		next();
	}

	void Machine::sub(Context* ctx)
	{
		Integer* a = static_cast<Integer*>(ctx->pop());
		Integer* b = static_cast<Integer*>(ctx->pop());
		ctx->push(new Integer(a->c_int() - b->c_int()));
		next();
	}

	void Machine::mul(Context* ctx)
	{
		Integer* a = static_cast<Integer*>(ctx->pop());
		Integer* b = static_cast<Integer*>(ctx->pop());
		ctx->push(new Integer(a->c_int() * b->c_int()));
		next();
	}

	void Machine::div(Context* ctx)
	{
		Integer* a = static_cast<Integer*>(ctx->pop());
		Integer* b = static_cast<Integer*>(ctx->pop());
		ctx->push(new Integer(a->c_int() / b->c_int()));
		next();
	}

	void Machine::mod(Context* ctx)
	{
		Integer* a = static_cast<Integer*>(ctx->pop());
		Integer* b = static_cast<Integer*>(ctx->pop());
		ctx->push(new Integer(a->c_int() % b->c_int()));
		next();
	}

	void Machine::pow(Context* ctx)
	{
		Integer* a = static_cast<Integer*>(ctx->pop());
		Integer* b = static_cast<Integer*>(ctx->pop());
		ctx->push(new Integer(::pow(a->c_int(), b->c_int())));
		next();
	}

	void Machine::bitwise_not(Context* ctx)
	{
		Integer* a = static_cast<Integer*>(ctx->pop());
		ctx->push(new Integer(~(a->c_int())));
		next();
	}

	void Machine::eq(Context* ctx)
	{
		Object* a = ctx->pop();
		Object* b = ctx->pop();
		ctx->push(new Boolean(a->compare(b)));
		next();
	}

	void Machine::lt(Context* ctx)
	{
		Object* a = ctx->pop();
		Object* b = ctx->pop();
		ctx->push(new Boolean(a->compare(b) == -1));
		next();
	}

	void Machine::gt(Context* ctx)
	{
		Object* a = ctx->pop();
		Object* b = ctx->pop();
		ctx->push(new Boolean(a->compare(b) == 1));
		next();
	}

	void Machine::add_symbol(Context* ctx)
	{
		String* str = static_cast<String*>(ctx->pop());
		size_t idx = symtab.add(str);
		Integer* index = new Integer(idx);
		ctx->push(index);
		next();
	}

	void Machine::find_symbol(Context* ctx)
	{
		Integer* i = static_cast<Integer*>(ctx->pop());
		String* str = symtab.lookup(i->c_int());
		if(str)
			ctx->push(str);
		else
			ctx->push(Nil::instance());
		next();
	}

	void Machine::jmp(Context* ctx)
	{
		Integer* a = static_cast<Integer*>(ctx->pop());
		ip = a->c_int();
	}

	void Machine::jt(Context* ctx)
	{
		Integer* a = static_cast<Integer*>(ctx->pop());
		Boolean* c = static_cast<Boolean*>(ctx->pop());
		if(c->value())
			ip = a->c_int();
		else
			next();
	}

	void Machine::make_array(Context* ctx)
	{
		Integer* count = static_cast<Integer*>(ctx->pop());
		Object* tmp[count->c_int()];
		for(uintptr_t i = 0; i < count->c_int(); i++)
			tmp[i] = ctx->pop();
		Array* array = new Array(tmp, count->c_int());
		ctx->push(array);
		next();
	}

	void Machine::make_string(Context* ctx)
	{
		Integer* count = static_cast<Integer*>(ctx->pop());
		char* tmp = new char[count->c_int()];

		for(uintptr_t i = 0; i < count->c_int(); i++)
			tmp[i] = static_cast<Integer*>(ctx->pop())->c_int();

		String* str = new String(std::string(tmp));
		ctx->push(str);

		delete tmp;
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
		Message* message = static_cast<Message*>(ctx->pop());
		Object* sender = static_cast<Object*>(ctx->pop());
		Object* receiver = static_cast<Object*>(ctx->pop());

		next();
		push_new_context(ctx, receiver, sender, message);
		receiver->mailbox->deliver(get_current_context(), message);
	}

	void Machine::ret(Context* ctx)
	{
		Object* r = ctx->pop();
		ip = ctx->ip;
		ctx->previous->push(r);
	}

	void Machine::save_stack(Context* ctx)
	{
		Continuation* c = new Continuation(this);
		c->save_current_stack();
		ctx->push(reinterpret_cast<Object*>(c));
		next();
	}

	void Machine::restore_stack(Context* ctx)
	{
		Continuation* c = static_cast<Continuation*>(ctx->pop());
		c->restore_stack();
	}

	void Machine::run()
	{
		// TODO: Need to create a default context and push that onto the return stack before evaluating any bytecodes.
		push_new_context(nullptr, nullptr/*Lobby*/, nullptr/*Lobby*/, nullptr/*someMessage*/);
		ip = 0;

		while(ip != UINTPTR_MAX)
		{
			uint8_t byte = instruction_memory[ip];
			uintptr_t operand = 0;
			Object* oper;

			if(byte == Instructions::PUSH)
			{
				operand = (uintptr_t)instruction_memory[ip + 1];
				if(big_endian())
					endian_swap(operand);
				ip += sizeof(uintptr_t) - 1;
			}

			oper = reinterpret_cast<Object*>(new Integer(operand));
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
			case Instructions::HALT:
				ip = UINTPTR_MAX;
				break;
			case Instructions::PUSH:
				push(ctx, val);
				break;
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
			case Instructions::ROTATE:
				rotate(ctx);
				break;
			case Instructions::SAVE_STACK:
				save_stack(ctx);
				break;
			case Instructions::RESTORE_STACK:
				restore_stack(ctx);
				break;
			case Instructions::ADD:
				add(ctx);
				break;
			case Instructions::SUB:
				sub(ctx);
				break;
			case Instructions::MUL:
				mul(ctx);
				break;
			case Instructions::DIV:
				div(ctx);
				break;
			case Instructions::MOD:
				mod(ctx);
				break;
			case Instructions::POW:
				pow(ctx);
				break;
			case Instructions::NOT:
				not(ctx);
				break;
			case Instructions::EQ:
				eq(ctx);
				break;
			case Instructions::LT:
				lt(ctx);
				break;
			case Instructions::GT:
				gt(ctx);
				break;
			case Instructions::JMP:
				jmp(ctx);
				break;
			case Instructions::JT:
				jt(ctx);
				break;
			case Instructions::MAKE_ARRAY:
				make_array(ctx);
				break;
			case Instructions::MAKE_STRING:
				make_string(ctx);
				break;
			case Instructions::SEND:
				send(ctx);
				break;
			case Instructions::ADD_SYMBOL:
				add_symbol(ctx);
				break;
			case Instructions::FIND_SYMBOL:
				find_symbol(ctx);
				break;
		}
	}
}
