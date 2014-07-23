CFLAGS = --std=c89 -pedantic -Wall -Werror -g
TARGET = cosi-adventure

$(TARGET): main.o util.o trigger.o load.o
	$(CC) -o $@ $^

main.o: main.c util.h load.h
util.o: util.c
trigger.o: trigger.c trigger.h
load.o: load.c

.PHONY: clean
clean:
	$(RM) $(TARGET) *.o

