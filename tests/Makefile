SRC=cpu instructions operand memory error

../src/%.o: $(SRC:%=../src/%.h) ../src/%.c
	gcc -c $(subst .o,.c,$@) -o $(patsubst ../src/%.o,../src/build/%.o,$@)

test: $(SRC:%=../src/%.o) test.o test_instructions.o test_6502.o test_all.o
	gcc $(SRC:%=../src/build/%.o) build/test.o build/test_instructions.o build/test_6502.o build/test_all.o -o test

test.o: test.c test.h
	gcc -c test.c -o build/test.o

test_instructions.o: $(SRC:%=../src/%.h) test.h 6502/test_instructions.h 6502/test_instructions.c 
	gcc -c 6502/test_instructions.c -o build/test_instructions.o

test_6502.o: $(SRC:%=../src/%.h) test.h 6502/test_instructions.h 6502/test_6502.h 6502/test_6502.c
	gcc -c 6502/test_6502.c -o build/test_6502.o

test_all.o: $(SRC:%=../src/%.h) test.h 6502/test_instructions.h 6502/test_6502.h test_all.c
	gcc -c test_all.c -o build/test_all.o

clean:
	rm build/*.o test