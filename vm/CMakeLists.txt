set(CMAKE_CXX_FLAGS "-g -std=c++0x")
set(SRCS
  "tinythread.cpp"
  "register.cpp"
  "machine.cpp"
  "symtab.cpp"
  "state_machine.cpp"
  "output_writer.cpp"
  "input_reader.cpp"
  "gc.cpp"
  "object.cpp"
  "continuation.cpp"
  "message.cpp"
  "array.cpp"
  "integer.cpp"
  "string.cpp"
  "nil.cpp"
  "compiled_method.cpp"
  "object_space.cpp"
)
add_library(caribou SHARED ${SRCS})
add_executable(vm "main.cpp")
add_dependencies(vm caribou)
target_link_libraries(vm caribou)
