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
				r = static_cast<uint64_t>(get_int32_opcode()) << 32 | get_int32_opcode();
				break;
			case 4:
				r = get_int32_opcode();
				break;
		}

		if(big_endian())
			endian_swap(r);

		return r;
	}

	/* Copy contents of one register to another
	 * Inputs: Two registers - 1) Destination, 2) Source
	 * Copies the contents of the source register to the destination register
	 */
	void Machine::move(Object** regs, uint8_t a, uint8_t b)
	{
		regs[a] = regs[b];
	}

	/* Load a constant into a register
	 * Inputs: A register and a pointer sized operand
	 * Copies the object into the register
	 */
	void Machine::loadi(Object** regs, uint8_t a, uintptr_t i)
	{
		regs[a] = constants[i];
	}

	/* Push an item onto the stack
	 * Inputs: A pointer sized object
	 * Pushes the operand onto the stack.
	 */
	void Machine::push(Object** regs, uintptr_t a)
	{
		get_current_context()->push(reinterpret_cast<Object*>(a));
	}

	/* Pop an object off the stack
	 * Inputs: One register - Destination for the object
	 * Removes the top of the stack and places it into the register.
	 */
	void Machine::pop(Object** regs, uint8_t a)
	{
		regs[a] = get_current_context()->pop();
	}

	/* Duplicate the top item on the stack
	 * Inputs: Top of the stack
	 * Looks at the top of the stack, and pushes it into the stack again.
	 */
	void Machine::dup()
	{
		Context* ctx = get_current_context();
		ctx->push(ctx->top());
	}

	/* Swaps the top two items on the stack
	 * Inputs: Top two stack items
	 * Pops the top two items off the stack, and pushes them back on in reverse
	 * order
	 */
	void Machine::swap()
	{
		Context* ctx = get_current_context();
		Object* a = ctx->pop();
		Object* b = ctx->pop();
		ctx->push(a);
		ctx->push(b);
	}

	/* Rotate N items on the stack
	 * Inputs: One register - The number of items to rotate on the stack
	 * Pops N items off the stack, then pushes them back on preserving order.
	 * Before: (a b c d --)
	 * After: (d c b a --)
	 */
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

	/* Add two integers
	 * Inputs: Three registers - 1) Destination, 2) Integer A, 3) Integer B
	 */
	void Machine::add(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Integer* i1 = static_cast<Integer*>(regs[b]);
		Integer* i2 = static_cast<Integer*>(regs[c]);
		regs[a] = new Integer(i1->c_int() + i2->c_int());
	}

	/* Subtract two integers
	 * Inputs: Three registers - 1) Destination, 2) Integer A, 3) Integer B
	 */
	void Machine::sub(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Integer* i1 = static_cast<Integer*>(regs[b]);
		Integer* i2 = static_cast<Integer*>(regs[c]);
		regs[a] = new Integer(i1->c_int() - i2->c_int());
	}

	/* Multiply two integers
	 * Inputs: Three registers - 1) Destination, 2) Integer A, 3) Integer B
	 */
	void Machine::mul(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Integer* i1 = static_cast<Integer*>(regs[b]);
		Integer* i2 = static_cast<Integer*>(regs[c]);
		regs[a] = new Integer(i1->c_int() * i2->c_int());
	}

	/* Divide two integers
	 * Inputs: Three registers - 1) Destination, 2) Integer A, 3) Integer B
	 */
	void Machine::div(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Integer* i1 = static_cast<Integer*>(regs[b]);
		Integer* i2 = static_cast<Integer*>(regs[c]);
		regs[a] = new Integer(i1->c_int() / i2->c_int());
	}

	/* Calculate the modulo of two integers
	 * Inputs: Three registers - 1) Destination, 2) Integer A, 3) Integer B
	 */
	void Machine::mod(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Integer* i1 = static_cast<Integer*>(regs[b]);
		Integer* i2 = static_cast<Integer*>(regs[c]);
		regs[a] = new Integer(i1->c_int() % i2->c_int());
	}

	/* Calculate the power of an integer
	 * Inputs: Three registers - 1) Destination, 2) Integer, 3) Exponent
	 * Returns the result of applying the exponent to the integer.
	 */
	void Machine::pow(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Integer* i1 = static_cast<Integer*>(regs[b]);
		Integer* i2 = static_cast<Integer*>(regs[c]);
		regs[a] = new Integer(i1->c_int() ^ i2->c_int());
	}

	/* Calculate the bitwise not of an integer
	 * Inputs: Two registers - 1) Destination 2) Source
	 * Applies a complement to the integer.
	 */
	void Machine::bitwise_not(Object** regs, uint8_t a, uint8_t b)
	{
		Integer* i = static_cast<Integer*>(regs[b]);
		regs[a] = new Integer(~i->c_int());
	}

	/* Check if two objects are equal
	 * Inputs: Three registers - 1) Destination, 2) Object A, 3) Object B
	 * This instruction expects to be paired with a JMP instruction or 4 noop
	 * instructions.
	 */
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

	/* Check if an object is less than another
	 * Inputs: Three registers - 1) Destination, 2) Object A, 3) Object B
	 * This instruction expects to be paired with a JMP instruction or 4 noop
	 * instructions.
	 */
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

	/* Check if an object is less than or equal to another
	 * Inputs: Three registers - 1) Destination, 2) Object A, 3) Object B
	 * This instruction expects to be paired with a JMP instruction or 4 noop
	 * instructions.
	 */
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

	/* Check if an object is greater than another
	 * Inputs: Three registers - 1) Destination, 2) Object A, 3) Object B
	 * This instruction expects to be paired with a JMP instruction or 4 noop
	 * instructions.
	 */
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

	/* Check if an object is greater than or equal to another
	 * Inputs: Three registers - 1) Destination, 2) Object A, 3) Object B
	 * This instruction expects to be paired with a JMP instruction or 4 noop
	 * instructions.
	 */
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

	/* Unconditional jump
	 * Inputs: Address to jump to
	 */
	void Machine::jmp(uint32_t loc)
	{
		ip = loc;
	}

	/* Send a message
	 * Inputs: Three registers - receiver of the message, the message, sending context
	 * Instructs the receiver to receive the message we are sending it. Passes along the
	 * sending context.
	 */
	void Machine::send(Object** regs, uint8_t a, uint8_t b, uint8_t c)
	{
		Object*& receiver = regs[a];
		Object*& sender = regs[c];
		Message* msg = static_cast<Message*>(regs[b]);
		receiver->mailbox->deliver(msg, sender);
	}

	/* Return from a method
	 * Inputs: None
	 * Stores the top of the stack (or Nil) into the return register of the previous
	 * context.
	 */
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

	/* Save the contents of the stack in a continuation
	 * Inputs: One register - for storing the continuation
	 * After this instruction finishes, we save a copy of the stack including the registers
	 * in a continuation. This continuation is placed in the destination register.
	 */
	void Machine::save(Object** regs, uint8_t a)
	{
		Continuation* c = new Continuation(this);
		c->save_current_stack();
		regs[a] = reinterpret_cast<Object*>(c);
	}

	/* Call a continuation and restore the stack
	 * Inputs: One register - containing the continuation to call
	 * After this instruction finishes, the instruction pointer will point at the saved IP,
	 * call stack will be restored along with each set of registers.
	 */
	void Machine::restore(Object** regs, uint8_t a)
	{
		Continuation* c = static_cast<Continuation*>(regs[a]);
		c->restore_stack();
	}

	/* Make a new array
	 * Inputs: One register - containing the count of items to pop off the stack
	 * Place the objects on the stack prior to invoking this instruction and place the count
	 * in a register that will be passed to this instruction.
	 */
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

	/* Make a new string
	 * Inputs: One register - containing the count of items to pop off the stack
	 * Place the bytes on the stack prior to invoking this instruction and place the count
	 * in a register that will be passed to this instruction.
	 */
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

	/* Add a string to the symbol table.
	 * Inputs: Two registers - 1) Destination, 2) String
	 * Takes a string object held in the second register, and places it in the symbol table.
	 * Places the identifier returned by that action into the first register.
	 */
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
