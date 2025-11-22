package main

import (
	"context"
	"log"
	"os"
	"wazero-jvm/go-library/engine"
)

func main() {
	log.Println("--- Starting Pure Go Sample ---")

	// 1. 准备环境
	ctx := context.Background()
	eng := engine.NewEngine(ctx)
	defer eng.Close()

	// 2. 读取文件 (模拟从 Android 传入 bytes)
	// 请确保工作目录下有 wasm/add.wasm
	wasmBytes, err := os.ReadFile("wasm/add.wasm")
	if err != nil {
		log.Panicf("Read wasm failed: %v", err)
	}

	// 3. 加载
	log.Println("Loading Wasm...")
	if err := eng.LoadWasm(wasmBytes); err != nil {
		log.Panicf("LoadWasm failed: %v", err)
	}

	// 4. 测试 Add (基础数值调用)
	log.Println("Executing 'add(10, 20)'...")
	results, err := eng.CallFunction("add", 10, 20)
	if err != nil {
		log.Panicf("CallFunction failed: %v", err)
	}
	log.Printf("Result: %d", results[0])

	// 5. (高级) 模拟 RPC / 字符串传递
	// 注意：这需要你的 wasm 里有 malloc 函数，如果没有会报错，但这演示了思路
	log.Println("--- Simulating RPC (String Passing) ---")
	// 假设我们要传 "Hello Wasm" 给 logcat
	/*
		    // 这里的逻辑是未来要实现的：
			ptr, size, err := eng.WriteStringIntoMemory("Hello Wasm")
			if err == nil {
				_, err = eng.CallFunction("logcat", uint64(ptr), uint64(size))
		        // ...
			} else {
		        log.Printf("Skipping string test: %v", err)
		    }
	*/

	log.Println("--- Sample Finished ---")
}
