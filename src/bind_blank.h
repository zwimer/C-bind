#ifndef __BIND_BLANK_H__
#define __BIND_BLANK_H__

#include "bind_defs.h"

#include <stdint.h>

typedef struct bind_blank_t {
	void ** args;
	Bindable fn;
	uint64_t n_total;
	uint64_t n_bound;
} bind_blank_t;

bind_blank_t * make_bind_blank();

#endif
