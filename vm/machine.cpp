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
		delete[] instructions;
	}

	uint8_t Machine::fetch_decode()
	{
		opcode = instructions[ip];
		return opcode;
	}

	uint8_t Machine::get_reg_opcode()
	{
		return instructions[++ip];
	}

	uint32_t Machine::get_int32_opcode()
	{
		uint32_t r = (instructions[++ip] << 24) | (instructions[++ip] << 16) | (instructions[++ip] << 8) | instructions[++ip];
		return r;
	}

	uintptr_t Machine::get_intptr_opcode()
	{
		uintptr_t r = 0;

		switch(sizeof(uintptr_t))
		{
			case 8:
				r = static_cast<uintptr_t>(get_int32_opcode()) << 32 | get_int32_opcode();
				break;
			case 4:
				r = get_int32_opcode();
				break;
		}

		if(big_endian())
			endian_swap(r);

		return r;
	}

	void Machine::move(Object** regs, uint8_t a, uint8_t b)
	{
		regs[a] = regs[b];
	}

	void Machine::loadi(Object** regs, uint8_t a, uintptr_t i)
	{
		regs[a] = constants[i];
	}

	void Machine::push(Object** regs, uintptr_t a)
	{
		get_current_context()->push(reinterpret_cast<Object*>(a));
	}

	void Machine::pop(Object** regs, uint8_t a)
	{
		regs[a] = get_current_context()->pop();
	}

	void Machine::dup()
	{
		Context* ctx = get_current_context();
		Object* obj = ctx->pop();
		ctx->push(obj);
		ctx->push(obj);
	}

	void Machine::swap()
	{
		Context* ctx = get_current_context();
		Object* a = ctx->pop();
		Object* b = ctx->pop();
		ctx->push(a);
		ctx->push(b);
	}

	void Machine::rotate(Object** regs, uint8_t a)
	{
		Context* ctx = get_current_context();
		size_t count = static_cast<Integer*>(regs[a])->c_int();
		Object* tmp[count];
		size_t i;

		for(i = 0; i < count; i++)
			tmp[i] = ctx->pop();

		for(i = 0; i < count; i++)
			ctx->push(tmp[i]);
	}

	void Machine::add(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Integer* i1 = static_cast<Integer*>(regs[b]);
		Integer* i2 = static_cast<Integer*>(regs[c]);
		regs[a] = new Integer(i1->c_int() + i2->c_int());
	}

	void Machine::sub(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Integer* i1 = static_cast<Integer*>(regs[b]);
		Integer* i2 = static_cast<Integer*>(regs[c]);
		regs[a] = new Integer(i1->c_int() - i2->c_int());
	}

	void Machine::mul(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Integer* i1 = static_cast<Integer*>(regs[b]);
		Integer* i2 = static_cast<Integer*>(regs[c]);
		regs[a] = new Integer(i1->c_int() * i2->c_int());
	}

	void Machine::div(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Integer* i1 = static_cast<Integer*>(regs[b]);
		Integer* i2 = static_cast<Integer*>(regs[c]);
		regs[a] = new Integer(i1->c_int() / i2->c_int());
	}

	void Machine::mod(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Integer* i1 = static_cast<Integer*>(regs[b]);
		Integer* i2 = static_cast<Integer*>(regs[c]);
		regs[a] = new Integer(i1->c_int() % i2->c_int());
	}

	void Machine::pow(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Integer* i1 = static_cast<Integer*>(regs[b]);
		Integer* i2 = static_cast<Integer*>(regs[c]);
		regs[a] = new Integer(i1->c_int() ^ i2->c_int());
	}

	void Machine::bitwise_not(Object** regs, uint8_t a, uint8_t b)
	{
		Integer* i = static_cast<Integer*>(regs[b]);
		regs[a] = new Integer(~i->c_int());
	}

	void Machine::eq(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Object* o1 = regs[b];
		Object* o2 = regs[c];
		int r = o1->compare(o2);
		regs[a] = new Boolean(r == 0);

		// Skip an extra instruction (the JMP)
		if(r != 0)
			next(4);
	}

	void Machine::lt(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Object* o1 = regs[b];
		Object* o2 = regs[c];
		int r = o1->compare(o2);
		regs[a] = new Boolean(r < 0);

		// Skip an extra instruction (the JMP)
		if(r != -1)
			next(4);
	}

	void Machine::lte(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Object* o1 = regs[b];
		Object* o2 = regs[c];
		int r = o1->compare(o2);
		regs[a] = new Boolean(r <= 0);

		// Skip an extra instruction (the JMP)
		if(r > 0)
			next(4);
	}

	void Machine::gt(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Object* o1 = regs[b];
		Object* o2 = regs[c];
		int r = o1->compare(o2);
		regs[a] = new Boolean(r > 0);

		// Skip an extra instruction (the JMP)
		if(r == 1)
			next(4);
	}

	void Machine::gte(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Object* o1 = regs[b];
		Object* o2 = regs[c];
		int r = o1->compare(o2);
		regs[a] = new Boolean(r >= 0);

		// Skip an extra instruction (the JMP)
		if(r < 0)
			next(4);
	}

	void Machine::jmp(uint32_t loc)
	{
		ip = loc;
	}

	void Machine::send(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Object*& receiver = regs[a];
		Object*& sender = regs[c];
		Message* msg = static_cast<Message*>(regs[b]);
		receiver->mailbox->deliver(msg, sender);
	}

	void Machine::ret()
	{
		Context* ctx = get_current_context();
		Object** regs = ctx->previous->registers;
		Object* r = ctx->top();

		if(r == NULL)
			regs[2] = Nil::instance();
		else
			regs[2] = r;
	}

	void Machine::save(Object** regs, uint8_t a)
	{
		Continuation* c = new Continuation(this);
		c->save_current_stack();
		regs[a] = reinterpret_cast<Object*>(c);
	}

	void Machine::restore(Object** regs, uint8_t a)
	{
		Continuation* c = static_cast<Continuation*>(regs[a]);
		c->restore_stack();
	}

	void Machine::make_array(Object** regs, uint8_t a)
	{
		Context* ctx = get_current_context();
		Integer* count = static_cast<Integer*>(regs[a]);
		Object* tmp[count->c_int()];
		for(uintptr_t i = 0; i < count->c_int(); i++)
			tmp[i] = ctx->pop();
		Array* array = new Array(tmp, count->c_int());
		ctx->push(array);
	}

	void Machine::make_string(Object** regs, uint8_t a)
	{
		Context* ctx = get_current_context();
		Integer* count = static_cast<Integer*>(regs[a]);
		char* tmp = new char[count->c_int()];

		for(uintptr_t i = 0; i < count->c_int(); i++)
			tmp[i] = static_cast<Integer*>(ctx->pop())->c_int();

		String* str = new String(std::string(tmp));
		ctx->push(str);

		delete tmp;
	}

	void Machine::addsym(Object** regs, uint8_t a, uint8_t b)
	{
		String* str = static_cast<String*>(regs[b]);
		size_t i = symtab.add(str);
		regs[a] = reinterpret_cast<Object*>(new Integer(i));
	}

	/* Find a symbol in the table.
	 * Inputs: Two registers - 1) Destination, 2) Index into the table
	 * Takes an index in the table held in the second register, and looks it up in the symbol
	 * table. Places the string returned by that action into the first register. Places Nil
	 * if no symbol could be found.
	 */
	void Machine::findsym(Object** regs, uint8_t a, uint8_t b)
	{
		Integer* idx = static_cast<Integer*>(regs[b]);
		Object* str = reinterpret_cast<Object*>(symtab.lookup(idx->c_int()));
		if(str)
			regs[a] = str;
		else
			regs[a] = Nil::instance();
	}

	void Machine::execute()
	{
		ip = 0;

		fetch_decode();

		VMMethod* main = new VMMethod(this, NULL, new String("__main"), ip, 0);
		Context* context = new Context(NULL, main, 0);
		rstack.push(context);

		while(opcode != Instructions::HALT && ip < icount)
		{
			process(opcode, get_current_context()->registers);
			fetch_decode();
		}
	}

	void Machine::process(uint8_t op, Object** regs)
	{
		switch(op)
		{
			case Instructions::NOOP:
				next();
				break;
			case Instructions::MOVE:
				move(regs, get_reg_opcode(), get_reg_opcode());
				next(3);
				break;
			case Instructions::LOADI:
				loadi(regs, get_reg_opcode(), get_intptr_opcode());
				next(2 + sizeof(uintptr_t));
				break;
			case Instructions::PUSH:
				push(regs, get_intptr_opcode());
				break;
			case Instructions::POP:
				pop(regs, get_reg_opcode());
				break;
			case Instructions::SWAP:
				swap();
				break;
			case Instructions::ROTATE:
				rotate(regs, get_reg_opcode());
				break;
			case Instructions::DUP:
				dup();
				break;
			case Instructions::ADD:
				add(regs, get_reg_opcode(), get_reg_opcode(), get_reg_opcode());
				next(4);
				break;
			case Instructions::SUB:
				sub(regs, get_reg_opcode(), get_reg_opcode(), get_reg_opcode());
				next(4);
				break;
			case Instructions::MUL:
				mul(regs, get_reg_opcode(), get_reg_opcode(), get_reg_opcode());
				next(4);
				break;
			case Instructions::DIV:
				div(regs, get_reg_opcode(), get_reg_opcode(), get_reg_opcode());
				next(4);
				break;
			case Instructions::MOD:
				mod(regs, get_reg_opcode(), get_reg_opcode(), get_reg_opcode());
				next(4);
				break;
			case Instructions::POW:
				pow(regs, get_reg_opcode(), get_reg_opcode(), get_reg_opcode());
				next(4);
				break;
			case Instructions::NOT:
				bitwise_not(regs, get_reg_opcode(), get_reg_opcode());
				next(3);
				break;
			case Instructions::EQ:
				eq(regs, get_reg_opcode(), get_reg_opcode(), get_reg_opcode());
				next(4);
				break;
			case Instructions::LT:
				lt(regs, get_reg_opcode(), get_reg_opcode(), get_reg_opcode());
				next(4);
				break;
			case Instructions::LTE:
				lte(regs, get_reg_opcode(), get_reg_opcode(), get_reg_opcode());
				next(4);
				break;
			case Instructions::GT:
				gt(regs, get_reg_opcode(), get_reg_opcode(), get_reg_opcode());
				next(4);
				break;
			case Instructions::GTE:
				gte(regs, get_reg_opcode(), get_reg_opcode(), get_reg_opcode());
				next(4);
				break;
			case Instructions::HALT:
				ip = UINTPTR_MAX;
				break;
			case Instructions::SEND:
				send(regs, get_reg_opcode(), get_reg_opcode(), get_reg_opcode());
				next(4);
				break;
			case Instructions::RET:
				ret();
				next();
				break;
			case Instructions::JMP:
				jmp(get_intptr_opcode());
				break;
			case Instructions::SAVE:
				save(regs, get_reg_opcode());
				next(2);
				break;
			case Instructions::RESTORE:
				restore(regs, get_reg_opcode());
				break;
			case Instructions::ADDSYM:
				addsym(regs, get_reg_opcode(), get_reg_opcode());
				next(3);
				break;
			case Instructions::FINDSYM:
				findsym(regs, get_reg_opcode(), get_reg_opcode());
				next(3);
				break;
			case Instructions::ARRAY:
				make_array(regs, get_reg_opcode());
				next(2);
				break;
			case Instructions::STRING:
				make_string(regs, get_reg_opcode());
				next(2);
				break;
		}
	}
}
