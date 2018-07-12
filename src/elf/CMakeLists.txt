add_library(${MYGDB}_elf STATIC
    info_elf.c
)
target_link_libraries(${MYGDB}_elf PUBLIC bfd)
#add_dependencies(${TC_PROJECT}_ast ${TC_PROJECT}_parse)
#target_compile_options(${MYGDB}_memory PRIVATE "-tadaronne")
target_include_directories(${MYGDB}_elf PUBLIC ../../${INCLUDE_DIRS})
