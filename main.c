#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "util.h"

#define MAX_CMD_ARGS 10

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
				clarkson->allItems->itemArray[x]->examine = getstring('\n', f);
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
	default: ;
	}

	if (n == NULL) {
		puts("You cannot go that way.");
	} else {
		*room = n;
		watsup(*room);
	}
}

int transfer(itemList_t * from, itemList_t * to, char * what){
	int i, last;
	item_t * tmp;

	if (from->size > 0){
		last = from->size - 1;

		/* Swap the item in 'from' to the end of the item list */
		for (i = 0; i < from->size; i++){
			if (striequ(from->itemArray[i]->name, what)){
				tmp = from->itemArray[i];
				from->itemArray[i] = from->itemArray[last];
				from->itemArray[last] = tmp;
				break;
			}
		}

		if (i != from->size) {
			/* Move the item at the end of 'from' to the end of 'to' */
			if (to->size == to->capacity){
				addItem(to);
			}
			to->itemArray[to->size] = from->itemArray[last];
			from->itemArray[last] = NULL;
			to->size++;
			from->size--;
			return 1;
		}
	}
	return 0;
}

void take(itemList_t * roomItems, itemList_t * inventory, char * what){
	if (transfer(roomItems, inventory, what)){
		puts("Taken.");
	}else{
		puts("I do not think such a thing is here.");
	}
}

void drop(itemList_t * roomItems, itemList_t * inventory, char * what){
	if (transfer(inventory, roomItems, what)){
		puts("Dropped.");
	}else{
		puts("I am not sure that you have such a thing in your possession.");
	}
}

void showinv(itemList_t * inv){
	int x;
	for (x = 0; x < (inv->size); x++){
		if (inv->itemArray[x] != NULL) puts(inv->itemArray[x]->name);
	}
}

void examine(itemList_t * ila, itemList_t * ilb, const char * what){
	int i;

	do{
		for (i = 0; i < ila->size; i++){
			if (striequ(ila->itemArray[i]->name, what)){
				puts(ila->itemArray[i]->examine);
				return;
			}
		}

		ila = ilb;
		ilb = NULL;
	}while (ila != NULL);

	puts("You examine that which is not there, and ponder the mysteries of existence.");
}

void parse(char * inp, char cmd[MAX_CMD_ARGS][80])
{
	int i;
	char * start;

	for (i = 0; i < MAX_CMD_ARGS && *inp; i++) {
		while (isspace(*inp)) inp++;
		start = inp;
		while (!isspace(*inp) && *inp) inp++;
		memcpy(cmd[i], start, inp-start);
		cmd[i][inp-start] = 0;
	}

	for (; i < MAX_CMD_ARGS; i++) {
		cmd[i][0] = 0;
	}
}

compass direction(char * dir)
{
	if (striequ(dir, "east")) return EAST;
	if (striequ(dir, "west")) return WEST;
	if (striequ(dir, "north")) return NORTH;
	if (striequ(dir, "south")) return SOUTH;
	return EAST_BY_EAST_WEST;
}

int main(){
	int quit=0;
	world_t * clarkson;
	char cmd[MAX_CMD_ARGS][80];
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
		fgets(inp, 79, stdin); /* Get commands */
		parse(inp, cmd);

		if (striequ(cmd[0],"quit")) quit=1;
		if (striequ(cmd[0],"north")) go(NORTH, &clarkson->room);
		if (striequ(cmd[0],"south")) go(SOUTH, &clarkson->room);
		if (striequ(cmd[0],"east")) go(EAST, &clarkson->room);
		if (striequ(cmd[0],"west")) go(WEST, &clarkson->room);
		if (striequ(cmd[0],"go")) go(direction(cmd[1]), &clarkson->room);
		if (striequ(cmd[0],"take")) take(clarkson->room->items, clarkson->inventory, cmd[1]);
		if (striequ(cmd[0],"drop")) drop(clarkson->room->items, clarkson->inventory, cmd[1]);
		if (striequ(cmd[0],"look")) watsup(clarkson->room);
		if (striequ(cmd[0],"inv")) showinv(clarkson->inventory);
		if (striequ(cmd[0],"examine")) examine(clarkson->inventory, clarkson->room->items, cmd[1]);

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
