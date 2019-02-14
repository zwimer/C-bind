#include "bind.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0
// Vector structure
typedef struct vector_t {

	// Representation
	int data_size;
	int len;
	int max_len;
	void ** data;

	void (* push_back) ( char * str );
	int (* size) ();

} StringVec;

void vec_push_back( void ** args ) {
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

int vec_size( void ** args ) {
	return ((StringVec *) args[0])->len;
}

StringVec* make_vector() {
	StringVec * v = malloc(sizeof(StringVec));
	v->data_size = sizeof(void*);
	v->max_len = 0;
	v->data = NULL;
	v->len = 0;

	v->push_back = (void (*) (char *)) partial_bind(vec_push_back, 2, 1, v);
	v->size = (int (*) ()) full_bind(vec_size, 1, v);
	return v;
}


int main() {

	// String to add
	char * buf = malloc(0x1000);
	strcpy(buf, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

	// The vector
	StringVec * v = make_vector();
	for ( int i = 0; i < 32; ++i ) {
		v->push_back( buf + i );
	}

	// Printing
	printf("Size = %d\n", (int) v->size());
	for ( int i = 0; i < v->size(); ++i ) {
		printf("v->data[%d] = %s\n", i, (char*) v->data[i]);
	}
}
#else
void * same(void ** in) {
	return in[0];
}
int main() {
	void * (*f) () = full_bind(same, 1, 0x41424344);
	void * (*f2) () = full_bind(same, 1, 0x45464748);
	printf("%p\n", f());
	printf("%p\n", f2());
}
#endif
