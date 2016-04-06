CC=clang
CFLAGS:=-std=c11 ${CFLAGS}

C_INCLUDE_PATH:=${CURDIR}/include:${C_INCLUDE_PATH}
export C_INCLUDE_PATH


all: bin/create_vectors bin/split_output


bin/split_output: src/split_output.py
	@cp src/split_output.py bin/split_output
	chmod +x bin/split_output

bin/create_vectors: src/main.c obj/float_utils.o obj/flt_mat.o obj/hex_mat.o obj/input_gen.o obj/vector_creation.o
	${CC} ${CFLAGS} obj/* src/main.c -o bin/create_vectors -lm

obj/float_utils.o: src/float_utils.c include/float_utils.h include/static_assert.h
	${CC} ${CFLAGS} -c src/float_utils.c -o obj/float_utils.o

obj/flt_mat.o: src/flt_mat.c include/flt_mat.h obj/float_utils.o
	${CC} ${CFLAGS} -c src/flt_mat.c -o obj/flt_mat.o

obj/hex_mat.o: src/hex_mat.c include/hex_mat.h obj/float_utils.o
	${CC} ${CFLAGS} -c src/hex_mat.c -o obj/hex_mat.o

obj/input_gen.o: src/input_gen.c include/input_gen.h obj/flt_mat.o
	${CC} ${CFLAGS} -c src/input_gen.c -o obj/input_gen.o

obj/vector_creation.o: src/vector_creation.c include/vector_creation.h obj/flt_mat.o obj/hex_mat.o
	${CC} ${CFLAGS} -c src/vector_creation.c -o obj/vector_creation.o


.PHONY: clean
clean:
	${RM} -r obj/*
	${RM} -r bin/*