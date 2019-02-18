#include "bind.h"
#include "bind_vec.h"
#include "bind_utilities.h"

#include <signal.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <syscall.h>
#include <sys/mman.h>


/***************************************************************/
/*                                                             */
/*                       Not From header                       */
/*                                                             */
/***************************************************************/


/** Ensure FN(ARGS) returns 0 */
#define ASSERT_ZERO(FN, ...) bind_assert(FN(__VA_ARGS__) == 0, ""#FN "() failed.");


// Globals

/** A global bind vec */
bind_vec * global_bv = NULL;
/** A global bind_lock for full systemv inokes' args*/
bind_lock_t * full_systemv_arg_lock = NULL;
/** The full_system_lock arg value protected by a lock */
bound_internals_t * full_systemv_arg_global;
/** A global bind_lock for full systemv inokes' return */
bind_lock_t * full_systemv_ret_lock = NULL;
/** The full_system_lock ret value protected by a lock */
ret_t full_systemv_ret_global;
/** An int representing the signal number used by the bind library internals
 *  This variable must be protected by all locks that invoke this signal */
int systemv_invoke_sig = SIGUSR2;


/** A macro used to set an argument, used for consistency */
#define SET_ARG(REG, INVOKE_SYM)       \
	"mov (%%rax), %%" # REG "\n\t"     \
	"add $8, %%rax\n\t"                \
	"dec %%r12\n\t"                    \
	"test %%r12, %%r12\n\t"            \
	"jz " # INVOKE_SYM "%=\n\t"

/** The signal handler used to invoke the systemv call */
void systemv_invoke_helper(int signo) {

	// Retrieve the stored bound_internals
	register bound_internals_t * bb asm("r15");
	bb = full_systemv_arg_global;
	bind_unlock(full_systemv_arg_lock);

	// Setup the registers and stack needed to
	// invoke the function with the desired arguments
	ret_t ret;
	asm(
		"mov %[argsv], %%rax\n\t" /* Args */
		"mov %[func], %%r11\n\t" /* Func */
		"mov %[n], %%r12\n\t" /* n */
		"mov %%rsp, %%r14\n\t" /* Store rsp */

		// Set first 6 args if they exist
		SET_ARG(rdi, invoke_sym)
		SET_ARG(rsi, invoke_sym)
		SET_ARG(rdx, invoke_sym)
		SET_ARG(rcx, invoke_sym)
		SET_ARG(r8, invoke_sym)
		SET_ARG(r9, invoke_sym)

		// Prep to set the rest of the args
		"mov %%r12, %%r13\n\t"
		"shl $3, %%r13\n\t"
		"sub $8, %%r13\n\t"
		"add %%r13, %%rax\n\t"

		// For the 7+ args, push them onto the stack in reverse
		"loop_sym%=:\n\t"
		"mov (%%rax), %%r13\n\t"
		"push %%r13\n\t"
		"sub $8, %%rax\n\t"
		"dec %%r12\n\t"
		"test %%r12, %%r12\n\t"
		"jnz loop_sym%=\n\t"

		// Invoke the function
		"invoke_sym%=:\n\t"
		"call %%r11\n\t"

		// Restore rsp and set the return value
		"mov %%r14, %%rsp\n\t"
		"mov %%rax, %[retv]"

		// Variables
		: [retv] "=r" (ret)
		: [argsv] "r" (bb->args),
		  [func] "r" (bb->fn),
		  [n] "r" (bb->n_total)
		:
	);

	// Return the value
	bind_lock(full_systemv_ret_lock);
	full_systemv_ret_global = ret;
	(void) signo;
}

/** get tid */
static long gettid() {
	return syscall(SYS_gettid);
}

/** tkill syscall */
static long tkill(int tid, int sig) {
	return syscall(SYS_tkill, tid, sig);
}

/** Invoke a systemv bound_internals_t on its store arguments */
__attribute__ ((noinline))
ret_t systemv_invoke(bound_internals_t * bb) {

	// Get the r11'th bound_internals then invoke it
	uint64_t index;
	asm volatile("mov %%r11, %0" : "=rm" (index));

	// Create a new sigaction
	struct sigaction new, old;
	ASSERT_ZERO(sigemptyset, & (new.sa_mask));
	ASSERT_ZERO(sigaddset, &(new.sa_mask), systemv_invoke_sig);
	new.sa_handler = & systemv_invoke_helper;
	new.sa_flags = 0;

	// Invoke the helper via a signal then restore the signal handler
	ASSERT_ZERO(sigaction, systemv_invoke_sig, &new, &old);
	bind_lock(full_systemv_arg_lock);
	full_systemv_arg_global = bb;
	ASSERT_ZERO(tkill, gettid(), systemv_invoke_sig);
	ASSERT_ZERO(sigaction, systemv_invoke_sig, &old, NULL);

	// Release the ret lock and return the return value
	ret_t ret = full_systemv_ret_global;
	bind_unlock(full_systemv_ret_lock);
	return ret;
}

/** Invoke a fully bound function
 *  The function index is stored in r11 */
__attribute__ ((noinline))
ret_t invoke_full_systemv_bound() {

	// Get the r11'th bound_internals then invoke it
	uint64_t index;
	asm volatile("mov %%r11, %0" : "=rm" (index) );
	bound_internals_t * bb = bv_get(global_bv, index);
	return systemv_invoke(bb);
}

/** Invoke a fully bound function
 *  The function index is stored in r11 */
