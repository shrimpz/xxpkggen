#pragma once
namespace xxlib
{

template <typename TK>
Hash<TK>::Hash( int capacity)
{
    if( capacity <= 0 )
    {
        capacity = 8;
    }
	pool.Init<Node>( capacity );
	int prime = GetPrime( capacity );
	nodes.Reserve( prime );
	buckets.Resize( prime );
}
template <typename TK>
Hash<TK>::Hash()
	: Hash( 16 )
{
}

template <typename TK>
Hash<TK>::Hash( Hash const& o )
	: Hash( o.Size() )
{
	operator=( o );
}

template <typename TK>
Hash<TK>::Hash( Hash&& o )
{
	buckets = std::move( o.buckets );
	nodes = std::move( o.nodes );
	pool = std::move( o.pool );
}

template <typename TK>
Hash<TK>& Hash<TK>::operator=( Hash const& o )
{
	if( this == &o ) return *this;
	Clear();
	Reserve( o.Size() );
	for( int i = 0; i < o.Size(); ++i )
	{
		auto on = o.nodes[ i ];
		uint mod = on->hash % (uint)buckets.dataLen;
		auto n = (Node*)pool.FastAlloc();
		n->next = buckets[ mod ];
		n->hash = on->hash;
		n->index = nodes.dataLen;
		new ( &n->key ) TK( on->key );
		buckets[ mod ] = n;
		nodes.Add( n );
	}
	return *this;
}

template <typename TK>
Hash<TK>& Hash<TK>::operator=( Hash&& o )
{
	buckets = std::move( o.buckets );
	nodes = std::move( o.nodes );
	pool = std::move( o.pool );
	return *this;
}

template <typename TK>
Hash<TK>::~Hash()
{
	for( int i = 0; i < nodes.dataLen; ++i )
		Dispose( nodes[ i ] );
}

template <typename TK>
template<typename...PTS>
std::pair<typename Hash<TK>::Node*, bool> Hash<TK>::EmplaceAdd( PTS&&...ps )
{
	std::pair<typename Hash<TK>::Node*, bool> rtv;
	auto n = (Node*)pool.FastAlloc();
	new ( &n->key ) TK( std::forward<PTS>( ps )... );
	n->hash = (uint)GetHashCode( n->key );
	uint mod = n->hash % (uint)buckets.dataLen;
	auto node = buckets[ mod ];
	while( node )
	{
		if( node->hash == n->hash && node->key == n->key )
		{
			rtv.first = node;
			rtv.second = false;
			n->key.~TK();
			pool.Free( n );
			return rtv;
		}
		node = node->next;
	};
	n->next = buckets[ mod ];
	n->index = nodes.dataLen;
	buckets[ mod ] = n;
	nodes.Add( n );
	if( nodes.dataLen == buckets.dataLen ) Resize();
	rtv.first = n;
	rtv.second = true;
	return rtv;
}

template <typename TK>
std::pair<typename Hash<TK>::Node*, bool> Hash<TK>::Add( TK&& k )
{
	return EmplaceAdd( (TK&&)k );
}
template <typename TK>
std::pair<typename Hash<TK>::Node*, bool> Hash<TK>::Add( TK const& k )
{
	return EmplaceAdd( k );
}

template <typename TK>
typename Hash<TK>::Node* Hash<TK>::Find( TK const& k )
{
	uint hashCode = (uint)GetHashCode( k );
	uint mod = hashCode % (uint)buckets.dataLen;
	auto node = buckets[ mod ];
	while( node )
	{
		if( node->hash == hashCode && node->key == k ) return node;
		node = node->next;
	}
	return nullptr;
}

template <typename TK>
void Hash<TK>::Remove( TK const& k )
{
	auto n = Find( k );
	if( !n ) return;
	Remove( n );
}

template <typename TK>
void Hash<TK>::Remove( Node* n )
{
	auto mod = n->hash % (uint)buckets.dataLen;
	auto node = buckets[ mod ];
	if( node == n )
	{
		buckets[ mod ] = node->next;
	}
	else
	{
		auto parent = node;
		while( ( node = node->next ) )
		{
			if( node == n )
			{
				parent->next = node->next;
				break;
			}
			parent = node;
		}
	}

	auto last = nodes.Top();
	nodes.Pop();
	if( n != last )
	{
		nodes[ n->index ] = last;
		last->index = n->index;
	}

	Dispose( n );
	pool.Free( n );
}

template <typename TK>
void Hash<TK>::Clear()
{
	for( int i = 0; i < nodes.dataLen; ++i )
	{
		Dispose( nodes[ i ] );
		pool.Free( nodes[ i ] );
	}
	nodes.Clear();
	memset( buckets.buf, 0, buckets.DataByteLen() );
}


template <typename TK>
void Hash<TK>::Reserve( int capacity )
{
	if( capacity <= buckets.dataLen ) return;
	int prime = GetPrime( (int)Round2n( capacity ) );
	if( pool.allPageItemCounter < prime )
	{
		pool.SetPageSizeByItemCount( prime - pool.allPageItemCounter );
		pool.NewPage();
	}
	nodes.Reserve( prime );
	buckets.Resize( prime, false );
	memset( buckets.buf, 0, buckets.DataByteLen() );      // clean up
	for( int i = 0; i < nodes.dataLen; ++i )
	{
		auto& o = nodes[ i ];
		auto mod = o->hash % prime;
		o->next = buckets[ mod ];
		buckets[ mod ] = o;
	}
}


template <typename TK>
int Hash<TK>::Size() const
{
	return nodes.dataLen;
}

template <typename TK>
void Hash<TK>::Dispose( Node* n )
{
	n->key.~TK();
}


template <typename TK>
bool Hash<TK>::Empty()
{
	return nodes.dataLen == 0;
}


template <typename TK>
void Hash<TK>::Resize()
{
	int prime = GetPrime( nodes.dataLen * 3 );
	nodes.Reserve( prime );
	buckets.Resize( prime, false );
	memset( buckets.buf, 0, buckets.DataByteLen() );      // clean up
	for( int i = 0; i < nodes.dataLen; ++i )
	{
		auto& o = nodes[ i ];
		auto mod = o->hash % prime;
		o->next = buckets[ mod ];
		buckets[ mod ] = o;
	}
}

template <typename TK>
typename Hash<TK>::Node const* Hash<TK>::operator[]( int idx ) const
{
	assert( idx < nodes.dataLen );
	return nodes[ idx ];
}
template <typename TK>
typename Hash<TK>::Node* Hash<TK>::operator[]( int idx )
{
	assert( idx < nodes.dataLen );
	return nodes[ idx ];
}
template <typename TK>
typename Hash<TK>::Node const* Hash<TK>::IndexAt( int idx ) const
{
	assert( idx < nodes.dataLen );
	return nodes[ idx ];
}
template <typename TK>
typename Hash<TK>::Node* Hash<TK>::IndexAt( int idx )
{
	assert( idx < nodes.dataLen );
	return nodes[ idx ];
}


}
