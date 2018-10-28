add_library(${MYGDB}_cli STATIC
    cli-parser.c
)
add_dependencies(${MYGDB}_cli ${MYGDB}_command)
add_dependencies(${MYGDB}_cli ${MYGDB}_misc)
target_link_libraries(${MYGDB}_cli PRIVATE ${MYGDB}_command)
target_link_libraries(${MYGDB}_cli PRIVATE ${MYGDB}_misc)
#target_compile_options(${MYGDB}_memory PRIVATE "-test")
target_include_directories(${MYGDB}_cli PUBLIC ../../${INCLUDE_DIRS})
