# Bytecode Format

The Caribou bytecode is a fairly simple, but not trivial one. Tentatively, it looks like this:

```
0             5             8         N (max = 75)
|-------------|-------------|---------|
| instruction | Opcode size | Operand |
|-------------|-------------|---------|
```

1. The instruction bits occupy the first 5 bytes of an instruction sequence. The instructions themselves are documented in the source file `instructions.hpp`.
2. If the following three bits are non-zero, they are used as the size (in bytes) of an operand to consume up to a maximum of 8 bytes. Most instructions do not need an operand, and as such, this bit is almost always zero. See the instructions documentation for more information on which instructions require an operand.
3. The Operand consumes N bytes, where N is the operand size given in the high 3 bits of the previous byte. It may be any value that will fit into at most 64-bits, and the instruction can demarshall.
