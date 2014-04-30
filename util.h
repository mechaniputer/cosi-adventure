#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

/* Reads a string from given file 'f' until delimiter 'd' or EOF is found
   Returns an allocated string read from file */
char * getstring(char d, FILE * f);

#endif
