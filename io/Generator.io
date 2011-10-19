BigNum asSequence := method(
	segments := list
	source := self
	while(source != 0,
		segments append(source % 256)
		source = source / 256
		)
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

	binaryHeader := method("Caribou!" ..(version asCharacter))
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
)
