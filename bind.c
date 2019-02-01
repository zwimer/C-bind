#include "bind.h"
#include "bind_utilities.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


// Globals used to pass arguments to functions that take none
int bind_total_n_args;
Bindable bind_fn;
void ** bind_args;
int bind_n_args;

// TODO: put into bind_list.h
#define NUM_BINDS 1
#define NUM_BINDS_NO_ARGS 1

void * bind_blank_1( void * fst, ... ); void * bind_blank_no_args_1();
PartBound bind_blank_arr[NUM_BINDS] = { bind_blank_1 };
FullBound bind_blank_no_args_arr[NUM_BINDS_NO_ARGS] = { bind_blank_no_args_1 };


void * bind_blank_1( void * fst, ... ) {

	// Static internals
	static void ** args = NULL;
	static Bindable fn = NULL;
	static int total_size = 0;
	static int size = 0;

	// Setup if needed
	if ( fn == NULL ) {
		size = bind_n_args;
		total_size = bind_total_n_args;
		args = bind_args;
		fn = bind_fn;
		return 0;
	}

	// Otherwise invoke the function
	else if (size + 1 == total_size) {
		args[size] = fst;
		return fn(args);
	}
	else {
		va_list copy_me;
		va_start(copy_me, fst);
		args[size] = fst;
		for ( int i = 1; i < (total_size - size); ++i ) {
			args[size + i] = va_arg(copy_me, void *);
		}
		va_end(copy_me);
		return fn(args);
	}
}

void * bind_blank_no_args_1() {

	// Static internals
	static void ** args = NULL;
	static Bindable fn = NULL;

	// Setup if needed
	if ( fn == NULL ) {
		args = bind_args;
		fn = bind_fn;
		return 0;
	}

	// Otherwise invoke the function
	else {
		return fn(args);
	}
}

FullBound full_bind(void * func, const int n,  ...) {

	// Setup global arguments
	bind_total_n_args = n;
	bind_n_args = n;
	bind_fn = func;

	// Setup global args argument
	bind_args = bind_safe_malloc(n, sizeof(void*));
	va_list copy_me;
	va_start(copy_me, n);
	for ( int i = 0; i < n; ++i ) {
		bind_args[i] = va_arg(copy_me, void *);
	}
	va_end(copy_me);

	// Bind the arguments to a function and return it
	static unsigned long long index = 0;
	bind_assert( index < NUM_BINDS, "full_bind() called too many times." );
	FullBound ret = bind_blank_no_args_arr[index++];
	ret();
	return ret;
}

PartBound partial_bind(void * func, const int total_n, const int n, ...) {

	// Setup global arguments
	bind_total_n_args = total_n;
	bind_n_args = n;
	bind_fn = func;

	// Setup global args argument
	bind_args = bind_safe_malloc(total_n, sizeof(void*));
	va_list copy_me;
	va_start(copy_me, n);
	for ( int i = 0; i < n; ++i ) {
		bind_args[i] = va_arg(copy_me, void *);
	}
	va_end(copy_me);

	// Bind the arguments to a function and return it
	static unsigned long long index = 0;
	bind_assert( index < NUM_BINDS, "partial_bind() called too many times." );
	PartBound ret = bind_blank_arr[index++];
	ret(0);
	return ret;
}
