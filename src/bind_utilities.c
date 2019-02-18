#include "bind_utilities.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>


/** A simple mutex */
struct bind_lock_t {
	pthread_mutex_t m;
};

// This functions prints Msg and exits the program
void bind_err(const char * Msg) {
	fprintf(stderr, "Error: %s\n", Msg);
	exit(EXIT_FAILURE);
}

// This function calls perror then bind_err
void bind_p_err(const char * Msg) {
	perror("\n");
	bind_err(Msg);
}

// bind_err if !b
void bind_assert(const int b, const char * Msg) {
	if (!b) bind_err(Msg);
}

// This function errors and prints Msg if pnt is null
void bind_mem_check(void * pnt, const char * Msg) {
	if (!pnt) bind_p_err(Msg);
}

// Malloc, but check to see if failed
void * bind_safe_malloc(size_t c, size_t s) {
	void * ret = malloc(c*s);
	bind_mem_check(ret, "malloc failed");
	return ret;
}

// Realloc, but check to see if failed
void * bind_safe_realloc(void * p, size_t c, size_t s) {
	void * ret = realloc(p, c*s);
	bind_mem_check(ret, "realloc failed");
	return ret;
}

// Create a bind_lock
bind_lock_t * make_bind_lock() {
	bind_lock_t * ret = bind_safe_malloc(1, sizeof(bind_lock_t));
	const int rv = pthread_mutex_init( & (ret->m), 0);
	bind_assert(rv == 0, "bind_vec mutex init failed.");
	return ret;
}

// Lock a bind_lock
void bind_lock(bind_lock_t * const l) {
	const int rv = pthread_mutex_lock( & (l->m) );
	bind_assert(rv == 0, "failed to aquire bind_vec lock.");
}

// Unlock a bind_vec's mutex
void bind_unlock(bind_lock_t * const l) {
	const int rv = pthread_mutex_unlock( & (l->m) );
	bind_assert(rv == 0, "failed to release bind_vec lock.");
}

