Instructions
============

The complete list of instruction and their bit representation is defined in
the file `instructions.hpp`. The currently defined instructions are:

    0  NOOP
    1  HALT
    2  PUSHA
    3  PUSHB
    4  POP
    5  DUP
    6  SWAP
    7  ROTATE
    8  SAVE_STACK
    9  RESTORE_STACK
    10  ADD
    11 SUB
    12 MUL
    13 DIV
    14 MOD
    15 POW
    16 NOT
    17 EQ
    18 LT
    19 GT
    20 JMP
    21 JT
    22 ADD_SYMBOL
    23 FIND_SYMBOL
    24 MAKE_ARRAY
    25 MAKE_STRING
    26 SEND
    27 RET
    28..63 are reserved for new instructions, unspecified behaviour.

 -  **NOOP**: `( -- )` Does nothing.

 -  **HALT**: `( -- )` Stops the machine from processing any more bytecodes.

 -  **PUSHA**: `( -- a )` Consumes an opcode, pushing its value onto the data stack. The size of this operand is the native pointer size of your platform.

 -  **PUSHB**: `( -- a )` Consumes an opcode, pushing its value onto the data stack. The size of this operand is the size of one byte on your platform.

 -  **POP**: `( a -- )` Pop an argument from the data stack

 -  **DUP**: `( a -- a a )` Duplicates the last element on the data stack

 -  **SWAP**: `( a b -- b a )` Swap the last two elements on the data stack

 -  **ROTATE**: `( a b c 3 -- c b a )` Pops a count off the stack, and then pops that many items off the stack. Then pushes those items onto the stack such that that subset of data is now reversed in position. i.e., 1,2,3,4 after rotate of 4 items we will see 4,3,2,1.

 -  **SAVE_STACK**: `( -- c )` Creates a continuation from VM memory and push a pointer to it on the data stack

 -  **RESTORE_STACK**: `( c -- )` Pop a continuation pointer from the data stack and restore the stacks from this continuation.

 -  **ADD**: `( a b -- c )` Pop two numbers off the stack, adds them together and pushes the result onto the stack.

 -  **SUB**: `( a b -- c )` Pop two numbers off the stack, subtracts the first from the second, and pushes the result onto the stack.

 -  **MUL**: `( a b -- c )` Pops two numbers off the stack, multiplies them together and pushes the result onto the stack.

 -  **DIV**: `( a b -- c )` Pops two numbers off the stack, divides the first by the second, and pushes the result onto the stack.

 -  **MOD**: `( a b -- c )` Pops two numbers off the stack, performs a modulus, and pushes the result onto the stack.

 -  **POW**: `( a b -- c )` Pops two numbers off the stack, calculates the exponent, and pushes the result onto the stack.

 -  **NOT**: `( a -- b )` Pops one number off the stack, performs a bitwise not and pushes the result onto the stack.

 -  **EQ**: `( a b -- c )` Pops two items off the stack. Compares the two values and pushes a boolean result onto the stack.

 -  **LT**: `( a b -- c )` Pops `a` off the stack and `b` off the stack. Performs an `a < b` and pushes a boolean result onto the stack.

 -  **GT**: `( a b -- c )` Pops `a` off the stack and `b` off the stack. Performs an `a > b` and pushes a boolean result onto the stack.

 -  **JMP**: `( a -- )` Unconditional jump. Pops an address to jump to off of the data stack. Jumps to the specified address.

 -  **JT**: `( c a -- )` If `c` is `true`, jump to address `a`.

 -  **ADD_SYMBOL**: `( a -- i )` Insert a symbol in the symbol table and push on the data stack its identifier.

 -  **FIND_SYMBOL**: `( i -- a )` Lookup a symbol in the symbol table given its identifier, pushing the symbol onto the stack if it was found. If it is not found, pushes nil onto the stack.

 -  **MAKE_ARRAY**: `( a b c 3 -- d )` Create an array. Pop a count of items off the stack, use that count to pop N items off the stack in addition, adding to the array in order that they are popped off.

 -  **MAKE_STRING**: `( a b c 3 -- d )` Creates a string. Pop a count of items off the stack, use that count to pop N items off the stack in addition, adding them to the string. The string is formed in reverse, and pushed onto the stack when completely constructed.

 -  **SEND**: `( a b c -- d )` Pop three elements from the data stack. The context of the caller, the object to receive the message, and the message itself. Creates an activation record with the address of our locals and ip, and pushes it onto the return stack. Jumps to the method. Returns whatever was returned via the `ret` instruction in the method we called.

 -  **RET**: `( a -- )` Pop the last context from the return stack. Rerstore the instruction pointer from it and push on the previous context's data stack the return value.

