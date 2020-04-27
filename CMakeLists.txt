cmake_minimum_required(VERSION 3.0)
project(metronome CXX)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(metronome src/main.cpp) 
target_link_libraries(metronome ${CONAN_LIBS})

file(COPY snd DESTINATION bin)

if(GNU)
    target_compile_options(metronome PUBLIC -Wall -Wextra -Werror)
endif()