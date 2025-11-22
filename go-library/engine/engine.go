package engine

import (
	"context"
	"fmt"

	"github.com/tetratelabs/wazero"
	"github.com/tetratelabs/wazero/api"
)

type WasmEngine struct {
	ctx     context.Context
	runtime wazero.Runtime
	module  api.Module
}

func NewEngine(ctx context.Context) *WasmEngine {
	return &WasmEngine{
		ctx:     ctx,
		runtime: wazero.NewRuntime(ctx),
	}
}

func (e *WasmEngine) LoadWasm(wasmBytes []byte) error {
	mod, err := e.runtime.Instantiate(e.ctx, wasmBytes)
	if err != nil {
		fmt.Printf("❌ [Go] LoadWasm Failed2222222222222: %v\n", err) // <--- 关键调试信息
		return fmt.Errorf("failed to instantiate module: %w", err)
	}
	e.module = mod
	return nil
}

// Close releases runtime resources.
func (e *WasmEngine) Close() {
	_ = e.runtime.Close(e.ctx)
}

// ============================================================================
// 1. Generic Numeric Interface (For C++ Sample / Simple Math)
// ============================================================================

// CallFunction calls any exported Wasm function with numeric arguments.
func (e *WasmEngine) CallFunction(funcName string, args ...uint64) ([]uint64, error) {
	if e.module == nil {
		return nil, fmt.Errorf("module not loaded")
	}

	fn := e.module.ExportedFunction(funcName)
	if fn == nil {
		return nil, fmt.Errorf("function '%s' not found", funcName)
	}

	results, err := fn.Call(e.ctx, args...)
	if err != nil {
		return nil, fmt.Errorf("call failed: %w", err)
	}
	return results, nil
}

// ============================================================================
// 2. JSON-RPC Interface (For Kotlin/Zipline)
// ============================================================================

// CallGuest implements the Zipline-style protocol: String -> Memory -> Invoke -> String
func (e *WasmEngine) CallGuest(service, method, argsJson string) (string, error) {
	if e.module == nil {
		return "", fmt.Errorf("module not loaded")
	}

	// 1. Write arguments to Wasm memory
	sPtr, sLen, err := e.writeStringToMemory(service)
	if err != nil {
		return "", err
	}

	mPtr, mLen, err := e.writeStringToMemory(method)
	if err != nil {
		return "", err
	}

	aPtr, aLen, err := e.writeStringToMemory(argsJson)
	if err != nil {
		return "", err
	}

	// 2. Invoke guest_invoke
	guestInvoke := e.module.ExportedFunction("guest_invoke")
	if guestInvoke == nil {
		return "", fmt.Errorf("guest_invoke not exported")
	}

	results, err := guestInvoke.Call(e.ctx,
		uint64(sPtr), uint64(sLen),
		uint64(mPtr), uint64(mLen),
		uint64(aPtr), uint64(aLen),
	)
	if err != nil {
		return "", fmt.Errorf("guest_invoke failed: %w", err)
	}

	// 3. Read return value
	packed := results[0]
	resPtr := uint32(packed >> 32)
	resLen := uint32(packed)

	bytes, ok := e.module.Memory().Read(resPtr, resLen)
	if !ok {
		return "", fmt.Errorf("memory read failed")
	}

	return string(bytes), nil
}

func (e *WasmEngine) writeStringToMemory(str string) (uint32, uint32, error) {
	strLen := uint64(len(str))
	malloc := e.module.ExportedFunction("guest_malloc")
	if malloc == nil {
		return 0, 0, fmt.Errorf("guest_malloc not exported")
	}

	results, err := malloc.Call(e.ctx, strLen)
	if err != nil {
		return 0, 0, err
	}

	ptr := uint32(results[0])
	if !e.module.Memory().WriteString(ptr, str) {
		return 0, 0, fmt.Errorf("memory write failed")
	}

	return ptr, uint32(strLen), nil
}
