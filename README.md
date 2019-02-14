# C-bind
Function binding in C !!!

# How to use

## Invocation
To invoke the bound version of `my_func`, one could simply invoke it as 
```C
bound_func( arg1, arg2, arg3 )
```
*Despite the odd function signature of the function to be bound, it can be invoked like a standard function.*
Note, if you pass extra unexpected arguments to `bound_func` they will be ignored.

## Signature
Functions to be bound must have a unique signature, however when calling them they may be called as standard functions.
To bind a function, it must have the following signature:
```C
void * my_func( void ** args )
```
The return value may simply be any other primitive so you cast it properly during the binding call.
You can think of `args` as an array of arguments!
Parsing the `args` array is the job of `my_func`. `my_func` must have a defined maximum number of 'arguments'. 
*That is, `my_func` must expect that no more `num_args` number of elements in the `args` array to be passed.*

## Fully binding
To fully bind a functon, invoke
```C
bound_func = full_bind( my_func, num_args, arg1, arg2, arg3 )
```
Here `num_args` is the number of elements in the `args` array `my_func` expects to be passed. Notice that if you pass in more arguments `num_args` they will be ignored.

### Example
```C
// This function expects exactly 1 argument!
void * id( void ** args ) { return args[0]; }
bound_func = full_bind( id, 1, /* Arguments begin */ 0x41414141 );
printf( "My id = %d\n", (int) bound_func() );  // Prints out 0x41414141
```

### Partial binding
To partially bind a function, invoke
```C
bound_func = partial_bind( my_func, num_args, num_args_to_bind, arg1, arg2 );
```
Here `num_args_to_bind` is the number of arguments currently being bound!

### Example
```C
// This function expects exactly 3 arguments
void * sum3( void ** args ) { return args[0] + args[1] + args[2]; }
bound_func = partial_bind( sum3, 3, 2, /* Arguments begin */ 100, 200 );
printf( "Total sum = %d\n", (int) bound_func(300) );  // Prints out 600
```

## Thread Safety
Yes :)

## How to test
To test that this works:
```bash
git clone https://github.com/zwimer/C-bind && \
mkdir C-bind/src/build && cd C-bind/src/build && \
cmake .. && make && \
./main.out
```

# Future Plans

### Removing the signature requirement
By emulating the `x86_64` calling convention I can remove the requirement of needing that special signature to bind a functon.

### Memory efficiency
Right now the `get_stub` function maps an entire page of memory per stub generated. Realistically it should only require just a few bytes. This can be done by placing multiple stub functions on the same page.

# Restrictions
Currently this only works on 64-bit `x86`.
