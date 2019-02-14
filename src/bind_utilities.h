#ifndef __BIND_UTILITIES_HEADER_GAURD__
#define __BIND_UTILITIES_HEADER_GAURD__


#include <stdlib.h>


/*--------------------Safe functions*--------------------*/


/* This functions prints Msg and exits the program */
void bind_err(const char* Msg);

/* This function calls perror then err */
void bind_p_err(const char* Msg);

/* err if !b */
void bind_assert(const int b, const char* Msg);

/* This function errors and prints Msg if pnt is null */
void bind_mem_check(void *pnt, const char* Msg);

/* Malloc, but check to see if failed */
void * bind_safe_malloc(size_t c, size_t s);

/* Calloc, but check to see if failed */
void * bind_safe_calloc(size_t c, size_t s);

/* Realloc, but check to see if failed */
void * bind_safe_realloc(void *p, size_t c, size_t s);


#endif
