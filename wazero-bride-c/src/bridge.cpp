#include <jni.h>
#include "libwazerocore.h" // Include the Go-generated header file

#ifdef __cplusplus
extern "C" {
#endif

// Convert uintptr_t (the handle returned by Go) to jlong (Java's long).
// This is the standard way to pass pointers/handles across JNI.
#define HANDLE_TO_JLONG(h) (jlong)(h)
#define JLONG_TO_HANDLE(j) (uintptr_t)(j)

/*
 * Class:     com_bridge_wasmline_WasmBridge
 * Method:    nativeCreate
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_bridge_wasmline_WasmBridge_nativeCreate(
    JNIEnv *env,
    jclass clazz) {
    // Directly call the exported Go function.
    uintptr_t handle = WazeroBridge_Create();
    return HANDLE_TO_JLONG(handle);
}

/*
 * Class:     com_bridge_wasmline_WasmBridge
 * Method:    nativeLoadWasm
 * Signature: (J[B)I
 */
JNIEXPORT jint JNICALL Java_com_bridge_wasmline_WasmBridge_nativeLoadWasm(
    JNIEnv *env,
    jclass clazz,
    jlong handle,
    jbyteArray wasm_bytes) {

    jbyte* wasm_ptr = env->GetByteArrayElements(wasm_bytes, nullptr);
    jsize wasm_size = env->GetArrayLength(wasm_bytes);

    // Call the Go function.
    jint result = WazeroBridge_LoadWasm(JLONG_TO_HANDLE(handle), (unsigned char*)wasm_ptr, wasm_size);

    // Release the memory without copying back changes.
    env->ReleaseByteArrayElements(wasm_bytes, wasm_ptr, JNI_ABORT);
    return result;
}


/*
 * Class:     com_bridge_wasmline_WasmBridge
 * Method:    nativeExecuteAdd
 * Signature: (JII)I
 */
JNIEXPORT jint JNICALL Java_com_bridge_wasmline_WasmBridge_nativeExecuteAdd(
    JNIEnv *env,
    jclass clazz,
    jlong handle,
    jint a,
    jint b) {
    // Directly call the exported Go function.
    return WazeroBridge_ExecuteAdd(JLONG_TO_HANDLE(handle), a, b);
}

/*
 * Class:     com_bridge_wasmline_WasmBridge
 * Method:    nativeDestroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_bridge_wasmline_WasmBridge_nativeDestroy(
    JNIEnv *env,
    jclass clazz,
    jlong handle) {
    // Directly call the exported Go function.
    WazeroBridge_Destroy(JLONG_TO_HANDLE(handle));
}

#ifdef __cplusplus
}
#endif