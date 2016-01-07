#pragma once
namespace xxlib
{

template<typename T>
xxlib::List<T>::List( std::initializer_list<T> items )
	: List( (int)items.size() )
{
	for( auto& i : items )
	{
		Add( i );
	}
}

template<typename T>
List<T>::List( int capacity )
{
    if( capacity <= 0 )
    {
        capacity = 8;
    }
	auto byteLen = Round2n( capacity * sizeof( T ) );
	buf = ( T* )new char[ byteLen ];
	bufLen = int( byteLen / sizeof( T ) );
	dataLen = 0;
}

template<typename T>
List<T>::List()
	: List( 8 )
{
}

template<typename T>
List<T>::~List()
{
	if( buf )
	{
		Clear();
		delete[]( char* )buf;
	}
}

template<typename T>
List<T>::List( List&& o )
	: buf( o.buf )
	, bufLen( o.bufLen )
	, dataLen( o.dataLen )
{
	o.buf = nullptr;
	o.bufLen = 0;
	o.dataLen = 0;
}

template<typename T>
List<T>::List( List const& o )
	: List( o.dataLen )
{
	if( std::is_pod<T>::value )
	{
		memcpy( buf, o.buf, o.DataByteLen() );
	}
	else
	{
		for( int i = 0; i < o.dataLen; ++i )
		{
			new ( buf + i ) T( o.buf[ i ] );
		}
	}
	dataLen = o.dataLen;
}

template<typename T>
List<T>& List<T>::operator=( List&& o )
{
	Clear();
	delete[]( char* )buf;
	buf = o.buf;
	bufLen = o.bufLen;
	dataLen = o.dataLen;
	o.buf = nullptr;
	o.bufLen = 0;
	o.dataLen = 0;
	return *this;
}

template<typename T>
List<T>& List<T>::operator=( List const& o )
{
	if( this == &o )
	{
		return *this;
	}
	Clear();
	Reserve( o.dataLen );
    dataLen = o.dataLen;
    if( std::is_pod<T>::value )
	{
		memcpy( buf, o.buf, o.DataByteLen() );
	}
	else
	{
		for( int i = 0; i < o.dataLen; ++i )
		{
			new ( buf + i ) T( o.buf[ i ] );
		}
	}
	return *this;
}


template<typename T>
void List<T>::FastAdd( T&& v )
{
	new ( buf + dataLen++ ) T( (T&&)v );
}

template<typename T>
void List<T>::FastAdd( T const& v )
{
	new ( buf + dataLen++ ) T( v );
}

template<typename T>
void List<T>::Add( T&& v )
{
	if( dataLen == bufLen )
	{
		Reserve( dataLen + 1 );
	}
	new ( buf + dataLen++ ) T( (T&&)v );
}

template<typename T>
void List<T>::Add( T const& v )
{
	if( dataLen == bufLen )
	{
		Reserve( dataLen + 1 );
	}
	new ( buf + dataLen++ ) T( v );
}

template<typename T>
template<typename...PTS>
T& List<T>::EmplaceAdd( PTS&&...ps )
{
	if( dataLen == bufLen )
	{
		Reserve( dataLen + 1 );
	}
	return *new ( buf + dataLen++ ) T( std::forward<PTS>( ps )... );
}


template<typename T>
void List<T>::Pop()
{
	assert( dataLen > 0 );
	buf[ --dataLen ].~T();
}

template<typename T>
T& List<T>::Top()
{
	assert( dataLen > 0 );
	return buf[ dataLen - 1 ];
}
template<typename T>
T const& List<T>::Top() const
{
	assert( dataLen > 0 );
	return buf[ dataLen - 1 ];
}

template<typename T>
bool List<T>::Pop( T& output )
{
	if( !dataLen )
	{
		return false;
	}
	output = std::move( buf[ --dataLen ] );
	buf[ dataLen ].~T();
	return true;
}

template<typename T>
void List<T>::Clear()
{
	for( int i = 0; i < dataLen; ++i )
	{
		buf[ i ].~T();
	}
	dataLen = 0;
}

template<typename T>
void List<T>::Reserve( int capacity )
{
	if( capacity <= bufLen )
	{
		return;
	}
	auto byteLen = (int)Round2n( capacity * sizeof( T ) );
	bufLen = byteLen / sizeof( T );
	auto newBuf = ( T* )new char[ byteLen ];
	if( std::is_pod<T>::value || std::is_base_of<Memmoveable, T>::value )
	{
		memcpy( newBuf, buf, DataByteLen() );
	}
	else
	{
		for( int i = 0; i < dataLen; ++i )
		{
			new ( newBuf + i ) T( std::move( buf[ i ] ) );
			buf[ i ].~T();
		}
	}
	delete[]( char* )buf;
	buf = newBuf;
}

template<typename T>
void List<T>::Resize( int capacity, bool init /*= true */ )
{
	if( capacity == dataLen )
	{
		return;
	}
	else if( capacity < dataLen )
	{
		if( !std::is_pod<T>::value )
		{
			for( int i = capacity; i < dataLen; ++i )
			{
				buf[ i ].~T();
			}
		}
	}
	else
	{
		Reserve( capacity );
		if( init )
		{
			if( std::is_pod<T>::value )
			{
				memset( buf + dataLen, 0, ( capacity - dataLen ) * sizeof( T ) );
			}
			else
			{
				for( int i = dataLen; i < capacity; ++i )
				{
					new ( buf + i ) T();
				}
			}
		}
	}
	dataLen = capacity;
}


template<typename T>
int List<T>::DataByteLen() const
{
	return dataLen * sizeof( T );
}

template<typename T>
int List<T>::BufByteLen() const
{
	return bufLen * sizeof( T );
}

template<typename T>
T const& List<T>::operator[]( int idx ) const
{
	assert( idx < dataLen && idx >= 0 );
	return buf[ idx ];
}

template<typename T>
T& List<T>::operator[]( int idx )
{
	assert( idx < dataLen && idx >= 0 );
	return buf[ idx ];
}

template<typename T>
T const& List<T>::IndexAt( int idx ) const
{
	assert( idx < dataLen && idx >= 0 );
	return buf[ idx ];
}

template<typename T>
T& List<T>::IndexAt( int idx )
{
	assert( idx < dataLen && idx >= 0 );
	return buf[ idx ];
}

template<typename T>
void List<T>::Set( int idx, T&& v )
{
	assert( idx >= 0 && idx < dataLen );
	buf[ idx ] = (T&&)v;
}
template<typename T>
void List<T>::Set( int idx, T const& v )
{
	assert( idx >= 0 && idx < dataLen );
	buf[ idx ] = v;
}

template<typename T>
int List<T>::Find( T const& v ) const
{
	for( int i = 0; i < dataLen; ++i )
	{
		if( buf[ i ] == v )
		{
			return i;
		}
	}
	return -1;
}

template<typename T>
void List<T>::RemoveAt( int idx )
{
	assert( idx >= 0 && idx < dataLen );
	--dataLen;
	if( std::is_pod<T>::value || std::is_base_of<Memmoveable, T>::value )
	{
		buf[ idx ].~T();
		memmove( buf + idx, buf + idx + 1, ( dataLen - idx ) * sizeof( T ) );
	}
	else
	{
		for( int i = idx; i < dataLen; ++i )
		{
			buf[ i ] = (T&&)buf[ i + 1 ];
		}
		buf[ dataLen ].~T();
	}
}

template<typename T>
void List<T>::FastRemoveAt( int idx )
{
	assert( idx >= 0 && idx < dataLen );
	--dataLen;
	if( dataLen == idx )	// last one
	{
		buf[ idx ].~T();
		return;
	}
	if( std::is_pod<T>::value )
	{
		buf[ idx ] = buf[ dataLen ];
	}
	else if( std::is_base_of<Memmoveable, T>::value )
	{
		buf[ idx ].~T();
		memcpy( buf + idx, buf + dataLen, sizeof( T ) );
	}
	else
	{
		buf[ idx ] = (T&&)buf[ dataLen ];
		buf[ dataLen ].~T();
	}
}

template<typename T>
template<typename...PTS>
T& List<T>::EmplaceInsertAt( int idx, PTS&&...ps )
{
	assert( idx >= 0 && idx <= dataLen );
	Reserve( dataLen + 1 );

	if( std::is_pod<T>::value || std::is_base_of<Memmoveable, T>::value )
	{
		memmove( buf + idx + 1, buf + idx, ( dataLen - idx ) * sizeof( T ) );
		++dataLen;
		return *new ( buf + idx ) T( std::forward<PTS>( ps )... );
	}
	else
	{
		if( idx < dataLen )
		{
			new ( buf + dataLen ) T( (T&&)buf[ dataLen - 1 ] );
			for( int i = dataLen - 1; i > idx; --i )
			{
				buf[ i ] = (T&&)buf[ i - 1 ];
			}
			buf[ idx ].~T();
		}
		++dataLen;
		return *new ( buf + idx ) T( std::forward<PTS>( ps )... );
	}
}

template<typename T>
void List<T>::InsertAt( int idx, T&& v )
{
	EmplaceInsertAt( idx, (T&&)v );
}
template<typename T>
void List<T>::InsertAt( int idx, T const& v )
{
	EmplaceInsertAt( idx, v );
}


























template<typename T>
void List<T>::WriteTo( ByteBuffer& bb ) const
{
	bb.VarWrite( (uint)dataLen );
	if( !std::is_pointer<T>::value && std::is_pod<T>::value )
	{
		if( !dataLen )
		{
			return;
		}
		bb.Write( (char*)buf, dataLen * sizeof( T ) );
		return;
	}
	for( int i = 0; i < dataLen; ++i )
	{
		bb.Write( buf[ i ] );
	}
}

template<typename T>
void List<T>::FastWriteTo( ByteBuffer& bb ) const
{
	bb.FastVarWrite( (uint)dataLen );
	if( !std::is_pointer<T>::value && std::is_pod<T>::value )
	{
		if( !dataLen )
		{
			return;
		}
		bb.FastWrite( (char*)buf, dataLen * sizeof( T ) );
		return;
	}
	for( int i = 0; i < dataLen; ++i )
	{
		bb.FastWrite( buf[ i ] );
	}
}

template<typename T>
bool List<T>::ReadFrom( ByteBuffer& bb )
{
	return ReadFrom( bb, 0, 0 );
}
template<typename T>
bool List<T>::ReadFrom( ByteBuffer& bb, int minLen, int maxLen )
{
	int len;
	if( !bb.VarRead( *(uint*)&len ) )
	{
		return false;
	}
	if( len < minLen || ( maxLen > 0 && len > maxLen ) )
	{
		return false;
	}
	if( !std::is_pointer<T>::value && std::is_pod<T>::value )
	{
		int siz = len * ( int )sizeof( T );
		if( bb.offset + siz > bb.dataLen )
		{
			return false;
		}
		Clear();
		Resize( len, false );
		memcpy( buf, bb.buf + bb.offset, siz );
		bb.offset += siz;
		return true;
	}
	Clear();
	if( !len )
	{
		return true;
	}
	Reserve( len );
	for( int i = 0; i < len; ++i )
	{
		new ( buf + i ) T();
		dataLen = i + 1;
		if( !bb.Read( buf[ i ] ) )
		{
			return false;
		}
	}
	return true;
}


template<typename T>
void List<T>::VarWriteTo( ByteBuffer& bb ) const
{
	bb.VarWrite( (uint)dataLen );
	for( int i = 0; i < dataLen; ++i )
	{
		bb.VarWrite( buf[ i ] );
	}
}

template<typename T>
bool List<T>::VarReadFrom( ByteBuffer& bb )
{
	return VarReadFrom( bb, 0, 0 );
}
template<typename T>
bool List<T>::VarReadFrom( ByteBuffer& bb, int minLen, int maxLen )
{
	int len;
	if( !bb.VarRead( *(uint*)&len ) )
	{
		return false;
	}
	if( len < minLen || ( maxLen > 0 && len > maxLen ) )
	{
		return false;
	}
	Clear();
	if( !len )
	{
		return true;
	}
	Reserve( len );
	for( int i = 0; i < len; ++i )
	{
		new ( buf + i ) T();
		dataLen = i + 1;
		if( !bb.VarRead( buf[ i ] ) )
		{
			return false;
		}
	}
	return true;
}


}
