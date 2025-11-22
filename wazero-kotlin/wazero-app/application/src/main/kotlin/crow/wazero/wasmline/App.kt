package crow.wazero.wasmline

import java.io.File
import kotlin.system.measureNanoTime

fun main() {
    println("=== 🌉 Wazero Bridge Host Sample ===")

    // ------------------------------------------------------------
    // 1. Configuration & Validation
    // ------------------------------------------------------------
    val wasmPath = "./lib/wazero-crow.wazero.wasmline-wazero-core-wazero-plugin.wasm"
    val wasmFile = File(wasmPath)

    if (!wasmFile.exists()) {
        System.err.println("❌ [Fatal] Wasm artifact not found at: ${wasmFile.absolutePath}")
        System.err.println("   Ensure the 'wazero-plugin' project has been built successfully.")
        return
    }

    // ------------------------------------------------------------
    // 2. Runtime Initialization
    // ------------------------------------------------------------
    println("🔄 [Host] Initializing Wazero Runtime (Go/C++)...")
    val handle: Long = WasmBridge.nativeCreate()

    if (handle == 0L) {
        System.err.println("❌ [Fatal] Failed to initialize native runtime.")
        return
    }

    try {
        // ------------------------------------------------------------
        // 3. Module Loading
        // ------------------------------------------------------------
        println("📥 [Host] Loading Module (${wasmFile.length()} bytes)...")
        val wasmBytes = wasmFile.readBytes()

        println(wasmBytes.toHexString())


        val loadDuration = measureNanoTime {
            val result = WasmBridge.nativeLoadWasm(handle, wasmBytes)
            if (result != 0) {
                throw RuntimeException("Native load failed with error code: $result")
            }
        }
        println("✅ [Host] Module loaded in ${loadDuration / 1_000_000.0} ms.")

        // ------------------------------------------------------------
        // 4. Execution: Rich Path (JSON-RPC)
        // ------------------------------------------------------------
        // Simulates a Zipline-style call: Service -> Method -> JSON Arguments.
        // The Guest (Wasm) is expected to route this to the actual implementation.
        val serviceName = "Calculator"
        val methodName = "add"
        val jsonArgs = "[100, 55]" // Arguments serialized as JSON array

        println("📞 [Host] Invoking: $serviceName.$methodName($jsonArgs)")

        var response: String = ""
        val execDuration = measureNanoTime {
            // Transmits String data across the JNI -> C -> Go -> Wasm boundary.
            response = WasmBridge.nativeCallGuest(
                handle,
                serviceName,
                methodName,
                jsonArgs
            )
        }

        // ------------------------------------------------------------
        // 5. Result Handling
        // ------------------------------------------------------------
        println("✨ [Host] Response: $response")
        println("⏱️ [Host] Execution time: ${execDuration / 1_000_000.0} ms")

    } catch (e: Exception) {
        System.err.println("💥 [Exception] ${e.message}")
        e.printStackTrace()
    } finally {
        // ------------------------------------------------------------
        // 6. Resource Cleanup
        // ------------------------------------------------------------
        // Crucial: Ensures Go resources (Context, Runtime) are freed to prevent leaks.
        println("🧹 [Host] Shutting down runtime...")
        WasmBridge.nativeDestroy(handle)
        println("=== Session Finished ===")
    }
}