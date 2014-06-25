#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "util.h"

/* Loads data into structs */
void init(world_t * clarkson){
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

	clarkson->allItems = malloc(sizeof(itemList_t));
	clarkson->allItems->itemArray = NULL;
	clarkson->allItems->capacity = 0;
	clarkson->allItems->size = 0;

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
				if (x == clarkson->numRooms) addRoom(clarkson);
				clarkson->allRooms[x].description = getstring('\n', f);
				break;

			case ROOM_LINKS:
				assert(4 == fscanf(f, " %d %d %d %d\n", &n, &s, &e, &w));
				clarkson->allRooms[x].north = n != -1 ? clarkson->allRooms + n : NULL;
				clarkson->allRooms[x].south = s != -1 ? clarkson->allRooms + s : NULL;
				clarkson->allRooms[x].east  = e != -1 ? clarkson->allRooms + e : NULL;
				clarkson->allRooms[x].west  = w != -1 ? clarkson->allRooms + w : NULL;
				break;

			case OBJ_PROP:
				if (clarkson->allItems->size == clarkson->allItems->capacity){
					addItem(clarkson->allItems);
				}
				clarkson->allItems->itemArray[x] = malloc(sizeof(item_t));
				clarkson->allItems->itemArray[x]->name = getstring('\n', f);
				fscanf(f, "%d ", &x);
				clarkson->allItems->itemArray[x]->description = getstring('\n', f);
				fscanf(f, "%d ", &x);
				fgets(str, 159, f);
				clarkson->allItems->size++;
				break;

			case ROOM_OBJS:
				assert(2 == fscanf(f, " %d %d\n", &rm, &itm));
				if(clarkson->allRooms[rm].items->size == clarkson->allRooms[rm].items->capacity){
					addItem(clarkson->allRooms[rm].items);
				}
				clarkson->allRooms[rm].items->itemArray[clarkson->allRooms[rm].items->size] = clarkson->allItems->itemArray[itm];
				(clarkson->allRooms[rm].items->size)++;
				break;

			default:
				fgets(str, 159, f);
				fprintf(stderr, "Error: default case reached?\n");
				break;
			}
		}
	}

	fclose(f);

	clarkson->room = clarkson->allRooms;

	/* Initialize inventory */
	clarkson->inventory = malloc(sizeof(itemList_t));
	clarkson->inventory->itemArray = NULL;
	clarkson->inventory->capacity = 0;
	clarkson->inventory->size = 0;

	return;
}

/* Describes the situation */
int watsup(room_t * room){
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

void go(compass c, room_t ** room){
	room_t * n = NULL;

	switch (c) {
	case NORTH: n = (*room)->north; break;
	case SOUTH: n = (*room)->south; break;
	case EAST:  n = (*room)->east;  break;
	case WEST:  n = (*room)->west;  break;
	}

	if (n == NULL) {
		puts("You cannot go that way.");
	} else {
		*room = n;
		watsup(*room);
	}
}

void take(itemList_t * roomItems, itemList_t * inventory){
	if (roomItems->size > 0){
		if (inventory->size == inventory->capacity){
			addItem(inventory);
		}
		inventory->itemArray[inventory->size] = roomItems->itemArray[(roomItems->size)-1];
		(inventory->size)++;
		roomItems->itemArray[(roomItems->size)-1] = NULL;
		(roomItems->size)--;
		puts("Taken.");
	}else puts("Nothing here.");
}

void drop(itemList_t * roomItems, itemList_t * inventory){
	if (inventory->size > 0){
		if(roomItems->size == roomItems->capacity){
			addItem(roomItems);
		}
		roomItems->itemArray[roomItems->size] = inventory->itemArray[(inventory->size)-1];
		(roomItems->size)++;
		inventory->itemArray[(inventory->size)-1] = NULL;
		(inventory->size)--;
	}else puts("Your inventory is empty.");
}

void showinv(itemList_t * inv){
	int x;
	for (x = 0; x < (inv->size); x++){
		if (inv->itemArray[x] != NULL) puts(inv->itemArray[x]->name);
	}
}

int main(){
	int quit=0;
	world_t * clarkson;
	char inp[80];
	inp[79] = 0;

	clarkson = malloc(sizeof(world_t));

	worldInit(clarkson);

	/* Load data, create world */
	init(clarkson);
	watsup(clarkson->room);

	/* loop runs watsup() and inner loop takes commands until one works */
	while(!quit){
		printf("What do? ");
		scanf ("%79s",inp); /* Get commands */

		if (!strncmp(inp,"quit",4)) quit=1;
		if (!strncmp(inp,"north",5)) go(NORTH, &clarkson->room);
		if (!strncmp(inp,"south",5)) go(SOUTH, &clarkson->room);
		if (!strncmp(inp,"east",4)) go(EAST, &clarkson->room);
		if (!strncmp(inp,"west",4)) go(WEST, &clarkson->room);
		if (!strncmp(inp,"take",4)) take(clarkson->room->items, clarkson->inventory);
		if (!strncmp(inp,"drop",4)) drop(clarkson->room->items, clarkson->inventory);
		if (!strncmp(inp,"look",4)) watsup(clarkson->room);
		if (!strncmp(inp,"inv",3)) showinv(clarkson->inventory);

		/* This is where a parse function would be called, and it would call other functons accordingly.
		   To get started, let's implement "go <dir>, take <obj>, look, eat <inv item>. */
	}

	/* free() everything */
	for (quit = 0; quit < clarkson->allItems->capacity; quit++){
		free(clarkson->allItems->itemArray[quit]->name);
		free(clarkson->allItems->itemArray[quit]->description);
		free(clarkson->allItems->itemArray[quit]);
	}
	free(clarkson->allItems->itemArray);
	free(clarkson->allItems);

	for (quit = 0; quit < clarkson->numRooms; quit++){
		free(clarkson->allRooms[quit].items->itemArray);
		free(clarkson->allRooms[quit].items);
		free(clarkson->allRooms[quit].description);
	}
	free(clarkson->allRooms);
	free(clarkson->inventory->itemArray);
	free(clarkson->inventory);
	free(clarkson);

	return 0;
}
