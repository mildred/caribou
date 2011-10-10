# Bytecode Format

The Caribou bytecode is a fairly simple, but not trivial one. Tentatively, it looks like this:

```
0          1             6                  7
|----------|-------------|------------------|
| reserved | instruction | Consume next op? |
|----------|-------------|------------------|
```

1. The instruction bits represent one of instructions found in other
   documentation and defined in `instructions.hpp`.
2. If the "Consume next op?" bit is set, we consume the next 64-bits as an
   argument to the instruction. Only one instruction requires an argument, and
   that is push.
3. Reserved bits may be used

