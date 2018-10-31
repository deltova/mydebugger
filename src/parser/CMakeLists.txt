add_library(${MYGDB}_parser STATIC
    parser.cc
)
target_include_directories(${MYGDB}_parser PUBLIC ../../${INCLUDE_DIRS})
