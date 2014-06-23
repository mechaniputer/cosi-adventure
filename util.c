#include <stdlib.h>
#include "util.h"

char * getstring(char d, FILE * f) {
	char c;
	size_t i = 0, size = 20;
	char * buf = malloc(size+1);

	while ((c = fgetc(f)) != d && c != EOF) {
		if (size == i) {
			size <<= 1;
			buf = realloc(buf, size+1);
		}
		buf[i++] = c;
	}
	buf[i] = 0;
	return buf;
}

void roomInit(room_t newroom){
	int x;
	newroom.description = NULL;
	newroom.north = NULL;
	newroom.south = NULL;
	newroom.east  = NULL;
	newroom.west  = NULL;

	for (x = 0; x < 3; x++){
		newroom.items->itemArray[x] = NULL;
	}

	newroom.items->capacity = 3;
	newroom.items->size = 0;
}
