cmake_minimum_required(VERSION 3.0)
project(lakes VERSION 0.1 LANGUAGES C)

configure_file(cmake_config.h.in ../project_config.h)

#targets
add_executable(testmacro tests/macrotest.c lib/macro.c include/macro.h)
add_executable(testmain tests/maintest.c lib/macro.c include/macro.h lib/writeout.c)
add_executable(tkgen lib/tokmap_generator.c)
add_executable(testnfa lib/nfa.c lib/writeout.c lib/input.c tests/nfatest.c lib/macro.c)

#tests
enable_testing()
add_test(NAME macrotest COMMAND testmacro)
add_test(NAME maintest COMMAND testmain)
add_test(NAME nfatest COMMAND testnfa)

#libraries dir
target_link_directories(testmacro PUBLIC "${CMAKE_INSTALL_PREFIX}/lib")
target_link_directories(testmain PUBLIC "${CMAKE_INSTALL_PREFIX}/lib")
target_link_directories(testnfa PUBLIC "${CMAKE_INSTALL_PREFIX}/lib")

#libraries
target_link_libraries(testmacro PUBLIC map)
target_link_libraries(testmain PUBLIC map)
target_link_libraries(testnfa PUBLIC map set esc)

#include dirs
target_include_directories(testmacro PUBLIC include "${CMAKE_INSTALL_PREFIX}/include")
target_include_directories(testmain PUBLIC include "${CMAKE_INSTALL_PREFIX}/include")
target_include_directories(testnfa PUBLIC include "${CMAKE_INSTALL_PREFIX}/include")

#compile defs
target_compile_definitions(testmacro PUBLIC DEBUG)
target_compile_definitions(testmain PUBLIC DEBUG)
target_compile_definitions(testnfa PUBLIC DEBUG)