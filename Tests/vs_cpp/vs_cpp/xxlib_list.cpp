#include "xxlib.h"
namespace xxlib
{

List<bool>::List( int capacity )
{
	assert( capacity > 0 );
	auto len = capacity / 8;
	if( len < 64 ) len = 64;
	else len = (int)Round2n( len );
	buf = new char[ len ];
	bufLen = len * 8;
}


List<bool>::List( std::initializer_list <bool> items )
    : List( (int)items.size() )
{
    for( auto& i : items )
    {
        Add( i );
    }
}


List<bool>::List()
	: List( 512 )
{
}

List<bool>::~List()
{
	if( buf )
	{
		Clear();
		delete[] buf;
	}
}

List<bool>::List( List<bool> && o )
	: buf( o.buf )
	, bufLen( o.bufLen )
	, dataLen( o.dataLen )
{
	o.buf = nullptr;
}

List<bool>::List( List<bool> const& o )
	: List<bool>( o.dataLen )
{
	memcpy( buf, o.buf, o.DataByteLen() );
}

List<bool>& List<bool>::operator=( List<bool> && o )
{
	delete[] buf;
	buf = o.buf;
	dataLen = o.dataLen;
	bufLen = o.bufLen;
	o.buf = nullptr;
	return *this;
}

List<bool>& List<bool>::operator=( List<bool> const& o )
{
    if( this == &o )
    {
        return *this;
    }
	dataLen = o.dataLen;
	if( bufLen < o.dataLen )
	{
		auto len = o.dataLen / 8;
		if( len < 64 ) len = 64;
		else len = (int)Round2n( len );
		bufLen = len * 8;
		delete[] buf;
		buf = new char[ len ];
	}
	memcpy( buf, o.buf, o.DataByteLen() );
	return *this;
}

void List<bool>::Add( bool v )
{
	if( dataLen == bufLen ) Reserve( dataLen + 1 );
	Set( dataLen++, v );
}

void List<bool>::Pop()
{
	assert( dataLen > 0 );
	--dataLen;
}

bool List<bool>::Top() const
{
	assert( dataLen > 0 );
	return operator[]( dataLen - 1 );
}

void List<bool>::Clear()
{
	dataLen = 0;
}

void List<bool>::Reserve( int capacity )
{
	assert( capacity > 0 );
	if( capacity <= bufLen ) return;
	auto len = (int)Round2n( ( capacity - 1 ) / 8 + 1 );
	bufLen = len * 8;
	auto newBuf = new char[ len ];
	memcpy( newBuf, buf, DataByteLen() );
	delete[] buf;
	buf = newBuf;
}


int List<bool>::DataByteLen() const
{
	if( dataLen ) return ( dataLen - 1 ) / 8 + 1;
	return 0;
}

int List<bool>::BufByteLen() const
{
	if( bufLen ) return ( bufLen - 1 ) / 8 + 1;
	return 0;
}

bool List<bool>::operator[]( int idx ) const
{
	return IndexAt( idx );
}

bool List<bool>::IndexAt( int idx ) const
{
	return ( ( (size_t*)buf )[ idx / ( sizeof( size_t ) * 8 ) ] &
			 ( size_t( 1 ) << ( idx % ( sizeof( size_t ) * 8 ) ) ) ) > 0;
}

void List<bool>::Set( int idx, bool v )
{
	assert( idx >= 0 && idx < dataLen );
	if( v ) SetTrue( idx );
	else SetFalse( idx );
}

void List<bool>::SetTrue( int idx )
{
	assert( idx >= 0 && idx < dataLen );
	( (size_t*)buf )[ idx / ( sizeof( size_t ) * 8 ) ] |=
		( size_t( 1 ) << ( idx % ( sizeof( size_t ) * 8 ) ) );
}

void List<bool>::SetFalse( int idx )
{
	assert( idx >= 0 && idx < dataLen );
	( (size_t*)buf )[ idx / ( sizeof( size_t ) * 8 ) ] &=
		~( size_t( 1 ) << ( idx % ( sizeof( size_t ) * 8 ) ) );
}

void List<bool>::FillTrue()
{
	memset( buf, 0xFFFFFFFFu, ( dataLen - 1 ) / 8 + 1 );
}

void List<bool>::FillFalse()
{
	memset( buf, 0, ( dataLen - 1 ) / 8 + 1 );
}

void List<bool>::Fill( bool v, int idxFrom, int idxTo )
{
	assert( dataLen > 0 && idxFrom >= 0 && idxTo >= 0 && idxFrom < dataLen && idxTo < dataLen );
	if( idxFrom == idxTo )
	{
		Set( idxFrom, v );
		return;
	}
	if( idxFrom > idxTo )
	{
		std::swap( idxFrom, idxTo );
	}
	auto byteIdxFrom = idxFrom >> 3;
	auto byteIdxTo = idxTo >> 3;

	if( byteIdxFrom == byteIdxTo )
	{
		// 搞一个 中间一段是 v 的 byte 出来
		if( v )
		{
			buf[ byteIdxFrom ] |= (byte)0xFFu >> ( 7 - ( idxTo - idxFrom ) ) << ( idxFrom & 7 );
		}
		else
		{
			buf[ byteIdxFrom ] &= ~( (byte)0xFFu >> ( 7 - ( idxTo - idxFrom ) ) << ( idxFrom & 7 ) );
		}
	}
	else
	{
		// 分别搞一头一尾, 再 memset 中间
		auto idxFrom7 = idxFrom & 7;
		auto idxTo7 = idxTo & 7;
		if( v )
		{
			buf[ byteIdxFrom ] |= (byte)0xFFu << idxFrom7;
			buf[ byteIdxTo ] |= (byte)0xFFu >> ( 7 - idxTo7 );
		}
		else
		{
			buf[ byteIdxFrom ] &= ~( (byte)0xFFu << idxFrom7 );
			buf[ byteIdxTo ] &= ~( (byte)0xFFu >> ( 7 - idxTo7 ) );
		}
		if( idxFrom7 ) ++byteIdxFrom;
		if( idxTo7 ) --byteIdxTo;
		if( byteIdxFrom <= byteIdxTo )
		{
			memset( buf + byteIdxFrom, v ? 0xFFFFFFFFu : 0, byteIdxTo - byteIdxFrom );
		}
	}
}

void List<bool>::Resize( int capacity, bool init )
{
	if( capacity == dataLen ) return;
	if( capacity < dataLen )
	{
		dataLen = capacity;
		return;
	}
	Reserve( capacity );
	auto oldLen = dataLen;
	dataLen = capacity;
	if( init )
	{
		Fill( false, oldLen, capacity - 1 );
	}
}

void List<bool>::CleanUp() const
{
	if( auto mod = dataLen % 8 )
	{
		buf[ ( dataLen - 1 ) / 8 ] &= ~( 0xFF << mod );
	}
}






void List<bool>::WriteTo( ByteBuffer& bb ) const
{
	bb.VarWrite( (uint)dataLen );
	if( dataLen )
	{
		CleanUp();
		bb.Write( buf, DataByteLen() );
	}
}

void List<bool>::FastWriteTo( ByteBuffer& bb ) const
{
	bb.FastVarWrite( (uint)dataLen );
	if( dataLen )
	{
		CleanUp();
		bb.FastWrite( buf, DataByteLen() );
	}
}


bool List<bool>::ReadFrom( ByteBuffer& bb )
{
	return ReadFrom( bb, 0, 0 );
}
bool List<bool>::ReadFrom( ByteBuffer& bb, int minLen, int maxLen )
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
	if( len == 0 )
	{
		Clear();
		return true;
	}
	auto byteLen = ( len - 1 ) / 8 + 1;
	if( bb.offset + byteLen > bb.dataLen )
	{
		return false;
	}
	Reserve( len );
	dataLen = len;
	memcpy( buf, bb.buf + bb.offset, byteLen );
	bb.offset += byteLen;

	return true;
}

void List<bool>::VarWriteTo( ByteBuffer& bb ) const
{
	WriteTo( bb );
}
bool List<bool>::VarReadFrom( ByteBuffer& bb )
{
	return ReadFrom( bb );
}
bool List<bool>::VarReadFrom( ByteBuffer& bb, int minLen, int maxLen )
{
	return ReadFrom( bb, minLen, maxLen );
}

}
