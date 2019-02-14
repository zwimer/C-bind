#ifndef __BIND_H__
#define __BIND_H__

#include "bind_defs.h"
#include "bind_check_bits.h"
#include <stdint.h>

FullBound full_bind(Bindable func, const uint64_t n_total,  ...);
PartBound partial_bind(Bindable func, const uint64_t n_total, const uint64_t n_bound, ...);

#endif
