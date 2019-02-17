#ifndef __BIND_H__
#define __BIND_H__

#include "bind_defs.h"
#include <stdint.h>

/** Only allow 64 bit */
#ifndef __x86_64__
	*** Error: Only 64-bit currently supported! ***
#endif


/** Ensure bind thread safety */
void enable_multi_thread_binding();

/** Fully bind a function to the n_total arguments provided */
FullBound full_bind(Bindable func, const uint64_t n_total,  ...);

/** Partially bind a function that takes n_total argument to the n_bound arguments provided */
PartBound partial_bind(Bindable func, const uint64_t n_total, const uint64_t n_bound, ...);

/** Fully bind a function to the n_total arguments
 * provided using the SystemV calling convention */
FullBound full_bind_systemv(BindableSystemV func, const uint64_t n_total,  ...);


#endif
