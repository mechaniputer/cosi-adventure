#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

typedef struct room room_t;
typedef struct item item_t;
typedef struct itemList itemList_t;
typedef struct world world_t;

struct item {
	char * name;
	char * description;
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

/* Initializes everything in a room struct to NULL */
void roomInit(room_t newroom);

/* Adds a NULL item to an itemList */
void addItem(itemList_t * inv);

/* Adds a room to a world */
void addRoom(world_t * clarkson);

/* Initializes a world to NULL and 0 rooms */
void worldInit(world_t * clarkson);

#endif
