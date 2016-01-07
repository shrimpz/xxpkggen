#include "xxlib.h"
namespace xxlib
{


int GetToStringMaxLength( uint8   v )
{
	return 3;
}
int GetToStringMaxLength( uint16  v )
{
	return 5;
}
int GetToStringMaxLength( uint    v )
{
	return 10;
}
int GetToStringMaxLength( uint64  v )
{
	return 19;
}
int GetToStringMaxLength( int8    v )
{
	return 4;
}
int GetToStringMaxLength( int16   v )
{
	return 6;
}
int GetToStringMaxLength( int     v )
{
	return 11;
}
int GetToStringMaxLength( int64   v )
{
	return 20;
}
int GetToStringMaxLength( double  v )
{
	return 19;
}
int GetToStringMaxLength( float   v )
{
	return 10;
}
int GetToStringMaxLength( bool    v )
{
	return 5;
}
int GetToStringMaxLength( char    v )
{
	return 1;
}
int GetToStringMaxLength( char const* v )
{
	return (int)strlen( v );
}
//#if defined(XX_LINUX) && defined(XX_64)
//int GetToStringMaxLength( size_t    v )
//{
//	return 19;
//}
//#endif


// the code is copy from following URL:
// https://github.com/miloyip/itoa-benchmark/tree/master/src

const char gDigitsLut[ 200 ] = {
	'0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9',
	'1', '0', '1', '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8', '1', '9',
	'2', '0', '2', '1', '2', '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
	'3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3', '7', '3', '8', '3', '9',
	'4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5', '4', '6', '4', '7', '4', '8', '4', '9',
	'5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
	'6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8', '6', '9',
	'7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9',
	'8', '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9',
	'9', '0', '9', '1', '9', '2', '9', '3', '9', '4', '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'
};

// Branching for different cases (forward)
// Use lookup table of two digits

int u32toa_branchlut( uint value, char* buffer )
{
	auto bak = buffer;
	if( value < 10000 )
	{
		const uint d1 = ( value / 100 ) << 1;
		const uint d2 = ( value % 100 ) << 1;

		if( value >= 1000 )
			*buffer++ = gDigitsLut[ d1 ];
		if( value >= 100 )
			*buffer++ = gDigitsLut[ d1 + 1 ];
		if( value >= 10 )
			*buffer++ = gDigitsLut[ d2 ];
		*buffer++ = gDigitsLut[ d2 + 1 ];
	}
	else if( value < 100000000 )
	{
		// value = bbbbcccc
		const uint b = value / 10000;
		const uint c = value % 10000;

		const uint d1 = ( b / 100 ) << 1;
		const uint d2 = ( b % 100 ) << 1;

		const uint d3 = ( c / 100 ) << 1;
		const uint d4 = ( c % 100 ) << 1;

		if( value >= 10000000 )
			*buffer++ = gDigitsLut[ d1 ];
		if( value >= 1000000 )
			*buffer++ = gDigitsLut[ d1 + 1 ];
		if( value >= 100000 )
			*buffer++ = gDigitsLut[ d2 ];
		*buffer++ = gDigitsLut[ d2 + 1 ];

		*buffer++ = gDigitsLut[ d3 ];
		*buffer++ = gDigitsLut[ d3 + 1 ];
		*buffer++ = gDigitsLut[ d4 ];
		*buffer++ = gDigitsLut[ d4 + 1 ];
	}
	else
	{
		// value = aabbbbcccc in decimal

		const uint a = value / 100000000; // 1 to 42
		value %= 100000000;

		if( a >= 10 )
		{
			const unsigned i = a << 1;
			*buffer++ = gDigitsLut[ i ];
			*buffer++ = gDigitsLut[ i + 1 ];
		}
		else
			*buffer++ = '0' + static_cast<char>( a );

		const uint b = value / 10000; // 0 to 9999
		const uint c = value % 10000; // 0 to 9999

		const uint d1 = ( b / 100 ) << 1;
		const uint d2 = ( b % 100 ) << 1;

		const uint d3 = ( c / 100 ) << 1;
		const uint d4 = ( c % 100 ) << 1;

		*buffer++ = gDigitsLut[ d1 ];
		*buffer++ = gDigitsLut[ d1 + 1 ];
		*buffer++ = gDigitsLut[ d2 ];
		*buffer++ = gDigitsLut[ d2 + 1 ];
		*buffer++ = gDigitsLut[ d3 ];
		*buffer++ = gDigitsLut[ d3 + 1 ];
		*buffer++ = gDigitsLut[ d4 ];
		*buffer++ = gDigitsLut[ d4 + 1 ];
	}
	//*buffer++ = '\0';
	return int( buffer - bak );
}

int i32toa_branchlut( int value, char* buffer )
{
	uint u = static_cast<uint>( value );
	if( value < 0 )
	{
		*buffer++ = '-';
		u = ~u + 1;
		return u32toa_branchlut( u, buffer ) + 1;
	}
	return u32toa_branchlut( u, buffer );
}

int u64toa_branchlut( uint64 value, char* buffer )
{
	auto bak = buffer;
	if( value < 100000000 )
	{
		uint v = static_cast<uint>( value );
		if( v < 10000 )
		{
			const uint d1 = ( v / 100 ) << 1;
			const uint d2 = ( v % 100 ) << 1;

			if( v >= 1000 )
				*buffer++ = gDigitsLut[ d1 ];
			if( v >= 100 )
				*buffer++ = gDigitsLut[ d1 + 1 ];
			if( v >= 10 )
				*buffer++ = gDigitsLut[ d2 ];
			*buffer++ = gDigitsLut[ d2 + 1 ];
		}
		else
		{
			// value = bbbbcccc
			const uint b = v / 10000;
			const uint c = v % 10000;

			const uint d1 = ( b / 100 ) << 1;
			const uint d2 = ( b % 100 ) << 1;

			const uint d3 = ( c / 100 ) << 1;
			const uint d4 = ( c % 100 ) << 1;

			if( value >= 10000000 )
				*buffer++ = gDigitsLut[ d1 ];
			if( value >= 1000000 )
				*buffer++ = gDigitsLut[ d1 + 1 ];
			if( value >= 100000 )
				*buffer++ = gDigitsLut[ d2 ];
			*buffer++ = gDigitsLut[ d2 + 1 ];

			*buffer++ = gDigitsLut[ d3 ];
			*buffer++ = gDigitsLut[ d3 + 1 ];
			*buffer++ = gDigitsLut[ d4 ];
			*buffer++ = gDigitsLut[ d4 + 1 ];
		}
	}
	else if( value < 10000000000000000 )
	{
		const uint v0 = static_cast<uint>( value / 100000000 );
		const uint v1 = static_cast<uint>( value % 100000000 );

		const uint b0 = v0 / 10000;
		const uint c0 = v0 % 10000;

		const uint d1 = ( b0 / 100 ) << 1;
		const uint d2 = ( b0 % 100 ) << 1;

		const uint d3 = ( c0 / 100 ) << 1;
		const uint d4 = ( c0 % 100 ) << 1;

		const uint b1 = v1 / 10000;
		const uint c1 = v1 % 10000;

		const uint d5 = ( b1 / 100 ) << 1;
		const uint d6 = ( b1 % 100 ) << 1;

		const uint d7 = ( c1 / 100 ) << 1;
		const uint d8 = ( c1 % 100 ) << 1;

		if( value >= 1000000000000000 )
			*buffer++ = gDigitsLut[ d1 ];
		if( value >= 100000000000000 )
			*buffer++ = gDigitsLut[ d1 + 1 ];
		if( value >= 10000000000000 )
			*buffer++ = gDigitsLut[ d2 ];
		if( value >= 1000000000000 )
			*buffer++ = gDigitsLut[ d2 + 1 ];
		if( value >= 100000000000 )
			*buffer++ = gDigitsLut[ d3 ];
		if( value >= 10000000000 )
			*buffer++ = gDigitsLut[ d3 + 1 ];
		if( value >= 1000000000 )
			*buffer++ = gDigitsLut[ d4 ];
		if( value >= 100000000 )
			*buffer++ = gDigitsLut[ d4 + 1 ];

		*buffer++ = gDigitsLut[ d5 ];
		*buffer++ = gDigitsLut[ d5 + 1 ];
		*buffer++ = gDigitsLut[ d6 ];
		*buffer++ = gDigitsLut[ d6 + 1 ];
		*buffer++ = gDigitsLut[ d7 ];
		*buffer++ = gDigitsLut[ d7 + 1 ];
		*buffer++ = gDigitsLut[ d8 ];
		*buffer++ = gDigitsLut[ d8 + 1 ];
	}
	else
	{
		const uint a = static_cast<uint>( value / 10000000000000000 ); // 1 to 1844
		value %= 10000000000000000;

		if( a < 10 )
			*buffer++ = '0' + static_cast<char>( a );
		else if( a < 100 )
		{
			const uint i = a << 1;
			*buffer++ = gDigitsLut[ i ];
			*buffer++ = gDigitsLut[ i + 1 ];
		}
		else if( a < 1000 )
		{
			*buffer++ = '0' + static_cast<char>( a / 100 );

			const uint i = ( a % 100 ) << 1;
			*buffer++ = gDigitsLut[ i ];
			*buffer++ = gDigitsLut[ i + 1 ];
		}
		else
		{
			const uint i = ( a / 100 ) << 1;
			const uint j = ( a % 100 ) << 1;
			*buffer++ = gDigitsLut[ i ];
			*buffer++ = gDigitsLut[ i + 1 ];
			*buffer++ = gDigitsLut[ j ];
			*buffer++ = gDigitsLut[ j + 1 ];
		}

		const uint v0 = static_cast<uint>( value / 100000000 );
		const uint v1 = static_cast<uint>( value % 100000000 );

		const uint b0 = v0 / 10000;
		const uint c0 = v0 % 10000;

		const uint d1 = ( b0 / 100 ) << 1;
		const uint d2 = ( b0 % 100 ) << 1;

		const uint d3 = ( c0 / 100 ) << 1;
		const uint d4 = ( c0 % 100 ) << 1;

		const uint b1 = v1 / 10000;
		const uint c1 = v1 % 10000;

		const uint d5 = ( b1 / 100 ) << 1;
		const uint d6 = ( b1 % 100 ) << 1;

		const uint d7 = ( c1 / 100 ) << 1;
		const uint d8 = ( c1 % 100 ) << 1;

		*buffer++ = gDigitsLut[ d1 ];
		*buffer++ = gDigitsLut[ d1 + 1 ];
		*buffer++ = gDigitsLut[ d2 ];
		*buffer++ = gDigitsLut[ d2 + 1 ];
		*buffer++ = gDigitsLut[ d3 ];
		*buffer++ = gDigitsLut[ d3 + 1 ];
		*buffer++ = gDigitsLut[ d4 ];
		*buffer++ = gDigitsLut[ d4 + 1 ];
		*buffer++ = gDigitsLut[ d5 ];
		*buffer++ = gDigitsLut[ d5 + 1 ];
		*buffer++ = gDigitsLut[ d6 ];
		*buffer++ = gDigitsLut[ d6 + 1 ];
		*buffer++ = gDigitsLut[ d7 ];
		*buffer++ = gDigitsLut[ d7 + 1 ];
		*buffer++ = gDigitsLut[ d8 ];
		*buffer++ = gDigitsLut[ d8 + 1 ];
	}

	//*buffer = '\0';
	return int( buffer - bak );
}

int i64toa_branchlut( int64 value, char* buffer )
{
	uint64 u = static_cast<uint64>( value );
	if( value < 0 )
	{
		*buffer++ = '-';
		u = ~u + 1;
		return u64toa_branchlut( u, buffer ) + 1;
	}
	return u64toa_branchlut( u, buffer );
}













int ToStringCore( bool in, char* out )
{
	if( in )
	{
		out[ 0 ] = 't';
		out[ 1 ] = 'r';
		out[ 2 ] = 'u';
		out[ 3 ] = 'e';
		return 4;
	}
	else
	{
		out[ 0 ] = 'f';
		out[ 1 ] = 'a';
		out[ 2 ] = 'l';
		out[ 3 ] = 's';
		out[ 4 ] = 'e';
		return 5;
	}
}

int ToStringCore( double in, char* out )
{
	return DiyFp::dtoa_milo( in, out );
}

int ToStringCore( float in, char* out )
{
	return DiyFp::dtoa_milo( in, out );
}

int ToString( char* dstBuf, uint8   v )
{
	return u32toa_branchlut( v, dstBuf );
}
int ToString( char* dstBuf, uint16  v )
{
	return u32toa_branchlut( v, dstBuf );
}
int ToString( char* dstBuf, uint    v )
{
	return u32toa_branchlut( v, dstBuf );
}
int ToString( char* dstBuf, uint64  v )
{
	return u64toa_branchlut( v, dstBuf );
}
int ToString( char* dstBuf, int8    v )
{
	return i32toa_branchlut( v, dstBuf );
}
int ToString( char* dstBuf, int16   v )
{
	return i32toa_branchlut( v, dstBuf );
}
int ToString( char* dstBuf, int     v )
{
	return i32toa_branchlut( v, dstBuf );
}
int ToString( char* dstBuf, int64   v )
{
	return i64toa_branchlut( v, dstBuf );
}
int ToString( char* dstBuf, double  v )
{
	return ToStringCore( v, dstBuf );
}
int ToString( char* dstBuf, float   v )
{
	return ToStringCore( v, dstBuf );
}
int ToString( char* dstBuf, bool    v )
{
	return ToStringCore( v, dstBuf );
}

int ToString( char* dstBuf, char const* v )
{
	auto len = strlen( v );
	memcpy( dstBuf, v, len );
	return (int)len;
}

int ToString( char* dstBuf, char v )
{
	*dstBuf = v;
	return 1;
}

//#if defined(XX_LINUX) && defined(XX_64)
//int ToString( char* dstBuf, size_t v )
//{
//	return ToString( dstBuf, (uint64)v );
//}
//#endif








static char const Int2HexTable[] =
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F'
};

