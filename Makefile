CFLAGS = -g -Werror -Wall

compile:
	mkdir -p build
	$(CC) $(CFLAGS) ./src/main.c -o ./build/interpreter

run:
	./build/interpreter ./data/test.script
