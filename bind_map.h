#ifndef __BIND_MAP_H__
#define __BIND_MAP_H__


// The node type
typedef struct bind_map_node_t_struct {
	void * key;
	void * value;
	struct bind_map_node_t_struct * left;
	struct bind_map_node_t_struct * right;
} bind_map_node_t;

// The map type
typedef struct bind_map_t_struct {
	bind_map_node_t ** roots;
	unsigned long long cur_num_roots;
} bind_map_t;

// Constructor
bind_map_t * make_bind_map();

// [] operator, concats each of the n_args pointers in arr to form the key
bind_map_node_t * bind_map_get_value(bind_map_t * m,
	const unsigned long long n_args, void ** arr);

#endif
