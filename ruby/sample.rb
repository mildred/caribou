require 'rubygems'
require 'bindata'

class Header < BinData::Record
  endian :little
  string :name, :length => 8
  uint16le :year
  uint16le :release_id
  # Additional header info
  uint32le :remaining_size
  array :data, :type => :uint8, :initial_length => :remaining_size
end

class SimpleInstruction < BinData::Record
  endian :little
  uint8 :opcode
end

class OperandInstruction < SimpleInstruction
  uint64le :operand
end

f = File.open("test.data", "wb")
header = Header.new
header.name = "Caribou!"
header.year = 2011
header.release_id = 1
header.remaining_size = 0
header.write(f)

def instruction(opcode, operand=nil)
  if opcode == 1
    instr = OperandInstruction.new
    instr.operand = operand
  else
    instr = SimpleInstruction.new
  end
  instr.opcode = opcode
  instr
end

instruction(1, 5).write(f)
instruction(1, 2).write(f)
instruction(8).write(f)
instruction(5).write(f)
instruction(8).write(f)