int ToHexString( char* dstBuf, uint8 v )
{
	dstBuf[ 0 ] = Int2HexTable[ v >> 4 ];
	dstBuf[ 1 ] = Int2HexTable[ v & 15 ];
	return 2;
}

int ToHexString( char* dstBuf, uint16 v )
{
	ToHexString( dstBuf, uint8( v >> 8 ) );
	ToHexString( dstBuf + 2, (uint8)v );
	return 4;
}

int ToHexString( char* dstBuf, uint v )
{
	ToHexString( dstBuf, uint16( v >> 16 ) );
	ToHexString( dstBuf + 4, (uint16)v );
	return 8;
}

int ToHexString( char* dstBuf, uint64 v )
{
	ToHexString( dstBuf, uint32( v >> 32 ) );
	ToHexString( dstBuf + 8, (uint32)v );
	return 16;
}


int ToHexString( char* dstBuf, int8 v )
{
	return ToHexString( dstBuf, (uint8)v );
}

int ToHexString( char* dstBuf, char v )
{
	return ToHexString( dstBuf, (uint8)v );
}

int ToHexString( char* dstBuf, int16 v )
{
	return ToHexString( dstBuf, (uint16)v );
}

int ToHexString( char* dstBuf, int v )
{
	return ToHexString( dstBuf, (uint32)v );
}

