CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)
SET(PACKAGE_NAME "MYGDB")
SET(MYGDB "mygdb")
SET(INCLUDE_DIRS "include")
set(CMAKE_CXX_STANDARD 17)
PROJECT(${MYGDB} CXX)

ADD_EXECUTABLE(${MYGDB} src/main.cc)

add_compile_options("-std=c++17" "-pedantic" "-Wall" "-Wextra")
target_link_libraries(${MYGDB} PUBLIC bfd)
target_link_libraries(${MYGDB} PUBLIC capstone)
target_link_libraries(${MYGDB} PUBLIC readline)
target_link_libraries(${MYGDB} PUBLIC dwarf++)
target_link_libraries(${MYGDB} PUBLIC elf++)

TARGET_INCLUDE_DIRECTORIES(${MYGDB} PUBLIC "include")
TARGET_INCLUDE_DIRECTORIES(${MYGDB} PUBLIC "src/parser")

ADD_SUBDIRECTORY(src/debugger)
ADD_SUBDIRECTORY(src/elf)
ADD_SUBDIRECTORY(src/memory)
ADD_SUBDIRECTORY(src/misc)
ADD_SUBDIRECTORY(src/file-cacher)

TARGET_LINK_LIBRARIES(${MYGDB} PUBLIC ${MYGDB}_debugger)
TARGET_LINK_LIBRARIES(${MYGDB} PUBLIC ${MYGDB}_elf)
TARGET_LINK_LIBRARIES(${MYGDB} PUBLIC ${MYGDB}_memory)
TARGET_LINK_LIBRARIES(${MYGDB} PUBLIC ${MYGDB}_misc)
TARGET_LINK_LIBRARIES(${MYGDB} PUBLIC ${MYGDB}_file_cacher)

option(GIT_SUBMODULE "Check submodules during build" ON)
if(GIT_SUBMODULE)
    message(STATUS "Submodule update")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive ${CMAKE_ROOT}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
endif()
