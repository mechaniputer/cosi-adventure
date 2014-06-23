#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

typedef struct room room_t;
typedef struct item item_t;
typedef struct itemList itemList_t;

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

/* Although cumbersome, this will hopefully make dynamic allocation easier */
struct itemList {
	item_t ** itemArray;
	int capacity;
	int size;
};

typedef enum {
	NORTH,
	SOUTH,
	EAST,
	WEST
} compass;

/* Reads a string from given file 'f' until delimiter 'd' or EOF is found
   Returns an allocated string read from file */
char * getstring(char d, FILE * f);

/* Initializes everything in a room struct to NULL */
void roomInit(room_t newroom);

#endif
