Instructions
============

The complete list of instruction and their bit representation is defined in
the file `instructions.hpp`. The currently defined instructions are:

    0  NOOP
    1  PUSH
    2  POP
    3  RET
    4  DUP
    5  SWAP
    6  ROT3
    7  SAVE_STACK
    8  RESTORE_STACK
    9  ADD_SYMBOL
    10 FIND_SYMBOL
    11 JZ
    12 MAKE_ARRAY
    13 SEND
    14..63 are reserved for new instructions, unspecified behaviour.

 -  **NOOP**: Does nothing

 -  **PUSH**: Requires and argument and push it to the data stack

 -  **POP**: Pop an argument from the data stack

 -  **RET**: Pop the last context from the return stack. Rerstore the
    instruction pointer from it and push on the previous context's data stack
    the return value.

 -  **DUP**: Duplicates the last element on the data stack

 -  **SWAP**: Swap the last two elements on the data stack

 -  **ROT3**: Swap the second and third elements on the data stack.

 -  **SAVE_STACK**: Creates a continuation from VM memory and push a pointer to
    it on the data stack

 -  **RESTORE_STACK**: Pop a continuation pointer from the data stack and
    restore the stacks from this continuation.

 -  **ADD_SYMBOL**: not specified. Insert a symbol in the symbol table and push
    on the data stack its identifier. Skip the next 7 bytes in instruction
    memory.

 -  **FIND_SYMBOL**: not specified. Lookup a symbol in the symbol table and if
    the symbol is found, push its identifier on the data stack. If it is not
    found, pushes nil onto the stack.

 -  **JZ**: Jump on zero. Pop an instruction pointer and a condition from
    the data stack. If the condition is zero, jusp to the specified address.

 -  **MAKE_ARRAY**: Create an array. Pop a count of items off the stack, use that
    count to pop N items off the stack in addition, adding to the array in order
    that they are popped off.

 -  **SEND**: Pop three elements from the data stack. The context of the
    caller, the object to receive the message, and the message itself. Creates
    an activation record with the address of our locals and ip, and pushes
    it onto the return stack. Jumps to the method.

