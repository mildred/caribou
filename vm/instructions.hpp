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

#ifndef __CARIBOU__INSTRUCTIONS_HPP__
#define __CARIBOU__INSTRUCTIONS_HPP__

namespace Caribou
{
	struct Instructions
	{
		enum
		{
			NOOP = 0,
			MOVE,
			LOADI,

			// Stack operations.
			PUSH = 0x10,
			POP,
			SWAP,
			ROTATE,
			DUP,

			// Numerical operations.
			//  Each instruction takes three registers.
			ADD = 0x20,
			SUB,
			MUL,
			DIV,
			MOD,
			POW,
			NOT,

			// Comparison operations.
			//  These are intended to be paired with JMP operations. On a successful
			//  comparison, each will bump the IP by one (to the JMP). On failure,
			//  each will bump the IP by 2 (to the instruction just past the JMP).
			//  Each instruction takes three registers.
			EQ = 0x30,
			LT,
			LTE,
			GT,
			GTE,

			// Control flow operations.
			HALT = 0x40,
			SEND,
			RET,
			JMP,
			SAVE,
			RESTORE,

			// Primitive object operations.
			ADDSYM = 0x50,
			FINDSYM,
			ARRAY,
			STRING
		};
	};
}

#endif /* !__CARIBOU__INSTRUCTIONS_HPP__ */
