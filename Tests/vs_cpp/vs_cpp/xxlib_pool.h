#pragma once
namespace xxlib
{
class Pool : Memmoveable
{
public:

	Pool() {};							        // do nothing. need Init
	Pool( int itemSize, int pageSize );
	~Pool();
	Pool( Pool&& o ) = default;
	Pool& operator=( Pool&& o ) = default;
	Pool( Pool const& o ) = delete;
	Pool& operator=( Pool const& o ) = delete;

	void Init( int itemSize, int pageSize );	// store itemSize, Requ	estPage( pageSize )
	template<typename T>
	inline void Init( int pageItemCount = 8 )
	{
		Init( (int)MAX( sizeof( T ), sizeof( void* ) ), pageItemCount * (int)MAX( sizeof( T ), sizeof( void* ) ) );
	}

	void SetPageSize( int newPageSize );
	void SetPageSizeByItemCount( int itemCount );
	void NewPage();								// spaceCounter += xxx, new & fill mem ptrs, pageSize *= 2

	// c style funcs
	void* Alloc();
	void* FastAlloc();							// unsafe: do not ensure the space is enough
	void Free( void* p );

	// c++ style funcs	
	template<typename T, typename...PTS>
	inline T* New( PTS&&...ps )
	{
		assert( itemSize >= sizeof( T ) );
		auto p = Alloc();
		return new ( p ) T( std::forward<PTS>( ps )... );
	}

	template<typename T>
	inline void Delete( T* p )
	{
		p->T::~T();
		Free( p );
	}

private:
	void* header = nullptr;					// current alloc ptr( *header = next ptr )
public:
	int itemSize = 0, pageSize = 0;			// size: aligned byte count
	int allPageItemCounter = 0;
private:
	List<void*> pages;

#ifdef XX_DEBUG
public:
	int allocatedCounter = 0;
#endif
} const emptyPool;

}
