cmake_minimum_required(VERSION 3.0)
project(lakes VERSION 0.1 LANGUAGES C)

configure_file(cmake_config.h.in ../project_config.h)

#targets
add_executable(macro tests/macrotest.c lib/macro.c include/macro.h)
add_executable(main tests/maintest.c lib/macro.c include/macro.h lib/writeout.c)
add_executable(tkgen lib/tokmap_generator.c)
add_executable(nfa lib/nfa.c lib/writeout.c lib/input.c tests/nfatest.c lib/macro.c)
add_executable(interpreter tests/interpretertest.c tests/misc.c lib/macro.c lib/nfa.c lib/nfa_interpreter.c)
add_executable(dfa tests/dfatest.c lib/dfa.c lib/writeout.c tests/misc.c lib/macro.c lib/nfa.c lib/nfa_interpreter.c)

#tests
enable_testing()
add_test(NAME macrotest COMMAND macro)
add_test(NAME maintest COMMAND main)
add_test(NAME nfatest COMMAND nfa "${CMAKE_SOURCE_DIR}/tests/demo.lex")
#add_test(NAME nfafailtest COMMAND nfa "${CMAKE_SOURCE_DIR}/tests/demo2.lex")
add_test(NAME interpretertest COMMAND interpreter)
add_test(NAME dfatest COMMAND dfa)

#test properties
#set_tests_properties(nfafailtest PROPERTIES WILL_FAIL TRUE)

#libraries dir
target_link_directories(macro PUBLIC "${CMAKE_INSTALL_PREFIX}/lib")
target_link_directories(main PUBLIC "${CMAKE_INSTALL_PREFIX}/lib")
target_link_directories(nfa PUBLIC "${CMAKE_INSTALL_PREFIX}/lib")
target_link_directories(interpreter PUBLIC "${CMAKE_INSTALL_PREFIX}/lib")
target_link_directories(dfa PUBLIC "${CMAKE_INSTALL_PREFIX}/lib")

#libraries
target_link_libraries(macro PUBLIC map)
target_link_libraries(main PUBLIC map)
target_link_libraries(nfa PUBLIC map set esc)
target_link_libraries(interpreter PUBLIC map set esc)
target_link_libraries(dfa PUBLIC map set esc)

#include dirs
target_include_directories(macro PUBLIC include "${CMAKE_INSTALL_PREFIX}/include")
target_include_directories(main PUBLIC include "${CMAKE_INSTALL_PREFIX}/include")
target_include_directories(nfa PUBLIC include "${CMAKE_INSTALL_PREFIX}/include")
target_include_directories(interpreter PUBLIC include "${CMAKE_INSTALL_PREFIX}/include")
target_include_directories(dfa PUBLIC include "${CMAKE_INSTALL_PREFIX}/include")

#compile defs
target_compile_definitions(macro PUBLIC DEBUG)
target_compile_definitions(main PUBLIC DEBUG)
target_compile_definitions(nfa PUBLIC DEBUG)
target_compile_definitions(interpreter PUBLIC DEBUG)
# target_compile_definitions(dfa PUBLIC DEBUG)