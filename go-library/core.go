package main

import (
	"context"
	"log"
	"unsafe"
	"github.com/tetratelabs/wazero"
	"github.com/tetratelabs/wazero/api"
	"C"
)

// WazeroBridge 結構體將持有運行時和編譯好的模塊
type WazeroBridge struct {
	ctx          context.Context
	runtime      wazero.Runtime
	module       api.Module
}

// 全局 map 用於存儲實例，C 語言通過一個 handle (指針的整數表示) 來查找
var bridges = make(map[uintptr]*WazeroBridge)
var nextHandle uintptr = 1

//export WazeroBridge_Create
func WazeroBridge_Create() uintptr {
	ctx := context.Background()
	r := wazero.NewRuntime(ctx)

	bridge := &WazeroBridge{
		ctx:     ctx,
		runtime: r,
	}

	handle := nextHandle
	bridges[handle] = bridge
	nextHandle++

	return handle
}

//export WazeroBridge_LoadWasm
func WazeroBridge_LoadWasm(handle uintptr, wasmBytes *C.uchar, wasmSize C.int) C.int {
	bridge, ok := bridges[handle]
	if !ok {
		return -1 // Error: invalid handle
	}

	// 將 C 的 byte array 轉換為 Go 的 slice
	goWasmBytes := C.GoBytes(unsafe.Pointer(wasmBytes), wasmSize)

	// 編譯並實例化模塊
	mod, err := bridge.runtime.Instantiate(bridge.ctx, goWasmBytes)
	if err != nil {
		log.Printf("Error instantiating wasm: %v", err)
		return -2 // Error: instantiation failed
	}

	bridge.module = mod
	return 0 // Success
}


//export WazeroBridge_ExecuteAdd
func WazeroBridge_ExecuteAdd(handle uintptr, a C.int, b C.int) C.int {
	bridge, ok := bridges[handle]
	if !ok {
		return -1 // Error: invalid handle
	}
	if bridge.module == nil {
		return -2 // Error: module not loaded
	}

	add := bridge.module.ExportedFunction("add")
	if add == nil {
		return -3 // Error: function not found
	}

	results, err := add.Call(bridge.ctx, uint64(a), uint64(b))
	if err != nil {
		log.Printf("Error calling function: %v", err)
		return -4 // Error: call failed
	}

	return C.int(results[0])
}

//export WazeroBridge_ExecuteAdd
func WazeroBridge_ExecuteLogcat(handle uintptr, a C.int, b C.int) C.int {
	bridge, ok := bridges[handle]
	if !ok {
		return -1 // Error: invalid handle
	}
	if bridge.module == nil {
		return -2 // Error: module not loaded
	}

	add := bridge.module.ExportedFunction("logcat")
	if add == nil {
		return -3 // Error: function not found
	}

	results, err := add.Call(bridge.ctx, uint64(a), uint64(b))
	if err != nil {
		log.Printf("Error calling function: %v", err)
		return -4 // Error: call failed
	}

	return C.int(results[0])
}


//export WazeroBridge_Destroy
func WazeroBridge_Destroy(handle uintptr) {
	bridge, ok := bridges[handle]
	if !ok {
		return
	}

	// 確保 runtime 被正確關閉
	if err := bridge.runtime.Close(bridge.ctx); err != nil {
		log.Printf("Error closing runtime: %v", err)
	}

	delete(bridges, handle)
}

func main() {}
