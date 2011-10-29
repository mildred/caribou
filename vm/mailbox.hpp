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

#ifndef __CARIBOU__MAILBOX_HPP__
#define __CARIBOU__MAILBOX_HPP__

#include "Message.hpp"

namespace Caribou
{
	// Mailbox is a lock-free queue safe for up to two concurrent threads manipulating
	// it so long as one is delivering and the other is receiving.
	class Mailbox
	{
	private:
		struct Node {
			Node(const Message& msg) : message(msg), next(nullptr) {}
			const Message& message;
			Node* next;
		};
		Node* first;
		Node* divider;
		Node* last;

		void trim_to(Node* upto)
		{
			while(first != upto)
			{
				Node* tmp = first;
				first = tmp->next;
				delete tmp;
			}
		}

	public:
		Mailbox()
		{
			Message msg = Message();
			first = divider = last = new Node(msg);
		}

		~Mailbox()
		{
			trim_to(nullptr);
		}

		void deliver(const Message& msg)
		{
			last->next = new Node(msg);
			trim_to(divider);
		}

		bool receive(Message& result)
		{
			if(divider != last)
			{
				result = divider->next->message;
				return true;
			}
			return false;
		}
	};
}

#endif /* !__CARIBOU__MAILBOX_HPP__ */
