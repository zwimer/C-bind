#ifndef __BIND_H__
#define __BIND_H__

#include "bind_defs.h"

#include <stdint.h>

// TODO: delete
void * gen_stub_full(const uint64_t index);

FullBound full_bind(Bindable func, const uint64_t n_total,  ...);
PartBound partial_bind(Bindable func, const uint64_t n_total, const uint64_t n_bound, ...);

#endif
