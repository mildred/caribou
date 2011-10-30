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

#include <string>
#include <map>
#include <vector>
#include "object.hpp"
#include "mailbox.hpp"

namespace Caribou
{
	extern GarbageCollector* collector;

	Object::Object() : mailbox(new Mailbox()), slots(), traits()
	{
	}

	Object::~Object()
	{
		delete mailbox;
	}

	void Object::add_slot(const std::string name, Object* value)
	{
		slots.insert(std::pair<std::string, Object*>(name, value));
	}

	void Object::remove_slot(const std::string& name)
	{
		slots.erase(name);
	}

	void Object::add_trait(Object* trait)
	{
		for(auto s : trait->slot_keys())
		{
			Object* result;

			if(implements(s, result))
				throw SlotExistsError("Conflict: Slot '" + s + "' found on an existing trait.", result);
		}

		traits.push_back(trait);
	}

	void Object::receive()
	{
		Message msg;
		if(mailbox->receive(msg))
		{
		}
	}

	void Object::walk()
	{
		for(auto v : slots)
			collector->shade(v.second);

		for(auto t : traits)
			collector->shade(t);
	}

	// We don't want any conflicts. Returns true if we already implement name.
	bool Object::implements(const std::string& name, Object*& obj)
	{
		SlotTable::iterator it;

		for(auto t : traits)
		{
			// When we find that one of our traits already implements a given slot,
			// we should through an exception.
			SlotTable& st = t->slot_table();
			it = st.find(name);
			if(it != st.end())
			{
				obj = t;
				return true;
			}
		}

		it = slots.find(name);
		if(it != slots.end())
			return true;

		return false;
	}
}
