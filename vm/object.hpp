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
	class Message;
	class Mailbox;

	typedef std::map<std::string, Object*> SlotTable;

	class SlotExistsError
	{
	private:
		std::string string;
		Object*     offender;

	public:
		SlotExistsError(std::string s, Object* obj) : string(s), offender(obj) {}
		const std::string& message() const { return string; }
	};

	class Object : public GCObject
	{
	private:
		// First three fields are for the GC.
		Object*              next;
		Object*              prev;
		unsigned int         colour:2;

		// The slot table is our local container to hold slot definitions.
		SlotTable            slots;

		// Our traits list contains other composable objects of behaviour and state.
		std::vector<Object*> traits;

	public:
		Object();
		~Object();

		void add_slot(const std::string, Object*);
		void remove_slot(const std::string&);
		void add_trait(Object*);

		// Receives a message. Messages dispatched to this object should already be
		// in the queue. Therefore, this method pops an item off, and begins the
		// lookup process.
		void receive();

		SlotTable& slot_table() { return slots; }

		virtual void walk();

		// The mailbox is where messages come into. This allows us to decouple
		// message sending and message receiving.
		Mailbox*             mailbox;

	private:
		bool implements(const std::string&, Object*& obj);
	};
}

#endif /* !__CARIBOU__OBJECT_HPP__ */
