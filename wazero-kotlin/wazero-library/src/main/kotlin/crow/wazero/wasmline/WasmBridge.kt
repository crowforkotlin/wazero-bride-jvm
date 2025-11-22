package crow.wazero.wasmline

import java.io.File

object WasmBridge {
    init {
        try {
            val libFile = File("lib/libwazero-bridge.dll")
            System.load(libFile.absolutePath)
            println("✅ [JNI] Loaded native library: ${libFile.absolutePath}")
        } catch (e: Throwable) {
            System.err.println("💥 [JNI] Failed to load native library: ${e.message}")
        }
    }

    @JvmStatic
    external fun nativeCreate(): Long

    @JvmStatic
    external fun nativeLoadWasm(handle: Long, wasmBytes: ByteArray): Int

    /**
     * FAST PATH: Invokes a Wasm function with numeric arguments (e.g., malloc).
     * @param args Pass null for no arguments.
     */
    @JvmStatic
    external fun nativeCallFunc(handle: Long, funcName: String, args: LongArray?): Long

    /**
     * RICH PATH: Invokes the guest router with JSON strings.
     */
    @JvmStatic
    external fun nativeCallGuest(handle: Long, service: String, method: String, jsonArgs: String): String

    @JvmStatic
    external fun nativeDestroy(handle: Long)
}