#ifndef __bound_internals_H__
#define __bound_internals_H__

#include "bind_defs.h"

#include <stdint.h>

/** A structure that holds all data needed to bind a function */
typedef struct bound_internals_t {
	/** A data array to store the bound arguments */
	arg_t * args;
	/** The function being bound */
	Bindable fn;
	/** The total number of arguments */
	uint64_t n_total;
	/** The number of arguments currently bound */
	uint64_t n_bound;
} bound_internals_t;

#endif
