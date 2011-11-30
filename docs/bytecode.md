# Bytecode Format

The Caribou bytecode is a fairly simple, but not trivial one. It is composed of a header, followed by a constant pool, followed by a sequence of instructions.

## Header

0        32        40          72            104
|--------|---------|-----------|-------------|
| "CRBU" | version | pool-size | custom-size |
|--------|---------|-----------|-------------|

The header is composed of a 4 byte array, representing the string "CRBU" followed by an 8-bit version and a 32-bit pool size. Immediately following is 32-bits of storage for a custom size, which will denote the size of any custom header. The constants pool should immediately follow the header, followed immediately by the custom header.

## Instruction Formats

Caribou doesn't use a rigid instruction format. Opcodes are always 8-bits, and always start the instruction. We offload instruction decoding responsibilities to the instructions themselves. They'll fetch an integer if they want it, a register if they want it. Additionally, they'll fetch them in whatever order they feel like. That said, our instructions will be in one of the three forms below.

### ABC

0        7       15      23      31
|--------|-------|-------|-------|
| opcode | reg-a | reg-b | reg-c |
|--------|-------|-------|-------|

Here we have three operands to the opcode, all 8-bit indexes representing registers in the system. Valid values here range from 0 to 255.

### ABmore

0        7       15          48/80
|--------|-------|-----------|
| opcode | reg-a | immediate |
|--------|-------|-----------|

In this case, we have one register followed by a pointer sized immediate value which will be used by the instruction to load an object into the register.

### Immed

0        8           40/72
|--------|-----------|
| opcode | immediate |
|--------|-----------|

This instruction format is generally only used for stack operations like push, rotate and for unconditional jumping.
