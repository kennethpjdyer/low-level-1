TARGET=bin/dbview
SRC=$(wildcard src/*.c)
OBJ=$(patsubst src/%.c, obj/%.o, $(SRC))
CFLAGS=-std=c23 \
   -Wall -Wextra  -Wconversion -Wshadow -Wpointer-arith \
   -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -pedantic \
   -Wformat=2 -fstack-protector-strong

run: bin obj clean default
	./$(TARGET) -nf dbview.db
	./$(TARGET) -f dbview.db

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
