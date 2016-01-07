#pragma once

#ifdef XX_MSVC
#define aligned_alloc( a, s )   _aligned_malloc( s, a )
#define aligned_free( p )       _aligned_free( p )
#else
inline void* aligned_alloc( size_t alignment, size_t size )
{
	assert( !( alignment & ( alignment - 1 ) ) );
	auto offset = sizeof( void* ) + ( --alignment );
	auto p = (char*)malloc( offset + size );
	if( !p )
	{
		return nullptr;
	}
	auto r = reinterpret_cast<void*>( reinterpret_cast<size_t>( p + offset ) & ( ~alignment ) );
	( (void**)r )[ -1 ] = p;
	return r;
}
inline void aligned_free( void* p )
{
	free( static_cast<void**>( p )[ -1 ] );
}
#endif
