#include "xxlib.h"
namespace xxlib
{

int GetHash_CS( byte const* buf, int len )
{
	assert( (size_t)buf % 4 == 0 );
	if( !len ) return 0;
	int n2 = 0x15051505, n1 = 0x15051505, mod = len % 8, i = 0;
	for( ; i < len - mod; i += 8 )
	{
		n1 = ( ( ( n1 << 5 ) + n1 ) + ( n1 >> 0x1b ) ) ^ *(int*)( buf + i );
		n2 = ( ( ( n2 << 5 ) + n2 ) + ( n2 >> 0x1b ) ) ^ *(int*)( buf + i + 4 );
	}
	if( mod > 4 )
	{
		n1 = ( ( ( n1 << 5 ) + n1 ) + ( n1 >> 0x1b ) ) ^ *(int*)( buf + i );
		n2 = ( ( ( n2 << 5 ) + n2 ) + ( n2 >> 0x1b ) ) ^ ( *(int*)( buf + i + 4 ) & ( 0xFFFFFFFF >> ( ( 8 - mod ) * 8 ) ) );
	}
	else if( mod )
	{
		n1 = ( ( ( n1 << 5 ) + n1 ) + ( n1 >> 0x1b ) ) ^ ( *(int*)( buf + i ) & ( 0xFFFFFFFF >> ( ( 4 - mod ) * 8 ) ) );
	}
	return n2 + n1 * 0x5d588b65;
}

int GetHash_Lua( byte const* buf, int len )
{
	if( !len ) return 0;
	uint seed = 131, hash = 0;
	for( int i = 0; i < len; ++i )
		hash = hash * seed + (uint8)buf[ i ];
	return (int)hash;
}


}
