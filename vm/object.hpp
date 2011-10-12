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

#ifndef __CARIBOU__OBJECT_HPP__
#define __CARIBOU__OBJECT_HPP__

#include <map>
#include <string>
#include <vector>
#include "gc.hpp"

namespace Caribou
{
	class Object;

	typedef std::map<std::string, Object*> SlotTable;

	class Object : public GCObject
	{
	private:
		Object*              next;
		Object*              prev;
		SlotTable            slots;
		std::vector<Object*> traits;

	public:
		Object() : slots(), traits() { }

		void add_slot(const std::string, Object*);
		void remove_slot(const std::string&);
		void add_trait(Object*);

		virtual void walk();

	private:
		bool implements(const std::string&);
	};
}

#endif /* !__CARIBOU__OBJECT_HPP__ */
