import com.bridge.wasmline.WasmBridge
import java.io.File
import kotlin.system.measureNanoTime

fun main() {
    var handle: Long = 0L
    val wasmPath = "../lib/add.wasm"
    val a = 500
    val b = 599

    println("+-------------------------------------+")
    println("|    Kotlin WASM Execution Sample     |")
    println("+-------------------------------------+")

    try {
        // --- STEP 1: Read WASM file ---
        val wasmFile = File(wasmPath)
        if (!wasmFile.exists()) {
            System.err.println("[ERR] STEP 1: WASM file not found at $wasmPath!")
            return
        }
        val wasmBytes = wasmFile.readBytes()
        println("[OK] STEP 1: Read $wasmPath (${wasmBytes.size} bytes)")

        // --- STEP 2: Create wazero instance ---
        handle = WasmBridge.nativeCreate()
        if (handle == 0L) {
            System.err.println("[ERR] STEP 2: nativeCreate() failed! Check Go/C linking.")
            return
        }
        println("[OK] STEP 2: Instance created. Handle: $handle")

        // --- STEP 3: Load WASM module ---
        WasmBridge.nativeLoadWasm(handle, wasmBytes).also { result ->
            if (result != 0) {
                System.err.println("[ERR] STEP 3: nativeLoadWasm() failed with code $result")
                return
            }
        }
        println("[OK] STEP 3: WASM module loaded.")

        // --- STEP 4: Execute 'add' function ---
        println("[RUN] STEP 4: Executing add function: $a + $b...")
        val executionTime = measureNanoTime {
            val result = WasmBridge.nativeExecuteAdd(handle, a, b)
            println("    -> RESULT: $a + $b = $result")
        }
        println("    (Time) Execution Time: ${executionTime / 1_000_000.0} ms")

    } catch (e: UnsatisfiedLinkError) {
        System.err.println("\n[!!!] FATAL ERROR: JNI Linkage Issue!")
        System.err.println("   Please check: 1. DLL loaded? 2. Package/Class/Method names match C++? 3. Function signatures correct?")
        e.printStackTrace()
    } catch (e: Exception) {
        System.err.println("\n[!] UNEXPECTED EXCEPTION:")
        e.printStackTrace()
    } finally {
        // --- STEP 5: Destroy instance ---
        if (handle != 0L) {
            WasmBridge.nativeDestroy(handle)
            println("[FREE] STEP 5: Instance destroyed.")
        }
    }
    println("=====================================")
    println("           Test Finished             ")
}