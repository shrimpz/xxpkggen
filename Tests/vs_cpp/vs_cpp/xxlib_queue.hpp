#pragma once
namespace xxlib
{


template <class T>
Queue<T>::Queue( int capacity )
{
    if( capacity <= 0 )
    {
        capacity = 8;
    }
	auto byteLen = (int)Round2n( capacity * sizeof( T ) );
	bufLen = byteLen / ( int )sizeof( T );
	buf = ( T* )new char[ byteLen ];
	// FR..............................
	front = 0;
	rear = 0;
}

template <class T>
Queue<T>::Queue()
	: Queue( 8 )
{
}

template <class T>
Queue<T>::~Queue()
{
	if( buf )
	{
		Clear();
		delete[]( char* )buf;
	}
}

template <class T>
int Queue<T>::Size() const
{
	//......Front+++++++++++Rear......
	//...............FR...............
	if( front <= rear )
	{
		return rear - front;
	}
	// ++++++Rear...........Front++++++
	else
	{
		return rear + ( bufLen - front );
	}
}

template <class T>
bool Queue<T>::Empty() const
{
	return front == rear;
}

template <class T>
void Queue<T>::Pop()
{
	assert( front != rear );
	buf[ front++ ].~T();
	if( front == bufLen )
	{
		front = 0;
	}
}

template <class T>
T const& Queue<T>::Front() const
{
	assert( front != rear );
	return buf[ front ];
}

template <class T>
T& Queue<T>::Front()
{
	assert( front != rear );
	return buf[ front ];
}


template <class T>
template<typename...PTS>
T& Queue<T>::EmplacePush( PTS&&...ps )
{
	auto idx = rear;
	new ( buf + rear++ ) T( std::forward<PTS>( ps )... );

	if( rear == bufLen )		// cycle
	{
		rear = 0;
	}
	if( rear == front )			// no more space
	{
		idx = bufLen;
		Reserve( bufLen + 1, true );
	}
	return buf[ idx ];
}
template <class T>
void Queue<T>::Push( T const& v )
{
	EmplacePush( v );
}
template <class T>
void Queue<T>::Push( T&& v )
{
	EmplacePush( (T&&)v );
}


template <class T>
void Queue<T>::Clear()
{
	if( front == rear )
	{
		return;
	}
	if( !std::is_pod<T>::value )
	{
		//......Front+++++++++++Rear......
		if( front < rear )
		{
			for( int i = front; i < rear; ++i )
			{
				buf[ i ].~T();
			}
		}
		// ++++++Rear...........Front++++++
		else
		{
			int i = 0;
			for( ; i < rear; ++i )
			{
				buf[ i ].~T();
			}
			for( i = front; i < bufLen; ++i )
			{
				buf[ i ].~T();
			}
		}
	}
	//........FR......................
	front = rear;
}

template <typename T>
int Queue<T>::PopMulti( int count )
{
	if( count <= 0 )
	{
		return 0;
	}
	auto dataLen = Size();
	if( count >= dataLen )
	{
		Clear();
		return dataLen;
	}
	// count < dataLen

	//......Front+++++++++++Rear......
	if( front < rear )
	{
		if( !std::is_pod<T>::value )
		{
			for( int i = front; i < count; i++ )
			{
				buf[ i ].~T();
			}
		}
		front += count;
	}
	// ++++++Rear...........Front++++++
	else
	{
		auto frontDataLen = bufLen - front;
		if( !std::is_pod<T>::value )
		{
			//...Front+++
			if( count < frontDataLen )
			{
				for( int i = front; i < front + count; ++i )
				{
					buf[ i ].~T();
				}
				front += count;
			}
			else
			{
				//...Front++++++
				int i = front;
				for( ; i < bufLen; ++i )
				{
					buf[ i ].~T();
				}
				// ++++++Rear...
				count -= frontDataLen;
				for( i = 0; i < count; ++i )
				{
					buf[ i ].~T();
				}
				front = count;
			}
		}
	}
	return count;
}

template <class T>
void Queue<T>::Reserve( int capacity, bool afterPush )
{
	assert( capacity > 0 );
	if( capacity <= bufLen )
	{
		return;
	}

	// afterPush: ++++++++++++++RF++++++++++++++++
	auto dataLen = afterPush ? bufLen : Size();

	auto newBufByteLen = (int)Round2n( capacity * sizeof( T ) );
	auto newBufLen = newBufByteLen / ( int )sizeof( T );
	auto newBuf = ( T* )new char[ newBufByteLen ];

	//......Front+++++++++++Rear......
	if( front < rear )
	{
		if( std::is_pod<T>::value || std::is_base_of<Memmoveable, T>::value )
		{
			memcpy( newBuf, buf + front, dataLen * sizeof( T ) );
		}
		else
		{
			for( int i = 0; i < dataLen; ++i )
			{
				new ( newBuf + i ) T( std::move( buf[ front + i ] ) );
			}
		}
	}
	// ++++++Rear...........Front++++++
	// ++++++++++++++RF++++++++++++++++
	else
	{
		//...Front++++++
		auto frontDataLen = bufLen - front;
		if( std::is_pod<T>::value || std::is_base_of<Memmoveable, T>::value )
		{
			memcpy( newBuf, buf + front, frontDataLen * sizeof( T ) );
		}
		else
		{
			for( int i = 0; i < frontDataLen; ++i )
			{
				new ( newBuf + i ) T( std::move( buf[ front + i ] ) );
			}
		}
		// ++++++Rear...
		auto dstPos = newBuf + frontDataLen;
		if( std::is_pod<T>::value || std::is_base_of<Memmoveable, T>::value )
		{
			memcpy( dstPos, buf, rear * sizeof( T ) );
		}
		else
		{
			for( int i = 0; i < rear; ++i )
			{
				new ( dstPos + i ) T( std::move( buf[ i ] ) );
			}
		}
	}

	if( !std::is_pod<T>::value && !std::is_base_of<Memmoveable, T>::value )
	{
		for( int i = 0; i < bufLen; ++i )		// clean up old data
		{
			buf[ i ].~T();
		}
	}
	delete[]( char* )buf;

	buf = newBuf;
	bufLen = newBufLen;
	// Front+++++++++++Rear............
	front = 0;
	rear = dataLen;
}


template<typename T>
T const& Queue<T>::operator[]( int idx ) const
{
	return IndexAt( idx );
}

template<typename T>
T& Queue<T>::operator[]( int idx )
{
	return IndexAt( idx );
}

template<typename T>
T const& Queue<T>::IndexAt( int idx ) const
{
	return const_cast<Queue<T>*>( this )->IndexAt( idx );
}

template<typename T>
T& Queue<T>::IndexAt( int idx )
{
	assert( idx < Size() );
	// = buf[ ( front + idx ) % bufLen ];  but slowly 3 times than following code
	if( front + idx >= bufLen )
	{
		return buf[ front + idx - bufLen ];
	}
	else
	{
		return buf[ front + idx ];
	}
}


template <typename T>
bool Queue<T>::Pop( T& outVal )
{
	if( front == rear )
	{
		return false;
	}
	outVal = std::move( buf[ front ] );
	Pop();
	return true;
}


}
