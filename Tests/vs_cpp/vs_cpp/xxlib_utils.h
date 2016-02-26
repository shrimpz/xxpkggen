#pragma execution_character_set("utf-8")
#pragma warning(disable:4819)

#pragma once
#include <stddef.h>     // size_t
#include <chrono>       // time...
#include <functional>   // std::function
#include "memory.h"     // alloc free

/*
macro / Funcs:
    XXLIB_IA
    XXLIB_64BIT
    XXLIB_ALIGN2/4/8/16/32/64
    XXLIB_LIKELY / UNLIKELY / INLINE / NOINLINE
    MIN
    MAX
    Sleep
    _countof
    XXLIB_ENUM_OPERATOR_EXT

Utils:
    Clz
    Round2n
    Calc2n
    AlignedAlloc / AlignedFree
    GetPrime
    Equals
    SetConsoleCursorPosition
    SetConsoleUtf8

class:
    HasParms
    TypeID
	Memmoveable
	Memcpy
    MaxSizeof
    Stopwatch
    IndexSequence / MakeIndexSequence
    ScopeGuard
*/



/************************************************************************/
/* XXLIB_IA                                                             */
/************************************************************************/

#if defined(__i386__) || defined(i386) || defined(_M_IX86) || defined(_X86_) || defined(__THW_INTEL) || defined(__x86_64__) || defined(_M_X64)
#    define XXLIB_IA
#endif


/************************************************************************/
/* XXLIB_64BIT                                                          */
/************************************************************************/

#if defined(__LP64__) || __WORDSIZE == 64 || defined(_WIN64) || defined(_M_X64)
#    define XXLIB_64BIT
#endif


/************************************************************************/
/* XX_ALIGN2/4/8/16/32/64                                               */
/************************************************************************/

#ifdef _MSC_VER
#    define XXLIB_ALIGN2( x )		    __declspec(align(2)) x
#    define XXLIB_ALIGN4( x )		    __declspec(align(4)) x
#    define XXLIB_ALIGN8( x )		    __declspec(align(8)) x
#    define XXLIB_ALIGN16( x )		    __declspec(align(16)) x
#    define XXLIB_ALIGN32( x )		    __declspec(align(32)) x
#    define XXLIB_ALIGN64( x )		    __declspec(align(64)) x
#else
#    define XXLIB_ALIGN2( x )          x __attribute__ ((aligned (2)))
#    define XXLIB_ALIGN4( x )          x __attribute__ ((aligned (4)))
#    define XXLIB_ALIGN8( x )          x __attribute__ ((aligned (8)))
#    define XXLIB_ALIGN16( x )         x __attribute__ ((aligned (16)))
#    define XXLIB_ALIGN32( x )         x __attribute__ ((aligned (32)))
#    define XXLIB_ALIGN64( x )         x __attribute__ ((aligned (64)))
#endif



/************************************************************************/
/* INLINE / NOINLINE / LIKELY / UNLIKELY                                */
/************************************************************************/

#ifdef _MSC_VER
#    define XXLIB_INLINE                    __forceinline
#    define XXLIB_NOINLINE                  __declspec(noinline)
#    define XX_LIKELY(x)                    (x)
#    define XX_UNLIKELY(x)                  (x)
#else
#    define XXLIB_INLINE                    inline __attribute__((__always_inline__))
#    define XXLIB_NOINLINE                  __attribute__((__noinline__))
#    define XX_UNLIKELY(x)                  __builtin_expect((x), 0)
#    define XX_LIKELY(x)                    __builtin_expect((x), 1)
#endif






/************************************************************************/
/* MIN / MAX                                                            */
/************************************************************************/

#ifndef MIN
#    define MIN( a, b )             ( (a) < (b) ? (a) : (b) )
#endif

#ifndef MAX
#    define MAX( a, b )             ( (a) > (b) ? (a) : (b) )
#endif



/************************************************************************/
/* Sleep                                                                */
/************************************************************************/

// this_thread sleep_for will be create a new thread for sleep, so....
// btw: windows need include windows.h
#ifdef _WIN32
#else
#    ifndef Sleep
#        define Sleep( x )          usleep( ( x ) * 1000 )
#    endif
#endif 


/************************************************************************/
/* _countof                                                             */
/************************************************************************/

// _countof
#ifndef _countof
template<typename T, size_t N>
int _countof(T const (&arr)[N])
{
    return N;
}
#endif



/************************************************************************/
/* XXLIB_ENUM_OPERATOR_EXT                                              */
/************************************************************************/

#define XXLIB_ENUM_OPERATOR_EXT( EnumTypeName )                                                                                                 \
    inline EnumTypeName operator+(EnumTypeName const &a, typename std::underlying_type<EnumTypeName>::type const &b)                            \
    {                                                                                                                                           \
        return EnumTypeName((typename std::underlying_type<EnumTypeName>::type)(a) + b);                                                        \
    }                                                                                                                                           \
    inline EnumTypeName operator+(EnumTypeName const &a, EnumTypeName const &b)                                                                 \
    {                                                                                                                                           \
        return EnumTypeName((typename std::underlying_type<EnumTypeName>::type)(a) + (typename std::underlying_type<EnumTypeName>::type)(b));   \
    }                                                                                                                                           \
    inline EnumTypeName operator-(EnumTypeName const &a, typename std::underlying_type<EnumTypeName>::type const &b)                            \
    {                                                                                                                                           \
        return EnumTypeName((typename std::underlying_type<EnumTypeName>::type)(a) - b);                                                        \
    }                                                                                                                                           \
    inline typename std::underlying_type<EnumTypeName>::type operator-(EnumTypeName const &a, EnumTypeName const &b)                            \
    {                                                                                                                                           \
        return (typename std::underlying_type<EnumTypeName>::type)(a) - (typename std::underlying_type<EnumTypeName>::type)(b);                 \
    }                                                                                                                                           \
    inline EnumTypeName operator++(EnumTypeName &a)                                                                                             \
    {                                                                                                                                           \
        a = EnumTypeName((typename std::underlying_type<EnumTypeName>::type)(a) + 1);                                                           \
        return a;                                                                                                                               \
    }




