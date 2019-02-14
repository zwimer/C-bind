#include "bind_vector.h"
#include "bind_utilities.h"

#include <pthread.h>

#define BV_DEFAULT_SIZE 8


// An obscured mutex
struct obscurred_mutex { pthread_mutex_t m; };

void lock(bind_vec * const bv) {
	const int rv = pthread_mutex_lock( & bv->lock->m );
	bind_assert(rv == 0, "failed to aquire bind_vec lock.");
}

void unlock(bind_vec * const bv) {
	const int rv = pthread_mutex_unlock( & bv->lock->m );
	bind_assert(rv == 0, "failed to release bind_vec lock.");
}


bind_vec * make_bind_vec() {
	bind_vec * ret = bind_safe_malloc(1, sizeof(bind_vec));
	ret->msize = BV_DEFAULT_SIZE;
	ret->arr = bind_safe_malloc(ret->msize, sizeof(bind_blank_t*));
	ret->size = 0;
	ret->lock = bind_safe_malloc(1, sizeof(struct obscurred_mutex));
	const int rv = pthread_mutex_init(& ret->lock->m, 0);
	bind_assert(rv == 0, "bind_vec mutex init failed.");
	return ret;
}

uint64_t bv_consume_add_blank(bind_vec * const bv, bind_blank_t * const add) {
	lock(bv);
	if (bv->size == bv->msize) {
		bv->msize *= 2;
		bv->arr = bind_safe_realloc(bv->arr, bv->msize, sizeof(bind_blank_t*));
	}
	bv->arr[bv->size] = add;
	const uint64_t ret = bv->size;
	bv->size += 1;
	unlock(bv);
	return ret;
}

bind_blank_t * bv_get(bind_vec * const bv, const uint64_t index) {
	lock(bv);
	bind_assert(index < bv->size, "accessing illegal bv index");
	bind_blank_t * ret = bv->arr[index];
	unlock(bv);
	return ret;
}
