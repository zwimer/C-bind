#ifndef __BIND_H__
#define __BIND_H__

typedef void * (* FullBound) ( );
typedef void * (* PartBound) ( void * fst, ... );
typedef void * (* Bindable) ( void ** args );


FullBound full_bind(void * func, const int n, ...);
PartBound partial_bind(void * func, const int total_n, const int n, ...);

#endif
