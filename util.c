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
	newroom.description = NULL;
	newroom.north = NULL;
	newroom.south = NULL;
	newroom.east  = NULL;
	newroom.west  = NULL;

	newroom.items->capacity = 0;
	newroom.items->size = 0;
}

void addItem(itemList_t * inv){
	(inv->capacity)++;
	inv->itemArray = realloc(inv->itemArray, sizeof(item_t) * (inv->capacity));
	inv->itemArray[(inv->capacity)-1] = NULL;
}

void addRoom(world_t * clarkson){
	clarkson->numRooms++;
	clarkson->allRooms = realloc(clarkson->allRooms, sizeof(room_t) * clarkson->numRooms);
	clarkson->allRooms[clarkson->numRooms-1].items = malloc(sizeof(itemList_t));
	clarkson->allRooms[clarkson->numRooms-1].items->itemArray = NULL;
	roomInit(clarkson->allRooms[clarkson->numRooms-1]);
}

void worldInit(world_t * clarkson){
	clarkson->allRooms = NULL;
	clarkson->numRooms = 0;
	clarkson->room = NULL;
	clarkson->inventory = NULL;
	clarkson->allItems = NULL;
}