int ToHexString( char* dstBuf, int64 v )
{
	return ToHexString( dstBuf, (uint64)v );
}

//#if defined(XX_LINUX) && defined(XX_64)
//int ToHexString( char* dstBuf, size_t v )
//{
//	return ToHexString( dstBuf, (uint64)v );
//}
//#endif




















// char* 转为各种长度的 有符号整数. Out 取值范围： int8~64
template <typename OutType>
void toInt( char const * in, OutType & out )
{
	auto in_ = in;
	if( *in_ == '0' )
	{
		out = 0;
		return;
	}
	bool b;
	if( *in_ == '-' )
	{
		b = true;
		++in_;
	}
	else b = false;
	OutType r = *in_ - '0';
	char c;
	while( ( c = *( ++in_ ) ) ) r = r * 10 + ( c - '0' );
	out = b ? -r : r;
}

// char* (不能有减号打头) 转为各种长度的 无符号整数. Out 取值范围： uint8, uint16, uint32, uint64
template <typename OutType>
void toUInt( char const * in, OutType & out )
{
	assert( in );
	auto in_ = in;
	if( *in_ == '0' )
	{
		out = 0;
		return;
	}
	OutType r = *(in_)-'0';
	char c;
	while( ( c = *( ++in_ ) ) ) r = r * 10 + ( c - '0' );
	out = r;
}


