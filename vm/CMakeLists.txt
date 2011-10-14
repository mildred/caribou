set(SRCS
  "machine.cpp"
  "symtab.cpp"
  "output_writer.cpp"
  "input_reader.cpp"
  "gc.cpp"
  "object.cpp"
  "message.cpp"
)
add_library(caribou SHARED ${SRCS})
add_executable(vm "main.cpp")
add_dependencies(vm caribou)
target_link_libraries(vm caribou)
