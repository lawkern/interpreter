compile:
	mkdir -p build
	$(CC) ./src/main.c -o ./build/interpreter

run:
	./build/interpreter ./data/test.script
