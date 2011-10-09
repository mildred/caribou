set(SRCS
  "machine.cpp"
  "symtab.cpp"
  "output_writer.cpp"
  "input_reader.cpp"
  "gc.cpp"
)
add_library(caribou SHARED ${SRCS})
