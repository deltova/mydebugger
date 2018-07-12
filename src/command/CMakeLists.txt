add_library(${MYGDB}_command STATIC
    cmd_handler.c
)
add_dependencies(${MYGDB}_command ${MYGDB}_elf)
target_link_libraries(${MYGDB}_command PRIVATE ${MYGDB}_elf)
#add_dependencies(${TC_PROJECT}_ast ${TC_PROJECT}_parse)
#arget_compile_options(${MYGDB}_memory PRIVATE "-tadaronne")
target_include_directories(${MYGDB}_command PUBLIC ../../${INCLUDE_DIRS})
