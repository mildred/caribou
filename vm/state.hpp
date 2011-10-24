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

#ifndef __CARIBOU__STATE_HPP__
#define __CARIBOU__STATE_HPP__

#include <string>
#include "tinythread.h"
#include "state_machine.hpp"

namespace Caribou
{
	class StateException
	{
	public:
		StateException(const std::string& msg) : message(msg) {}
		~StateException() {}

		std::string get_message() const { return message; }

	private:
		std::string message;
	};

	class State
	{
	public:
		State() : mutex() {}
		virtual ~State() {}

		virtual void transition(StateMachine* machine, State* other)
		{
			tthread::lock_guard<tthread::mutex> lk(mutex);
			will_leave(machine);
			machine->set_current(other);
			did_leave(machine);
			other->will_enter(machine);
			other->did_enter(machine);
		}

		virtual void will_enter(StateMachine* machine) {}
		virtual void did_enter(StateMachine* machine) {}
		virtual void will_leave(StateMachine* machine) {}
		virtual void did_leave(StateMachine* machine) {}

	private:
		tthread::mutex mutex;
	};
}

#endif /* !__CARIBOU__STATE_HPP__ */
