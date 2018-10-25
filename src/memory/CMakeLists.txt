add_library(${MYGDB}_memory STATIC
    memory_mapping.cc
)
#add_dependencies(${TC_PROJECT}_ast ${TC_PROJECT}_parse)
#target_compile_options(${MYGDB}_memory PRIVATE "-tadaronne")
target_compile_options(${MYGDB}_memory PUBLIC "-lstdc++fs")
target_link_libraries(${MYGDB}_memory PUBLIC "-lstdc++fs")
target_include_directories(${MYGDB}_memory PUBLIC ../../${INCLUDE_DIRS})
