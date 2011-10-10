set(SRCS
  "machine.cpp"
  "symtab.cpp"
  "output_writer.cpp"
  "input_reader.cpp"
  "gc.cpp"
)
add_library(caribou SHARED ${SRCS})
add_library(caribou-static ${SRCS})
add_executable(vm "main.cpp" ${SRCS})
#add_dependencies(vm caribou-static)
#target_link_libraries(vm caribou-static)
