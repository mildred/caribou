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

#ifndef __CARIBOU__SYMTAB_HPP__
#define __CARIBOU__SYMTAB_HPP__

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include "string.hpp"

// This is INT32_MAX instead of INTPTR_MAX due to ILP64 systems who define
// size_t to be 4 bytes instead of following the size of a pointer. One
// platform that pulls this bait and switch is 64-bit Windows.
#define SYMTAB_NOT_FOUND INT32_MAX

namespace Caribou
{
	typedef std::map<String*, size_t> SymMap;

	class Symtab
	{
	public:
		size_t add(String* str);

		size_t lookup(String* str);
		size_t lookup_or_add(String* str);
		String* lookup(const uintptr_t idx);

		size_t size();

	private:
		std::vector<String*> mapping;
		SymMap               symbols;
	};
}

#endif /* !__CARIBOU__SYMTAB_HPP__ */
