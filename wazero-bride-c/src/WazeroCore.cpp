#include "WazeroCore.h"
#include <stdexcept>
#include <cstdlib> // for free()

WazeroCore::WazeroCore() {
    goHandle = WazeroBridge_Create();
    if (goHandle == 0) {
        throw std::runtime_error("Failed to create Wazero runtime.");
    }
}

WazeroCore::~WazeroCore() {
    if (goHandle != 0) {
        WazeroBridge_Destroy(goHandle);
        goHandle = 0;
    }
}

bool WazeroCore::loadModule(const uint8_t* data, size_t size) {
    if (goHandle == 0) return false;
    // Pass raw bytes to Go.
    int result = WazeroBridge_LoadWasm(goHandle, (unsigned char*)data, (int)size);
    return result == 0;
}

uint64_t WazeroCore::callNumeric(const std::string& funcName, const std::vector<uint64_t>& args) {
    if (goHandle == 0) return 0;

    const unsigned long long* argsPtr = nullptr;
    if (!args.empty()) {
        // Cast vector data to C array for CGO compatibility.
        argsPtr = reinterpret_cast<const unsigned long long*>(args.data());
    }

    // Invoke Go numeric interface.
    unsigned long long result = WazeroBridge_CallFunc(
        goHandle,
        (char*)funcName.c_str(),
        (unsigned long long*)argsPtr, 
        (int)args.size()
    );

    return (uint64_t)result;
}

std::string WazeroCore::callGuest(const std::string& service, const std::string& method, const std::string& jsonArgs) {
    if (goHandle == 0) return "";

    // Invoke Go string interface.
    char* c_result = WazeroBridge_CallGuest(
        goHandle,
        (char*)service.c_str(),
        (char*)method.c_str(),
        (char*)jsonArgs.c_str()
    );

    if (c_result == nullptr) return "";

    std::string result(c_result);
    
    // Free the C string allocated by Go's C.CString().
    free(c_result);

    return result;
}