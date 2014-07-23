#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "trigger.h"

/* getCtype and getRtype could probably be consolidated */
con_type getCtype(const char * str){
	static const char * names[]={
		"in",
		"rand"
	};

	int i;
	for (i = 0; i < C_MAX; i++) {
		if (!strcmp(names[i], str)) return i;
	}

	return C_NONE;
}

res_type getRtype(const char * str){
	static const char * names[]={
		"link",
		"echo",
		"enable",
		"disable"
	};

	int i;
	for (i = 0; i < R_MAX; i++) {
		if (!strcmp(names[i], str)) return i;
	}

	return R_NONE;
}

void initTrigger(trigger_t * t){
	int i;

	t->enabled = 1;
	t->next_res = 0;
	t->next_con = 0;

	for (i = 0; i < MAX_CON_CNT; i++) {
		t->con[i].type = C_NONE;
	}

	for (i = 0; i < MAX_RES_CNT; i++) {
		t->res[i].type = R_NONE;
	}
}

static int condGood(const condition_t * c, world_t * w) {
	int rm = w->room - w->allRooms;
	assert(rm < w->numRooms && rm >= 0);

	switch (c->type){
	case C_NONE: return 1;
	case C_RAND: return !(rand() % c->param);
	case C_IN:   return c->param == rm;
	default: assert(0);
	}
	return 0;
}

static void resulting(const result_t * r, world_t * w) {
	switch (r->type){
	case R_NONE:
		break;
	case R_LINK:
		linkRoom(w, r->param[0].i, r->param[1].i, r->param[2].i);
		break;
	case R_ECHO:
		puts(r->param[0].s);
		break;
	case R_ENABLE:
	case R_DISABLE:
		w->allTrigs[r->param[0].i].enabled = (r->type == R_ENABLE);
		break;
	default:
		assert(0);
	}
}

int trigVerify(world_t * w){
	int i, j, cnt = 0;

	for (i = 0; i < w->numTrigs; i++) {
		if (w->allTrigs[i].enabled){
			/* Check all conditions */
			for (j = 0; j < MAX_CON_CNT; j++) {
				if (!condGood(&w->allTrigs[i].con[j], w)) break;
			}

			if (j == MAX_CON_CNT) {
				/* Trigger fires */
				for (j = 0; j < MAX_RES_CNT; j++) {
					resulting(&w->allTrigs[i].res[j], w);
				}
				cnt++;
			}
		}
	}

	return cnt;
}

