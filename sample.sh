echo "-------------- go wazero sample --------------"
go run go-sample/wazero-sample.go

echo ""
echo ""

echo "-------------- c/c++ wazero sample --------------"
./wazero-bride-c/build/sample.exe

echo ""
echo ""
echo "-------------- kotlin wazero sample --------------"
cd wazero-kotlin
./gradlew app:run
cd ..
