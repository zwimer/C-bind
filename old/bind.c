#include "bind.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


/** Used to generate a unique name */
#define UNIQUE_NAME(base) INTERPRET_THEN_CAT(base, __COUNTER__)
#define INTERPRET_THEN_CAT(a, b) CAT(a, _, b)
#define CAT(a, b, c) a ## b ## c


void * bind_blank_1( const int n, void ** copy ) {
	static bool setup = false;
	static void ** args = NULL;
	static Bindable fn = NULL;
	static int size = 0;
	if ( ! setup ) {
		args = malloc((n-1) * sizeof(void*));
		fn = (Bindable) copy[0];
		setup = true;
		size = n - 1;
		for ( int i = 1; i < n; ++i ) {
			args[i-1] = copy[i];
		}
		return 0;
	}
	else {
		args = realloc( args, (n + size) * sizeof(void*) );
		for ( int i = 0; i < n; ++i ) {
			args[ size + i ] = copy[i];
		}
		return fn(n + size, args);
	}
}



Bound bind(const int n, ...) {

	void ** args = malloc(n * sizeof(void*));

	va_list copy;
	va_start(copy, n);
	for ( int i = 0; i < n; ++i ) {
		args[i] = va_arg(copy, void *);
	}
	va_end(copy);

	bind_blank_1(n, args);
	return bind_blank_1;
}
