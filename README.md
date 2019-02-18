# C-bind: Function binding in C !!!
[![Build Status](https://api.travis-ci.org/zwimer/C-bind.svg?branch=master)](https://travis-ci.org/zwimer/C-bind)

# Table of Contents

1. [Requirements](#requirements)
1. [Usage](#usage)
  - [General](#general)
  - [SystemV] (#systemv)
  - [Non-SystemV] (#non-systemv)
1. [Testing](#testing)
1. [CI](#ci)
1. [Documentation](#documentation)
1. [Future Plans](#future-plans)

---

# Requirements
Currently `C-bind` requires `pthread` to be installed, and requires `x86_64`. `C-bind` has only been tested on Ubuntu 18.04`

# Usage

## General

### Setup
To setup the bind library, the `bind_setup` function must be invoked first!
```C
bind_setup();
```

### Invocation
To invoke the bound version of `my_func`, one could simply invoke it as 
```C
bound_func( arg1, arg2, arg3 )
```
*Note*: if you pass extra unexpected arguments to `bound_func` they will be ignored.

### Thread Safety
Yes.

### SystemV vs Non-SystemV

The most common calling convention of `x86_64` / `amd64` is called SystemV. Unless otherwise specified, most major compilers should compile your code to meet this standard. This library provides functions for binding functions that follow the SystemV calling convention; functions that are not compliant have an API for them is provided for them as well.

### Exceptions

1. This library cannot bind variadic functions
1. This library *may* fail if registers other than `rdi`, `rsi,` `rdx`, `rcx`, `r8`, and `r9` are used to pass arguments. This however is exceedingly rare.

## SystemV

Currently only full binding is support for SystemV.

### Signature
SystemV functions to be bound must return an object of type `ret_t` or smaller. Do not attempt to return a large struct as it may fail!

The return value may simply be any other primitive so you cast it properly during the binding call.
You can think of `args` as an array of arguments!
Parsing the `args` array is the job of `my_func`. `my_func` must have a defined maximum number of 'arguments'. 
*That is, `my_func` must expect that no more `num_args` number of elements in the `args` array to be passed.*

### Full binding
To fully bind a functon, invoke
```C
bound_func = full_bind( my_func, num_args, arg1, arg2, arg3 )
```
Here `num_args` is the number of elements in the `args` array `my_func` expects to be passed. If you pass in more arguments `num_args` they will be ignored.

### Full binding example
```C
// This function expects exactly 1 argument!
int sum( int a, int b ) { return a + b; }
bound_func = full_systemv_bind( id, 1, /* Arguments begin */ 1, 2 );
printf( "My id = %d\n", (int) bound_func() );  // Prints out 3
```

### Partial binding
This is currently not supported by `C-bind`.

## Non-SystemV

### Signature
Non-SystemV functions to be bound must have a unique signature, however when calling them they may be called as standard functions.
To bind a function, it must have the following signature:
```C
ret_t my_func( arg_t * args )
```
The return value may simply be any other primitive so you cast it properly during the binding call.
You can think of `args` as an array of arguments!
Parsing the `args` array is the job of `my_func`. `my_func` must have a defined maximum number of 'arguments'. 
*That is, `my_func` must expect that no more `num_args` number of elements in the `args` array to be passed.*

### Full binding
To fully bind a functon, invoke
```C
bound_func = full_bind( my_func, num_args, arg1, arg2, arg3 )
```
Here `num_args` is the number of elements in the `args` array `my_func` expects to be passed. If you pass in more arguments `num_args` they will be ignored.

### Full binding Example
```C
// This function expects exactly 1 argument!
ret_t id( arg_t * args ) { return args[0]; }
bound_func = full_bind( id, 1, /* Arguments begin */ 0x41414141 );
printf( "My id = %x\n", (int) bound_func() );  // Prints out 0x41414141
```

### Partial binding
To partially bind a function, invoke
```C
bound_func = partial_bind( my_func, num_args, num_args_to_bind, arg1, arg2 );
```
Here `num_args_to_bind` is the number of arguments currently being bound!

### Partial binding Example
```C
// This function expects exactly 3 arguments
ret_t sum3( arg_t * args ) { return args[0] + args[1] + args[2]; }
bound_func = partial_bind( sum3, 3, 2, /* Arguments begin */ 100, 200 );
printf( "Total sum = %d\n", (int) bound_func(300) );  // Prints out 600
```

---

# Testing
To test that this works:
```bash
git clone https://github.com/zwimer/C-bind && \
mkdir C-bind/src/build && cd C-bind/src/build && \
cmake .. && make && \
./test.out
```

# CI

Continuous Integration is provided by [Travis CI](https://travis-ci.org). To view the CI setup, click [here](https://travis-ci.org/zwimer/C-bind/).

# Documentation

Documentation is stored in on the `gh-pages` branch and hosted [here](https://zwimer.github.io/C-bind/docs/html/index.html) on [zwimer.com](https://zwimer.com). Documentation is generated via [doxygen](http://www.doxygen.nl/). To manually generate it install doxygen (from `apt-get` if you have it) then
```bash
cd C-bind && doxygen
```

# Future Plans

### Removing the signature requirement
Add partial binding for SystemV.

### Memory efficiency
Right now the `get_stub` function maps an entire page of memory per stub generated. Realistically it should only require just a few bytes. This can be done by placing multiple stub functions on the same page.
