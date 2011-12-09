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

#ifndef __CARIBOU__TRACE_RECORDER_HPP__
#define __CARIBOU__TRACE_RECORDER_HPP__

#include <stdint.h>
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"

namespace Caribou
{
	using namespace llvm;

	typedef std::pair<uint8_t, size_t> TracePair;

	class TraceRecorder
	{
		struct TraceNode
		{
			uint8_t    opcode;
			size_t     ip;
			size_t     depth;

			TraceNode(uint8_t o, size_t p, size_t d) : opcode(o), ip(p), depth(d), left(nullptr), right(nullptr) {}

			TraceNode* left;
			TraceNode* right;
		};

		enum TraceMode
		{
			kTraceModeProfiling = 0,
			kTraceModeRecording,
			kTraceModeExpressionBegin,
			kTraceModeExpression
		};

		TraceMode        mode;

		TraceNode*       expression_root;
		TraceNode*       expression_leaf;
		uint8_t*         iteration_count;

		TracePair*       trace_begin;
		TracePair*       trace_end;
		TracePair*       trace_tail;

		DenseMap<size_t, TraceNode*> trace_map;
		DenseSet<size_t> blacklist;

		Module*          module;
		BasicBlock*      header;
		PHINode*         header_phi;
		Value*           data;
		ExecutionEngine* engine;

	public:
		TraceRecorder();
		~TraceRecorder();
	};
}

#endif /* !__CARIBOU__TRACE_RECORDER_HPP__ */
