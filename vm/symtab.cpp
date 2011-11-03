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

#include <vector>
#include <string>
#include <map>
#include <utility>
#include "symtab.hpp"

namespace Caribou
{
	size_t Symtab::add(std::string str)
	{
		mapping.push_back(str);
		symbols.insert(std::pair<std::string, size_t>(str, mapping.size() - 1));
		return symbols.size() - 1;
	}

	size_t Symtab::lookup(const std::string& str)
	{
		SymMap::iterator it = symbols.find(str);
		if(it == symbols.end())
			return SYMTAB_NOT_FOUND;
		return it->second;
	}

	size_t Symtab::lookup_or_add(const std::string& str)
	{
		size_t idx = lookup(str);
		if(idx == SYMTAB_NOT_FOUND)
			idx = add(str);
		return idx;
	}

	std::string Symtab::lookup(const uintptr_t idx)
	{
		return mapping.at(idx);
	}

	size_t Symtab::size()
	{
		return symbols.size();
	}
}
