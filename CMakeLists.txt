cmake_minimum_required(VERSION 3.20)
project(via-builder VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(via-builder)

target_sources(via-builder PRIVATE
    src/SerializedCourse/Course.cpp
    src/SerializedCourse/Section.cpp
    src/SerializedCourse/Timetable.cpp
    src/TimetableBuilder.cpp
    src/config.cpp
    src/ViaBuilderAPI.cpp
    src/ViaBuilderAPIBindings.cpp
)

if(NOT EMSCRIPTEN)
    target_sources(via-builder PRIVATE src/main.cpp)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Applying GCC/Clang flags")
    target_compile_options(via-builder PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Release>:-O2>
    )
    target_link_options(via-builder PRIVATE)
endif()

if(EMSCRIPTEN)
    message(STATUS "Applying Emscripten flags")
    target_compile_options(via-builder PRIVATE
        $<$<CONFIG:Release>:-O2>
    )
    target_link_options(via-builder PRIVATE
        "--no-entry"
        "-sMODULARIZE=1"
        "-sEXPORT_NAME=createViaBuilderModule"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sEXCEPTION_CATCHING_ALLOWED='1'"
        "-sASSERTIONS=1"
        "-lembind"
    )
    target_include_directories(via-builder PRIVATE
        "${EMSCRIPTEN_ROOT_PATH}/system/include"
        "${EMSCRIPTEN_ROOT_PATH}/system/include/emscripten"
    )
    set_target_properties(via-builder PROPERTIES SUFFIX ".js")
endif()

add_custom_command(
    TARGET via-builder POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${CMAKE_SOURCE_DIR}/build/compile_commands.json"
)