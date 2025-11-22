package main

/*
#include <stdlib.h>
*/
import "C"
import (
	"context"
	"fmt"
	"unsafe"
	"wazero-jvm/go-library/engine" // Ensure this path matches your go.mod
)

var bridgeMap = make(map[uintptr]*engine.WasmEngine)
var nextHandle uintptr = 1

//export WazeroBridge_Create
func WazeroBridge_Create() uintptr {
	eng := engine.NewEngine(context.Background())
	h := nextHandle
	bridgeMap[h] = eng
	nextHandle++
	return h
}

//export WazeroBridge_LoadWasm
func WazeroBridge_LoadWasm(handle uintptr, data *C.uchar, size C.int) C.int {
	eng, ok := bridgeMap[handle]
	if !ok {
		fmt.Println("❌ [Go] Invalid Handle")
		return -1
	}

	goBytes := C.GoBytes(unsafe.Pointer(data), size)

	// 修改这里：打印具体的 error
	if err := eng.LoadWasm(goBytes); err != nil {
		fmt.Printf("❌ [Go] LoadWasm Failed: %v\n", err) // <--- 关键调试信息
		return -2
	}
	return 0
}

//export WazeroBridge_Destroy
func WazeroBridge_Destroy(handle uintptr) {
	if eng, ok := bridgeMap[handle]; ok {
		eng.Close()
		delete(bridgeMap, handle)
	}
}

// ------------------------------------------------------------------
// API 1: Numeric Call (For C++ Sample / add.wasm)
// ------------------------------------------------------------------

//export WazeroBridge_CallFunc
func WazeroBridge_CallFunc(handle uintptr, funcName *C.char, args *C.ulonglong, count C.int) C.ulonglong {
	eng, ok := bridgeMap[handle]
	if !ok {
		return 0
	}

	goName := C.GoString(funcName)
	var goArgs []uint64
	if count > 0 {
		// Create a slice view of the C array (Zero-Copy)
		goArgs = unsafe.Slice((*uint64)(unsafe.Pointer(args)), count)
	}

	res, err := eng.CallFunction(goName, goArgs...)
	if err != nil || len(res) == 0 {
		return 0
	}
	return C.ulonglong(res[0])
}

// ------------------------------------------------------------------
// API 2: String Call (For Kotlin / Zipline Style)
// ------------------------------------------------------------------

//export WazeroBridge_CallGuest
func WazeroBridge_CallGuest(handle uintptr, sName *C.char, mName *C.char, args *C.char) *C.char {
	eng, ok := bridgeMap[handle]
	if !ok {
		return C.CString(`{"error":"invalid handle"}`)
	}

	// Call Engine
	res, err := eng.CallGuest(C.GoString(sName), C.GoString(mName), C.GoString(args))
	if err != nil {
		return C.CString(`{"error":"` + err.Error() + `"}`)
	}

	return C.CString(res)
}

func main() {}
