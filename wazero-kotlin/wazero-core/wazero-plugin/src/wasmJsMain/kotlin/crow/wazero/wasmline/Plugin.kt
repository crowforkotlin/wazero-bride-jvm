import kotlin.wasm.WasmExport

// 模拟一个简单的堆内存，用于接收 Host 传来的数据
// 在真实生产环境中，这里应该使用更复杂的内存管理或 Kotlin 的 Unsafe API

@WasmExport
fun guest_malloc(size: Int): Int {
    return 0
}

@WasmExport
fun guest_free(ptr: Int) {
}

@WasmExport
fun guest_invoke(sPtr: Int, sLen: Int, mPtr: Int, mLen: Int, aPtr: Int, aLen: Int): Long {
    // 1. 从我们的“堆”中读取字符串
    // 注意：这里 Host 实际上是把数据写进了 Wasm 的线性内存
    // 我们需要一种方式去读取它。

    // 在当前的 Kotlin/Wasm (WasmJS) 目标下，Host 写入内存后，
    // 我们其实很难直接通过 ByteArray 访问到 Host 写入的那个绝对物理地址，
    // 除非我们使用了 Unsafe 或者特定的 Interop 库。

    // --- 为了演示跑通，我们先做个最简单的回显 (Echo) ---
    // 假设 Host 写对了位置，我们这里由业务逻辑直接返回一个固定字符串

    return 0
}

@WasmExport
fun add(a: Int, b: Int): Int {

    val a = 1
    val b= 1
    println(a)
    println(b)
    return a + b
}

@WasmExport
fun main() {

}