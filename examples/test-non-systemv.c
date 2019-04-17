#include "../src/bind.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Vector structure
typedef struct vector_t {
	size_t data_size, len, max_len;
	void ** data;
	void (* push_back) ( char * str );
	FullBound size;
} StringVec;


// A vector push_back function
void vec_push_back( arg_t * args ) {
	StringVec * vec = args[0];
	char * add = args[1];
	if ( vec->data == 0 ) {
		vec->max_len = 8;
		vec->data = malloc( vec->data_size * vec->max_len );
	}
	else if ( vec->len >= vec->max_len ) {
		vec->max_len *= 2;
		vec->data = realloc(vec->data, vec->max_len * vec->data_size);
	}
	vec->data[vec->len] = add;
	vec->len += 1;
}

// A vector size function
size_t vec_size( arg_t * args ) {
	return ((StringVec *) args[0])->len;
}

// A vector constructor
StringVec* make_vector() {
	StringVec * v = calloc(1, sizeof(StringVec));
	v->data_size = sizeof(void*);

	// Partially bind push_back
	v->push_back = partial_nonsystemv_bind((void*)vec_push_back, 2, 1, v);

	// Fully bind size
	v->size = full_nonsystemv_bind((void*)vec_size, 1, v);
	return v;
}


// Test C-bind for non-systemv functions
int main() {

	// String to add
	char * buf = malloc(0x1000);
	strcpy(buf, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

	// The vector
	StringVec * v = make_vector();
	for ( size_t i = 0; i < strlen(buf); ++i ) {
		v->push_back( buf + i );
	}

	// Printing
	for ( size_t i = 0; i < (size_t) v->size(); ++i ) {
		printf("v->data[%ld] = %s\n", i, (char*) v->data[i]);
	}
	printf("Size = %ld\n", (size_t) v->size());
	return EXIT_SUCCESS;
}
