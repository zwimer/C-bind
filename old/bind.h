#ifndef __BIND_H__
#define __BIND_H__


typedef enum bool_t { false = 0, true } bool;
typedef void * (* Bound) ( const int n, void ** args );
typedef void * (* Bindable) ( const int n, void ** args );


Bound bind(const int n, ...);


#endif
