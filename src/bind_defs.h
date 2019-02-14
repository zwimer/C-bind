#ifndef __BIND_DEFS_H__
#define __BIND_DEFS_H__

/** A bindable function type */
typedef void * (* Bindable) ( void ** args );

/** A fully bound function type */
typedef void * (* FullBound) ( );

/** A partially bound function type
 *  Because this can have multiple different
 *  type signatures it is left as a void * */
typedef void * PartBound;

#endif
