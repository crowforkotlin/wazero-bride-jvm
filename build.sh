#!/bin/bash

# 遇到错误立即停止
set -e

echo "🛠️  Building Go Static Library..."

go build -buildmode=c-archive -o wazero-bride-c/vendor/libwazerocore.a ./go-library/bridge/bridge.go

echo "✅ Go Build Success."

echo "🛠️  Configuring CMake..."
cd wazero-bride-c

sh configure.sh

echo "🛠️  Building C++ DLL..."
sh build.sh

echo "✅ C++ Build Success."

cd .. 
# cd wazero-kotlin
# ./gradlew app:run