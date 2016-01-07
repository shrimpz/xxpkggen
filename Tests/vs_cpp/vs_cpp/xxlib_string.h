#pragma once
namespace xxlib
{

class ByteBuffer;
class Pool;

// utils_str ext for String
class String;
int GetToStringMaxLength( String const& v );
int ToString( char* dstBuf, String const& v );
void FromString( String& dstVar, char const* s );

class String : Memmoveable
{
public:
	char*       buf = nullptr;
	int         bufLen = 0;
	int         dataLen = 0;

	String();													// String( 64 )
	explicit String( int capacity );                            // prepare
	String( Pool& p );                                          // prepare with pool buffer
	template<int len>
	String( Pool& p, char const ( &s )[ len ] );					// Get _buf from pool and Copy s into
	String( Pool& p, char const* _buf, int _dataLen );          // Get _buf from pool and Copy s into
	String( char const* _buf, int _bufLen, int _dataLen, bool isRef );    // ref or Copy
	String( char const* s, bool isRef = false );                // Copy or ref
	String( String const& o );                                  // Copy
	String( String&& o );                                       // move
	String& operator=( String const& o );                       // Copy
	String& operator=( String&& o );                            // move
	~String();                                                  // Dispose _buf
	void Assign( char const* _buf, int _bufLen, int _dataLen, bool isRef );     // ref or Copy
	void Assign( char const* s, int sLen, bool isRef );         // Copy or ref
	void Reserve( int capacity );                               // prepare memory
	void Resize( int len );										// cut( maybe damage utf8 ) or grow up ( does not set 0 )
	void Clear();                                               // Set dataLen to 0
	char* C_str();												// return buf( place \0 at the end )
	char const& operator[] ( int idx ) const;                   // return buf[ idx ]
	char& operator[] ( int idx );                               // return buf[ idx ]
	char const& IndexAt( int idx ) const;
	char& IndexAt( int idx );
	void Add( char c );
	void Pop();
	char& Top();
	char const& Top() const;
	bool Empty() const;
	void Insert( int idx, char const* s, int sLen = 0 );
	void Insert( int idx, String const& s );

	// todo: more util funcs like  Find, trim, split, replace,....
	// todo: Formatter with AppendTo( String& s ) interface
	// sample: auto f = Formatter().setScale( 2 ).setWidth( 10, '0' );  s.Append(...... f( 1.2345 )...... ) will be Append  0000001.23
	// 

	bool operator==( String const& o ) const;
	bool operator!=( String const& o ) const;
	bool operator<( String const& o ) const;
	bool operator>( String const& o ) const;
	bool operator<=( String const& o ) const;
	bool operator>=( String const& o ) const;

	bool operator==( char const* o ) const;

	template<size_t sLen>
	bool operator==( char const( &s )[ sLen ] ) const
	{
		if( sLen != dataLen ) return false;
		return memcmp( buf, s, dataLen ) == 0;
	}



	void ToLower();
	void ToUpper();
	void ToUpperFirstChar();
	void FastToLower();										// 2x faster than ToLower(). bad chars：   @[\]^_` 

	int GetHashCode() const;


	template<typename...TS>
	void Append( TS const&...vs );

	template<typename...TS>
	void AppendFormat( char const* format, TS const&...vs );

	template<typename T>
	void AppendHex( T const& v );

	template<typename T>
	String& operator<<( T const& v );


	// like Append, return strlen.
	// dangerous: buffer overflow
	template<typename...TS>
	static int Fill( char * _buf, TS const&...vs );

	template<typename...TS>
	static int FillHex( char * _buf, TS const&...vs );



	template<typename...TS>
	static String Make( TS const&...vs );

	template<typename...TS>
	static String MakeFormat( char const* format, TS const&...vs );

	template<typename...TS>
	static String Make( Pool& p, TS const&...vs );

	template<typename...TS>
	static String MakeFormat( Pool& p, char const* format, TS const&...vs );

	template<typename...TS, int _bufLen>
	static String Make( char( &_buf )[ _bufLen ], TS const&...vs );

	template<typename T>
	static String ToString( T const& v );

	template<typename T>
	static String ToHexString( T const& v );




	// dangerous: Get first buffer from static circle pool char[ 32 ][ 1024 ]
	template<typename...TS>
	static String FastMake( TS const&...vs );

	// dangerous: Get first buffer from static circle pool char[ 32 ][ 128 ]
	template<typename T>
	static String const FastToString( T const& v );

	// dangerous: Get first buffer from static circle pool char[ 32 ][ 32 ]
	template<typename T>
	static String const FastToHexString( T const& v );





	// for dump binary data
	void DumpCore( char const* buf, int len );
	void Dump( char const* buf, int len );




	// for ByteBuffer
	void WriteTo( ByteBuffer& fb ) const;
	void FastWriteTo( ByteBuffer& fb ) const;
	bool ReadFrom( ByteBuffer& fb );
	bool ReadFrom( ByteBuffer& fb, int minLen, int maxLen );
	void VarWriteTo( ByteBuffer& fb ) const;
	bool VarReadFrom( ByteBuffer& fb );
	bool VarReadFrom( ByteBuffer& fb, int minLen, int maxLen );

private:

	template<typename T>
	void AppendFormatCore( String& s, int& n, T const& v );

	template<typename T, typename...TS>
	void AppendFormatCore( String& s, int& n, T const& v, TS const&...vs );

	template<typename T>
	static void FillCore( char * & _buf, int & offset, T const& v );

	template<typename T, typename...TS>
	static void FillCore( char * & _buf, int & offset, T const& v, TS const&...vs );

	template<typename T>
	static void FillHexCore( char * & _buf, int & offset, T const& v );

	template<typename T, typename...TS>
	static void FillHexCore( char * & _buf, int & offset, T const& v, TS const&...vs );

	template<typename T>
	static void GetFillMaxLengthCore( int & len, T const& v );

	template<typename T, typename...TS>
	static void GetFillMaxLengthCore( int & len, T const& v, TS const&...vs );

	template<typename...TS>
	static int GetFillMaxLength( TS const&...vs );

	Pool*       disposer;		// = nullptr: delete   = emptyPool: isRef    other: pool.Free;
	void Dispose();

} const emptyString( nullptr, 0, 0, true );


}


