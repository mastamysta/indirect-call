add_executable(direct main.cpp)
target_include_directories(direct PUBLIC ../include)

add_executable(indirect main.cpp)
target_compile_definitions(indirect PUBLIC INDIRECT)
target_link_libraries(indirect ind)
#target_compile_options(indirect PUBLIC -fno-plt)

add_executable(pindirect main.cpp)
target_compile_definitions(pindirect PUBLIC PINDIRECT)
target_include_directories(pindirect PUBLIC ../include)

add_executable(bad_branching bad_branching.cpp)
target_compile_definitions(bad_branching PUBLIC BAD)

add_executable(good_branching bad_branching.cpp)
target_compile_definitions(good_branching PUBLIC GOOD)
