cmake_minimum_required(VERSION 3.10)
project(wazero-bridge-project CXX)

set(CMAKE_CXX_STANDARD 14)

# --- Configuration ---
set(WAZERO_CORE_LIB "${CMAKE_CURRENT_SOURCE_DIR}/vendor/libwazerocore.a")
set(WAZERO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(GO_SYSTEM_LIBS ws2_32 userenv)
endif()

# --- Target 1: JNI Shared Library ---
find_package(JNI REQUIRED)

add_library(wazero-bridge SHARED 
    ${SRC_DIR}/WazeroCore.cpp
    ${SRC_DIR}/BridgeJni.cpp
)

target_include_directories(wazero-bridge PRIVATE
    ${JNI_INCLUDE_DIRS}
    ${WAZERO_INCLUDE_DIR}
    ${SRC_DIR}
)

target_link_libraries(wazero-bridge PRIVATE
    ${JNI_LIBRARIES}
    ${WAZERO_CORE_LIB}
    ${GO_SYSTEM_LIBS}
)

# --- Target 2: C++ Sample ---
add_executable(sample 
    sample/WazeroSample.cpp
    ${SRC_DIR}/WazeroCore.cpp
)

target_include_directories(sample PRIVATE
    ${WAZERO_INCLUDE_DIR}
    ${SRC_DIR}
)

target_link_libraries(sample PRIVATE
    ${WAZERO_CORE_LIB}
    ${GO_SYSTEM_LIBS}
)

# Post-build: Copy Wasm assets for the sample
add_custom_command(
    TARGET sample POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../wasm
    ${CMAKE_BINARY_DIR}/wasm
)