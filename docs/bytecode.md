# Bytecode Format

The Caribou bytecode is a fairly simple, but not trivial one. It is composed of a header, followed by a constant pool, followed by a sequence of instructions.

## Header

0        32        40          72            104
|--------|---------|-----------|-------------|
| "CRBU" | version | pool-size | custom-size |
|--------|---------|-----------|-------------|

The header is composed of a 4 byte array, representing the string "CRBU" followed by an 8-bit version and a 32-bit pool size. Immediately following is 32-bits of storage for a custom size, which will denote the size of any custom header. The constants pool should immediately follow the header, followed immediately by the custom header.

## Instruction Formats

There are three different instruction formats in Caribou. They are defined below.

Opcodes are always 8-bits, and always start the instruction.

### ABC

0        7       15      23      31
|--------|-------|-------|-------|
| opcode | reg-a | reg-b | reg-c |
|--------|-------|-------|-------|

Here we have three operands to the opcode, all 8-bit indexes representing registers in the system. Valid values here range from 0 to 255.

### ABmore

0        7       15          31
|--------|-------|-----------|
| opcode | reg-a | immediate |
|--------|-------|-----------|

In this case, we have one register followed by a 16-bit immediate value which will be used by the instruction to load an object into the register.

### Immed

0        8           31
|--------|-----------|
| opcode | immediate |
|--------|-----------|

This instruction format generally only makes sense for unconditional jumping.

