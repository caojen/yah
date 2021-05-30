CC = gcc
CFLAG = -Isrc -g

yah: bin/yah

obj/yah.o: src/yah.c src/yah.h
	$(CC) $(CFLAG) -o $@ -c $<

obj/yah_log.o: src/yah_log.c src/yah_log.h
	$(CC) $(CFLAG) -o $@ -c $<

obj/yah_config.o: src/yah_config.c src/yah_config.h
	$(CC) $(CFLAG) -o $@ -c $<

obj/yah_daemon.o: src/yah_daemon.c src/yah_daemon.h
	$(CC) $(CFLAG) -o $@ -c $<

bin/yah: obj/yah.o obj/yah_log.o obj/yah_config.o obj/yah_daemon.o
	$(CC) $(CFLAG) $^ -o $@

clean:
	rm -rf **/*.o bin/yah
