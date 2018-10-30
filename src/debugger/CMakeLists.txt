add_library(${MYGDB}_debugger STATIC
    debugger.cc
)
target_include_directories(${MYGDB}_debugger PUBLIC ../../${INCLUDE_DIRS})
