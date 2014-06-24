#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "util.h"

room_t * room = NULL;
itemList_t * inventory = NULL;
itemList_t * allItems = NULL;
room_t * allRooms = NULL;


int numRooms = 0;

/* THIS SHOULD NOT BE HERE but lazy*/
void addRoom(){
	numRooms++;
	allRooms = realloc(allRooms, sizeof(room_t) * numRooms);

	allRooms[numRooms-1].items = malloc(sizeof(itemList_t));
	allRooms[numRooms-1].items->itemArray = malloc(sizeof(item_t) * 1);
	roomInit(allRooms[numRooms-1]);
}

/* THIS SHOULD NOT BE HERE EITHER but lazy*/
void addItem(itemList_t * inv){
	(inv->capacity)++;
	inv->itemArray = realloc(inv->itemArray, sizeof(item_t) * (inv->capacity));
}

/* Loads data into structs */
void init(){
	/* This enum defines the sections of the datafile in order */
	enum {
		NONE,
		ROOM_DESC,
		ROOM_LINKS,
		OBJ_PROP,
		ROOM_OBJS
	} section = NONE;

	int x;
	int rv;
	int n, s, e, w;
	int rm, itm;
	FILE * f;

	char str[160]; /* This limits the length of room descriptions */
	str[159] = 0;

	allItems = malloc(sizeof(itemList_t));
	allItems->capacity = 0;
	allItems->size = 0;

	f = fopen("data", "r");
	assert(f != NULL);

	/* KLUDGE - stupid assignment trick by Andrew */
	while ((rv = fscanf(f, "%d ", &x)) && rv != EOF) {
		if (-1 == x) {
			while ((rv = fgetc(f)) != '\n' && rv != EOF);
			section++;
		} else {
			switch (section) {
			case NONE:
				fprintf(stderr, "Error: Yous si fidodin somehtin srong.\n");
				break;

			case ROOM_DESC:
				if (x == numRooms) addRoom();
				allRooms[x].description = getstring('\n', f);
				break;

			case ROOM_LINKS:
				assert(4 == fscanf(f, " %d %d %d %d\n", &n, &s, &e, &w));

				allRooms[x].north = n != -1 ? allRooms + n : NULL;
				allRooms[x].south = s != -1 ? allRooms + s : NULL;
				allRooms[x].east  = e != -1 ? allRooms + e : NULL;
				allRooms[x].west  = w != -1 ? allRooms + w : NULL;
				break;

			case OBJ_PROP:

				if (allItems->size == allItems->capacity){
					addItem(allItems);
					/* This leaks 1 block for each global item but prevents a segfault*/
					allItems->itemArray[(allItems->capacity)-1] = malloc(sizeof(item_t));
				}
				allItems->itemArray[x]->name = getstring('\n', f);
				fscanf(f, "%d ", &x);
				allItems->itemArray[x]->description = getstring('\n', f);
				fscanf(f, "%d ", &x);
				fgets(str, 159, f);
				allItems->size++;
				break;

			case ROOM_OBJS:
				assert(2 == fscanf(f, " %d %d\n", &rm, &itm));
				if(allRooms[rm].items->size == allRooms[rm].items->capacity){;
					addItem(allRooms[rm].items);
				}
				allRooms[rm].items->itemArray[allRooms[rm].items->size] = allItems->itemArray[itm];
				(allRooms[rm].items->size)++;
				break;

			default:
				fgets(str, 159, f);
				fprintf(stderr, "Error: default case reached?\n");
				break;
			}
		}
	}

	fclose(f);

	room = allRooms;

	/* Initialize inventory */
	inventory = malloc(sizeof(itemList_t));
	inventory->itemArray = malloc(sizeof(item_t) * 1);
	
	inventory->capacity = 1;
	inventory->size = 0;

	return;
}

/* Describes the situation */
int watsup(){
	/* The room description should be able to change even within the same room.
	   Semi-random events should also be printed. (e.g. "Chris falls out of his chair")
	   Additionally, it should list objects in the room. */
	int x;

	assert(room != NULL);
	puts(room->description);

	for (x = 0; x < (room->items->size); x++){
		puts(room->items->itemArray[x]->description);
	}
	return 0;
}

void go(compass c){
	room_t * n = NULL;

	switch (c) {
	case NORTH: n = room->north; break;
	case SOUTH: n = room->south; break;
	case EAST:  n = room->east;  break;
	case WEST:  n = room->west;  break;
	}

	if (n == NULL) {
		puts("You cannot go that way.");
	} else {
		room = n;
		watsup();
	}
}

void take(){
	if (room->items->size > 0){
		if (inventory->size == inventory->capacity){
			addItem(inventory);
		}
		inventory->itemArray[inventory->size] = room->items->itemArray[(room->items->size)-1];
		(inventory->size)++;
		room->items->itemArray[(room->items->size)-1] = NULL;
		(room->items->size)--;
	}else puts("Nothing here.");
}

void drop(){
	if (inventory->size > 0){
		if(room->items->size == room->items->capacity){
			addItem(room->items);
		}
		room->items->itemArray[room->items->size] = inventory->itemArray[(inventory->size)-1];
		(room->items->size)++;
		inventory->itemArray[(inventory->size)-1] = NULL;
		(inventory->size)--;
	}else puts("Your inventory is empty, or the room is full");
}

void showinv(){
	int x;
	for (x = 0; x < (inventory->size); x++){
		if (inventory->itemArray[x] != NULL) puts(inventory->itemArray[x]->name);
	}
}

int main(){
	int dead=0,win=0,quit=0;
	char inp[80];
	inp[79] = 0;

	/* Load data, create world */
	init();
	watsup();

	/* loop runs watsup() and inner loop takes commands until one works */
	while(!dead && !win && !quit){
		printf("What do? ");
		scanf ("%79s",inp); /* Get commands */

		if (!strncmp(inp,"quit",4)) quit=1;
		if (!strncmp(inp,"north",5)) go(NORTH);
		if (!strncmp(inp,"south",5)) go(SOUTH);
		if (!strncmp(inp,"east",4)) go(EAST);
		if (!strncmp(inp,"west",4)) go(WEST);
		if (!strncmp(inp,"take",4)) take();
		if (!strncmp(inp,"drop",4)) drop();
		if (!strncmp(inp,"look",4)) watsup();
		if (!strncmp(inp,"inv",3)) showinv();

		/* This is where a parse function would be called, and it would call other functons accordingly.
		   To get started, let's implement "go <dir>, take <obj>, look, eat <inv item>. */
	}

	/* free() everything */
	for (quit = 0; quit < allItems->size; quit++){
		free(allItems->itemArray[quit]->name);
		free(allItems->itemArray[quit]->description);
	}
	free(allItems->itemArray);
	free(allItems);

	for (quit = 0; quit < numRooms; quit++){
		free(allRooms[quit].items->itemArray);
		free(allRooms[quit].items);
		free(allRooms[quit].description);
	}
	free(allRooms);
	free(inventory->itemArray);
	free(inventory);

	return 0;
}
