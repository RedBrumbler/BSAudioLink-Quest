cmake_minimum_required(VERSION 3.22)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/quest.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/kaleb.cmake)

project(bs-audiolink VERSION ${PACKAGE_VERSION})

# c++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# define that stores the actual source directory
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE c_files ${SRC_DIR}/*.c)
file(GLOB_RECURSE cpp_files ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE inline_hook_c_files ${EXTERN_DIR}/includes/beatsaber-hook/shared/inline-hook/*.c)
file(GLOB_RECURSE inline_hook_cpp_files ${EXTERN_DIR}/includes/beatsaber-hook/shared/inline-hook/*.cpp)

# add all src files to compile
add_library(
    bs-audiolink
    SHARED
    ${c_files}
    ${cpp_files}
    ${bs_hook_c_files}
    ${bs_hook_cpp_files}
)

add_assets(assets-bs-audiolink STATIC ${CMAKE_CURRENT_LIST_DIR}/assets ${INCLUDE_DIR}/assets.hpp)

file(GLOB_RECURSE so_files ${EXTERN_DIR}/libs/*.so)
file(GLOB_RECURSE a_files ${EXTERN_DIR}/libs/*.a)

target_link_libraries(
    bs-audiolink
    PRIVATE
    ${so_files}
    ${a_files}
    assets-bs-audiolink
)

target_include_directories(bs-audiolink PRIVATE ${INCLUDE_DIR})
target_include_directories(bs-audiolink PRIVATE ${SHARED_DIR})
target_include_directories(bs-audiolink PRIVATE ${EXTERN_DIR}/includes)
target_include_directories(bs-audiolink SYSTEM PRIVATE ${EXTERN_DIR}/includes/paper/shared/utfcpp/source)
target_include_directories(bs-audiolink SYSTEM PRIVATE ${EXTERN_DIR}/includes/fmt/fmt/include/)
target_include_directories(bs-audiolink SYSTEM PRIVATE ${EXTERN_DIR}/includes/beatsaber-hook/shared/rapidjson/include/)
target_include_directories(bs-audiolink SYSTEM PRIVATE ${EXTERN_DIR}/includes/libil2cpp/il2cpp/libil2cpp)
target_include_directories(bs-audiolink SYSTEM PRIVATE ${EXTERN_DIR}/includes/bs-cordl/include)
target_compile_options(bs-audiolink PUBLIC -fdeclspec -Wno-invalid-offsetof -fvisibility=hidden)
target_compile_definitions(bs-audiolink PRIVATE FMT_HEADER_ONLY)
target_compile_definitions(bs-audiolink PRIVATE MOD_ID="${PACKAGE_NAME}")
target_compile_definitions(bs-audiolink PRIVATE VERSION="${PACKAGE_VERSION}")
target_compile_definitions(bs-audiolink PRIVATE UNITY_2021)
target_compile_definitions(bs-audiolink PRIVATE HAS_CODEGEN)
target_compile_definitions(bs-audiolink PRIVATE KALEB_VERSION="${KALEB_VERSION}")

add_custom_command(
    TARGET bs-audiolink POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    ${CMAKE_CURRENT_BINARY_DIR}/debug
    COMMENT "Ensure debug dir exists"
)

add_custom_command(
    TARGET bs-audiolink POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename
    ${CMAKE_CURRENT_BINARY_DIR}/libbs-audiolink.so ${CMAKE_CURRENT_BINARY_DIR}/debug/libbs-audiolink.so
    COMMENT "move normal binary into debug folder"
)

add_custom_command(
    TARGET bs-audiolink POST_BUILD
    COMMAND ${CMAKE_STRIP} -g -S -d --strip-all
    ${CMAKE_CURRENT_BINARY_DIR}/debug/libbs-audiolink.so -o ${CMAKE_CURRENT_BINARY_DIR}/libbs-audiolink.so
    COMMENT "Strip binary"
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/debug)
foreach(bin_file IN LISTS so_list a_list)
    cmake_path(GET bin_file FILENAME file_name)

    add_custom_command(
        TARGET bs-audiolink POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${bin_file} ${CMAKE_CURRENT_BINARY_DIR}/debug/${file_name}
        COMMENT "Copy so files for ndk stack"
    )

    add_custom_command(
        TARGET bs-audiolink POST_BUILD
        COMMAND ${CMAKE_STRIP} -g -S -d --strip-all
        ${bin_file} -o ${CMAKE_CURRENT_BINARY_DIR}/${file_name}
        COMMENT "Strip debug symbols from the dependencies"
    )
endforeach()
