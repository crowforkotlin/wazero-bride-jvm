#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

go build -buildmode=c-archive -o wazero-bride-c/vendor/libwazerocore.a go-library/core.go

cd wazero-bride-c

sh configure.sh

sh build.sh

cd ../wazero-kotlin

./gradlew app:run