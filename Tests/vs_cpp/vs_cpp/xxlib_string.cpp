#include "xxlib.h"
namespace xxlib
{


int GetToStringMaxLength( String const& v )
{
	return v.dataLen;
}

int ToString( char* dstBuf, String const& v )
{
	memcpy( dstBuf, v.buf, v.dataLen );
	return v.dataLen;
}

void FromString( String& dstVar, char const* s )
{
	dstVar = s;
}







String::String( int capacity )
{
	if( capacity < 64 )
	{
		bufLen = 64;
	}
	else
	{
		bufLen = (int)Round2n( capacity );
	}
	disposer = nullptr;
	dataLen = 0;
	buf = new char[ bufLen ];
}


String::String()
	: String( 64 )
{
}


String::String( Pool& p )
{
	buf = (char*)p.Alloc();
	bufLen = p.itemSize;
	dataLen = 0;
	disposer = &p;
}

String::String( Pool& p, char const* buf_, int dataLen_ )
{
	assert( dataLen_ <= p.itemSize );
	buf = (char*)p.Alloc();
	bufLen = p.itemSize;
	dataLen = dataLen_;
	disposer = &p;
	memcpy( buf, buf_, dataLen_ );
}


String::String( char const* buf_, int bufLen_, int dataLen_, bool isRef )
{
	dataLen = dataLen_;
	if( isRef )
	{
		buf = (char*)buf_;
		bufLen = bufLen_;
		disposer = (Pool*)&emptyPool;
	}
	else
	{
		bufLen = (int)Round2n( dataLen_ );
		buf = new char[ bufLen ];
		disposer = nullptr;
		memcpy( buf, buf_, dataLen_ );
	}
}

String::String( char const* s, bool isRef )
{
	dataLen = (int)strlen( s );
	if( isRef )
	{
		buf = (char*)s;
		bufLen = dataLen + 1;
		disposer = (Pool*)&emptyPool;
	}
	else
	{
		bufLen = (int)Round2n( dataLen + 1 );
		buf = new char[ bufLen ];
		disposer = nullptr;
		memcpy( buf, s, dataLen );
	}
}

String::String( String const& o )
	: String( o.dataLen )
{
	memcpy( buf, o.buf, o.dataLen );
	dataLen = o.dataLen;
}

String::String( String&& o )
	: buf( o.buf )
	, bufLen( o.bufLen )
	, dataLen( o.dataLen )
	, disposer( o.disposer )
{
	o.buf = nullptr;
	o.bufLen = 0;
	o.dataLen = 0;
	o.disposer = nullptr;
}


String& String::operator=( String const& o )
{
	if( XX_UNLIKELY( this == &o ) )
	{
		return *this;
	}
	dataLen = o.dataLen;
	if( bufLen >= o.dataLen )
	{
		memcpy( buf, o.buf, o.dataLen );
	}
	else
	{
		Dispose();
		bufLen = (int)Round2n( o.dataLen );
		disposer = nullptr;
		buf = new char[ bufLen ];
		memcpy( buf, o.buf, o.dataLen );
	}
	return *this;
}

String& String::operator=( String && o )
{
	Dispose();
	buf = o.buf;
	bufLen = o.bufLen;
	dataLen = o.dataLen;
	disposer = o.disposer;
	o.buf = nullptr;
	o.bufLen = 0;
	o.dataLen = 0;
	o.disposer = nullptr;
	return *this;
}


String::~String()
{
	Dispose();
}

void String::Assign( char const* buf_, int bufLen_, int dataLen_, bool isRef )
{
	assert( buf != buf_ );
	if( isRef )
	{
		Dispose();
		buf = (char*)buf_;
		bufLen = bufLen_;
		dataLen = dataLen_;
		disposer = (Pool*)&emptyPool;
		return;
	}
	if( bufLen <= dataLen_ )
	{
		Dispose();
		bufLen = (int)Round2n( dataLen_ );
		disposer = nullptr;
		buf = new char[ bufLen ];
	}
	dataLen = dataLen_;
	memcpy( buf, buf_, dataLen_ );
}

void String::Assign( char const* s, int sLen, bool isRef )
{
	if( !sLen )
	{
		sLen = (int)strlen( s );
	}
	Assign( s, sLen, sLen, isRef );
}

void String::Reserve( int capacity )
{
	if( XX_UNLIKELY( capacity <= bufLen ) )
	{
		return;
	}
	bufLen = (int)Round2n( capacity );
	auto newBuf = new char[ bufLen ];
	memcpy( newBuf, buf, dataLen );
	Dispose();
	disposer = nullptr;
	buf = newBuf;
}

void String::Resize( int len )
{
	if( XX_UNLIKELY( len == dataLen ) )
	{
		return;
	}
	else if( len < dataLen )
	{
		dataLen = len;
	}
	else
	{
		Reserve( len );
		dataLen = len;
	}
}

void String::Clear()
{
	dataLen = 0;
}

char* String::C_str()
{
	if( XX_UNLIKELY( dataLen == bufLen ) )
	{
		Reserve( dataLen + 1 );
	}
	buf[ dataLen ] = '\0';
	return buf;
}

char const& String::operator[]( int idx ) const
{
	assert( idx >= 0 && idx < dataLen );
	return buf[ idx ];
}
char& String::operator[]( int idx )
{
	assert( idx >= 0 && idx < dataLen );
	return buf[ idx ];
}

char const& String::IndexAt( int idx ) const
{
	assert( idx >= 0 && idx < dataLen );
	return buf[ idx ];
}

char& String::IndexAt( int idx )
{
	assert( idx >= 0 && idx < dataLen );
	return buf[ idx ];
}

void String::Add( char c )
{
	if( XX_UNLIKELY( dataLen == bufLen ) )
	{
		Reserve( dataLen + 1 );
	}
	buf[ dataLen ] = c;
	++dataLen;
}


void String::Dispose()
{
	if( disposer )	// is not new
	{
		if( disposer != &emptyPool )	// is not ref
		{
			disposer->Free( buf );		// is pool
		}
	}
	else
	{
		delete[] buf;					// is new
	}
}

void String::Pop()
{
	assert( dataLen > 0 );
	--dataLen;
}

char& String::Top()
{
	assert( dataLen > 0 );
	return buf[ dataLen - 1 ];
}

char const& String::Top() const
{
	assert( dataLen > 0 );
	return buf[ dataLen - 1 ];
}

bool String::Empty() const
{
	return dataLen == 0;
}

void String::Insert( int idx, char const* s, int sLen )
{
	if( XX_UNLIKELY( idx >= dataLen ) )
	{
		Append( String( s, sLen, sLen, true ) );
		return;
	}
	if( !sLen )
	{
		sLen = (int)strlen( s );
		if( !sLen )
		{
			return;
		}
	}
	Reserve( dataLen + sLen );
	memmove( buf + idx + sLen, buf + idx, dataLen - idx );
	memcpy( buf + idx, s, sLen );
	dataLen += sLen;
}

void String::Insert( int idx, String const& s )
{
	Insert( idx, s.buf, s.dataLen );
}






static byte const lowerchars[] =
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
	27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106,
	107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 91, 92, 93, 94, 95,
	96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116,
	117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136,
	137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156,
	157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
	177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196,
	197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216,
	217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236,
	237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};
