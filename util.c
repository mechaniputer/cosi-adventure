#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include "util.h"

char * getstring(char d, FILE * f){
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

int striEqu(const char * a, const char * b){
	while (*a) {
		if (tolower(*a) != tolower(*b)) return 0;
		a++; b++;
	}
	return *a == *b;
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

void addTrig(world_t * clarkson){
	clarkson->numTrigs++;
	clarkson->allTrigs = realloc(clarkson->allTrigs, sizeof(trigger_t) * clarkson->numTrigs);
	initTrigger(clarkson->allTrigs + clarkson->numTrigs - 1);
}

void linkRoom(world_t * clarkson, int a, int b, compass dir){
	switch (dir){
	case NORTH:
		clarkson->allRooms[a].north = clarkson->allRooms + b;
		clarkson->allRooms[b].south = clarkson->allRooms + a;
		break;
	case SOUTH:
		clarkson->allRooms[a].south = clarkson->allRooms + b;
		clarkson->allRooms[b].north = clarkson->allRooms + a;
		break;
	case EAST:
		clarkson->allRooms[a].east = clarkson->allRooms + b;
		clarkson->allRooms[b].west = clarkson->allRooms + a;
		break;
	case WEST:
		clarkson->allRooms[a].west = clarkson->allRooms + b;
		clarkson->allRooms[b].east = clarkson->allRooms + a;
		break;
	default:
		assert(0);
	}
}

void worldInit(world_t * clarkson){
	clarkson->allRooms = NULL;
	clarkson->numRooms = 0;
	clarkson->room = NULL;
	clarkson->inventory = NULL;
	clarkson->allItems = NULL;
	clarkson->allTrigs = NULL;
	clarkson->numTrigs = 0;
}

compass direction(char * dir){
	if (striEqu(dir, "east")) return EAST;
	if (striEqu(dir, "west")) return WEST;
	if (striEqu(dir, "north")) return NORTH;
	if (striEqu(dir, "south")) return SOUTH;
	return EAST_BY_EAST_WEST;
}

