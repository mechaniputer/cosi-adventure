#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct room room_t;
typedef struct item item_t;

struct item {
	char * name;
	char * description;
	char ** actions;
};

struct room {
	char * description;
	room_t * east, * west, * north, * south;
	item_t * items;
	int item_count;
};

// global variable to hold room
room_t * room = NULL; 

// Loads data into structs
void init(){
	f = fopen("data", "r");
	assert(f != NULL);

	// ...

	fclose(f);
	return;
}

// Describes the situation
int watsup(){
	// This function should use a data file of rooms and a global room number to print the room description.
	// The room description should be able to change even within the same room.
	// Semi-random events should also be printed. (e.g. "Chris falls out of his chair")
	// Additionally, it should list objects in the room.
	printf("No rooms exist yet. Bummer.\n");
	return 0;
}

int main(){
	int dead=0,win=0,quit=0;
	char inp[80];
	inp[79] = 0;
	
	// Load data, create world
	init();

	// loop runs watsup() and inner loop takes commands until one works
	while(!dead && !win && !quit){
		watsup(); // Tell the user watsup.
		printf("What do? ");
		scanf ("%79s",inp); // Get commands

		if (!strncmp(inp,"quit",4)) quit=1; // Check if quitting
		
		// This is where a parse function would be called, and it would call other functons accordingly.
		// To get started, let's implement "go <dir>, take <obj>, look, eat <inv item>.
		
	}
	return 0;
}
