BigNum asSequence := method(
	segments := list
	source := self
	while(source != 0,
		segments append(source % 256)
		source = source / 256
		)
	segments map(asNumber asCharacter) reduce(..)
	)
