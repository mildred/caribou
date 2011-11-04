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
#include "machine.hpp"
#include "integer.hpp"

namespace Caribou
{
	extern GarbageCollector* collector;

	Object::Object() : mailbox(new Mailbox()), slots(), traits()
	{
		collector->add_value(dynamic_cast<GCMarker*>(this));
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
		// DISCUSS: Should this be greedy or non-greedy? I.e., currently, we perform this algorithm:
		//          1. Look in our current slot table for a name
		//          2. Remove it.
		//
		// What if the slot you want to remove is in a trait? Now we can't just go removing it from the
		// trait, what about other objects that depend on it? But, what we can do, is copy that trait,
		// install that copy in place of the original in this objects trait list only.
		//
		// Should we implement it this way?
		slots.erase(name);
	}

	void Object::add_trait(Object* trait)
	{
		for(auto p : trait->slot_table())
		{
			Object* result;

			if(implements(p.first, result))
				throw SlotExistsError("Conflict: Slot '" + p.first + "' found on an existing trait.", result);
		}

		traits.push_back(trait);
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

	void Object::receive(Context* ctx)
	{
		Message msg;
		if(mailbox->receive(msg))
		{
			Object* slot_context;
			lookup(msg.get_name(), slot_context);
		}
	}

	bool Object::local_lookup(SlotTable& st, const std::string str, Object*& value, Object*& slot_context)
	{
		SlotTable::iterator it = st.find(str);
		if(it != st.end())
		{
			slot_context = this;
			value = it->second;
			return true;
		}
		return false;
	}

	Object* Object::lookup(const std::string str, Object*& slot_context)
	{
		Object* value = NULL;

		if(local_lookup(slots, str, value, slot_context))
			return value;

		for(auto t : traits)
		{
			if(local_lookup(t->slot_table(), str, value, slot_context))
				return value;
		}

		return NULL;
	}

	void Object::walk()
	{
		for(auto v : slots)
			collector->shade(v.second);

		for(auto t : traits)
			collector->shade(t);
	}

	const std::string Object::object_name()
	{
		return "Object";
	}

	void Object::bytecode(Machine* m)
	{
		Context* ctx = m->get_current_context();
		m->find_symbol(ctx);
		m->push(ctx, new String("clone"));
		m->send(ctx);
	}
}
