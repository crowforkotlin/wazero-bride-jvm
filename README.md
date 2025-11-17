## 🏗️ Build Steps (Step-by-Step)

### 💻 Go $\rightarrow$ C (Go Library to C Archive)

| Type | Command | Description |
| :--- | :--- | :--- |
| **Sample** | `go run go-sample/wazero-sample.go` | Run the Go sample directly. |
| **Library**| `go build -buildmode=c-archive -o libwazerocore.a core.go` | Build the Go code into a static C archive (`.a`) for linking with C/C++. |

### 🛠️ WAT $\rightarrow$ WASM

**This step requires the `wabt` toolchain (available at [https://github.com/WebAssembly/wabt])**.

| Action | Command |
| :--- | :--- |
| **Conversion** | `wat2wasm wasm/add.wat -o wasm/add.wasm` | Compile WebAssembly Text format (`.wat`) to WebAssembly Binary format (`.wasm`). |

### ⚙️ C Build (Using CMake)

**Navigate to the C bridge directory and execute the following commands:**

```shell
cd wazero-bride-c
# Clean up previous builds
rm -rf build/
# Create and enter the build directory
mkdir build
cd build
# Generate build files using MinGW Makefiles
cmake -G "MinGW Makefiles" ..
# Build the project
cmake --build .
# Execute the sample application
./sample.exe
```

***

## 🚀 Quick Commands

| Action | Command | Description |
| :--- | :--- | :--- |
| **Build All** | `sh build.sh` | Execute the main build script. |
| **Run Sample**| `sh sample.sh` | Execute the main sample script. |

***

## 📦 Pre-construction Environment

| Tool | Requirement |
| :--- | :--- |
| **GoLang** | Go programming language compiler and tools. |
| **CMake** | Cross-platform build system. |
| **MinGW** | Minimalist GNU for Windows (used for C/C++ compilation on Windows). |