static byte const upperchars[] =
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
	27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
	77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 65, 66, 67, 68, 69,
	70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 123, 124, 125,
	126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145,
	146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165,
	166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185,
	186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205,
	206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225,
	226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245,
	246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};

void String::ToLower()
{
	for( int i = 0; i < dataLen; ++i )
	{
		buf[ i ] = lowerchars[ (int)buf[ i ] ];
	}
}

void String::ToUpper()
{
	for( int i = 0; i < dataLen; ++i )
	{
		buf[ i ] = upperchars[ (int)buf[ i ] ];
	}
}

void String::ToUpperFirstChar()
{
	if( XX_LIKELY( dataLen ) )
	{
		buf[ 0 ] = upperchars[ (int)buf[ 0 ] ];
	}
}

void String::FastToLower()
{
	int i = 0;
	int mod = (size_t)buf & ( sizeof( size_t ) - 1 );
	if( mod )
	{
		if( mod > dataLen )
		{
			mod = dataLen;
		}
		for( ; i < mod; ++i )
		{
			buf[ i ] |= 0x20;
		}
	}
	mod = ( dataLen - mod ) % sizeof( size_t );
	for( ; i < dataLen - mod; i += sizeof( size_t ) )
	{
		*(size_t*)( buf + i ) |= size_t( 0x2020202020202020 );
	}
	for( ; i < dataLen; ++i )
	{
		buf[ i ] |= 0x20;
	}
}


