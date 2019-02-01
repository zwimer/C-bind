#include "bind_utilities.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


/*--------------------Safe functions*--------------------*/


/* This functions prints Msg and exits the program */
void bind_err(const char* Msg) {
	fprintf(stderr, "Error, %s failed\n", Msg);
	exit(EXIT_FAILURE);
}

/* This function calls perror then bind_err */
void bind_p_err(const char* Msg) {
	perror("\n");
	bind_err(Msg);
}

/* bind_err if !b */
void bind_assert(const int b, const char* Msg) {
	if ( !b ) bind_err(Msg);
}

/* This function errors and prints Msg if pnt is null */
void bind_mem_check(void *pnt, const char* Msg) {
	if (!pnt) bind_p_err(Msg);
}

/* Malloc, but check to see if failed */
void * bind_safe_malloc(size_t c, size_t s) {
	void * ret = malloc(c*s);
	bind_mem_check(ret, "malloc failed");
	return ret;
}

/* Calloc, but check to see if failed */
void * bind_safe_calloc(size_t c, size_t s) {
	void * ret = calloc(c, s);
	bind_mem_check(ret, "calloc failed");
	return ret;
}

/* Realloc, but check to see if failed */
void * bind_safe_realloc(void *p, size_t c, size_t s) {
	void * ret = realloc(p, c*s);
	bind_mem_check(ret, "realloc failed");
	return ret;
}
