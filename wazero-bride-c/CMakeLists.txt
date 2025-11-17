cmake_minimum_required(VERSION 3.10)
project(wazero-bridge CXX)


# --- Shared Configuration (for both DLL and EXE) ---
# Define variables for shared paths and libraries to avoid repetition.
set(WAZERO_CORE_LIB "${CMAKE_CURRENT_SOURCE_DIR}/vendor/libwazerocore.a")
set(WAZERO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor")

# Define Go's system dependencies based on the OS.
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(GO_SYSTEM_LIBS ws2_32 userenv)
endif()
# You can add elseif for "Linux" here if needed.


# ==========================================================
# Target 1: JNI Shared Library (your original target)
# ==========================================================

# Find JNI libraries and headers.
find_package(JNI REQUIRED)

# Add our C++ library, which will be compiled into wazero-bridge.dll
add_library(wazero-bridge SHARED src/bridge.cpp)

# Set include directories for the DLL.
target_include_directories(wazero-bridge PRIVATE
    ${JNI_INCLUDE_DIRS}
    ${WAZERO_INCLUDE_DIR}
)

# Link libraries for the DLL.
target_link_libraries(wazero-bridge PRIVATE
    ${JNI_LIBRARIES}
    ${WAZERO_CORE_LIB}
    ${GO_SYSTEM_LIBS}
)


# ==========================================================
# Target 2: C++ Test Executable (the new target)
# ==========================================================

# Add our C++ executable from main.cpp
# ASSUMPTION: main.cpp is located in the src/ directory.
add_executable(sample sample/wazero-sample.cpp)

# Set include directories for the executable.
# It only needs to find libwazerocore.h
target_include_directories(sample PRIVATE
    ${WAZERO_INCLUDE_DIR}
)

# Link libraries for the executable.
# It needs the Go library and its system dependencies.
target_link_libraries(sample PRIVATE
    ${WAZERO_CORE_LIB}
    ${GO_SYSTEM_LIBS}
)


# ==========================================================
# Post-Build Step: Copy WASM files to the output directory
# ==========================================================

# Define the source wasm directory (relative to the project root)
set(WASM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../wasm)

# Define the destination directory (where the .exe is created)
set(WASM_DESTINATION_DIR ${CMAKE_BINARY_DIR}/wasm)

# Add a custom command that runs AFTER the 'sample' executable is built.
# This command copies the entire wasm directory.
add_custom_command(
    TARGET sample POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${WASM_SOURCE_DIR}
            ${WASM_DESTINATION_DIR}
    COMMENT "Copying WASM files to output directory..."
)