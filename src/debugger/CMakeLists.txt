add_library(${MYGDB}_debugger STATIC
    debugger.cc
    debugger-dwarf.cc
)
target_include_directories(${MYGDB}_debugger PUBLIC ../../${INCLUDE_DIRS})
