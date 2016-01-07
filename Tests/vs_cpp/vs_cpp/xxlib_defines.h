#pragma once

/* Common types for easy use */
typedef int8_t      int8, sbyte;
typedef int16_t     int16;
typedef int32_t     int32;
typedef int64_t     int64;
typedef uint8_t     uint8, byte;
typedef uint16_t    uint16, ushort;
typedef uint32_t    uint32, uint;
typedef uint64_t    uint64;


/*************************************************************/
// macros, class, funcs, templates summary:
/*************************************************************/
/*
	XX_IA            XX_IA32              XX_IA64
	XX_X64           XX_X86 / XX_X32      XX_32/64
	XX_DEBUG         XX_RELEASE
	XX_BE            XX_LE
	XX_IOS           XX_ANDROID           XX_LINUX             XX_WINDOWS
	XX_GCC           XX_MSVC              XX_RVCT

	XX_INLINE          XX_NOINLINE
	XX_LIKELY          XX_UNLIKELY
	ALIGN2/4/8/16/32

	XX_HAS_FUNC
	XX_DELETE_COPY_ASSIGN
	MIN, MAX

	msleep
	_countof

	TypeID
	Memmoveable

	MaxSizeof
	HasParms
*/







/* CPU(X86) - i386 / x86 32-bit */
#if defined(__i386__) || defined(i386) || defined(_M_IX86) || defined(_X86_) || defined(__THW_INTEL)
#define XX_IA32
#endif

/* CPU(X86_64) - AMD64 / Intel64 / x86_64 64-bit */
#if defined(__x86_64__) || defined(_M_X64)
#define XX_IA64
#endif

/* CPU - INTEL / AMD */
#if defined(XX_IA32) || defined(XX_IA64)
#define XX_IA
#endif

/* Compile - X86 / X64 */
#if defined(__LP64__) || defined(_WIN64) || defined(_M_X64)
#define XX_X64
#else
#define XX_X86
#endif

#if defined(XX_IA64) || defined(XX_X64)
#define XX_64
#else
#define XX_32
#endif

/* Compile - DEBUG / RELEASE */
#if defined(NDEBUG) && !defined(DEBUG)
#define XX_RELEASE
#else
#define XX_DEBUG
#endif

/* CPU - big endian / little endian */
#if defined(BIGENDIAN) || defined(BIG_ENDIAN)
#define XX_BE
#else
#define XX_LE
#endif

/* OS - windows, ios, android, linux 这里完全不考虑 mac osx */
#ifdef _WIN32
#define XX_WINDOWS
#elif defined(__APPLE__)
#define XX_IOS
#elif defined(__ANDROID__)
#define XX_ANDROID
#endif
#ifdef __linux
#define XX_LINUX
#endif

/* Compiler - ARM RealView Compilation Tools */
#if defined(__CC_ARM) || defined(__ARMCC__)
#define XX_RVCT
#endif

/* Compiler - GNU Compiler Collection */
/* --gnu option of the RVCT compiler also defines __GNUC__ */
#if defined(__GNUC__) && !defined(XX_RVCT)
#define XX_GCC
#endif

/* Compiler - Microsoft Visual C++ */
#if defined(_MSC_VER)
#define XX_MSVC
#endif


/* Compile - enforce inline */
#if defined(XX_GCC) && defined(XX_RELEASE) && !defined(__MINGW32__)
#define XX_INLINE inline __attribute__((__always_inline__))
#elif (defined(XX_MSVC) || defined(XX_GCC)) && defined(XX_RELEASE)
#define XX_INLINE __forceinline
#else
#define XX_INLINE inline
#endif

/* Compile - enforce noinline */
#if defined(XX_GCC)
#define XX_NOINLINE __attribute__((__noinline__))
#elif defined(XX_MSVC) || defined(XX_RVCT)
#define XX_NOINLINE __declspec(noinline)
#else
#define XX_NOINLINE
#endif

/* CPU - branch prediction: unlikely */
#ifndef XX_UNLIKELY
#if defined(XX_GCC)
#define XX_UNLIKELY(x) __builtin_expect((x), 0)
#else
#define XX_UNLIKELY(x) (x)
#endif
#endif

/* CPU - branch prediction: likely */
#ifndef XX_LIKELY
#if defined(XX_GCC)
#define XX_LIKELY(x) __builtin_expect((x), 1)
#else
#define XX_LIKELY(x) (x)
#endif
#endif

