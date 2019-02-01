#include "bind.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Vector structure
typedef struct vector_t {

	// Representation
	int data_size;
	int len;
	int max_len;
	void ** data;

	void (* push_back) ( const int len, void ** args );

} StringVec;

void vec_push_back( const int len, void ** args ) {
	if ( len != 2 ) {
		fprintf(stderr, "Bad args passed to push_back!");
	}
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

StringVec* make_vector() {
	StringVec * v = malloc(sizeof(StringVec));
	v->data_size = sizeof(void*);
	v->max_len = 0;
	v->data = NULL;
	v->len = 0;

	v->push_back = (void (*) (const int, void**)) bind(2, vec_push_back, v);

	return v;
}


int main() {

	// String to add
	char * buf = malloc(1000);
	strcpy(buf, "ABCD");

	// The vector
	StringVec * v = make_vector();
	v->push_back(1, & buf );

	// Print vector stats
	printf("v->data[0] = %s\n", (char*) v->data[0]);
}
