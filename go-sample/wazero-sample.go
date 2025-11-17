package main

import (
	"context"
	"log"
	"os"

	"github.com/tetratelabs/wazero"
)

func main() {
	// 创建一个上下文
	ctx := context.Background()

	// 读取 Wasm 文件
	wasmBytes, err := os.ReadFile("wasm/add.wasm")
	if err != nil {
		log.Panicf("failed to read wasm file: %v", err)
	}

	// 创建一个新的 wazero 运行时
	r := wazero.NewRuntime(ctx)
	defer r.Close(ctx) // 确保在 main 函数结束时关闭运行时

	// 编译 Wasm 模块
	module, err := r.CompileModule(ctx, wasmBytes)
	if err != nil {
		log.Panicf("failed to compile module: %v", err)
	}

	// 实例化模块
	mod, err := r.InstantiateModule(ctx, module, wazero.NewModuleConfig())
	if err != nil {
		log.Panicf("failed to instantiate module: %v", err)
	}

	// 获取导出的 "add" 函数
	add := mod.ExportedFunction("add")

	// 调用 Wasm 函数
	results, err := add.Call(ctx, 2, 3)
	if err != nil {
		log.Panicf("failed to call function: %v", err)
	}

	// 打印结果
	log.Printf("2 + 3 = %d", results[0])
}
