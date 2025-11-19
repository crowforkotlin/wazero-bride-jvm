@file:Suppress("PackageDirectoryMismatch")

import kotlin.wasm.WasmExport

@WasmExport
fun add(a: Int, b: Int): Int {
    return  a * b
}

@WasmExport
fun main() {
    // TODO provide bind service for host
    WasmLine.bind<CommonService>(name = "logger", WasmLogcatImpl())
}

interface WasmLogcatImpl : CommonService {
    override fun logcat(message: String) {
        println("wasm locat message : $message")
    }
}