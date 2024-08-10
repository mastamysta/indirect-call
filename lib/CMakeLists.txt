add_library(ind SHARED ind.cpp)

target_include_directories(ind PUBLIC ../include)
target_include_directories(ind PUBLIC .)
