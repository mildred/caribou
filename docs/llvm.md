# Draft of LLVM Integration code, necessary primitives

## Instructions

### puship

1. Get the instruction pointer
2. Cast the value to a long integer
3. Store the value on the return stack

### push (integer)

1. Get the data stack pointer
2. Cast the value to a long integer
3. Store the value on the data stack

### push (string)

1. Get the length of the string
2. Create a type for the string constant (include NUL padding)
3. Create internal linkage global variable to hold the constant

This is a simple algorithm, we'll use something more complex when we include the trie support.

### pop (integer)

1. Get the data stack pointer
2. Load the value
3. Decrement the data stack pointer

### pop (string)

1. Get the data stack pointer
2. Load the value
3. Cast the value to an SByteTy
4. Decrement the data stack pointer

### ret

### dup

### swap

### noop

### load

### store

### add_literal

### find_proto

### create_object

### send