/* Memory address align */
#if defined(XX_MSVC)
#define XX_ALIGN2( x )		    __declspec(align(2)) x
#define XX_ALIGN4( x )		    __declspec(align(4)) x
#define XX_ALIGN8( x )		    __declspec(align(8)) x
#define XX_ALIGN16( x )		    __declspec(align(16)) x
#define XX_ALIGN32( x )		    __declspec(align(32)) x
#elif defined(XX_GCC)
#define XX_ALIGN2( x )          x __attribute__ ((aligned (2)))
#define XX_ALIGN4( x )          x __attribute__ ((aligned (4)))
#define XX_ALIGN8( x )          x __attribute__ ((aligned (8)))
#define XX_ALIGN16( x )         x __attribute__ ((aligned (16)))
#define XX_ALIGN32( x )         x __attribute__ ((aligned (32)))
#else
#define XX_ALIGN2( x )	        x
#define XX_ALIGN4( x )	        x
#define XX_ALIGN8( x )	        x
#define XX_ALIGN16( x )         x
#define XX_ALIGN32( x )         x
#endif




/*

SFINAE check menber function exists
sample：

XX_HAS_FUNC( FUNC_NAME_checker, T_MEMBER_FUNC_NAME, RETURN_TYPE ( T::* )( T_MEMBER_FUNC_PARMS ) const );

template<typename T>
typename std::enable_if<FUNC_NAME_checker<T>::value, RETURN_TYPE>::type FUNC_NAME_switch( T const& v )
{
	return v.T_MEMBER_FUNC_NAME( T_MEMBER_FUNC_PARMS );
};
template<typename T>
typename std::enable_if<!FUNC_NAME_checker<T>::value, RETURN_TYPE>::type FUNC_NAME_switch( T const& v )
{
	return................;
};

template<typename T>
RETURN_TYPE FUNC_NAME( T const& v )
{
	return FUNC_NAME_switch( v );
}


*/
#define XX_HAS_FUNC( CN, FN, FT )   \
template<typename CT>                                                               \
class CN                                                                            \
{                                                                                   \
    template<typename T, FT> class FuncMatcher;                                     \
    template<typename T> static char HasFunc( FuncMatcher<T, &T::FN>* );            \
    template<typename T> static int HasFunc(... );                                  \
public:                                                                             \
    static const bool value = sizeof( HasFunc<CT>( nullptr ) ) == sizeof(char);     \
}





#define XX_DELETE_COPY_ASSIGN( T )			\
    T( T const& ) = delete;				\
    T& operator=( T const& ) = delete;





#ifndef MIN
#define MIN( a, b )  ( (a) < (b) ? (a) : (b) )
#endif
#ifndef MAX
#define MAX( a, b )  ( (a) > (b) ? (a) : (b) )
#endif





// this_thread sleep_for will be create a new thread for sleep, so....
// btw: windows need include windows.h
#if defined(XX_WINDOWS)
#define msleep( x ) Sleep( x )
#else
#define msleep( x ) usleep( ( x ) * 1000 )
#endif 





// _countof
#ifndef _countof
//#if XX_ANDROID
//#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
//#else
template<typename T, size_t N>
int _countof( T const ( &arr )[ N ] )
{
	return N;
}
//#endif
#endif




// bytebuffer serial struct's base code
#define XX_SERIALABLE_STRUCT_BASE( CN )         \
CN() {}                                         \
CN( CN const& o ) = default;                    \
CN( CN && o ) = default;                        \
CN& operator=( CN const& o ) = default;         \
CN& operator=( CN && o ) = default;             \
void WriteTo( xxlib::ByteBuffer& bb ) const;    \
bool ReadFrom( xxlib::ByteBuffer& bb );




namespace xxlib
{

// for gcc explicit specialization template func
template<typename T>
class TypeID
{
};


// inherit this class that mean memory can be safe memmove
class Memmoveable
{
};


template<typename T, typename... Args>
class MaxSizeof
{
public:
	enum
	{
		value = sizeof( T ) > MaxSizeof<Args...>::value
		? sizeof( T )
		: MaxSizeof<Args...>::value
	};
};
template<typename T>
class MaxSizeof < T >
{
public:
	enum
	{
		value = sizeof( T )
	};
};





template<typename...TS>
class HasParms
{
public:
	static const bool value = sizeof...( TS ) > 0;
};

}

