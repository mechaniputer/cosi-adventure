#include <stdlib.h>
#include <assert.h>
#include "load.h"

/* TODO this function should probably be split up to better manage it */
void loadDataFile(world_t * clarkson){

	/* This enum defines the sections of the datafile in order */
	enum {
		NONE,
		ROOM_DESC,
		ROOM_LINKS,
		OBJ_PROP,
		ROOM_OBJS,
		TRIGGERS
	} section = NONE;

	int x;
	int rv;
	int n, s, e, w;
	int rm, itm;
	char ch;
	FILE * f;
	con_type ctype;
	res_type rtype;
	condition_t * con;
	result_t * res;

	char str[160]; /* This limits the length of lines of data */
	str[159] = 0;

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
				assert(x < clarkson->numRooms);
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
				if (clarkson->allItems->size == clarkson->allItems->capacity) {
					addItem(clarkson->allItems);
				}
				assert(clarkson->allItems->size == x);

				clarkson->allItems->itemArray[x] = malloc(sizeof(item_t));
				clarkson->allItems->itemArray[x]->name = getstring('\n', f);

				fscanf(f, "%d ", &x);
				assert(clarkson->allItems->size == x);
				clarkson->allItems->itemArray[x]->description = getstring('\n', f);

				fscanf(f, "%d ", &x);
				assert(clarkson->allItems->size == x);
				clarkson->allItems->itemArray[x]->examine = getstring('\n', f);

				fscanf(f, "%d ", &x);
				assert(clarkson->allItems->size == x);
				fgets(str, 159, f);

				clarkson->allItems->size++;
				break;

			case ROOM_OBJS:
				assert(2 == fscanf(f, " %d %d\n", &rm, &itm));
				if(clarkson->allRooms[rm].items->size == clarkson->allRooms[rm].items->capacity) {
					addItem(clarkson->allRooms[rm].items);
				}
				clarkson->allRooms[rm].items->itemArray[clarkson->allRooms[rm].items->size] = clarkson->allItems->itemArray[itm];
				(clarkson->allRooms[rm].items->size)++;
				break;

			case TRIGGERS:
				if (x == clarkson->numTrigs) addTrig(clarkson);
				assert(x < clarkson->numTrigs);

				assert(2 == fscanf(f, "%c %159s ", &ch, str));
				assert(ch == 'r' || ch == 'c');

				if (ch == 'r') {
					/* Add trigger result */
					rtype = getRtype(str);
					res = clarkson->allTrigs[x].res + clarkson->allTrigs[x].next_res;
					res->type = rtype;
					clarkson->allTrigs[x].next_res++;

					switch (rtype){
					case R_LINK:
						assert(3 == fscanf(f, "%d %d %6s ", &res->param[0].i, &res->param[1].i, str));
						res->param[2].i = direction(str);
						assert(res->param[2].i != EAST_BY_EAST_WEST);
						break;
					case R_ECHO:
						res->param[0].s = getstring('\n', f);
						break;
					case R_ENABLE:
					case R_DISABLE:
						assert(1 == fscanf(f, "%d ", &res->param[0].i));
						break;
					case R_TELEPORT:
						assert(1 == fscanf(f, " %d\n", &res->param[0].i)); 
						break;
					default:
						assert(0);
					}
				}else{
					/* Add trigger condition */
					ctype = getCtype(str);
					con = clarkson->allTrigs[x].con + clarkson->allTrigs[x].next_con;
					con->type = ctype;
					clarkson->allTrigs[x].next_con++;

					switch (ctype){
					case C_IN:
					case C_RAND:
						assert(1 == fscanf(f, "%d ", &con->param));
						break;
					case C_HAS:
						assert(1 == fscanf(f, "%d ", &con->param));
						break;
					default:
						assert(0);
					}
				}
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
}
