#include "xxlib.h"
namespace xxlib
{

static const int primes[] =
{
	1, 2, 3, 7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749, 65521, 131071, 262139, 524287,
	1048573, 2097143, 4194301, 8388593, 16777213, 33554393, 67108859, 134217689, 268435399, 536870909, 1073741789
};
int GetPrime( int n )
{
	return primes[ Calc2n( n ) ];
}


#ifdef __GNUC__
int Clz( size_t x )
{
#ifdef XX_X64
	return __builtin_clzl( x );
#else
	return __builtin_clz( x );
#endif
}
#elif defined(XX_IA) && defined(_MSC_VER)
int Clz( size_t x )
{
	unsigned long r = 0;
#ifdef XX_X64
	_BitScanReverse64( &r, x );
	return 63 - r;
#else
	_BitScanReverse( &r, x );
	return 31 - r;
#endif
}
#else
#ifdef XX_X64
#error not support
#endif
int Popcnt( size_t x )
{
	x -= ( ( x >> 1 ) & 0x55555555 );
	x = ( ( ( x >> 2 ) & 0x33333333 ) + ( x & 0x33333333 ) );
	x = ( ( ( x >> 4 ) + x ) & 0x0f0f0f0f );
	x += ( x >> 8 );
	x += ( x >> 16 );
	return x & 0x0000003f;
}
int Clz( size_t x )
{
	x |= ( x >> 1 );
	x |= ( x >> 2 );
	x |= ( x >> 4 );
	x |= ( x >> 8 );
	x |= ( x >> 16 );
	return 32 - Popcnt( x );
}
#endif


// 返回刚好大于 x 的 2^n 的值用于内存分配
// 如果 x 本身 2^n 系 则返回原值
size_t Round2n( size_t len )
{
#ifdef XX_X64
	int bits = 63 - Clz( len );
#else
	int bits = 31 - Clz( len );
#endif
	size_t rtv = size_t( 1 ) << bits;
	if( rtv == len ) return len;
	return rtv << 1;
}

// 算一个数是 2 的几次方
size_t Calc2n( size_t len )
{
#ifdef XX_X64
	return 63 - Clz( len );
#else
	return 31 - Clz( len );
#endif
}



}
