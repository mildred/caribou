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

	uint32_t Machine::fetch()
	{
		return instructions[ip];
	}

	void Machine::decode(uint32_t instr)
	{
		if(big_endian())
			endian_swap(instr);

		opcode   = instr & 0xff;
		fields.a = instr & 0x00ff;
		fields.b = instr & 0x0000ff;
		fields.c = instr & 0x000000ff;
	}

	void Machine::move(Object** regs, uint8_t a, uint8_t b)
	{
		regs[a] = regs[b];
	}

	void Machine::loadi(Object** regs, uint8_t a, uint16_t i)
	{
		regs[a] = constants[i];
	}

	void Machine::push(Object** regs, uint8_t a)
	{
		get_current_context()->push(regs[a]);
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
		if(r == 0)
			next();
	}

	void Machine::lt(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Object* o1 = regs[b];
		Object* o2 = regs[c];
		int r = o1->compare(o2);
		regs[a] = new Boolean(r < 0);

		// Skip an extra instruction (the JMP)
		if(r < 0)
			next();
	}

	void Machine::lte(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Object* o1 = regs[b];
		Object* o2 = regs[c];
		int r = o1->compare(o2);
		regs[a] = new Boolean(r <= 0);

		// Skip an extra instruction (the JMP)
		if(r <= 0)
			next();
	}

	void Machine::gt(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Object* o1 = regs[b];
		Object* o2 = regs[c];
		int r = o1->compare(o2);
		regs[a] = new Boolean(r > 0);

		// Skip an extra instruction (the JMP)
		if(r > 0)
			next();
	}

	void Machine::gte(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Object* o1 = regs[b];
		Object* o2 = regs[c];
		int r = o1->compare(o2);
		regs[a] = new Boolean(r >= 0);

		// Skip an extra instruction (the JMP)
		if(r >= 0)
			next();
	}

	void Machine::jmp(uint32_t loc)
	{
		ip = loc;
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

	void Machine::execute()
	{
		ip = 0;

		decode(fetch());

		while(opcode != Instructions::HALT && ip < icount)
		{
			process(opcode, rstack.top()->registers);
			decode(fetch());
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
				move(regs, fields.a, fields.b);
				next();
				break;
			case Instructions::LOADI:
				loadi(regs, fields.a, fields.bmore);
				next();
				break;
			case Instructions::PUSH:
				push(regs, fields.a);
				break;
			case Instructions::POP:
				pop(regs, fields.a);
				break;
			case Instructions::SWAP:
				swap();
				break;
			case Instructions::ROTATE:
				rotate(regs, fields.a);
				break;
			case Instructions::DUP:
				dup();
				break;
			case Instructions::ADD:
				add(regs, fields.a, fields.b, fields.c);
				next();
				break;
			case Instructions::SUB:
				sub(regs, fields.a, fields.b, fields.c);
				next();
				break;
			case Instructions::MUL:
				mul(regs, fields.a, fields.b, fields.c);
				next();
				break;
			case Instructions::DIV:
				div(regs, fields.a, fields.b, fields.c);
				next();
				break;
			case Instructions::MOD:
				mod(regs, fields.a, fields.b, fields.c);
				next();
				break;
			case Instructions::POW:
				pow(regs, fields.a, fields.b, fields.c);
				next();
				break;
			case Instructions::NOT:
				bitwise_not(regs, fields.a, fields.b);
				next();
				break;
			case Instructions::EQ:
				eq(regs, fields.a, fields.b, fields.c);
				next();
				break;
			case Instructions::LT:
				lt(regs, fields.a, fields.b, fields.c);
				next();
				break;
			case Instructions::LTE:
				lte(regs, fields.a, fields.b, fields.c);
				next();
				break;
			case Instructions::GT:
				gt(regs, fields.a, fields.b, fields.c);
				next();
				break;
			case Instructions::GTE:
				gte(regs, fields.a, fields.b, fields.c);
				next();
				break;
			case Instructions::HALT:
				ip = UINTPTR_MAX;
				break;
			case Instructions::SEND:
				break;
			case Instructions::RET:
				break;
			case Instructions::JMP:
				jmp(fields.immed);
				break;
			case Instructions::SAVE:
				save(regs, fields.a);
				next();
				break;
			case Instructions::RESTORE:
				restore(regs, fields.a);
				break;
			case Instructions::ADDSYM:
				break;
			case Instructions::FINDSYM:
				break;
			case Instructions::ARRAY:
				break;
			case Instructions::STRING:
				break;
		}
	}
}
