Instructions
============

The complete list of instruction and their bit representation is defined in
the file `instructions.hpp`. The currently defined instructions are:

    0  NOOP
    1  PUSH
    2  POP
    3  PUSHIP
    4  POPIP
    5  DUP
    6  SWAP
    7  ROT3
    8  ADD
    9  LSHIFT
    10 RSHIFT
    11 AND
    12 OR
    13 XOR
    14 NOT
    15 SAVE_STACK
    16 RESTORE_STACK
    17 LOAD
    18 STORE
    19 ADD_SYMBOL
    20 FIND_SYMBOL
    21 JZ
    22 SEND
    23..63 are reserved for new instructions, unspecified behaviour.

 -  **NOOP**: Does nothing

 -  **PUSH**: Requires and argument and push it to the data stack

 -  **POP**: Pop an argument from the data stack

 -  **PUSHIP**: Creates a new activation record on VM memory with the current
    instruction pointer and the locals index poped from the data stack. Push
    this activation record in the return stack.

 -  **POPIP**: Pop the last activation recordfrom the return stack. Rerstore the
    instruction pointer from it and push on the data stack the locals index.

 -  **DUP**: Duplicates the last element on the data stack

 -  **SWAP**: Swap the last two elements on the data stack

 -  **ROT3**: Swap the second and third elements on the data stack.

 -  **ADD**: Pop two elements on the data stack and push the result of their
    addition.

 -  **LSHIFT**: Pop two elements from the data stack and push the top element
    left shifted by the number of bits specified by the second element.

 -  **RSHIFT**: Pop two elements from the data stack and push the top element
    right shifted by the number of bits specified by the second element.

 -  **AND**: Pop two elements from the data stack, perform a bitwise and and
    push it.

 -  **OR**: Pop two elements from the data stack, perform a bitwise or and push
    it.

 -  **XOR**: Pop two elements from the data stack, perform a bitwise xor and
    push it.

 -  **NOT**: Pop two elements from the data stack, perform a bitwise not and
    push it.

 -  **SAVE_STACK**: Creates a continuation from VM memory and push a pointer to
    it on the data stack

 -  **RESTORE_STACK**: Pop a continuation pointer from the data stack and
    restore the stacks from this continuation.

 -  **LOAD**: Pop a VM pointer from the data stack and push instead the value
    located at the specified address in the VM memory.

 -  **STORE**: Pop a VM pointer and then a data word from the data stack and
    store in the VM memory, at address specified by the pointer, the data word.

 -  **ADD_SYMBOL**: not specified. Insert a symbol in the symbol table and push
    on the data stack its identifier. Skip the next 7 bytes in instruction
    memory.

 -  **FIND_SYMBOL**: not specified. Lookup a symbol in the symbol table and if
    the symbol is found, push its identifier on the data stack. If it is not
    found, pushes nil onto the stack.

 -  **JZ**: Jumo on zero. Pop an instruction pointer and a condition from
    the data stack. If the condition is zero, jusp to the specified address.

 -  **SEND**: Pop three elements from the data stack. The context of the
    caller, the object to receive the message, and the message itself. Creates
    an activation record with the address of our locals and ip, and pushes
    it onto the return stack. Jumps to the method.