namespace xxlib
{

    /************************************************************************/
    /* Utils                                                                */
    /************************************************************************/

    namespace Utils
    {

        // 按 bit 数前面 0 的个数
        int Clz(size_t x);

        // 返回刚好大于 x 的 2^n 的值用于内存分配
        // 如果 x 本身 2^n 系 则返回原值
        size_t Round2n(size_t len);

        // 算一个数是 2 的几次方
        size_t Calc2n(size_t len);



        // 内存对齐分配 与 回收
        void* AlignedAlloc(size_t alignment, size_t size);

        // 内存对齐回收
        void AlignedFree(void* p);



        // 返回一个 内存占用为 2^N ( capacity * dataSize ) 的 大于等于原值的 质数 capacity
        int GetPrime(int capacity, int dataSize);


        // 包一下令指针对比并非直接对比指针本身
        template<typename T1, typename T2>
        inline bool Equals(T1 const &a, T2 const &b) { return a == b; }


        template<typename T>
        inline bool Equals(T *a, T *b) { return *a == *b; }



        // 设置控制台光标位置
        void SetConsoleCursorPosition(int x, int y);


        // 设置 windows 控制台编码用 65001, 字体用 Lucida Console 以完美输出 utf8 编码
        void SetConsoleUtf8();


        // 向控制台输出一段文本
        void ConsoleWrite(char* buf, size_t len);
    }



    /************************************************************************/
    /* HasParms                                                             */
    /************************************************************************/

    template<typename...TS>
    struct HasParms
    {
        static const bool value = sizeof...(TS) > 0;
    };


    /************************************************************************/
    /* TypeID                                                               */
    /************************************************************************/

    // for gcc explicit specialization template func
    template<typename T>
    class TypeID
    {
    };


    /************************************************************************/
    /* Memmoveable                                                          */
    /************************************************************************/

    // inherit this class that mean memory can be safe memmove
    class Memmoveable
    {
    };


	/************************************************************************/
	/* Memcpyable                                                           */
	/************************************************************************/

	// inherit this class that serial can be safe memcpy
	class Memcpyable
	{
	};




    /************************************************************************/
    /* MaxSizeof                                                            */
    /************************************************************************/

    template<typename T, typename... Args>
    class MaxSizeof
    {
    public:
        enum
        {
            value = sizeof(T) > MaxSizeof<Args...>::value
            ? sizeof(T)
            : MaxSizeof<Args...>::value
        };
    };
    template<typename T>
    class MaxSizeof < T >
    {
    public:
        enum
        {
            value = sizeof(T)
        };
    };



    /************************************************************************/
    /* Stopwatch                                                            */
    /************************************************************************/

	struct Stopwatch
	{
		Stopwatch() { Reset(); }
		inline void Reset() { beginTime = std::chrono::high_resolution_clock::now(); }
		inline long long operator()()
		{
			auto bak = beginTime;
			Reset();
			return std::chrono::duration_cast<std::chrono::milliseconds>(beginTime - bak).count();
		}
        inline long long micros()
        {
            auto bak = beginTime;
            Reset();
            return std::chrono::duration_cast<std::chrono::microseconds>(beginTime - bak).count();
        }
    private:
		std::chrono::time_point<std::chrono::high_resolution_clock> beginTime;
	};







    /************************************************************************/
    /* IndexSequence / MakeIndexSequence                                    */
    /************************************************************************/

    // for C++11 ( same as  C++14  std::make_index_sequence )
    template<size_t...T>
    struct IndexSequence
    {
        typedef IndexSequence<T...> type;
    };

    template<size_t...T>
    struct MakeIndexSequence;

    template<>
    struct MakeIndexSequence < 0 >
    {
        typedef IndexSequence<>::type type;
    };

    template<>
    struct MakeIndexSequence < 1 >
    {
        typedef IndexSequence<0>::type type;
    };

    template<size_t...TN>
    struct MakeIndexSequence < 0, TN... >
    {
        typedef typename IndexSequence<0, TN...>::type type;
    };

    template<size_t N>
    struct MakeIndexSequence < N >
    {
        typedef typename MakeIndexSequence<N - 2, N - 1>::type type;
    };

    template<size_t N, size_t... TN>
    struct MakeIndexSequence < N, TN... >
    {
        typedef typename MakeIndexSequence<N - 1, N, TN...>::type type;
    };







    /************************************************************************/
    /* ScopeGuard                                                           */
    /************************************************************************/

    struct ScopeGuard
    {
        template<typename T>
        ScopeGuard(T&& f) : func(std::forward<T>(f)) {}
        ~ScopeGuard() { Run(); }
        inline void RunAndCancel() { Run(); Cancel(); }
        inline void Run() { if (func) func(); }
        inline void Cancel() { func = nullptr; }
        template<typename T>
        inline void Set(T&& f) { func = std::forward<T>(f); }
    private:
        std::function<void()> func;
        ScopeGuard(ScopeGuard const&) = delete;
        ScopeGuard &operator=(ScopeGuard const&) = delete;
    };

}
