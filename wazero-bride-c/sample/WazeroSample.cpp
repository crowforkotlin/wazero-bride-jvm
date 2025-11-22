#include <iostream>
#include <fstream>
#include <vector>
#include "WazeroCore.h" // Using the shared core

// Helper: Read file binary
std::vector<uint8_t> readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

int main() {
    std::cout << "=== Wazero C++ Core Sample ===" << std::endl;

    try {
        // 1. Create Runtime (Implicitly calls WazeroBridge_Create)
        WazeroCore engine;
        std::cout << "[Step 1] Engine initialized." << std::endl;

        // 2. Read Wasm File
        // Ensure the path matches your build output location
        auto wasmBytes = readFile("wasm/add.wasm");
        std::cout << "[Step 2] Loaded " << wasmBytes.size() << " bytes from disk." << std::endl;

        // 3. Load Module into Engine
        if (!engine.loadModule(wasmBytes.data(), wasmBytes.size())) {
            std::cerr << "❌ Failed to load Wasm module." << std::endl;
            return 1;
        }
        std::cout << "[Step 3] Wasm module instantiated." << std::endl;

        // 4. Execute Function: add(200, 25)
        // using the Fast Numeric Path
        std::cout << "[Step 4] Invoking 'add(200, 25)'..." << std::endl;
        
        std::vector<uint64_t> args = {200, 25};
        uint64_t result = engine.callNumeric("add", args);

        std::cout << "✅ Result: " << result << std::endl;

        // 5. (Optional) Simulate a JSON Call if your wasm supported it
        // std::string jsonRes = engine.callGuest("Service", "Method", "{}");

    } catch (const std::exception& e) {
        std::cerr << "💥 Exception: " << e.what() << std::endl;
        return 1;
    }

    // 6. Destroy Runtime (Implicitly calls WazeroBridge_Destroy via destructor)
    std::cout << "[Step 5] Engine destroyed." << std::endl;

    return 0;
}