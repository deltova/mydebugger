add_library(${MYGDB}_file_cacher STATIC
    file_cacher.cc
)

target_include_directories(${MYGDB}_file_cacher PUBLIC ../../${INCLUDE_DIRS})
