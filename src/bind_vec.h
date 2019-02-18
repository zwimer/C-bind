#ifndef __BIND_VECTOR_H__
#define __BIND_VECTOR_H__

#include "bind_defs.h"
#include "bind_utilities.h"
#include "bound_internals.h"

#include <stdint.h>


/** A thread-safe vector of bound_internals */
typedef struct bind_vec_t {
	/** The data array */
	bound_internals_t ** arr;
	/** The size of the vector */
	uint64_t size;
	/** The max size of the vector allocated */
	uint64_t msize;
	/** An obscurred thread-safe lock */
	bind_lock_t * lock;
} bind_vec;

/** Create a bind_vec */
bind_vec * make_bind_vec();

/** Consume a bound_internals and append it to the vector */
uint64_t bv_consume_add_blank(bind_vec * const bv, bound_internals_t * const add);

/** The subscript operator */
bound_internals_t * bv_get(bind_vec * const bv, const uint64_t index);


#endif
