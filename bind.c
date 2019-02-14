#include "bind.h"
#include "check_bits.h"
#include "bind_vector.h"
#include "bind_utilities.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


bind_vec * get_global_bv() {
	static bind_vec * ret = NULL;
	if ( ret == NULL ) {
		ret = make_bind_vec();
	}
	return ret;
}

int gl; // TODO
void invoke_full_bound() {
	bind_blank_t * bb = bv_get(get_global_bv(), gl);
	bb->fn(bb->args);
}

void stub() { gl = 0; invoke_full_bound(); }

void * gen_stub_full(const uint64_t index) {
	(void) index;
	return stub;
}
void * gen_stub_partial(const uint64_t index) {
	(void) index;
	return stub;
}


#define STORE_ARGS_RETURN_BOUND(LAST_ARG)                                \
	bind_blank_t * bb = make_bind_blank();                               \
	bb->n_total = n_total;                                               \
	bb->n_bound = n_bound;                                               \
	bb->args = bind_safe_malloc(bb->n_total, sizeof(void*));             \
	bb->fn = func;                                                       \
	va_list args;                                                        \
	va_start(args, LAST_ARG);                                            \
	for ( uint64_t i = 0; i < n_bound; ++i ) {                           \
		bb->args[i] = va_arg(args, void *);                              \
	}                                                                    \
	va_end(args);                                                        \
	const int index = bv_consume_add_blank(get_global_bv(), bb);

FullBound full_bind(Bindable func, const uint64_t n_total,  ...) {
	const uint64_t n_bound = n_total;
	STORE_ARGS_RETURN_BOUND(n_total);
	return gen_stub_full(index);
}
PartBound partial_bind(Bindable func, const uint64_t n_total, const uint64_t n_bound, ...) {
	STORE_ARGS_RETURN_BOUND(n_bound);
	return gen_stub_partial(index);
}

#if 0


mov r11, index
jmp func,
asm(mov index, r11)

#endif

#if 0
#include <pthread.h>
#include <sys/mman.h>


// The lock used by a bind-stub invocation
pthread_mutex_t lock;

// The bind-index- must be protected by a mutex!
uint64_t cbind_stub_index;



Generate assembly:

	// TODO: error check syscalls

	// Stub index
	static uint64_t idx = 0;

	// Assembly
	char * const template =

		// Save registers
		"\x50\x57\x56"
		// gettid, mov rdi, rax
		"\x48\xb8\xba\x00\x00\x00\x00\x00\x00\x00" "\x0f\x05" "\x48\x89\xc7"

		// tkill -AAAAAAAA
		"\x48\xb8\xc8\x00\x00\x00\x00\x00\x00\x00"
		"\x48\xbe\xc8" "AAAAAAAA" // Signal
		"\x0f\x05"

		// mov [BBBBBBBB], CCCCCCCC
		"\x48\xb8" "BBBBBBBB" // & cbind_stub_index
		"\x49\xbb" "CCCCCCCC" // indx
		"\x4c\x89\x18"

		// Restore registrs
		"\x5e\x5f\x58";

		// jmp DDDDDDDD
		"\x49\xbb" "DDDDDDDD" // function
		"\x41\xff\xD3";

	// Create the function template
	const int size = 0x1000;
	char * func = mmap(0, size, PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	memset(func, 0xcc, size);
	memcpy(func, template, sizeof(template));

	// Fill in the blanks
	memcpy(strstr(func, "AAAAAAAA"), & bind_signal );
	uint64_t * stub_index_addr = & cbind_stub_index;
	memcpy(strstr(func, "BBBBBBBB"), & stub_index_addr );
	idx += 1;
	memcpy(strstr(func, "CCCCCCCC"), & idx );
	memcpy(strstr(func, "DDDDDDDD"), & invoked_function );

	// Protect the function
	mprotect(func, size, PROT_READ | PROT_EXEC);

	// Return the function
	return (void *) func;
}

#endif
#if 0

// Globals used to pass arguments to functions that take none
int bind_n_total_args;
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
	static bind_blank * bb = make_bind_blank();

	// Setup if needed
	if ( bb->fn == NULL ) {
		bb->size = bind_n_args;
		bb->total_size = bind_n_total_args;
		bb->args = bind_args;
		bb->fn = bind_fn;
		return 0;
	}

	// Otherwise invoke the function
	else if (bb->size + 1 == bb->total_size) {
		bb->args[bb->size] = fst;
		return bb->fn(bb->args);
	}
	else {
		va_list copy_me;
		va_start(copy_me, fst);
		bb->args[bb->size] = fst;
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

FullBound full_bind(void * func, const uint64_t n,  ...) {

	// Setup global arguments
	bind_n_total_args = n;
	bind_n_args = n;
	bind_fn = (Bindable) func;

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

PartBound partial_bind(void * func, const uint64_t n_total, const uint64_t n, ...) {

	// Setup global arguments
	bind_n_total_args = n_total;
	bind_n_args = n;
	bind_fn = (Bindable) func;

	// Setup global args argument
	bind_args = bind_safe_malloc(n_total, sizeof(void*));
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
#endif
