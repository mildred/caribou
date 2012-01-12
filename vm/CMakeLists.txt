find_package(LLVM)

set(CMAKE_CXX_FLAGS "-g -std=c++0x -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS")
set(SRCS
  "machine.cpp"
  "symtab.cpp"
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
  "object_space.cpp"
  "vmmethod.cpp"
  "trace_recorder.cpp"
)

include_directories(${LLVM_CFLAGS})

add_library(caribou SHARED ${SRCS})
target_link_libraries(caribou "${LLVM_LDFLAGS} ${LLVM_JIT_LIBS}")
add_executable(vm "main.cpp")
add_dependencies(vm caribou)
target_link_libraries(vm caribou)
