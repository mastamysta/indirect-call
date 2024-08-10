add_executable(direct main.cpp)
target_include_directories(direct PUBLIC ../include)

add_executable(indirect main.cpp)
target_compile_definitions(indirect PUBLIC INDIRECT)
target_link_libraries(indirect ind)
