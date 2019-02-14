#include "bind.h"
#include "bind_vec.h"
#include "bind_utilities.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/mman.h>


/** A function that returns the global bv
 *  The first call to this function is not thread safe
 *  After completion of the first call, subsequent calls are */
bind_vec * get_global_bv() {
	static bind_vec * ret = NULL;
	if ( ret == NULL ) {
		ret = make_bind_vec();
	}
	return ret;
}

/** Invoke a fully bound function
 *  The function index is stored in r11 */
void * invoke_full_bound() {

	// Get the r11'th bound_internals then invoke it
	uint64_t index;
	asm("mov %%r11, %0" : "=rm" (index) );
	bound_internals_t * bb = bv_get(get_global_bv(), index);
	return bb->fn(bb->args);
}

/** Invoke a partially bound function with the additional args of a1, ...
 *  The function index is stored in r11 */
void * invoke_partial_bound( void * a1, ... ) {

	// Get the r11'th bound_internals
	uint64_t index;
	asm("mov %%r11, %0" : "=rm" (index) );
	bound_internals_t * bb = bv_get(get_global_bv(), index);

	// Copy in the newly provided args then invoke the function
	bb->args[bb->n_bound] = a1;
	va_list args;
	va_start(args, a1);
	for ( uint64_t i = bb->n_bound + 1; i < bb->n_total; ++i ) {
		bb->args[i] = va_arg(args, void *);
	}
	va_end(args);
	return bb->fn(bb->args);
}

/** Generate a stub for a bound function
 *  Use invoker as the invoke function */
void * gen_stub(const uint64_t index, void * const invoker) {

	/* Generate as code the following (intex syntax) amd64 assembly:
	 *  mov r11, invoker
	 *  push r11
	 *  mov r11, index
	 *  ret
	*/
	uint64_t size = 0x1000;
	char * func = mmap(0, size, PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	bind_assert(func != MAP_FAILED, "mmap() failed.");
	const char stub[] = "\x49\xBB" "AAAAAAAA" "\x41\x53\x49\xBB" "BBBBBBBB" "\xC3";
	memcpy(func, stub, sizeof(stub));
	memcpy(strstr(func, "BBBBBBBB"), & index, 8 );
	memcpy(strstr(func, "AAAAAAAA"), & invoker, 8 );
	bind_assert(mprotect(func, size, PROT_READ | PROT_EXEC) == 0, "mprotect() failed.");
	return func;
}

/** Generate a stub for a fully bound function */
void * gen_stub_full(const uint64_t index) {
	return gen_stub(index, invoke_full_bound);
}

/** Generate a stub for a partially bound function */
void * gen_stub_partial(const uint64_t index) {
	return gen_stub(index, invoke_partial_bound);
}

/** A macro to set up a bound_internals
 *  This macro exists to ensure consistency */
#define STORE_ARGS_RETURN_BOUND(LAST_ARG)                                     \
	                                                                          \
	/** Initalize a bound_internals */                                        \
	bound_internals_t * bb = bind_safe_malloc(1, sizeof(bound_internals_t));  \
	bb->args = bind_safe_malloc(n_total, sizeof(void*));                      \
	bb->n_total = n_total;                                                    \
	bb->n_bound = n_bound;                                                    \
	bb->fn = func;                                                            \
	                                                                          \
	/** Copy in the passed arguments */                                       \
	va_list args;                                                             \
	va_start(args, LAST_ARG);                                                 \
	for ( uint64_t i = 0; i < n_bound; ++i ) {                                \
		bb->args[i] = va_arg(args, void *);                                   \
	}                                                                         \
	va_end(args);                                                             \
	                                                                          \
	/** Add the bound_internals to the global bind_vec */                     \
	const int index = bv_consume_add_blank(get_global_bv(), bb);

// Fully bind a function
FullBound full_bind(Bindable func, const uint64_t n_total,  ...) {
	const uint64_t n_bound = n_total;
	STORE_ARGS_RETURN_BOUND(n_total);
	return gen_stub_full(index);
}

// Partially bind a function
PartBound partial_bind(Bindable func, const uint64_t n_total, const uint64_t n_bound, ...) {
	bind_assert(n_total > n_bound, "partial_bind() called improperly");
	STORE_ARGS_RETURN_BOUND(n_bound);
	return gen_stub_partial(index);
}
