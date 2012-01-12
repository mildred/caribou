BigNum asSequence := method(
  segments := list
  source := self
  while(source != 0,
    segments append(source % 256)
    source = source / 256
  )
  if(segments isEmpty, segments append(0))
  segments map(asNumber asCharacter) reduce(..)
)

Opcode := Object clone do(
  name ::= "opcode"
  number ::= 0
  operand ::= nil
    
  binary := method(
    buffer := Sequence clone
    operand isNil ifTrue(
      buffer = buffer ..((number << 1) asCharacter)
    ) ifFalse(
      buffer = buffer ..(Sequence clone atPut(0,1) bitwiseOr((number << 1) asCharacter))
      buffer = buffer ..(Sequence clone atPut(7,0) bitwiseOr(operand asSequence exclusiveSlice(0,8)))
    )
    buffer
  )
  textual := method(name .. if(operand isNil, "", ":" .. operand asString))
)

Generator := Object clone do(
  buffer := list
	
  version ::= 0
    
  init := method(buffer = list)

  binaryHeader := method("Caribou!" ..(version asCharacter) ..(Sequence clone atPut(3,0)))
  textualHeader := method("Caribou! Version:" ..(version asString))
    
  binary  := method(
    output := binaryHeader
    b := buffer map(binary) reduce(acc, cur, acc .. cur)
    b isNil ifFalse(output = output ..(b))
    output
  )
  textual := method(
    output := textualHeader
    b := buffer map(textual) reduce(acc, cur, acc .. "\n" .. cur)
    b isNil ifFalse(output = output ..("\n") ..(b))
    output
  )

  register := method(number, name, operand,
    buffer push(Opcode clone setName(name) setNumber(number) setOperand(operand))
    self
  )
    
  noop         := method(   register(0x00, "NOOP"))
  move         := method(   register(0x01, "MOVE"))
  loadi        := method(v, register(0x02, "LOADI", if(v isNil, BigNum with(0), v)))
  
  push         := method(v, register(0x10, "PUSH", if(v isNil, BigNum with(0), v)))
  pop          := method(   register(0x11, "POP"))
  swap         := method(   register(0x12, "SWAP"))
  rotate       := method(   register(0x13, "ROTATE"))
  dup          := method(   register(0x14, "DUP"))
  
  add          := method(   register(0x20, "ADD"))
  sub          := method(   register(0x21, "SUB"))
  mul          := method(   register(0x22, "MUL"))
  div          := method(   register(0x23, "DIV"))
  mod          := method(   register(0x24, "MOD"))
  pow          := method(   register(0x25, "POW"))
  not          := method(   register(0x26, "NOT"))

  eq           := method(   register(0x30, "EQ"))
  lt           := method(   register(0x31, "LT"))
  lte          := method(   register(0x32, "LTE"))
  gt           := method(   register(0x33, "GT"))
  gte          := method(   register(0x34, "GTE"))

  halt         := method(   register(0x40, "HALT"))
  send         := method(   register(0x41, "SEND"))
  ret          := method(   register(0x42, "RET"))
  jmp          := method(v, register(0x43, "JMP", v))
  save         := method(   register(0x44, "SAVE"))
  restore      := method(   register(0x45, "RESTORE"))

  addsym       := method(   register(0x50, "ADDSYM"))
  findsym      := method(   register(0x51, "FINDSYM"))
  array        := method(   register(0x52, "ARRAY"))
  string       := method(   register(0x53, "STRING"))
)
