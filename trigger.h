#ifndef TRIGGER_H
#define TRIGGER_H

typedef struct condition condition_t;
typedef struct result result_t;
typedef struct trigger trigger_t;

#include "util.h"

typedef enum{
	C_NONE = -1,
	C_IN,
	C_RAND,
	C_HAS,
	C_MAX
} con_type;

typedef enum{
	R_NONE = -1,
	R_LINK,
	R_ECHO,
	R_ENABLE,
	R_DISABLE,
	R_TELEPORT,
	R_MAX
} res_type;

struct condition{
	con_type type;
	int param;
};

struct result{
	res_type type;
	union{
		char * s;
		int i;
	}param[3];
};

#define MAX_CON_CNT 8
#define MAX_RES_CNT 8

struct trigger{
	int enabled;
	condition_t con[MAX_CON_CNT];
	result_t res[MAX_RES_CNT];
	int next_con;
	int next_res;
};

/* These functions take a name of a condition/result and return the type */
con_type getCtype(const char *);
res_type getRtype(const char *);

/* Initializes the trigger */
void initTrigger(trigger_t *);

/* This function checks the list of triggers to see if any have their
   conditions fullfilled */
int trigVerify(world_t *);

#endif

