CC = gcc
OBJ_DIR = obj
LIB_DIR = lib
SRC_DIR = src
BIN_DIR = bin
TEST_DIR = ${SRC_DIR}/tests
CFLAG = -Wall -I${LIB_DIR} -I${LIB_DIR}/basic
HEADERS = ${LIB_DIR}/cjson.h ${LIB_DIR}/basic/cjson_array.h ${LIB_DIR}/basic/cjson_bool.h ${LIB_DIR}/basic/cjson_null.h \
	${LIB_DIR}/basic/cjson_number.h ${LIB_DIR}/basic/cjson_object.h ${LIB_DIR}/basic/cjson_string.h ${LIB_DIR}/basic/cjson_type.h \
	${LIB_DIR}/basic/cjson_list.h

OBJS = ${OBJ_DIR}/cjson_number.o ${OBJ_DIR}/cjson_null.o	\
			${OBJ_DIR}/cjson_bool.o ${OBJ_DIR}/cjson_string.o		\
			${OBJ_DIR}/cjson_array.o ${OBJ_DIR}/cjson_object.o

all: ${OBJS}

${OBJ_DIR}/cjson_number.o: ${LIB_DIR}/basic/cjson_number.c ${HEADERS}
	${CC} ${CFLAG} $< -c -o $@

${OBJ_DIR}/cjson_null.o: ${LIB_DIR}/basic/cjson_null.c ${HEADERS}
	${CC} ${CFLAG} $< -c -o $@

${OBJ_DIR}/cjson_bool.o: ${LIB_DIR}/basic/cjson_bool.c ${HEADERS}
	${CC} ${CFLAG} $< -c -o $@

${OBJ_DIR}/cjson_string.o: ${LIB_DIR}/basic/cjson_string.c ${HEADERS}
	${CC} ${CFLAG} $< -c -o $@

${OBJ_DIR}/cjson_array.o: ${LIB_DIR}/basic/cjson_array.c ${HEADERS}
	${CC} ${CFLAG} $< -c -o $@
${OBJ_DIR}/cjson_object.o: ${LIB_DIR}/basic/cjson_object.c ${HEADERS}
	${CC} ${CFLAG} $< -c -o $@

TESTS = ${BIN_DIR}/test_number ${BIN_DIR}/test_null	\
	${BIN_DIR}/test_bool ${BIN_DIR}/test_string ${BIN_DIR}/test_array \
	${BIN_DIR}/test_object

tests: ${TESTS}

${BIN_DIR}/test_number: ${TEST_DIR}/test_number.c ${OBJS}
	${CC} ${CFLAG} $^ -o $@

${BIN_DIR}/test_null: ${TEST_DIR}/test_null.c ${OBJS}
	${CC} ${CFLAG} $^ -o $@

${BIN_DIR}/test_bool: ${TEST_DIR}/test_bool.c ${OBJS}
	${CC} ${CFLAG} $^ -o $@

${BIN_DIR}/test_string: ${TEST_DIR}/test_string.c ${OBJS}
	${CC} ${CFLAG} $^ -o $@

${BIN_DIR}/test_array: ${TEST_DIR}/test_array.c ${OBJS}
	${CC} ${CFLAG} $^ -o $@

${BIN_DIR}/test_object: ${TEST_DIR}/test_object.c ${OBJS}
	${CC} ${CFLAG} $^ -o $@
	
clean:
	rm -rf bin/* obj/*