#ifndef __BIND_DEFS_H__
#define __BIND_DEFS_H__

typedef void * (* FullBound) ( );
typedef void * (* PartBound) ( void * fst, ... );
typedef void * (* Bindable) ( void ** args );

#endif
