ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SRC_DIR:=${ROOT_DIR}/src
OBJ_DIR:=${ROOT_DIR}/obj
BIN_DIR:=${ROOT_DIR}/bin
CC:=gcc
CFLAG:=-I${SRC_DIR} -g -Wall
HEADER = ${SRC_DIR}/yah_config.h ${SRC_DIR}/yah_const.h ${SRC_DIR}/yah_daemon.h \
	${SRC_DIR}/yah_error.h ${SRC_DIR}/yah_log.h ${SRC_DIR}/yah.h \
	${SRC_DIR}/yah_signal.h ${SRC_DIR}/yah_thread_pool_job.h

OBJS:=${OBJ_DIR}/yah.o ${OBJ_DIR}/yah_log.o ${OBJ_DIR}/yah_config.o \
	${OBJ_DIR}/yah_daemon.o ${OBJ_DIR}/yah_signal.o ${OBJ_DIR}/yah_thread_pool_job.o

yah: ${BIN_DIR}/yah

${BIN_DIR}/yah: ${OBJS}
	$(CC) $(CFLAG) $^ -o $@

${OBJ_DIR}/yah.o: src/yah.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_log.o: src/yah_log.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_config.o: src/yah_config.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_daemon.o: src/yah_daemon.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_signal.o: src/yah_signal.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

${OBJ_DIR}/yah_thread_pool_job.o: src/yah_thread_pool_job.c ${HEADER}
	$(CC) $(CFLAG) -o $@ -c $<

clean:
	rm -rf **/*.o ${BIN_DIR}/yah