void FromStringCore( char const * in, uint8 & out )
{
	toUInt( in, out );
}


void FromStringCore( char const * in, uint16 & out )
{
	toUInt( in, out );
}


void FromStringCore( char const * in, uint32 & out )
{
	toUInt( in, out );
}


void FromStringCore( char const * in, uint64 & out )
{
	toUInt( in, out );
}


void FromStringCore( char const * in, int8 & out )
{
	toInt( in, out );
}


void FromStringCore( char const * in, int16 & out )
{
	toInt( in, out );
}


void FromStringCore( char const * in, int & out )
{
	toInt( in, out );
}


void FromStringCore( char const * in, int64 & out )
{
	toInt( in, out );
}

void FromStringCore( char const * in, double & out )
{
	out = strtod( in, nullptr );
}

void FromStringCore( char const * in, float & out )
{
	out = (float)strtod( in, nullptr );
}


void FromStringCore( char const * in, bool & out )
{
	out = ( in[ 0 ] == '1' || in[ 0 ] == 'T' || in[ 0 ] == 't' );
}

void FromString( uint8   & dstVar, char const * s )
{
	FromStringCore( s, dstVar );
}
void FromString( uint16  & dstVar, char const * s )
{
	FromStringCore( s, dstVar );
}
void FromString( uint32  & dstVar, char const * s )
{
	FromStringCore( s, dstVar );
}
void FromString( uint64  & dstVar, char const * s )
{
	FromStringCore( s, dstVar );
}
void FromString( int8    & dstVar, char const * s )
{
	FromStringCore( s, dstVar );
}
void FromString( int16   & dstVar, char const * s )
{
	FromStringCore( s, dstVar );
}
void FromString( int     & dstVar, char const * s )
{
	FromStringCore( s, dstVar );
}
void FromString( int64   & dstVar, char const * s )
{
	FromStringCore( s, dstVar );
}
void FromString( double  & dstVar, char const * s )
{
	FromStringCore( s, dstVar );
}
void FromString( float   & dstVar, char const * s )
{
	FromStringCore( s, dstVar );
}
void FromString( bool    & dstVar, char const * s )
{
	FromStringCore( s, dstVar );
}



//#if defined(XX_LINUX) && defined(XX_64)
//void FromString( size_t& dstVar, char const * s )
//{
//	return FromString( (uint64&)dstVar, s );
//}
//#endif


}
