#include "bind.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// A sum function that takes in 8 arguments
ret_t sum(
	/* Stored in registers */ int a, int b, int c, int d, int e, int f,
	/* Stored on the stack */ int g, int h
	) {

	printf("Register args = %d, %d, %d, %d, %d, %d\n", a, b, c, d, e, f);
	printf("Stack args = %d, %d\n", g, h);
	return a+b+c+d+e+f+g+h;
}


// Test C-bind for systemv functions
int main() {
	bind_setup();

	// Bind the sum function to a few arguments
	FullBound my_func = full_systemv_bind( (void*) sum, 8,
		/* The argumetns to bind to sum */
		1, 2, 3, 4, 5, 6, 7, 8
	);

	puts("(1+2+3+4+5+6+7+8) = 34");

	// Call my_func()
	puts("Calling my_func() now!\n");
	const int rv = my_func();

	printf("\nmy_func() = sum(1,2,3,4,5,6,7,8) = %d\n", rv);
	return EXIT_SUCCESS;
}
