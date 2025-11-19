这是一份为您定制的项目架构蓝图与开发计划清单 (`README.md`)。

这份文档旨在清晰梳理 **wazero-bridge-jvm** 项目的构建逻辑，将复杂的技术实现（底层内存管理、指针操作、JNI 回调）封装在框架内部，从而为上层用户提供极简的开发体验（类似 Zipline 的 `bind`/`take` 模式）。

---

# 🌉 Wazero Bridge JVM

> **High-Performance, Type-Safe Kotlin/Wasm RPC Framework based on wazero.**

本项目致力于在 **Kotlin/JVM (Host)** 与 **Kotlin/Wasm (Guest)** 之间构建一座高效的通信桥梁。通过利用 Go 编写的 wazero 运行时作为底层引擎，实现类似 Zipline 的双向服务调用体验，但基于 WebAssembly 标准。

## 🏗 架构设计原则

*   **用户无感 (User Transparent)**：用户只需编写标准的 Kotlin 接口和业务逻辑，无需关心 Wasm 内存分配、指针传递或 JNI 细节。
*   **双向通讯 (Bi-Directional)**：Host 可以调用 Guest 的服务，Guest 也可以调用 Host 的服务。
*   **接口驱动 (Interface Driven)**：基于 `bind` (注册) 和 `take` (代理) 模式进行服务发现。
*   **分层实现 (Layered Architecture)**：
    *   **User Layer**: 纯 Kotlin 接口与业务逻辑。
    *   **SDK Layer**: 封装序列化、代理生成、服务路由 (Kotlin Host/Guest)。
    *   **Bridge Layer**: 处理 JNI 回调与 CGO 转换 (C/C++)。
    *   **Runtime Layer**: 管理 Wasm 内存与模块实例化 (Go/wazero)。

---

## 📅 开发计划清单 (Roadmap & Tasks)

以下是各端需要完成的具体工作清单。

### 1. 🟢 Layer: Runtime (Go / wazero)
**角色：内存搬运工与运行时管理者**
Go 层不处理具体的业务逻辑，只负责“数据的物理传输”和“运行环境的生命周期”。

- [ ] **实现 `host_invoke` (Host Function)**
    - [ ] 定义 Go 函数供 Wasm 导入。
    - [ ] 逻辑：从 Wasm 线性内存读取 `serviceName`, `methodName`, `argsJson`。
    - [ ] 桥接：将读取到的字符串数据通过 CGO 转发给 C 层 (最终到达 JNI)。
- [ ] **实现 `guest_call` (Exported Function)**
    - [ ] 定义 Go 函数供 C 层调用。
    - [ ] 逻辑：接收来自 C 层的字符串数据。
    - [ ] 内存操作：调用 Wasm 导出的 `guest_malloc` 分配内存。
    - [ ] 写入数据：将字符串参数写入 Wasm 内存。
    - [ ] 执行调用：调用 Wasm 导出的 `guest_invoke` 分发器。
    - [ ] 读取结果：获取返回值指针，从内存读取结果并返回给 C 层。
- [ ] **Wasm 生命周期管理**
    - [ ] 实现 `Instantiate` 逻辑，加载 `.wasm` 文件。
    - [ ] **关键**：在实例化后，自动查找并执行 `_start` 或 `main` 函数，触发 Guest 端的服务注册逻辑。

### 2. 🔵 Layer: Native Bridge (C / C++ / JNI)
**角色：JNI 穿透与胶水层**
C 层作为 Go 与 JVM 之间的粘合剂，处理函数签名的转换和回调机制。

- [ ] **JNI 方法定义 (Java -> C)**
    - [ ] `nativeCreate`: 创建 Go 运行时实例。
    - [ ] `nativeLoadWasm`: 将字节数组传递给 Go。
    - [ ] `nativeCallGuest`: 接收 JVM 的调用请求，转发给 Go 的 `guest_call`。
- [ ] **JNI 回调支持 (C -> Java)**
    - [ ] 实现 JNI `OnLoad` 以缓存 JVM 环境指针。
    - [ ] 实现 C 函数接收来自 Go 的 `host_invoke` 请求。
    - [ ] 通过 JNI 反向调用 Kotlin 层的 `onHostInvoke` 静态方法。
- [ ] **数据类型转换**
    - [ ] 处理 `jstring` <-> `char*` <-> `Go string` 的相互转换，确保内存安全，防止泄漏。

### 3. 🟣 Layer: Guest SDK (Kotlin/Wasm)
**角色：插件端框架核心**
这一层运行在 Wasm 内部，负责屏蔽底层的指针操作，向用户暴露干净的 API。

