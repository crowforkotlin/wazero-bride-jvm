#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

// Include the Go-generated header file
#include "libwazerocore.h"

// Helper function: reads all bytes from a file
std::vector<unsigned char> read_wasm_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + path);
    }
    return std::vector<unsigned char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

int main() {
    std::cout << "--- Wazero C++ Demo Start ---" << std::endl;

    // 1. Read the WASM file
    std::vector<unsigned char> wasm_bytes;
    try {
        wasm_bytes = read_wasm_file("wasm/add.wasm"); // Relative path
        std::cout << "[Step 1] Successfully read add.wasm (" << wasm_bytes.size() << " bytes)" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // 2. Create a wazero instance
    std::cout << "[Step 2] Creating wazero instance..." << std::endl;
    uintptr_t handle = WazeroBridge_Create();
    if (handle == 0) {
        std::cerr << "Error: WazeroBridge_Create() failed!" << std::endl;
        return 1;
    }
    std::cout << "         Instance created successfully, Handle: " << (void*)handle << std::endl;

    // 3. Load the WASM module
    std::cout << "[Step 3] Loading WASM module..." << std::endl;
    int load_result = WazeroBridge_LoadWasm(handle, wasm_bytes.data(), wasm_bytes.size());
    if (load_result != 0) {
        std::cerr << "Error: WazeroBridge_LoadWasm() failed, error code: " << load_result << std::endl;
        WazeroBridge_Destroy(handle);
        return 1;
    }
    std::cout << "         WASM module loaded successfully!" << std::endl;

    // 4. Execute the add function
    int a = 200;
    int b = 25;
    std::cout << "[Step 4] Executing add(" << a << ", " << b << ")..." << std::endl;
    int add_result = WazeroBridge_ExecuteAdd(handle, a, b);
    std::cout << "         Execution result: " << a << " + " << b << " = " << add_result << std::endl;

    // 5. Destroy the instance
    std::cout << "[Step 5] Destroying wazero instance..." << std::endl;
    WazeroBridge_Destroy(handle);
    std::cout << "         Instance destroyed successfully." << std::endl;

    std::cout << "--- Demo Finished ---" << std::endl;

    return 0;
}