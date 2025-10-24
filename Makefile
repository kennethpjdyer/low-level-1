TARGET=bin/dbview
SRC=$(wildcard src/*.c)
OBJ=$(patsubst src/%.c, obj/%.o, $(SRC))
#CFLAGS=-std=c23 \
   -Wall -Wextra  -Wconversion -Wshadow -Wpointer-arith \
   -Wcast-qual -Wstrict-prototypes -pedantic \
   -Wformat=2 -fstack-protector-strong
CFLAGS=-std=c23

run: bin obj clean default tests

tests: test-create test-read test-add

test-create:
	./$(TARGET) -nf dbview.db

test-read:
	./$(TARGET) -f dbview.db

test-add:
	./$(TARGET) -f dbview.db -a "Timmy H.,123 Sheshire Ln, 120"


default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $(CFLAGS) $?

obj/%.o: src/%.c
	gcc -c $< -o $@ $(CFLAGS) -Iinclude

bin:
	mkdir bin

obj:
	mkdir obj
