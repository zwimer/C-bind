#include "bind.h"
#include "check_bits.h"
#include "bind_vector.h"
#include "bind_utilities.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/mman.h>


bind_vec * get_global_bv() {
	static bind_vec * ret = NULL;
	if ( ret == NULL ) {
		ret = make_bind_vec();
	}
	return ret;
}

int gl; // TODO
void * invoke_full_bound() {
	uint64_t index;
	asm("mov %%r11, %0" : "=m" (index) );
	bind_blank_t * bb = bv_get(get_global_bv(), index);
	return bb->fn(bb->args);
}

void stub() { gl = 0; invoke_full_bound(); }

void * gen_stub_full(const uint64_t index) {
	/* mov r11, func
	push r11
	mov r11, index
	ret */
	uint64_t size = 0x1000;
	char * func = mmap(0, size, PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	bind_assert(func != MAP_FAILED, "mmap() failed.");
	const char stub[] = "\x49\xBB" "AAAAAAAA" "\x41\x53\x49\xBB" "BBBBBBBB" "\xC3";
	memcpy(func, stub, sizeof(stub));
	memcpy(strstr(func, "BBBBBBBB"), & index, 8 );
	void * (*invoke_ptr)() = & invoke_full_bound;
	memcpy(strstr(func, "AAAAAAAA"), & invoke_ptr, 8 );
	bind_assert(mprotect(func, size, PROT_READ | PROT_EXEC) == 0, "mprotect() failed.");
	return func;
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
