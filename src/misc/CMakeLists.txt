add_library(${MYGDB}_misc STATIC
    register.cc
)
#add_dependencies(${TC_PROJECT}_ast ${TC_PROJECT}_parse)
#target_compile_options(${MYGDB}_memory PRIVATE "-tadaronne")
target_include_directories(${MYGDB}_misc PUBLIC ../../${INCLUDE_DIRS})
