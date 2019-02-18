#include "bind_vec.h"
#include "bind_utilities.h"

#include <pthread.h>


// Create a bind_vec
bind_vec * make_bind_vec() {
	bind_vec * ret = bind_safe_malloc(1, sizeof(bind_vec));
	ret->msize = 8;
	ret->arr = bind_safe_malloc(ret->msize, sizeof(bound_internals_t*));
	ret->size = 0;
	ret->lock = make_bind_lock();
	return ret;
}

// Consume a bound_internals and add it to the bind_vec
uint64_t bv_consume_add_blank(bind_vec * const bv, bound_internals_t * const add) {

	// Do a thread-safe vector push_back
	bind_lock(bv->lock);
	if (bv->size == bv->msize) {
		bv->msize *= 2;
		bv->arr = bind_safe_realloc(bv->arr, bv->msize, sizeof(bound_internals_t*));
	}
	bv->arr[bv->size] = add;
	const uint64_t ret = bv->size;
	bv->size += 1;
	bind_unlock(bv->lock);
	return ret;
}

// Subscript operator
bound_internals_t * bv_get(bind_vec * const bv, const uint64_t index) {
	bind_lock(bv->lock);
	bind_assert(index < bv->size, "accessing illegal bv index");
	bound_internals_t * ret = bv->arr[index];
	bind_unlock(bv->lock);
	return ret;
}
