#ifndef UTIL_H
#define UTIL_H

typedef struct room room_t;
typedef struct item item_t;
typedef struct itemList itemList_t;
typedef struct world world_t;

#include <stdio.h>
#include "trigger.h"

struct item {
	char * name;
	char * description;
	char * examine;
	char ** actions;
};

struct room {
	char * description;
	room_t * north, * south, * east, * west;
	itemList_t * items;
};

struct itemList {
	item_t ** itemArray;
	int capacity;
	int size;
};

struct world {
	room_t * allRooms;
	int numRooms;
	room_t * room;
	itemList_t * inventory;
	itemList_t * allItems;
	trigger_t * allTrigs;
	int numTrigs;
};

typedef enum {
	NORTH,
	SOUTH,
	EAST,
	WEST,
	EAST_BY_EAST_WEST
} compass;

/* Reads a string from given file 'f' until delimiter 'd' or EOF is found
   Returns an allocated string read from file */
char * getstring(char d, FILE * f);

/* Case insensitive string equivalence. Returns 1 when strings equal, 0 if not */
int striequ(const char *, const char *);

/* Initializes everything in a room struct to NULL */
void roomInit(room_t newroom);

/* Adds a NULL item to an itemList */
void addItem(itemList_t * inv);

/* Adds a room to a world */
void addRoom(world_t * clarkson);

/* Adds a trigger to a world */
void addTrig(world_t * clarkson);

/* Links to rooms together */
void linkRoom(world_t * clarkson, int a, int b, compass dir);

/* Initializes a world to NULL and 0 rooms */
void worldInit(world_t * clarkson);

#endif
