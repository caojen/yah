ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SRC_DIR:=${ROOT_DIR}/src
OBJ_DIR:=${ROOT_DIR}/obj
BIN_DIR:=${ROOT_DIR}/bin
CC:=gcc
DEBUG:=-D DEBUG -g
PTHREADF:=-lpthread
SQLITEF:=-lsqlite3
TEST:=-D TEST -D DEBUG -g
CFLAG:=-I${SRC_DIR} -Wall \
	-D _GNU_SOURCE ${DEBUG} \
	${PTHREADF} ${SQLITEF}

HEADER = ${SRC_DIR}/yah_config.h ${SRC_DIR}/yah_const.h ${SRC_DIR}/yah_daemon.h \
	${SRC_DIR}/yah_error.h ${SRC_DIR}/yah_log.h ${SRC_DIR}/yah.h \
	${SRC_DIR}/yah_signal.h ${SRC_DIR}/yah_thread_pool_job.h ${SRC_DIR}/yah_thread_pool_worker.h \
	${SRC_DIR}/yah_thread_pool_manager.h ${SRC_DIR}/yah_thread_pool.h ${SRC_DIR}/yah_core.h \
	${SRC_DIR}/yah_pty.h ${SRC_DIR}/yah_exec.h ${SRC_DIR}/yah_airodump.h ${SRC_DIR}/yah_mem.h \
	${SRC_DIR}/yah_string.h ${SRC_DIR}/yah_lru.h ${SRC_DIR}/yah_sqlite3.h

OBJS:=${OBJ_DIR}/yah.o ${OBJ_DIR}/yah_log.o ${OBJ_DIR}/yah_config.o \
	${OBJ_DIR}/yah_daemon.o ${OBJ_DIR}/yah_signal.o ${OBJ_DIR}/yah_thread_pool_job.o \
	${OBJ_DIR}/yah_thread_pool_worker.o ${OBJ_DIR}/yah_thread_pool_manager.o \
	${OBJ_DIR}/yah_thread_pool.o ${OBJ_DIR}/yah_core.o ${OBJ_DIR}/yah_pty.o \
	${OBJ_DIR}/yah_exec.o ${OBJ_DIR}/yah_airodump.o ${OBJ_DIR}/yah_mem.o \
	${OBJ_DIR}/yah_string.o ${OBJ_DIR}/yah_lru.o ${OBJ_DIR}/yah_sqlite3.o

.PHONY: test clean all yah

all: yah test

yah: ${BIN_DIR}/yah
	@echo \> build succeed

${BIN_DIR}/yah: ${OBJS}
	$(CC) $(CFLAG) $^ -o $@

${OBJ_DIR}/yah.o: ${SRC_DIR}/yah.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_log.o: ${SRC_DIR}/yah_log.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_config.o: ${SRC_DIR}/yah_config.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_daemon.o: ${SRC_DIR}/yah_daemon.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_signal.o: ${SRC_DIR}/yah_signal.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_thread_pool_job.o: ${SRC_DIR}/yah_thread_pool_job.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_thread_pool_worker.o: ${SRC_DIR}/yah_thread_pool_worker.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_thread_pool_manager.o: ${SRC_DIR}/yah_thread_pool_manager.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_thread_pool.o: ${SRC_DIR}/yah_thread_pool.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_core.o: ${SRC_DIR}/yah_core.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_pty.o: ${SRC_DIR}/yah_pty.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_exec.o: ${SRC_DIR}/yah_exec.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_airodump.o: ${SRC_DIR}/yah_airodump.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_mem.o: ${SRC_DIR}/yah_mem.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_string.o: ${SRC_DIR}/yah_string.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_lru.o: ${SRC_DIR}/yah_lru.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_sqlite3.o: ${SRC_DIR}/yah_sqlite3.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

clean:
	rm -rf **/*.o ${BIN_DIR}/yah **/*.test

test: test/sqlite3.test
	@echo testing...
	@test/sqlite3.test > /dev/null
	@echo \> build \& run all tests done. test pass.

test/sqlite3.test: test/sqlite3.c ${HEADER}
	$(CC) $(CFLAG) $(TEST) $< ${SRC_DIR}/*.c -o $@
	@ echo \> test build: $@
