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
#include <iostream>
#include <cstring>
#include <stdint.h>
#include <stdlib.h>
#include "gc.hpp"
#include "machine.hpp"

namespace Caribou
{
	GarbageCollector::GarbageCollector(Machine* m) : machine(m)
	{
		blacks = new GCMarker(kGCColourBlack);
		greys  = new GCMarker(kGCColourGrey);
		whites = new GCMarker(kGCColourWhite);
		freed  = new GCMarker(kGCColourFreed);

		whites->loop();
		greys->resnap_after(whites);
		blacks->resnap_after(greys);
		freed->resnap_after(blacks);

		allocated = 0;
	}

	GarbageCollector::~GarbageCollector()
	{
		delete whites;
		delete greys;
		delete blacks;
		delete freed;
	}

	GCMarker* GarbageCollector::new_marker()
	{
		GCMarker* marker = freed->next;

		if(marker->colour != freed->colour)
			marker = new GCMarker();

		allocated++;
		add_value(marker);

		return marker;
	}

	void GarbageCollector::add_value(GCMarker* value)
	{
		value->resnap_after(whites);
		marks_queued += marks_alloc;
	}

	void GarbageCollector::scan_greys(size_t max)
	{
		GCMarker* v = greys->next;
		GCMarker* new_next;
		unsigned int c = greys->colour;

		while(v->colour == c)
		{
			new_next = v->next;

			// TODO: Scan roots and make the node black

			if(--max == 0)
				break;

			v = new_next;
		}

		/*
		std::vector<ActivationRecord*>& rstack = machine->get_return_stack().get_store();
		std::vector<ActivationRecord*>::iterator it;
		Stack<ActivationRecord*>        new_rstack;

		for(it = rstack.begin(); it < rstack.end(); it++);
			//new_rstack.push((ActivationRecord*)copy((GCObject*)(*it)));

		machine->set_return_stack(&new_rstack);
		*/
	}

	void GarbageCollector::sweep()
	{
		while(!greys->is_empty())
			scan_greys();

		GCMarker* tmp = blacks;
		blacks = whites;
		whites = tmp;
	}
}
