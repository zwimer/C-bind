#ifndef __BIND_VECTOR_H__
#define __BIND_VECTOR_H__

#include "bind_defs.h"
#include "bind_blank.h"

#include <stdint.h>


struct obscurred_mutex;
typedef struct bind_vec_t {
	bind_blank_t ** arr;
	uint64_t size, msize;
	struct obscurred_mutex * lock;
} bind_vec;

bind_vec * make_bind_vec();
uint64_t bv_consume_add_blank(bind_vec * const bv, bind_blank_t * const add);
bind_blank_t * bv_get(bind_vec * const bv, const uint64_t index);

#endif
