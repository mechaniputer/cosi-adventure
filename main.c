#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include "util.h"
#include "load.h"

#define MAX_CMD_ARGS 10
#define BUF_LEN 80

#define countof(X) (sizeof(X) / sizeof(*(X)))

/* Loads data into structs */
void init(world_t * clarkson){

	clarkson->allItems = malloc(sizeof(itemList_t));
	clarkson->allItems->itemArray = NULL;
	clarkson->allItems->capacity = 0;
	clarkson->allItems->size = 0;

	/* This was a huge block of code, so I put it in load.h */
	loadDataFile(clarkson);

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
	case UP:    n = (*room)->up;    break;
	case DOWN:  n = (*room)->down;  break;
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

	if (from->size > 0) {
		last = from->size - 1;

		/* Swap the item in 'from' to the end of the item list */
		for (i = 0; i < from->size; i++){
			if (striEqu(from->itemArray[i]->name, what)) {
				tmp = from->itemArray[i];
				from->itemArray[i] = from->itemArray[last];
				from->itemArray[last] = tmp;
				break;
			}
		}

		if (i != from->size) {
			/* Move the item at the end of 'from' to the end of 'to' */
			if (to->size == to->capacity) {
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
	if (transfer(roomItems, inventory, what)) {
		puts("Taken.");
	}else{
		puts("I do not think such a thing is here.");
	}
}

void drop(itemList_t * roomItems, itemList_t * inventory, char * what){
	if (transfer(inventory, roomItems, what)) {
		puts("Dropped.");
	}else{
		puts("I am not sure that you have such a thing in your possession.");
	}
}

void showinv(itemList_t * inv){
	int x;
	for (x = 0; x < (inv->size); x++) {
		if (inv->itemArray[x] != NULL) puts(inv->itemArray[x]->name);
	}
}

void examine(itemList_t * ila, itemList_t * ilb, const char * what){
	int i;

	do{
		for (i = 0; i < ila->size; i++){
			if (striEqu(ila->itemArray[i]->name, what)) {
				puts(ila->itemArray[i]->examine);
				return;
			}
		}

		ila = ilb;
		ilb = NULL;
	}while (ila != NULL);

	puts("You examine that which is not there, and ponder the mysteries of existence.");
}

void parse(char * inp, char cmd[MAX_CMD_ARGS][BUF_LEN]){
	int i, j;
	char * start;

	const struct {
		char * from, * to;
	} synonyms[] = {
		{ "n", "north" },
		{ "s", "south" },
		{ "e", "east"  },
		{ "w", "west"  },
		{ "u", "up"    },
		{ "d", "down"  },
		{ "pickup", "take" },
		{ "get", "take" },
		{ "inventory", "inv" },
	};

	const char * ignores[] = {
		"a",
		"the",
	};

	for (i = 0; i < MAX_CMD_ARGS && *inp; i++) {
		/* find the token */
		while (isspace(*inp)) inp++;
		start = inp;
		while (!isspace(*inp) && *inp) inp++;

		/* check for ignores */
		for (j = 0; j < countof(ignores); j++) {
			if (!strncmp(ignores[j], start, inp-start)) break;
		}

		if (j != countof(ignores)) {
			i--;
			continue;
		}

		/* check for synonyms */
		for (j = 0; j < countof(synonyms); j++) {
			if (!strncmp(synonyms[j].from, start, inp-start)) {
				strcpy(cmd[i], synonyms[j].to);
				break;
			}
		}

		if (j != countof(synonyms)) {
			continue;
		}

		/* standard copy of the word */
		memcpy(cmd[i], start, inp-start);
		cmd[i][inp-start] = 0;
	}

	for (; i < MAX_CMD_ARGS; i++) {
		cmd[i][0] = 0;
	}
}

int main(){
	int quit=0;
	world_t * clarkson;
	char cmd[MAX_CMD_ARGS][BUF_LEN];
	char inp[BUF_LEN];
	inp[BUF_LEN-1] = 0;

	srand(time(NULL));

	clarkson = malloc(sizeof(world_t));
	worldInit(clarkson);

	/* Load data, create world */
	init(clarkson);
	watsup(clarkson->room);

	/* This loop takes commands until one works */
	while(!quit) {
		printf("What do? ");
		fgets(inp, BUF_LEN-1, stdin); /* Get commands */
		parse(inp, cmd);

		if (striEqu(cmd[0],"quit")) quit=1;
		if (striEqu(cmd[0],"north")) go(NORTH, &clarkson->room);
		if (striEqu(cmd[0],"south")) go(SOUTH, &clarkson->room);
		if (striEqu(cmd[0],"east")) go(EAST, &clarkson->room);
		if (striEqu(cmd[0],"west")) go(WEST, &clarkson->room);
		if (striEqu(cmd[0],"up")) go(UP, &clarkson->room);
		if (striEqu(cmd[0],"down")) go(DOWN, &clarkson->room);
		if (striEqu(cmd[0],"go")) go(direction(cmd[1]), &clarkson->room);
		if (striEqu(cmd[0],"take")) take(clarkson->room->items, clarkson->inventory, cmd[1]);
		if (striEqu(cmd[0],"drop")) drop(clarkson->room->items, clarkson->inventory, cmd[1]);
		if (striEqu(cmd[0],"look")) watsup(clarkson->room);
		if (striEqu(cmd[0],"inv")) showinv(clarkson->inventory);
		if (striEqu(cmd[0],"examine")) examine(clarkson->inventory, clarkson->room->items, cmd[1]);

		trigVerify(clarkson);
	}

	/* free() everything */
	for (quit = 0; quit < clarkson->allItems->capacity; quit++) {
		free(clarkson->allItems->itemArray[quit]->name);
		free(clarkson->allItems->itemArray[quit]->description);
		free(clarkson->allItems->itemArray[quit]->examine);
		free(clarkson->allItems->itemArray[quit]);
	}
	free(clarkson->allItems->itemArray);
	free(clarkson->allItems);

	/* TODO Fix freeing of triggers. */
	/* This kludgery reduces it to 81 bytes per run */
	free(clarkson->allTrigs[0].res->param[0].s);
	free(clarkson->allTrigs);
	

	for (quit = 0; quit < clarkson->numRooms; quit++) {
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

