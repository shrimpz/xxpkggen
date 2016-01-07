#include "xxlib.h"

namespace xxlib
{


ByteBuffer::ByteBuffer( int capacity )
{
    if( capacity <= 0 )
    {
        capacity = 1024;
    }
	bufLen = (int)Round2n( capacity );
	isRef = false;
	buf = new char[ bufLen ];
	dataLen = offset = 0;
}

ByteBuffer::ByteBuffer()
	: ByteBuffer( 1024 )
{
}

ByteBuffer::ByteBuffer( char* _buf, int _bufLen, int _dataLen /*= 0*/, bool _isRef /*= false */ )
{
	dataLen = _dataLen;
	offset = 0;
	if( _isRef )
	{
		buf = _buf;
		bufLen = _bufLen;
		isRef = true;
	}
	else
	{
		bufLen = (int)Round2n( _dataLen );
		buf = new char[ bufLen ];
		memcpy( buf, _buf, _dataLen );
		isRef = false;
	}
}

ByteBuffer::ByteBuffer( ByteBuffer const& other )
	: ByteBuffer( other.dataLen + 1 )
{
	memcpy( buf, other.buf, other.dataLen + 1 );
	dataLen = other.dataLen;
}

ByteBuffer::ByteBuffer( ByteBuffer&& other )
	: buf( other.buf )
	, bufLen( other.bufLen )
	, dataLen( other.dataLen )
	, offset( other.offset )
	, isRef( other.isRef )
{
	//other.buf = nullptr;
	//other.bufLen = 0;
	//other.dataLen = 0;
	//other.offset = 0;
	other.isRef = true;     // 防回收
}

void ByteBuffer::Destroy()
{
	if( isRef )
	{
		return;
	}
	delete[] buf;
#ifdef XX_DEBUG
	buf = nullptr;
#endif
}

ByteBuffer::~ByteBuffer()
{
	Destroy();
#if XX_BYTEBUFFER_PTR_SUPPORT_ENABLED
	PtrStoreDestroy();
	IdxStoreDestroy();
#endif
}

void ByteBuffer::Clear()
{
	dataLen = offset = 0;
}

bool ByteBuffer::Empty() const
{
	return dataLen == 0;
}

bool ByteBuffer::Full() const
{
	return dataLen == offset;
}


void ByteBuffer::Assign( char const* _buf, int _bufLen, int _dataLen /*= 0*/, bool _isRef /*= false */ )
{
	assert( buf != _buf );
	offset = 0;
	if( _isRef )
	{
		Destroy();
		buf = (char*)_buf;
		bufLen = _bufLen;
		dataLen = _dataLen;
		isRef = _isRef;
		return;
	}
	if( bufLen < _dataLen )
	{
		Destroy();
		bufLen = (int)Round2n( _dataLen );
		_isRef = false;
		buf = new char[ bufLen ];
	}
	dataLen = _dataLen;
	memcpy( buf, _buf, _dataLen );
}

void ByteBuffer::Reserve( int capacity )
{
	if( capacity <= bufLen )
	{
		return;
	}
	bufLen = (int)Round2n( capacity );
	auto newBuf = new char[ bufLen ];
	memcpy( newBuf, buf, dataLen );
	Destroy();
	isRef = false;
	buf = newBuf;
}

void ByteBuffer::Resize( int _dataLen )
{
	if( _dataLen == dataLen )
	{
		return;
	}
	else if( _dataLen < dataLen )
	{
		dataLen = _dataLen;
		if( dataLen > offset )
		{
			offset = dataLen;
		}
	}
	else
	{
		Reserve( _dataLen );
		dataLen = _dataLen;
	}
}

ByteBuffer& ByteBuffer::operator=( ByteBuffer const& other )
{
	if( this == &other )
	{
		return *this;
	}
	offset = other.offset;
	dataLen = other.dataLen;
	if( bufLen >= other.dataLen )
	{
		memcpy( buf, other.buf, other.dataLen );
	}
	else
	{
		Destroy();
		bufLen = (int)Round2n( other.dataLen );
		isRef = false;
		buf = new char[ bufLen ];
		memcpy( buf, other.buf, other.dataLen );
	}
	return *this;
}

ByteBuffer& ByteBuffer::operator=( ByteBuffer&& other )
{
	Destroy();
	buf = other.buf;
	bufLen = other.bufLen;
	dataLen = other.dataLen;
	offset = other.offset;
	isRef = other.isRef;
	//other.buf = nullptr;
	//other.bufLen = 0;
	//other.dataLen = 0;
	//other.offset = 0;
	other.isRef = true;
	return *this;
}

char ByteBuffer::operator[]( int idx ) const
{
	assert( idx >= 0 && idx < dataLen );
	return buf[ idx ];
}

char& ByteBuffer::operator[]( int idx )
{
	assert( idx >= 0 && idx < dataLen );
	return buf[ idx ];
}

char ByteBuffer::IndexAt( int idx ) const
{
	assert( idx >= 0 && idx < dataLen );
	return buf[ idx ];
}

char& ByteBuffer::IndexAt( int idx )
{
	assert( idx >= 0 && idx < dataLen );
	return buf[ idx ];
}




void ByteBuffer::FastWrite( char const* _buf, int _dataLen )
{
	memcpy( buf + dataLen, _buf, _dataLen );
	dataLen += _dataLen;
}

void ByteBuffer::Write( char const* _buf, int _dataLen )
{
	Reserve( dataLen + _dataLen );
	FastWrite( _buf, _dataLen );
}

bool ByteBuffer::Read( char* _buf, int _dataLen )
{
	if( offset + _dataLen > dataLen )
	{
		return false;
	}
	memcpy( _buf, buf + offset, _dataLen );
	offset += _dataLen;
	return true;
}








void ByteBuffer::WriteTo( ByteBuffer& bb ) const
{
	bb.VarWrite( (uint)dataLen );
	if( !dataLen )
	{
		return;
	}
	bb.Write( buf, dataLen );
}

void ByteBuffer::FastWriteTo( ByteBuffer& bb ) const
{
	bb.FastVarWrite( (uint)dataLen );
	if( !dataLen )
	{
		return;
	}
	bb.FastWrite( buf, dataLen );
}

bool ByteBuffer::ReadFrom( ByteBuffer& bb )
{
    return ReadFrom( bb, 0, 0 );
}
bool ByteBuffer::ReadFrom( ByteBuffer& bb, int minLen, int maxLen )
{
	int len;
	if( !bb.VarRead( *(uint*)&len ) )
	{
		return false;
	}
	if( bb.offset + len > bb.dataLen )
	{
		return false;
	}
	if( len < minLen || ( maxLen > 0 && len > maxLen ) )
	{
		return false;
	}
	Clear();
	Reserve( len );
	dataLen = len;
	memcpy( buf, bb.buf + bb.offset, len );
	bb.offset += len;
	return true;
}


void ByteBuffer::VarWriteTo( ByteBuffer& bb ) const
{
	WriteTo( bb );
}

bool ByteBuffer::VarReadFrom( ByteBuffer& bb )
{
	return ReadFrom( bb );
}
bool ByteBuffer::VarReadFrom( ByteBuffer& bb, int minLen, int maxLen )
{
	return ReadFrom( bb, minLen, maxLen );
}










#if XX_BYTEBUFFER_PTR_SUPPORT_ENABLED
void ByteBuffer::PtrStoreInit()
{
	if( ptrStore )
	{
		ptrStore->Clear();
	}
	else
	{
		ptrStore = new Dict<void*, uint>();
	}
}
void ByteBuffer::PtrStoreDestroy()
{
	if( ptrStore )
	{
		delete ptrStore;
	}
}

void ByteBuffer::IdxStoreInit()
{
	if( idxStore )
	{
		idxStore->Clear();
	}
	else
	{
		idxStore = new Dict<uint, void*>();
	}
}
void ByteBuffer::IdxStoreDestroy()
{
	if( idxStore )
	{
		delete idxStore;
	}
}

#endif



// negative -> ZigZag positive
// 效果：负数变正，正数 *= 2
uint ByteBuffer::ZigZagEncode( int  i )
{
	return ( i << 1 ) ^ ( i >> 31 );
}
uint64 ByteBuffer::ZigZagEncode( int64  i )
{
	return ( i << 1 ) ^ ( i >> 63 );
}

// ZigZag positive -> negative
int  ByteBuffer::ZigZagDecode( uint i )
{
	return (int)( i >> 1 ) ^ ( -(int)( i & 1 ) );
}
int64  ByteBuffer::ZigZagDecode( uint64 i )
{
	return (int64)( i >> 1 ) ^ ( -(int64)( i & 1 ) );
}





void ByteBuffer::Write7Core( char* buf, int& offset, uint v )
{
Lab1:
	auto b7 = uint8( v );
	if( ( v >>= 7 ) )
	{
		buf[ offset++ ] = b7 | 0x80u;
		goto Lab1;
	}
	buf[ offset++ ] = b7;
}

void ByteBuffer::Write7Core( char* buf, int& offset, uint64 v )
{
	auto idx8 = offset + 8;
Lab1:
	auto b7 = uint8( v );
	if( ( v >>= 7 ) )
	{
		buf[ offset++ ] = b7 | 0x80u;
		if( offset == idx8 )
		{
			buf[ offset++ ] = (uint8)v;
		}
		else
		{
			goto Lab1;
		}
	}
	else
	{
		buf[ offset++ ] = b7;
	}
}

void ByteBuffer::FastRead7Core( uint64& v, char* buf, int& offset )
{
	auto p = buf + offset;
	uint64 i = 0, b7;
	uint lshift = 0, idx = 0;
Lab1:
	b7 = p[ idx++ ];
	if( b7 > 0x7F )
	{
		if( idx < 8 )
		{
			i |= ( b7 & 0x7F ) << lshift;
			lshift += 7;
			goto Lab1;
		}
		else
		{
			i |= ( b7 << lshift ) | ( (uint64)p[ idx++ ] << 28 << 28 );
		}
	}
	else
	{
		i |= b7 << lshift;
	}
	v = i;
	offset += idx;
}

bool ByteBuffer::Read7Core( uint64& v, char* buf, int dataLen, int& offset )
{
	if( offset >= dataLen )
	{
		return false;// ReadStatus::NotEnoughData;
	}
	auto p = buf + offset;
	uint64 i = 0, b7;
	int lshift = 0, idx = 0;
	dataLen -= offset;
Lab1:
	b7 = p[ idx++ ];
	if( b7 > 0x7F )
	{
		if( idx >= dataLen )
		{
			return false;// ReadStatus::NotEnoughData;
		}
		if( idx < 8 )
		{
			i |= ( b7 & 0x7F ) << lshift;
			lshift += 7;
			goto Lab1;
		}
		else
		{
			i |= ( b7 << lshift ) | ( (uint64)p[ idx++ ] << 28 << 28 );
		}
	}
	else
	{
		i |= b7 << lshift;
	}
	v = i;
	offset += idx;
	return true;// ReadStatus::Success;
}

bool ByteBuffer::Read7Core( uint& v, char * buf, int len, int & offset )
{
	if( offset >= len )
	{
		return false;// ReadStatus::NotEnoughData;
	}
	auto p = buf + offset;
	uint i = 0, b7;
	int lshift = 0, idx = 0;
	len -= offset;
Lab1:
	b7 = p[ idx++ ];
	if( b7 > 0x7F )
	{
		if( idx == 5 )
		{
			return false;// ReadStatus::Overflow;
		}
		if( idx >= len )
		{
			return false;// ReadStatus::NotEnoughData;
		}
		i |= ( b7 & 0x7F ) << lshift;
		lshift += 7;
		goto Lab1;
	}
	else
	{
		if( idx == 5 && b7 > 15 )
		{
			return false;// ReadStatus::Overflow;
		}
		else
		{
			i |= b7 << lshift;
		}
	}
	v = i;
	offset += idx;
	return true;// ReadStatus::Success;
}

void ByteBuffer::FastRead7Core( uint& v, char* buf, int& offset )
{
	auto p = buf + offset;
	uint i = 0, b7, lshift = 0, idx = 0;
Lab1:
	b7 = p[ idx++ ];
	if( b7 > 0x7F )
	{
		i |= ( b7 & 0x7F ) << lshift;
		lshift += 7;
		goto Lab1;
	}
	else
	{
		i |= b7 << lshift;
	}
	v = i;
	offset += idx;
}





void ByteBuffer::VarWrite( int v )
{
	Reserve( dataLen + sizeof( decltype( v ) ) );
	ByteBuffer::Write7Core( buf, dataLen, ByteBuffer::ZigZagEncode( v ) );
}

void ByteBuffer::VarWrite( uint v )
{
	Reserve( dataLen + sizeof( decltype( v ) ) );
	ByteBuffer::Write7Core( buf, dataLen, v );
}

void ByteBuffer::VarWrite( int64 v )
{
	Reserve( dataLen + sizeof( decltype( v ) ) );
	ByteBuffer::Write7Core( buf, dataLen, ByteBuffer::ZigZagEncode( v ) );
}

void ByteBuffer::VarWrite( uint64 v )
{
	Reserve( dataLen + sizeof( decltype( v ) ) );
	ByteBuffer::Write7Core( buf, dataLen, v );
}




bool ByteBuffer::VarRead( int& v )
{
	uint i;
	if( !VarRead( i ) )
	{
		return false;
	}
	v = ByteBuffer::ZigZagDecode( i );
	return true;
}

bool ByteBuffer::VarRead( uint& v )
{
	return ByteBuffer::Read7Core( v, buf, dataLen, offset );
}

bool ByteBuffer::VarRead( int64& v )
{
	uint64 i;
	if( !VarRead( i ) )
	{
		return false;
	}
	v = ByteBuffer::ZigZagDecode( i );
	return true;
}

bool ByteBuffer::VarRead( uint64& v )
{
	return ByteBuffer::Read7Core( v, buf, dataLen, offset );
}




void ByteBuffer::FastVarWrite( int v )
{
	ByteBuffer::Write7Core( buf, dataLen, ByteBuffer::ZigZagEncode( v ) );
}

void ByteBuffer::FastVarWrite( uint v )
{
	ByteBuffer::Write7Core( buf, dataLen, v );
}

void ByteBuffer::FastVarWrite( int64 v )
{
	ByteBuffer::Write7Core( buf, dataLen, ByteBuffer::ZigZagEncode( v ) );
}

void ByteBuffer::FastVarWrite( uint64 v )
{
	ByteBuffer::Write7Core( buf, dataLen, v );
}




void ByteBuffer::FastVarRead( int& v )
{
	uint i;
	FastVarRead( i );
	v = ByteBuffer::ZigZagDecode( i );
}

void ByteBuffer::FastVarRead( uint& v )
{
	ByteBuffer::FastRead7Core( v, buf, offset );
}

void ByteBuffer::FastVarRead( int64& v )
{
	uint64 i;
	FastVarRead( i );
	v = ByteBuffer::ZigZagDecode( i );
}

void ByteBuffer::FastVarRead( uint64& v )
{
	ByteBuffer::FastRead7Core( v, buf, offset );
}










int GetToStringMaxLength( ByteBuffer const& v )
{
	return 100 + ( v.dataLen / 16 + ( ( v.dataLen % 16 ) == 0 ? 0 : 1 ) ) * 90;
}


void ToStringCore( char* dstBuf, int& offset, char const* buf, int len )
{
	for( int i = 0; i < len; ++i )
	{
		auto c = (uint8)buf[ i ];
		if( c < 32 || c > 126 )
		{
			offset += ToString( dstBuf + offset, '.' );
		}
		else offset += ToString( dstBuf + offset, (char)c );
	}
}

int ToString( char* dstBuf, ByteBuffer const& v )
{
	if( v.dataLen == 0 )
	{
		return 0;
	}
	int len = v.dataLen;
	int offset = 0;
	offset += ToString( dstBuf + offset, "--------  0  1  2  3 | 4  5  6  7 | 8  9  A  B | C  D  E  F   dataLen = " );
    offset += ToString( dstBuf + offset, v.dataLen );
	int i = 0;
	for( ; i < len; ++i )
	{
		if( ( i % 16 ) == 0 )
		{
			if( i )
			{           // output ascii to the end of the line
				offset += ToString( dstBuf + offset, "  " );
				ToStringCore( dstBuf, offset, v.buf + i - 16, 16 );
			}
			offset += ToString( dstBuf + offset, '\n' );
			offset += ToHexString( dstBuf + offset, i );
			offset += ToString( dstBuf + offset, "  " );
		}
		else if( i && ( i % 4 == 0 ) )
		{
			offset += ToString( dstBuf + offset, "  " );
		}
		else
		{
			offset += ToString( dstBuf + offset, ' ' );
		}
		offset += ToHexString( dstBuf + offset, (uint8)v.buf[ i ] );
	}
	int left = i % 16;
	if( left == 0 )
	{
		left = 16;
	}
	if( left )
	{
		len = len + 16 - left;
		for( ; i < len; ++i )
		{
			if( i && ( i % 4 == 0 ) )
			{
				offset += ToString( dstBuf + offset, "  " );
			}
			else
			{
				offset += ToString( dstBuf + offset, ' ' );
			}
			offset += ToString( dstBuf + offset, "  " );
		}
		offset += ToString( dstBuf + offset, "  " );
		ToStringCore( dstBuf, offset, v.buf + i - 16, left );
	}
	return offset;
}



}
