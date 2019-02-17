#ifndef __BIND_DEFS_H__
#define __BIND_DEFS_H__

// TODO: make a variable
#define SYSTEMV_INVOKE_SIG 32

/** The argument type */
typedef void * arg_t;
/** The return type */
typedef void * ret_t;


/** A bindable function type */
typedef ret_t (* Bindable) ( arg_t * args );

/** A bindable function type for SystemV */
typedef void * BindableSystemV;


/** A fully bound function type */
typedef ret_t (* FullBound) ( );

/** A partially bound function type
 *  Because this can have multiple different
 *  type signatures it is left as a void * */
typedef void * PartBound;


#endif
