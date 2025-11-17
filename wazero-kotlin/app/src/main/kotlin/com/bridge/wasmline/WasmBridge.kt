package com.bridge.wasmline

import java.io.File

// JNI Bridge for wazero functions implemented in C/C++
object WasmBridge {

    // Static initialization block: Load the native library (DLL/SO)
    init {
        val libraryName = "libwazero-bridge.dll" 
        val dllPath = File("../lib/$libraryName").absolutePath
        
        println("--------------------------------------------------")
        println("| JNI Bridge: Initializing & Loading DLL...      |")
        println("| Path: $dllPath                                 |")
        try {
            System.load(dllPath)
            println("| Status: [OK] DLL Load SUCCESSFUL!              |")
        } catch (e: UnsatisfiedLinkError) {
            println("| Status: [ERR] DLL Load FAILED!                 |")
            System.err.println("!!! ERROR: Could not load the native library: $libraryName")
            System.err.println("!!! Check if the file exists and is in the specified path.")
            throw e
        } finally {
            println("--------------------------------------------------")
        }
    }

    // --- Native Function Declarations (JNI) ---
    // Handle is passed as jlong (Kotlin Long)
    // WASM bytes as jbyteArray (Kotlin ByteArray)

    /** Creates a new wazero instance and returns its handle (jlong). */
    external fun nativeCreate(): Long

    /** Loads the WASM module into the instance. Returns 0 on success. */
    external fun nativeLoadWasm(handle: Long, wasmBytes: ByteArray): Int

    /** Executes the 'add' WASM function. */
    external fun nativeExecuteAdd(handle: Long, a: Int, b: Int): Int

    /** Destroys the wazero instance and frees resources. */
    external fun nativeDestroy(handle: Long)
}