bool String::operator==( char const* o ) const
{
	if( !o )
	{
		return false;
	}
	String tmp( o, true );
	return operator==( tmp );
}

bool String::operator==( String const& o ) const
{
	if( this == &o )
	{
		return true;
	}
	if( dataLen != o.dataLen )
	{
		return false;
	}
	return memcmp( buf, o.buf, dataLen ) == 0;
}

bool String::operator!=( String const& o ) const
{
	return !operator==( o );
}

bool String::operator<( String const& o ) const
{
	if( this == &o )
	{
		return false;
	}
	auto r = memcmp( buf, o.buf, MIN( dataLen, o.dataLen ) );
	if( r == 0 )
	{
		return dataLen < o.dataLen;
	}
	return r < 0;
}

bool String::operator>( String const& o ) const
{
	if( this == &o )
	{
		return false;
	}
	auto r = memcmp( buf, o.buf, MIN( dataLen, o.dataLen ) );
	if( r == 0 )
	{
		return dataLen > o.dataLen;
	}
	return r > 0;
}

bool String::operator<=( String const& o ) const
{
	return !operator>( o );
}

bool String::operator>=( String const& o ) const
{
	return !operator<( o );
}

int String::GetHashCode() const
{
	return GetHash_CS( (byte const*)buf, dataLen );
}





void String::WriteTo( ByteBuffer& bb ) const
{
	bb.VarWrite( (uint)dataLen );
	if( !dataLen )
	{
		return;
	}
	bb.Write( buf, dataLen );
}
void String::FastWriteTo( ByteBuffer& bb ) const
{
	bb.FastVarWrite( (uint)dataLen );
	if( !dataLen )
	{
		return;
	}
	bb.FastWrite( buf, dataLen );
}

bool String::ReadFrom( ByteBuffer& bb )
{
	return ReadFrom( bb, 0, 0 );
}

bool String::ReadFrom( ByteBuffer& bb, int minLen, int maxLen )
{
	int len = -1;
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
	Assign( bb.buf + bb.offset, 0, len, false );
	bb.offset += len;
	return true;
}


void String::VarWriteTo( ByteBuffer& bb ) const
{
	WriteTo( bb );
}

bool String::VarReadFrom( ByteBuffer& bb )
{
	return ReadFrom( bb );
}

bool String::VarReadFrom( ByteBuffer& bb, int minLen, int maxLen )
{
	return ReadFrom( bb, minLen, maxLen );
}




void String::DumpCore( char const* buf, int len )
{
	for( int i = 0; i < len; ++i )
	{
		auto c = (uint8)buf[ i ];
		if( c < 32 || c > 126 )
		{
			Append( '.' );
		}
		else Append( (char)c );
	}
}

void String::Dump( char const* buf, int len )
{
	if( len == 0 )
	{
		return;
	}
	Append( "\n--------  0  1  2  3 | 4  5  6  7 | 8  9  A  B | C  D  E  F" );
	int i = 0;
	for( ; i < len; ++i )
	{
		if( ( i % 16 ) == 0 )
		{
			if( i )
			{           // output ascii to the end of the line
				Append( "  " );
				DumpCore( buf + i - 16, 16 );
			}
			Append( '\n' );
			AppendHex( i );
			Append( "  " );
		}
		else if( i && ( i % 4 == 0 ) )
		{
			Append( "  " );
		}
		else
		{
			Append( ' ' );
		}
		AppendHex( (uint8)buf[ i ] );
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
				Append( "  " );
			}
			else
			{
				Append( ' ' );
			}
			Append( "  " );
		}
		Append( "  " );
		DumpCore( buf + i - 16, left );
	}
}




}
