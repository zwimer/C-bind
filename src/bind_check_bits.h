// 64 bit
#ifdef __x86_64__
	#define BIT_64
#endif
#ifdef __ppc64__
	#define BIT_64
#endif
#ifdef _WIN64
	#define BIT_64
#endif
#ifdef __MINGW64__
	#define BIT_64
#endif

// 32 bit
#ifdef _WIN32
	#define BIT_32
#endif

// Check
#ifndef BIT_64

	*** Error: Only 64-bit currently supported! ***

#endif
