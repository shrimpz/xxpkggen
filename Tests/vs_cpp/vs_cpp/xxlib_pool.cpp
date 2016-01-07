#include "xxlib.h"
namespace xxlib
{

Pool::Pool( int itemSize, int pageSize )
{
	Init( itemSize, pageSize );
}

Pool::~Pool()
{
	for( int i = 0; i < pages.dataLen; ++i )
	{
		aligned_free( pages[ i ] );
	}
}

void Pool::Init( int itemSize_, int pageSize_ )
{
	assert( !itemSize );				// ensure init only execute 1 times
	if( itemSize_ < ( int )sizeof( void* ) )
	{
		itemSize_ = ( int )sizeof( void* );
	}
	itemSize = (int)Round2n( itemSize_ );
	SetPageSize( pageSize_ );
	NewPage();
}

void Pool::SetPageSize( int newPageSize )
{
	assert( itemSize );					// need init
	if( newPageSize < itemSize )
	{
		newPageSize = itemSize;
	}
	pageSize = (int)Round2n( newPageSize );
}

void Pool::SetPageSizeByItemCount( int itemCount )
{
	assert( itemSize );					// need init
	assert( itemCount );
	pageSize = (int)Round2n( itemCount * itemSize );
}

XX_NOINLINE void Pool::NewPage()
{
	assert( itemSize );					// need init
	allPageItemCounter += ( pageSize / itemSize );
	auto p = (char*)aligned_alloc( itemSize, pageSize );
	pages.Add( p );
	auto bak = p;
	auto pp = p + pageSize - itemSize;	// - itemSize: skip last cell
	for( ; p < pp; p += itemSize )
	{
		*(void**)p = p + itemSize;		// cell1 = &cell2
	}
	*(void**)p = header;				// last cell link to header or set nullptr
	header = bak;
	pageSize *= 2;
}

void* Pool::Alloc()
{
	assert( itemSize );					// need init
	if( !header )
	{
		NewPage();
	}
	auto rtv = header;
	header = *(void**)header;
#ifdef XX_DEBUG
	++allocatedCounter;
#endif
	return rtv;
}

void* Pool::FastAlloc()
{
	assert( itemSize );					// need init
	auto rtv = header;
	header = *(void**)header;
#ifdef XX_DEBUG
	++allocatedCounter;
#endif
	return rtv;
}

void Pool::Free( void* p )
{
	assert( itemSize );					// need init
	assert( p );
	*(void**)p = header;
	header = p;
#ifdef XX_DEBUG
	--allocatedCounter;
#endif
}

}