- [ ] **WasmLine 单例设计**
    - [ ] 维护 `inboundServices` Map (服务注册表)。
    - [ ] 提供 `bind<T>(name, instance)` API 给用户。
- [ ] **底层导出函数 (Exports)**
    - [ ] `guest_malloc(size)`: 封装 Kotlin/Wasm 的内存分配逻辑，返回指针地址。
    - [ ] `guest_free(ptr)`: (可选) 处理内存释放。
    - [ ] `guest_invoke(...)`: 核心分发器。接收指针，解析为字符串，查找服务，执行方法。
- [ ] **底层导入函数 (Imports)**
    - [ ] 声明 `@WasmImport("host_invoke")`，用于将调用发送给 Host。
- [ ] **代理与序列化 (Proxy & Serialization)**
    - [ ] 集成 `kotlinx.serialization` (JSON)。
    - [ ] **Phase 1 (反射/硬编码)**: 暂时通过简单的 `when` 语句或基本反射实现方法分发 (待 IR 插件完善)。
    - [ ] **Phase 2 (IR 插件准备)**: 设计 Adapter 结构，为未来通过编译器生成代码预留接口，避免运行时反射。

### 4. 🟠 Layer: Host SDK (Kotlin/JVM)
**角色：宿主端框架核心**
这一层运行在 Android/Desktop JVM 上，模拟 Zipline 的体验。

- [ ] **WasmLine 单例设计**
    - [ ] 维护 `wazeroHandle` (指向底层的指针)。
    - [ ] 维护 `inboundServices` Map (Host 提供的服务)。
- [ ] **动态代理 (Dynamic Proxy) 实现 `take`**
    - [ ] 实现 `take<T>(name)`。
    - [ ] 使用 `java.lang.reflect.Proxy` 拦截接口调用。
    - [ ] 将方法名、参数序列化为 JSON，调用 JNI 的 `nativeCallGuest`。
- [ ] **JNI 回调处理**
    - [ ] 实现 `@JvmStatic onHostInvoke(...)`。
    - [ ] 接收 JNI 传来的 JSON，反序列化参数。
    - [ ] 找到本地服务并执行，返回结果 JSON。
- [ ] **加载流程封装**
    - [ ] `load(bytes)`: 初始化 JNI，传递数据，并自动等待 Guest 初始化完成。

---

## 🚀 阶段性目标

### Phase 1: 基础连通 (MVP)
*   不涉及复杂的 Kotlin IR 编译器插件。
*   数据传递使用 JSON 字符串。
*   Guest 端的分发逻辑 (Adapter) 手动编写或简化处理。
*   验证完整的调用链路：`Kotlin(Host) -> JNI -> Go -> Wasm -> Kotlin(Guest)` 及其反向链路。

### Phase 2: 编译器增强 (Advanced)
*   引入 Kotlin Compiler Plugin (KCP)。
*   自动为 Guest 端生成 `Adapter` 类（类似 Zipline），消除 `guest_invoke` 中的手动分发逻辑。

好的，这是为您补充的后续阶段计划，以及项目结构和使用示例的完善部分。这部分内容将构成 `README.md` 的完整蓝图。

---

### Phase 2: 编译器增强 (Advanced) - *续*
*   引入 Kotlin Compiler Plugin (KCP)。
*   自动为 Guest 端生成 `Adapter` 类（类似 Zipline），消除 `guest_invoke` 中的手动分发逻辑。
*   **类型安全增强**：在编译期检查跨平台调用的参数类型，避免运行时的转换错误。
*   **序列化生成**：自动生成参数的序列化/反序列化代码，移除对反射的依赖，提升性能。

### Phase 3: 性能与内存优化 (Optimization)
*   **二进制协议**：探索使用 Protocol Buffers 或自定义二进制格式替代 JSON，减少序列化开销和内存占用。
*   **零拷贝尝试 (Zero-Copy)**：在 Go 和 Wasm 之间尝试减少不必要的内存复制，直接操作共享内存区域。
*   **资源管理**：完善 Host 和 Guest 的生命周期管理，确保 `guest_malloc` 分配的内存在使用后能被正确释放 (GC 集成)，防止内存泄漏。

### Phase 4: 异步与协程支持 (Coroutines)
*   **Suspend 函数支持**：支持在 Kotlin 接口中定义 `suspend` 函数。
*   **异步桥接**：设计异步回调机制，使得 Host 调用 Guest 的耗时操作时不会阻塞主线程，反之亦然。
*   **Wasm Async**：利用 Kotlin/Wasm 的协程特性，处理跨边界的异步任务调度。