__attribute__ ((noinline))
ret_t invoke_full_bound() {

	// Get the r11'th bound_internals then invoke it
	uint64_t index;
	asm("mov %%r11, %0" : "=rm" (index) );
	bound_internals_t * bb = bv_get(global_bv, index);
	return bb->fn(bb->args);
}

/** Invoke a partially bound function with the additional args of a1, ...
 *  The function index is stored in r11 */
__attribute__ ((noinline))
ret_t invoke_partial_bound( arg_t a1, ... ) {

	// Get the r11'th bound_internals
	uint64_t index;
	asm volatile("mov %%r11, %0" : "=rm" (index) );
	bound_internals_t * bb = bv_get(global_bv, index);

	// Copy in the newly provided args then invoke the function
	bb->args[bb->n_bound] = a1;
	va_list args;
	va_start(args, a1);
	for ( uint64_t i = bb->n_bound + 1; i < bb->n_total; ++i ) {
		bb->args[i] = va_arg(args, arg_t);
	}
	va_end(args);
	return bb->fn(bb->args);
}

/** Generate a stub for a bound function
 *  Use invoker as the invoke function */
void * gen_stub(const uint64_t index, void * const invoker) {

	/* Generate as code the following (intex syntax) amd64 assembly:
	 *  mov r11, invoker
	 *  push r11
	 *  mov r11, index
	 *  ret
	*/
	uint64_t size = 0x1000;
	char * func = mmap(0, size, PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	bind_assert(func != MAP_FAILED, "mmap() failed.");
	const char stub[] = "\x49\xBB" "AAAAAAAA" "\x41\x53\x49\xBB" "BBBBBBBB" "\xC3";
	memcpy(func, stub, sizeof(stub));
	memcpy(strstr(func, "BBBBBBBB"), & index, 8 );
	memcpy(strstr(func, "AAAAAAAA"), & invoker, 8 );
	bind_assert(mprotect(func, size, PROT_READ | PROT_EXEC) == 0, "mprotect() failed.");
	return func;
}

/** Generate a stub for a fully bound function */
FullBound gen_stub_full_systemv(const uint64_t index) {
	return gen_stub(index, invoke_full_systemv_bound);
}

/** Generate a stub for a fully bound function */
FullBound gen_stub_full(const uint64_t index) {
	return gen_stub(index, invoke_full_bound);
}

/** Generate a stub for a partially bound function */
PartBound gen_stub_partial(const uint64_t index) {
	return gen_stub(index, invoke_partial_bound);
}

/** A macro to set up a bound_internals
 *  This macro exists to ensure consistency */
#define STORE_ARGS_RETURN_BOUND(LAST_ARG)                                     \
	                                                                          \
	/** Initalize a bound_internals */                                        \
	bound_internals_t * bb = bind_safe_malloc(1, sizeof(bound_internals_t));  \
	bb->args = bind_safe_malloc(n_total, sizeof(arg_t));                      \
	bb->n_total = n_total;                                                    \
	bb->n_bound = n_bound;                                                    \
	bb->fn = func;                                                            \
	                                                                          \
	/** Copy in the passed arguments */                                       \
	va_list args;                                                             \
	va_start(args, LAST_ARG);                                                 \
	for ( uint64_t i = 0; i < n_bound; ++i ) {                                \
		bb->args[i] = va_arg(args, arg_t);                                    \
	}                                                                         \
	va_end(args);                                                             \
	                                                                          \
	/** Add the bound_internals to the global bind_vec */                     \
	const int index = bv_consume_add_blank(global_bv, bb);


/***************************************************************/
/*                                                             */
/*                         From header                         */
/*                                                             */
/***************************************************************/


// Ensure bind thread safety
void bind_setup() {
	global_bv = make_bind_vec();
	full_systemv_arg_lock = make_bind_lock();
	full_systemv_ret_lock = make_bind_lock();
}

// Fully bind a function to the n_total arguments
FullBound full_bind(Bindable func, const uint64_t n_total,  ...) {
	bind_assert(n_total > 0, "full_bind() called improperly");
	const uint64_t n_bound = n_total;
	STORE_ARGS_RETURN_BOUND(n_total);
	return gen_stub_full(index);
}

// Partially bind a function
PartBound partial_bind(Bindable func, const uint64_t n_total, const uint64_t n_bound, ...) {
	bind_assert(n_bound > 0, "partial_bind() called improperly");
	bind_assert(n_total > n_bound, "partial_bind() called improperly");
	STORE_ARGS_RETURN_BOUND(n_bound);
	return gen_stub_partial(index);
}

// Fully bind a function to the n_total arguments
// provided using the SystemV calling convention
FullBound full_systemv_bind(BindableSystemV func, const uint64_t n_total,  ...) {
	bind_assert(n_total > 0, "full_systemv_bind() called improperly");
	const uint64_t n_bound = n_total;
	STORE_ARGS_RETURN_BOUND(n_total);
	return gen_stub_full_systemv(index);
}

// Set the signal number that will be used by the bind library internally
void bind_set_signal_number(const int signo) {
	bind_assert(signo >= 0, "signals numbers cannot be negative");
	bind_lock(full_systemv_arg_lock);
	systemv_invoke_sig = signo;
	bind_unlock(full_systemv_arg_lock);
}

// Return the signal number used by the bind library internally
int bind_get_signal_number() {
	bind_lock(full_systemv_arg_lock);
	const int ret = systemv_invoke_sig;
	bind_unlock(full_systemv_arg_lock);
	return ret;
}

