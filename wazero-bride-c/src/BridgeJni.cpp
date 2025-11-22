#include <jni.h>
#include "WazeroCore.h"

#ifdef __cplusplus
extern "C" {
#endif

// Helper macro to cast jlong handle to C++ pointer.
#define HANDLE_TO_PTR(h) reinterpret_cast<WazeroCore*>(h)

/*
 * Class:     crow_wazero_wasmline_WasmBridge
 * Method:    nativeCreate
 */
JNIEXPORT jlong JNICALL Java_crow_wazero_wasmline_WasmBridge_nativeCreate(JNIEnv *env, jclass clazz) {
    try {
        WazeroCore* core = new WazeroCore();
        return reinterpret_cast<jlong>(core);
    } catch (...) {
        return 0;
    }
}

/*
 * Class:     crow_wazero_wasmline_WasmBridge
 * Method:    nativeLoadWasm
 */
JNIEXPORT jint JNICALL Java_crow_wazero_wasmline_WasmBridge_nativeLoadWasm(
    JNIEnv *env, jclass clazz, jlong handle, jbyteArray wasm_bytes) {
    
    WazeroCore* core = HANDLE_TO_PTR(handle);
    if (!core) return -1;

    jbyte* data_ptr = env->GetByteArrayElements(wasm_bytes, nullptr);
    jsize data_size = env->GetArrayLength(wasm_bytes);

    bool success = core->loadModule((const uint8_t*)data_ptr, (size_t)data_size);

    env->ReleaseByteArrayElements(wasm_bytes, data_ptr, JNI_ABORT);
    return success ? 0 : -1;
}

/*
 * Class:     crow_wazero_wasmline_WasmBridge
 * Method:    nativeCallFunc
 * Description: Fast path for numeric calls (e.g., malloc).
 */
JNIEXPORT jlong JNICALL Java_crow_wazero_wasmline_WasmBridge_nativeCallFunc(
    JNIEnv *env, jclass clazz, jlong handle, jstring funcName, jlongArray args) {

    WazeroCore* core = HANDLE_TO_PTR(handle);
    if (!core) return 0;

    // 1. Convert Function Name
    const char* c_func = env->GetStringUTFChars(funcName, nullptr);
    std::string strFunc(c_func);
    env->ReleaseStringUTFChars(funcName, c_func);

    // 2. Convert Arguments
    std::vector<uint64_t> vecArgs;
    if (args != nullptr) {
        jsize len = env->GetArrayLength(args);
        if (len > 0) {
            jlong* elems = env->GetLongArrayElements(args, nullptr);
            // Copy elements. jlong (int64) is bitwise compatible with uint64 for transmission.
            vecArgs.assign(reinterpret_cast<uint64_t*>(elems), reinterpret_cast<uint64_t*>(elems) + len);
            env->ReleaseLongArrayElements(args, elems, JNI_ABORT);
        }
    }

    return (jlong)core->callNumeric(strFunc, vecArgs);
}

/*
 * Class:     crow_wazero_wasmline_WasmBridge
 * Method:    nativeCallGuest
 * Description: Rich path for JSON-RPC calls.
 */
JNIEXPORT jstring JNICALL Java_crow_wazero_wasmline_WasmBridge_nativeCallGuest(
    JNIEnv *env, jclass clazz, 
    jlong handle, 
    jstring service, jstring method, jstring args) {

    WazeroCore* core = HANDLE_TO_PTR(handle);
    if (!core) return env->NewStringUTF("");

    // 1. Convert JNI Strings
    const char* c_svc = env->GetStringUTFChars(service, nullptr);
    const char* c_mtd = env->GetStringUTFChars(method, nullptr);
    const char* c_arg = env->GetStringUTFChars(args, nullptr);

    // 2. Call Core
    std::string result = core->callGuest(c_svc, c_mtd, c_arg);

    // 3. Release Memory
    env->ReleaseStringUTFChars(service, c_svc);
    env->ReleaseStringUTFChars(method, c_mtd);
    env->ReleaseStringUTFChars(args, c_arg);

    return env->NewStringUTF(result.c_str());
}

/*
 * Class:     crow_wazero_wasmline_WasmBridge
 * Method:    nativeDestroy
 */
JNIEXPORT void JNICALL Java_crow_wazero_wasmline_WasmBridge_nativeDestroy(JNIEnv *env, jclass clazz, jlong handle) {
    WazeroCore* core = HANDLE_TO_PTR(handle);
    if (core) {
        delete core;
    }
}

#ifdef __cplusplus
}
#endif