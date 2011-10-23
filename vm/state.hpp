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

namespace Caribou
{
	class StateMachine;

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
		State() : name("Unknown") {}
		State(const std::string& str) : name(str) {}
		virtual ~State() {}

		virtual void enter(StateMachine* machine)
		{
			throw(StateException("Already in the '" + name + "' state."));
		}

		virtual void leave(StateMachine* machine)
		{
			throw(StateException("Already in the '" + name + "' state."));
		}

	protected:
		std::string name;
	};

	class InitialState : public State
	{
	public:
		InitialState() { name = "Initial"; }

		void enter(StateMachine* machine)
		{
			throw(StateException("The Initial State cannot be entered."));
		}

		void leave(StateMachine* machine)
		{
			throw(StateException("Cannot leave the Initial State this way. Use set_current() on StateMachine directly for your first state."));
		}
	};
}

#endif /* !__CARIBOU__STATE_HPP__ */
