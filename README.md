# C-bind: Dynamic function binding in C !!!
[![Build Status](https://api.travis-ci.org/zwimer/C-bind.svg?branch=master)](https://travis-ci.org/zwimer/C-bind)

# Table of Contents

1. [Requirements](#requirements)
1. [Usage](#usage)
	- [General](#general)
	- [SystemV](#systemv)
	- [Non-SystemV](#non-systemv)
1. [Examples](#examples)
1. [Compilation](#compilation)
1. [Docker](#docker)
1. [CI](#ci)
1. [Documentation](#documentation)
1. [Future Plans](#future-plans)

---

# Requirements
Currently `C-bind` requires `pthread` to be installed, and requires `x86_64`. `C-bind` has only been tested on `Ubuntu 18.04`, compiled with `gcc 7.3`.

# Usage

## General

### Includes
To link the `C-bind` library simply include the header file ```bind.h```.

### Invocation
To invoke the bound version of `my_func`, one could simply invoke it as 
```C
bound_func( arg1, arg2, arg3 );
```
*Note*: if you pass extra unexpected arguments to `bound_func` they will be ignored.

### SystemV vs Non-SystemV

The most common calling convention of `x86_64` / `amd64` is called SystemV. Unless otherwise specified, most major compilers should compile your code to meet this standard. This library provides functions for binding functions that follow the SystemV calling convention; functions that are not compliant have an API for them is provided for them as well. The default API of this library assumes SystemV functions are being bound.

### Thread Safe
Yes.

### Important notes
1. This library uses a signal internally. By default this is `SIGUSR2`, however the user may change this whenever. This signal handler is install when-needed and restored when not, so for a single threaded application this is perfectly safe. However, in a multi-threaded enviornment it is important to set this signal to some (valid) unused signal! This can be done with the `bind_set_signal_number` function.

### Restrictions
1. This library cannot bind variadic functions
1. This library *may* fail if registers other than `rdi`, `rsi,` `rdx`, `rcx`, `r8`, and `r9` are used to pass arguments. However this is exceedingly rare.
1. This library will only compile for `x86_64` / `amd64`

## SystemV

### Function Signature
SystemV functions to be bound must return an object of type `ret_t` (which should be 8 bytes) or smaller; `void` is also valid. Do not attempt to return a large struct as it may fail! As for the arguments of the function, there are no restrictions except that the function may not be variadic! Consequently, `my_func` must have a defined maximum number of 'arguments'. *That is, `my_func` must expect that no more `num_args` number of elements to be passed.* For more info look in the `bind.h` file.

### Full binding
To fully bind a functon, invoke
```C
bound_func = full_bind( my_func, num_args, arg1, arg2, arg3 );
```
Here `num_args` is the number arguments to pass to be passed to `my_func`. If you pass in more arguments `num_args` they will be ignored.

### Full binding example
```C
int sum( int a, int b ) { return a + b; }
bound_func = full_bind( sum, 2, /* Arguments begin */ 1, 2 );
printf( "sum(1,2) = %d", (int) bound_func() );
```
The output of this code is: `sum(1,2) = 3`.

### Partial binding
To partially bind a function, invoke
```C
bound_func = partial_bind( my_func, num_args, num_args_to_bind, arg1, arg2 );
```
Here `num_args_to_bind` is the number of arguments currently being bound!

It is worth noting that fully binding a function via a partial bind is supported.

### Partial binding Example
```C
int sum3(int a, int b, int c) { return args[0] + args[1] + args[2]; }
bound_func = partial_bind( sum3, 3, 2, /* Arguments begin */ 100, 200 );
printf( "Total sum = %d", (int) bound_func(300) );
```
The output of this code is: `Total sum = 600`

## Non-SystemV

### Signature
Non-SystemV functions to be bound must have a unique signature, however when calling them they may be called as standard functions.
To bind a function, it must have the following signature:
```C
ret_t my_func( arg_t * args );
```
The return value may simply be any other primitive so you cast it properly during the binding call.
You can think of `args` as an array of arguments!
The function being bound may not be variadic, consequently, `my_func` must have a defined maximum number of 'arguments'. *That is, `my_func` must expect that no more `num_args` number of elements to be passed.* For more info look in the `bind.h` file.
Parsing the `args` array is the job of `my_func`.

### Full binding
To fully bind a functon, invoke
```C
bound_func = full_bind( my_func, num_args, arg1, arg2, arg3 );
```
Here `num_args` is the number of elements in the `args` array `my_func` expects to be passed. If you pass in more arguments `num_args` they will be ignored.

### Full binding Example
```C
ret_t sum( arg_t * args ) { return args[0] + args[1]; }
bound_func = full_systemv_bind( sum, 2, /* Arguments begin */ 1, 2 );
printf( "sum(1,2) = %d", (int) bound_func() );
```
The output of this code is: `sum(1,2) = 3`.

### Partial binding
To partially bind a function, invoke
```C
bound_func = partial_bind( my_func, num_args, num_args_to_bind, arg1, arg2 );
```
Here `num_args_to_bind` is the number of arguments currently being bound!

It is worth noting that fully binding a function via a partial bind is supported.

### Partial binding Example
```C
ret_t sum3( arg_t * args ) { return args[0] + args[1] + args[2]; }
bound_func = partial_systemv_bind( sum3, 3, 2, /* Arguments begin */ 100, 200 );
printf( "Total sum = %d", (int) bound_func(300) );
```
The output of this code is: `Total sum = 600`

---

# Examples
To test the examples first compile the code
```bash
git clone https://github.com/zwimer/C-bind && \
mkdir C-bind/build && cd C-bind/build && \
cmake ../examples && make
```

After that, run your desired test. Either `./test.out` (for SystemV tests) or `./test-non-systemv.out`.

# Compilation
This library should be compiled as a shared object without optimizations! On `gcc` version`7.3`, this library is able to compile even with the `-O3` flag. If you experience issues however, try compiling at a lower optimization level. See the `CMakeLists.txt` file in the `examples` directory for an example.

# Docker
A `Dockerfile` is provided with `C-bind` and example cases installed and build. The image is hosted [here](https://cloud.docker.com/repository/docker/zwimer/c-bind) on [docker.com](https://docker.com). To pull the image just execute:
```bash
docker pull zwimer/c-bind
```
To run the container  simply execute:
```bash
docker run --rm -it zwimer/c-bind
```
If you would like to build the container yourself execute:
```bash
git clone https://github.com/zwimer/C-bind && \
cd C-bind && \
docker build -t zwimer/c-bind .
```

# CI

Continuous Integration is provided by [Travis CI](https://travis-ci.org). To view the CI setup, click [here](https://travis-ci.org/zwimer/C-bind/).

# Documentation

Documentation is stored in on the `gh-pages` branch and hosted [here](https://zwimer.github.io/C-bind/docs/html/index.html) on [zwimer.com](https://zwimer.com). Documentation is generated via [doxygen](http://www.doxygen.nl/). To manually generate it install doxygen (from `apt-get` if you have it) then
```bash
cd C-bind && doxygen
```

# Future Plans
1. Right now hidden in the internals of how the binding mechanism, the `get_stub` function maps an entire page of memory per stub generated. Realistically it should only require just a few bytes. This can be done by placing multiple stub functions on the same page.
