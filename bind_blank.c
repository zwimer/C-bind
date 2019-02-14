#include "bind_blank.h"
#include "bind_utilities.h"

bind_blank_t * make_bind_blank() {
	bind_blank_t * ret = bind_safe_calloc(1, sizeof(bind_blank_t));
	return ret;
}
