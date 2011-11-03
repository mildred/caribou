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

  register := method(name, number, operand, buffer push(Opcode clone setName(name) setNumber(number) setOperand(operand));self)
    
  noop         := method(register("NOOP", 0))
  push         := method(v, register("PUSH", 1, if(v isNil, BigNum with(0), v)))
  pop          := method(register("POP", 2))
  ret          := method(register("RET", 3))
  dup          := method(register("DUP", 4))
  swap         := method(register("SWAP", 5))
  rot3         := method(register("ROT3", 6))
  jz           := method(register("JZ", 7))
  saveStack    := method(register("SAVE_STACK", 8))
  restoreStack := method(register("RESTORE_STACK", 9))
  addSymbol    := method(register("ADD_SYMBOL", 10))
  findSymbol   := method(register("FIND_SYMBOL", 11))
  makeArray    := method(register("MAKE_ARRAY", 12))
  send         := method(register("SEND", 13))
)
