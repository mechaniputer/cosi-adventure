CFLAGS = --std=c89 -pedantic -Wall -Werror -g
TARGET = cosi-adventure

$(TARGET): main.o util.o trigger.o
	$(CC) -o $@ $^

main.o: main.c util.h
	$(CC) -c $(CFLAGS) $<

util.o: util.c
	$(CC) -c $(CFLAGS) $<

trigger.o: trigger.c trigger.h
	$(CC) -c $(CFLAGS) $<

.PHONY: clean
clean:
	$(RM) -f $(TARGET) *.o
