#include "bind_map.h"
#include "bind_utilities.h"

#include <string.h>

#include <stdio.h>

// Brevity
typedef unsigned long long ul;


// Constructor
bind_map_t * make_bind_map() {
	bind_map_t * ret = bind_safe_malloc(1, sizeof(bind_map_t));
	ret->cur_num_roots = 8;
	ret->roots = bind_safe_calloc(ret->cur_num_roots, sizeof(bind_map_node_t*));
	for ( ul i = 0; i < ret->cur_num_roots; ++i ) {
		ret->roots[i] = NULL;
	}
	return ret;
}


// The following enforce consistency
#define SELECT_CHILD(node, res) ((res < 0) ? node->left : node->right)
bind_map_node_t * new_node(const ul n_args, void ** arr) {
	bind_map_node_t * ret = bind_safe_calloc(1, sizeof(bind_map_node_t));
	ret->key = bind_safe_malloc(n_args, sizeof(void*));
	memcpy(ret->key, arr, n_args * sizeof(void*));
	return ret;
}

// [] operator, concats each of the n_args pointers in arr to form the key
bind_map_node_t * bind_map_get_value(bind_map_t * m, const ul n_args, void ** arr) {

	// Resize if needed
	if ( n_args >= m->cur_num_roots ) {
		m->roots = bind_safe_realloc(m->roots, 2 * m->cur_num_roots, sizeof(bind_map_node_t *));
		for ( ul i = m->cur_num_roots; i < 2 * m->cur_num_roots; ++i ) {
			m->roots[i] = NULL;
		}
		m->cur_num_roots *= 2;
		return bind_map_get_value(m, n_args, arr);
	}

	// Get the current root
	if (m->roots[n_args] == NULL) {
		m->roots[n_args] = new_node(n_args, arr);
		return m->roots[n_args];
	}
	else {
		bind_map_node_t * parent = NULL;
		bind_map_node_t * root_n = m->roots[n_args];
		int res = 0;
		while ( root_n != NULL ) {
			res = memcmp(root_n->key, (void*) arr, n_args * sizeof(void*));
			if ( res == 0 ) {
				break;
			}
			parent = root_n;
			root_n = SELECT_CHILD(root_n, res);
		}
		if ( root_n != NULL ) {
			return root_n;
		}
		else {
			bind_map_node_t * child = new_node(n_args, arr);
			if (res < 0) { parent->left = child; }
			else { parent->right = child; }
			return child;
		}
	}
}
